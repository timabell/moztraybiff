/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
/* ***** BEGIN LICENSE BLOCK *****
 * Version: NPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is 
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 * Seth Spitzer <sspitzer@netscape.com>
 * Bhuvan Racham <racham@netscape.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or 
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the NPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the NPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "nsMessengerFreeDesktopIntegration.h"
#include <nsCOMPtr.h>
#include <nsIMsgAccountManager.h>
#include <nsIMsgMailSession.h>
#include <nsIMsgIncomingServer.h>
#include <nsIMsgIdentity.h>
#include <nsIMsgAccount.h>
#include <nsIRDFResource.h>
#include <nsIMsgFolder.h>
#include <nsMsgBaseCID.h>
#include <nsMsgFolderFlags.h>
#include <nsIProfile.h>
#include <nsIRDFService.h>

#include <nsIWindowMediator.h>
#include <nsIDOMWindowInternal.h>
#include <nsPIDOMWindow.h>
#include <nsIScriptGlobalObject.h>
#include <nsIDocShell.h>
#include <nsIBaseWindow.h>
#include <nsIWidget.h>

#include <nsIMessengerWindowService.h>
#include <prprf.h>
#include <nsIWeakReference.h>
#include <nsIStringBundle.h>
#include <nsIPrefService.h>
#include <nsIPrefBranchInternal.h>

// One day, we should use an icon from chrome.
// Meanwhile, we'll embed the icon:
#include "trayBiffIcon.h"

// Prefs in use
const char* PREF_BIFF_SHOW_ICON = "mail.biff.show_icon";

// Useful for debugging.
inline void PrintAtom(nsIAtom* atom)
{
  const char *res;
  atom->GetUTF8String(&res);
  fprintf(stderr, "Atom = %s\n", res);
}

#include "eggstatusicon.h"
#include <gtk/gtk.h>

// Refer to mail/base/content/mailCore.js : toMessengerWindow() to see
// how we raise the mailer window.

static void openMailWindow(const PRUnichar * aMailWindowName, const char * aFolderUri)
{
  nsCOMPtr<nsIWindowMediator> mediator ( do_GetService(NS_WINDOWMEDIATOR_CONTRACTID) );
  if (mediator)
  {
    nsCOMPtr<nsIDOMWindowInternal> domWindow;
    mediator->GetMostRecentWindow(aMailWindowName, getter_AddRefs(domWindow));
    if (domWindow)
    {
      nsCOMPtr<nsISupports> xpConnectObj;
      nsCOMPtr<nsPIDOMWindow> piDOMWindow(do_QueryInterface(domWindow));
      if (piDOMWindow)
      {
        piDOMWindow->GetObjectProperty(NS_LITERAL_STRING("MsgWindowCommands").get(), getter_AddRefs(xpConnectObj));
        nsCOMPtr<nsIMsgWindowCommands> msgWindowCommands = do_QueryInterface(xpConnectObj);
        if (msgWindowCommands)
          msgWindowCommands->SelectFolder(aFolderUri);
      }

      // TODO: Original Win32 code tried to restore minimized window and move
      // to foregroud. Should we do some GTK+ magic?
      // (original code was from nsNativeAppWinSupport.cpp)
      // focus() on X11 brings the window up but doesn't focus it.
      domWindow->Focus();
    }
    else
    {
      // the user doesn't have a mail window open already so open one for them...
      nsCOMPtr <nsIMessengerWindowService> messengerWindowService = do_GetService(NS_MESSENGERWINDOWSERVICE_CONTRACTID);
      // if we want to preselect the first account with new mail, here is where we would try to generate
      // a uri to pass in (and add code to the messenger window service to make that work)
      if (messengerWindowService) 
        messengerWindowService->OpenMessengerWindowWithUri("mail:3pane", aFolderUri, nsMsgKey_None);
    }
  }
}

nsMessengerFreeDesktopIntegration::nsMessengerFreeDesktopIntegration() :

	mTrayIcon(NULL),
	mTrayMenu(NULL),
	
	mShowBiffIcon(PR_TRUE)
	
{
  mBiffStateAtom = do_GetAtom("BiffState");
  NS_NewISupportsArray(getter_AddRefs(mFoldersWithNewMail));
}

nsMessengerFreeDesktopIntegration::~nsMessengerFreeDesktopIntegration()
{
  RemoveBiffIcon(); 

  if (mTrayMenu != NULL)
     gtk_widget_destroy(mTrayMenu);
}

NS_IMPL_ADDREF(nsMessengerFreeDesktopIntegration)
NS_IMPL_RELEASE(nsMessengerFreeDesktopIntegration)

NS_INTERFACE_MAP_BEGIN(nsMessengerFreeDesktopIntegration)
   NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIMessengerOSIntegration)
   NS_INTERFACE_MAP_ENTRY(nsIMessengerOSIntegration)
   NS_INTERFACE_MAP_ENTRY(nsIFolderListener)
   NS_INTERFACE_MAP_ENTRY(nsIObserver)
   NS_INTERFACE_MAP_ENTRY(nsISupportsWeakReference)
NS_INTERFACE_MAP_END

nsresult
nsMessengerFreeDesktopIntegration::Init()
{
  nsresult rv;
  
  /*
     Support for Thunderbird's "Disable Extension" functionality.
     Even though we don't really need the chrome to function (for now),
     the user will (rightfully) expect that "Disable Extension" would
     disable the XPCOM component as well.

     The following code is ripped from chatzilla-service.js.
  */
  // Checking if we're disabled in the Chrome Registry.
  nsCOMPtr<nsIRDFService> rdfSvc = do_GetService("@mozilla.org/rdf/rdf-service;1", &rv);
  NS_ENSURE_SUCCESS(rv, rv);
  nsCOMPtr<nsIRDFDataSource> rdfDS;
  rv = rdfSvc->GetDataSource("rdf:chrome", getter_AddRefs(rdfDS));
  NS_ENSURE_SUCCESS(rv, rv);
  nsCOMPtr<nsIRDFResource> resSelf;
  rv = rdfSvc->GetResource(NS_LITERAL_CSTRING("urn:mozilla:package:traybiff"), getter_AddRefs(resSelf));
  NS_ENSURE_SUCCESS(rv, rv);
  nsCOMPtr<nsIRDFResource> resDisabled;
  rv = rdfSvc->GetResource(NS_LITERAL_CSTRING("http://www.mozilla.org/rdf/chrome#disabled"), getter_AddRefs(resDisabled));
  NS_ENSURE_SUCCESS(rv, rv);
  nsCOMPtr<nsIRDFNode> resNode;
  rv = rdfDS->GetTarget(resSelf, resDisabled, true, getter_AddRefs(resNode));
  NS_ENSURE_SUCCESS(rv, rv);
  if (resNode != NULL)
  {
     // The chrome for this extension is marked as disabled.
     return NS_ERROR_FAILURE;
  }

  // get pref service
  nsCOMPtr<nsIPrefService> prefService;
  prefService = do_GetService(NS_PREFSERVICE_CONTRACTID, &rv);
  NS_ENSURE_SUCCESS(rv,rv);
  
  // get mail.biff pref branch and watch it for interesting prefs changing
  prefService->GetBranch(nsnull, getter_AddRefs(mPrefBranch));
  nsCOMPtr<nsIPrefBranchInternal> rootBranchInternal = do_QueryInterface(mPrefBranch);
  // ... with weak reference (PR_TRUE), to avoid cleaning up at shutdown.
  rv = rootBranchInternal->AddObserver(PREF_BIFF_SHOW_ICON, this, PR_TRUE);
  NS_ENSURE_SUCCESS(rv,rv);

  // because we care about biff notifications
  nsCOMPtr <nsIMsgAccountManager> accountManager = do_GetService(NS_MSGACCOUNTMANAGER_CONTRACTID, &rv);
  NS_ENSURE_SUCCESS(rv,rv);
  rv = accountManager->AddRootFolderListener(this);
  NS_ENSURE_SUCCESS(rv,rv);

  ApplyPrefs();

  return NS_OK;
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::Observe(nsISupports *aSubject, const char *aTopic, const PRUnichar *aData)
{
  if (nsCRT::strcmp(aTopic, NS_PREFBRANCH_PREFCHANGE_TOPIC_ID) == 0)
  {
     nsCOMPtr<nsIPrefBranchInternal> pPrefBranch( do_QueryInterface(aSubject) );
     nsCAutoString prefName;
     prefName.AppendWithConversion(aData);
     
     if (prefName.Equals(PREF_BIFF_SHOW_ICON))
        ApplyPrefs();
  }
}

void nsMessengerFreeDesktopIntegration::ApplyPrefs()
{
	mPrefBranch->GetBoolPref(PREF_BIFF_SHOW_ICON, &mShowBiffIcon);
	if (!mShowBiffIcon)
		RemoveBiffIcon();
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemPropertyChanged
#ifdef MOZ_TRUNK
(nsIRDFResource *, nsIAtom *, char const *, char const *)
#else
(nsISupports*, nsIAtom*, const char*, const char*)
#endif
{
  return NS_OK;
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemUnicharPropertyChanged
#ifdef MOZ_TRUNK
(nsIRDFResource *, nsIAtom *, const PRUnichar *, const PRUnichar *)
#else
(nsISupports*, nsIAtom*, const PRUnichar*, const PRUnichar*)
#endif
{
  return NS_OK;
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemRemoved
#ifdef MOZ_TRUNK
(nsIRDFResource *, nsISupports *)
#else
(nsISupports*, nsISupports*, const char*)
#endif
{
  return NS_OK;
}

nsresult nsMessengerFreeDesktopIntegration::GetStringBundle(nsIStringBundle **aBundle)
{
  nsresult rv = NS_OK;
  NS_ENSURE_ARG_POINTER(aBundle);
  nsCOMPtr<nsIStringBundleService> bundleService = do_GetService(NS_STRINGBUNDLE_CONTRACTID, &rv);
  nsCOMPtr<nsIStringBundle> bundle;
  if (bundleService && NS_SUCCEEDED(rv))
    bundleService->CreateBundle("chrome://messenger/locale/messenger.properties", getter_AddRefs(bundle));
  NS_IF_ADDREF(*aBundle = bundle);
  return rv;
}

void nsMessengerFreeDesktopIntegration::FillToolTipInfo()
{
  // iterate over all the folders in mFoldersWithNewMail
  nsXPIDLString accountName;
  nsXPIDLCString hostName; 
  nsAutoString toolTipText;
  nsCOMPtr<nsISupports> supports;
  nsCOMPtr<nsIMsgFolder> folder;
  nsCOMPtr<nsIWeakReference> weakReference;
  PRInt32 numNewMessages = 0;
  PRInt32 numNewMessagesTotal = 0;

  PRUint32 count = 0;
  mFoldersWithNewMail->Count(&count);

  for (PRUint32 index = 0; index < count; index++)
  {
    supports = getter_AddRefs(mFoldersWithNewMail->ElementAt(index));
    weakReference = do_QueryInterface(supports);
    folder = do_QueryReferent(weakReference);
    if (folder)
    {
      folder->GetPrettiestName(getter_Copies(accountName));
      numNewMessages = 0;   
      folder->GetNumNewMessages(PR_TRUE, &numNewMessages);
      numNewMessagesTotal += numNewMessages;
    } // if we got a folder
  } // for each folder
 
  nsCOMPtr<nsIStringBundle> bundle; 
  GetStringBundle(getter_AddRefs(bundle));
  if (bundle)
  { 
    nsAutoString numNewMsgsText;     
    numNewMsgsText.AppendInt(numNewMessagesTotal);

    const PRUnichar *formatStrings[] =
    {
      numNewMsgsText.get()
    };
    
    nsXPIDLString text;
    if (numNewMessages == 1)
      bundle->FormatStringFromName(NS_LITERAL_STRING("biffNotification_message").get(), formatStrings, 2, getter_Copies(text));
    else
      bundle->FormatStringFromName(NS_LITERAL_STRING("biffNotification_messages").get(), formatStrings, 2, getter_Copies(text));
      
    toolTipText = accountName + NS_LITERAL_STRING(" ") + text;
  } // if we got a bundle

  SetToolTipString(toolTipText.get());
}

// get the first top level folder which we know has new mail, then enumerate over all the subfolders
// looking for the first real folder with new mail. Return the folderURI for that folder.
nsresult nsMessengerFreeDesktopIntegration::GetFirstFolderWithNewMail(char ** aFolderURI)
{
  nsresult rv;
  NS_ENSURE_TRUE(mFoldersWithNewMail, NS_ERROR_FAILURE); 

  nsCOMPtr<nsISupports> supports;
  nsCOMPtr<nsIMsgFolder> folder;
  nsCOMPtr<nsIWeakReference> weakReference;
  PRInt32 numNewMessages = 0;

  PRUint32 count = 0;
  mFoldersWithNewMail->Count(&count);

  if (!count)  // kick out if we don't have any folders with new mail
    return NS_OK;

  supports = getter_AddRefs(mFoldersWithNewMail->ElementAt(0));
  weakReference = do_QueryInterface(supports);
  folder = do_QueryReferent(weakReference);
  
  if (folder)
  {
    PRUint32 biffState = nsIMsgFolder::nsMsgBiffState_NoMail; 
    nsCOMPtr<nsIMsgFolder> msgFolder;
    // enumerate over the folders under this root folder till we find one with new mail....
    nsCOMPtr<nsISupportsArray> allFolders;
    NS_NewISupportsArray(getter_AddRefs(allFolders));
    rv = folder->ListDescendents(allFolders);
    NS_ENSURE_SUCCESS(rv, rv);

    nsCOMPtr<nsIEnumerator> enumerator;
    allFolders->Enumerate(getter_AddRefs(enumerator));
    if (enumerator)
    {
      nsCOMPtr<nsISupports> supports;
      nsresult more = enumerator->First();
      while (NS_SUCCEEDED(more))
      {
        rv = enumerator->CurrentItem(getter_AddRefs(supports));
        if (supports)
        {			
          msgFolder = do_QueryInterface(supports, &rv);
          if (msgFolder)
          {
            numNewMessages = 0;   
            msgFolder->GetNumNewMessages(PR_FALSE, &numNewMessages);
            if (numNewMessages)
              break; // kick out of the while loop
            more = enumerator->Next();
          }
        } // if we have a folder
      }  // if we have more potential folders to enumerate
    }  // if enumerator
    
    if (msgFolder)
      msgFolder->GetURI(aFolderURI);
  }

  return NS_OK;
}

void nsMessengerFreeDesktopIntegration::AddBiffIcon()
{
  if (mTrayIcon == NULL)
  {
     GdkPixbuf* pixbuf = gdk_pixbuf_new_from_inline(-1, tray_biff_icon, FALSE, NULL);
     // This should be ideally replaced by a completely libpr0n-based icon rendering.
     mTrayIcon = egg_status_icon_new_from_pixbuf(pixbuf);
     g_signal_connect(G_OBJECT(mTrayIcon), "activate", G_CALLBACK(TrayIconActivate), this);
     g_signal_connect(G_OBJECT(mTrayIcon), "popup-menu", G_CALLBACK(TrayIconPopupMenu), this);
     g_object_unref(G_OBJECT(pixbuf));
  }
}

void nsMessengerFreeDesktopIntegration::RemoveBiffIcon()
{
  if (mTrayIcon != NULL)
  {
     g_object_unref(G_OBJECT(mTrayIcon));
     mTrayIcon = NULL;
  }
}

void nsMessengerFreeDesktopIntegration::OnBiffIconActivate()
{
  nsXPIDLCString folderURI;
  GetFirstFolderWithNewMail(getter_Copies(folderURI));

  openMailWindow(NS_LITERAL_STRING("mail:3pane").get(), folderURI /* or nsnull in original code */);
}

void nsMessengerFreeDesktopIntegration::OnBiffIconPopupMenu(unsigned int button, unsigned int activateTime)
{
  // Create the menu first time it's needed.
  if (mTrayMenu == NULL)
  {
     mTrayMenu = gtk_menu_new();

     GtkWidget *entry;

     entry = gtk_menu_item_new_with_label("Read mail...");
     g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(TrayIconActivate), this);
     gtk_menu_shell_append(GTK_MENU_SHELL(mTrayMenu), entry);

     entry = gtk_menu_item_new_with_label("Hide");
     g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(TrayIconHide), this);
     gtk_menu_shell_append(GTK_MENU_SHELL(mTrayMenu), entry);
 
     gtk_widget_show_all(mTrayMenu);
  }

  gtk_menu_popup(GTK_MENU(mTrayMenu), NULL, NULL, NULL, NULL, button, activateTime);
}

void nsMessengerFreeDesktopIntegration::OnBiffIconHide()
{
  // Set to hide the biff icon.
  mPrefBranch->SetBoolPref(PREF_BIFF_SHOW_ICON, PR_FALSE);
}

/*
 Static functions which are GTK+ event handlers; minimal code to cast user-data
 into nsMessengerFreeDesktopIntegration and call the appropriate method.
 */
 
void nsMessengerFreeDesktopIntegration::TrayIconActivate(GtkWidget *trayIcon, void *data)
{
   reinterpret_cast<nsMessengerFreeDesktopIntegration*>(data)->OnBiffIconActivate();
}

void nsMessengerFreeDesktopIntegration::TrayIconPopupMenu(GtkWidget *trayIcon, guint button, guint activateTime, void *data)
{
   reinterpret_cast<nsMessengerFreeDesktopIntegration*>(data)->OnBiffIconPopupMenu(button, activateTime);
}

void nsMessengerFreeDesktopIntegration::TrayIconHide(GtkMenuItem* menuItem, void *data)
{
   reinterpret_cast<nsMessengerFreeDesktopIntegration*>(data)->OnBiffIconHide();
}

void nsMessengerFreeDesktopIntegration::SetToolTipString(const PRUnichar * aToolTipString)
{
  if ((aToolTipString == NULL) || 
      (mTrayIcon == NULL))
     return;

  NS_ConvertUCS2toUTF8 utf8TooltipString(aToolTipString);
  egg_status_icon_set_tooltip(mTrayIcon, static_cast<const gchar*>(utf8TooltipString.get()), NULL);
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemPropertyFlagChanged(nsISupports *item, nsIAtom *property, PRUint32 oldFlag, PRUint32 newFlag)
{
  nsresult rv = NS_OK;

  // if we got new mail show a icon in the system tray
  if (mBiffStateAtom == property && mFoldersWithNewMail)
  {
    nsCOMPtr<nsIMsgFolder> folder = do_QueryInterface(item);
    NS_ENSURE_TRUE(folder, NS_OK);

    if (newFlag == nsIMsgFolder::nsMsgBiffState_NewMail) 
    {
      // if the icon is not already visible, only show a system tray icon iff 
      // we are performing biff (as opposed to the user getting new mail)
      PRBool performingBiff = PR_FALSE;
      nsCOMPtr<nsIMsgIncomingServer> server;
      folder->GetServer(getter_AddRefs(server));
      if (server)
        server->GetPerformingBiff(&performingBiff);
      if (!performingBiff) 
        return NS_OK; // kick out right now...
      nsCOMPtr<nsIWeakReference> weakFolder = do_GetWeakReference(folder); 

      // remove the element if it is already in the array....
      PRUint32 count = 0;
      PRUint32 index = 0; 
      mFoldersWithNewMail->Count(&count);
      nsCOMPtr<nsISupports> supports;
      nsCOMPtr<nsIMsgFolder> oldFolder;
      nsCOMPtr<nsIWeakReference> weakReference;
      for (index = 0; index < count; index++)
      {
        supports = getter_AddRefs(mFoldersWithNewMail->ElementAt(index));
        weakReference = do_QueryInterface(supports);
        oldFolder = do_QueryReferent(weakReference);
        if (oldFolder == folder) // if they point to the same folder
          break;
        oldFolder = nsnull;
      }

      if (oldFolder)
        mFoldersWithNewMail->ReplaceElementAt(weakFolder, index);
      else
        mFoldersWithNewMail->AppendElement(weakFolder);
    
      if (mShowBiffIcon)
      {
        AddBiffIcon();
        FillToolTipInfo();    
      }
    }
    else if (newFlag == nsIMsgFolder::nsMsgBiffState_NoMail)
    {
      // we are always going to remove the icon whenever we get our first no mail
      // notification. 
      mFoldersWithNewMail->Clear(); 
      RemoveBiffIcon();
    }
  } // if the biff property changed
  
  return NS_OK;
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemAdded
#ifdef MOZ_TRUNK
(nsIRDFResource *, nsISupports *)
#else
(nsISupports*, nsISupports*, const char*)
#endif
{
  return NS_OK;
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemBoolPropertyChanged
#ifdef MOZ_TRUNK
(nsIRDFResource *aItem, nsIAtom *aProperty, PRBool aOldValue, PRBool aNewValue)
#else
(nsISupports*, nsIAtom*, int, int)
#endif
{
  return NS_OK;
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemIntPropertyChanged
#ifdef MOZ_TRUNK
(nsIRDFResource *aItem, nsIAtom *aProperty, int aOldValue, int aNewValue)
#else
(nsISupports*, nsIAtom*, int, int)
#endif
{
  return NS_OK;
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemEvent(nsIMsgFolder *, nsIAtom *)
{
  return NS_OK;
}
