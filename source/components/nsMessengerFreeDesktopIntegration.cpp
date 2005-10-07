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
 * Ilya Konstantinov
 * Portions were taken or adapted from various Mozilla source files.
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
#include <nsIMsgHdr.h>
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
#include <nsMsgUtils.h>
#include <prprf.h>
#include <nsIWeakReference.h>
#include <nsIStringBundle.h>
#include <nsIPrefService.h>
#include <nsIPrefBranchInternal.h>

#include <nsAppDirectoryServiceDefs.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
       
// One day, we should use an icon from chrome.
// Meanwhile, we'll embed the icon:
#include "trayBiffIcon.h"

// Path to the New Mail Led control files
const char* HW_INDICATOR_CONTROL_FILENAMES[] = {
	// ASUS laptop led on Linux
	"/proc/acpi/asus/mled",
	// ACER New Mail led on Linux
	"/proc/driver/acerhk/led"
};

// Prefs in use
const char* PREF_BIFF_SHOW_ICON = "mail.biff.show_icon";
const char* PREF_BIFF_SHOW_ASUS_LED = "mail.biff.show_asus_led";
const char* PREF_BIFF_USE_HW_INDICATOR = "mail.biff.use_hw_indicator";
const char* PREF_BIFF_HW_INDICATOR_FILE = "mail.biff.hw_indicator_file";
const char* PREF_BIFF_ALWAYS_SHOW_ICON = "mail.biff.always_show_icon";

// String bundles
const char* STRING_BUNDLE_MESSENGER = "chrome://messenger/locale/messenger.properties";
const char* STRING_BUNDLE_TRAYBIFF = "chrome://traybiff/locale/traybiff.properties";

// Useful for debugging.
inline void PrintAtom(nsIAtom* atom)
{
	const char *res;
	atom->GetUTF8String(&res);
	fprintf(stderr, "Atom = %s\n", res);
}

#include "eggstatusicon.h"
#include <gtk/gtk.h>

/**
 * Modifies aFile to point at an icon file with the given name and suffix.  The
 * suffix may correspond to a file extension with leading '.' if appropriate.
 * Returns true if the icon file exists and can be read.
 * @note Taken from nsBaseWidget.cpp
 */
static PRBool
ResolveIconNameHelper(nsILocalFile *aFile,
                      const nsAString &aIconName,
                      const nsAString &aIconSuffix)
{
	aFile->Append(NS_LITERAL_STRING("icons"));
	aFile->Append(NS_LITERAL_STRING("default"));
	aFile->Append(aIconName + aIconSuffix);

	PRBool readable;
	return NS_SUCCEEDED(aFile->IsReadable(&readable)) && readable;
}

/**
 * Resolve the given icon name into a local file object.  This method is
 * intended to be called by subclasses of nsBaseWidget.  aIconSuffix is a
 * platform specific icon file suffix (e.g., ".ico" under Win32).
 *
 * If no file is found matching the given parameters, then null is returned.
 * @note Taken from nsBaseWidget.cpp
 */
void ResolveIconName(const nsAString &aIconName, const nsAString &aIconSuffix, nsILocalFile **aResult)
{ 
  *aResult = nsnull;

  nsCOMPtr<nsIProperties> dirSvc = do_GetService(NS_DIRECTORY_SERVICE_CONTRACTID);
  if (!dirSvc)
    return;

  // first check auxilary chrome directories

  nsCOMPtr<nsISimpleEnumerator> dirs;
  dirSvc->Get(NS_APP_CHROME_DIR, NS_GET_IID(nsISimpleEnumerator), getter_AddRefs(dirs));
  if (dirs) {
    PRBool hasMore;
    while (NS_SUCCEEDED(dirs->HasMoreElements(&hasMore)) && hasMore) {
      nsCOMPtr<nsISupports> element;
      dirs->GetNext(getter_AddRefs(element));
      if (!element)
        continue;
      nsCOMPtr<nsILocalFile> file = do_QueryInterface(element);
      if (!file)
        continue;
      if (ResolveIconNameHelper(file, aIconName, aIconSuffix)) {
        NS_ADDREF(*aResult = file);
        return;
      }
    }
  }

  // then check the main app chrome directory

  nsCOMPtr<nsILocalFile> file;
  dirSvc->Get(NS_APP_CHROME_DIR, NS_GET_IID(nsILocalFile),
              getter_AddRefs(file));
  if (file && ResolveIconNameHelper(file, aIconName, aIconSuffix))
    NS_ADDREF(*aResult = file);
}

GdkWindow* GetGdkWindowForDOMWindow(nsISupports *window)
{
	nsCOMPtr<nsIScriptGlobalObject> ppScriptGlobalObj( do_QueryInterface(window) );
	if ( !ppScriptGlobalObj )
		return 0;
  
	nsCOMPtr<nsIBaseWindow> ppBaseWindow = do_QueryInterface( ppScriptGlobalObj->GetDocShell() );
	if (!ppBaseWindow)
		return 0;

	nsCOMPtr<nsIWidget> ppWidget;
	ppBaseWindow->GetMainWidget( getter_AddRefs( ppWidget ) );
 
	return reinterpret_cast<GdkWindow*>( ppWidget->GetNativeData( NS_NATIVE_WIDGET ) );
}

//! @brief Opens a (preferrably existing) window, optionally going to a certain message.
//! @note Refer to mail/base/content/mailCore.js : toMessengerWindow() to see
//! how we raise the mailer window.
//! @param[in] aMailWindowName Name of the mailer window. Usually "mail:3pane".
//! @param[in] aFolderUri Optional parameter specifying the URI of a folder to switch the mail view to. 
//! @param[in] aMessageUri Optional parameter specifying the URI of a folder to switch the mail view to. 
static void openMailWindow(const PRUnichar * aMailWindowName, const char * aFolderUri, const char * aMessageUri)
{
	nsCOMPtr<nsIDOMWindowInternal> domWindow;
	
	nsCOMPtr<nsIWindowMediator> mediator ( do_GetService(NS_WINDOWMEDIATOR_CONTRACTID) );
	NS_ASSERTION(mediator, "no mediator");
	if (aMailWindowName != NULL)
	{
		mediator->GetMostRecentWindow(aMailWindowName, getter_AddRefs(domWindow));
		
		if (domWindow)
		{
			// Jump to the desired folder/message
			nsCOMPtr<nsISupports> xpConnectObj;
			nsCOMPtr<nsPIDOMWindow> piDOMWindow(do_QueryInterface(domWindow));
			if (piDOMWindow)
			{
				if ((aFolderUri != NULL) && (aMessageUri != NULL))
				{
					piDOMWindow->GetObjectProperty(NS_LITERAL_STRING("MsgWindowCommands").get(), getter_AddRefs(xpConnectObj));
					nsCOMPtr<nsIMsgWindowCommands> msgWindowCommands = do_QueryInterface(xpConnectObj);
					if (msgWindowCommands)
					{
						if (aFolderUri != NULL)
						{
							msgWindowCommands->SelectFolder(aFolderUri);
							if (aMessageUri != NULL)
								msgWindowCommands->SelectMessage(aMessageUri);
						}
					}
				}
			}

			// window.focus() on X11 brings the window up but doesn't focus it,
			// so we do some GDK magic:
			GdkWindow* pGdkWindow = GetGdkWindowForDOMWindow(domWindow);
			if (GDK_IS_WINDOW(pGdkWindow))
			{
				GdkWindow * root = gdk_window_get_toplevel(pGdkWindow);
				gdk_window_show(root);
				gdk_window_raise(root);
			}
			domWindow->Focus();
		}
	}
	
	if (!domWindow)
	{
		nsMsgKey msgKey = nsMsgKey_None;
		// If a specific message is needed, go through the hoops required to translate
		// a message URL into a message key.
		if (aMessageUri != NULL)
		{
			nsCOMPtr<nsIMsgDBHdr> hdr;
			GetMsgDBHdrFromURI(aMessageUri, getter_AddRefs(hdr));
			if (hdr)
				hdr->GetMessageKey(&msgKey);
		}
		
		nsCOMPtr <nsIMessengerWindowService> messengerWindowService = do_GetService(NS_MESSENGERWINDOWSERVICE_CONTRACTID);
		if (messengerWindowService) 
			messengerWindowService->OpenMessengerWindowWithUri("mail:3pane", aFolderUri, msgKey);
	}
}

static void toggleMailWindow(const PRUnichar * aMailWindowName)
{
	nsCOMPtr<nsIDOMWindowInternal> domWindow;
	
	nsCOMPtr<nsIWindowMediator> mediator ( do_GetService(NS_WINDOWMEDIATOR_CONTRACTID) );
	NS_ASSERTION(mediator, "no mediator");
	if (aMailWindowName != NULL)
	{
		mediator->GetMostRecentWindow(aMailWindowName, getter_AddRefs(domWindow));
		if (domWindow)
		{
			// window.focus() on X11 brings the window up but doesn't focus it,
			// so we do some GDK magic:
			GdkWindow* pGdkWindow = GetGdkWindowForDOMWindow(domWindow);
			if (GDK_IS_WINDOW(pGdkWindow))
			{
				GdkWindow * root = gdk_window_get_toplevel(pGdkWindow);
				if(gdk_window_is_visible(root))
				{
					gdk_window_hide(root);
				}
				else
				{
					gdk_window_show(root);
					gdk_window_raise(root);
					domWindow->Focus();
				}
			}
		}
	}
	
	if (!domWindow)
	{
		nsCOMPtr <nsIMessengerWindowService> messengerWindowService = do_GetService(NS_MESSENGERWINDOWSERVICE_CONTRACTID);
		if (messengerWindowService) 
			messengerWindowService->OpenMessengerWindowWithUri("mail:3pane", NULL, nsMsgKey_None);
	}
}

nsMessengerFreeDesktopIntegration::nsMessengerFreeDesktopIntegration() :
	mTrayIcon(NULL),
	mTrayMenu(NULL),
	mShowBiffIcon(PR_TRUE),
	mHasBiff(PR_FALSE)
{
	mBiffStateAtom = do_GetAtom("BiffState");
	NS_NewISupportsArray(getter_AddRefs(mFoldersWithNewMail));

	nsCOMPtr<nsILocalFile> iconFile;
	ResolveIconName(NS_LITERAL_STRING("messengerWindow16"), NS_LITERAL_STRING(".xpm"), getter_AddRefs(iconFile));
	if (iconFile)
	{
		iconFile->GetNativePath(mBrandIconPath);
	}
}

nsMessengerFreeDesktopIntegration::~nsMessengerFreeDesktopIntegration()
{
	RemoveBiffIcon();
	HideHwIndicator();

	if (mTrayMenu != NULL)
		gtk_widget_destroy(mTrayMenu);
}

NS_IMPL_ADDREF(nsMessengerFreeDesktopIntegration)
NS_IMPL_RELEASE(nsMessengerFreeDesktopIntegration)

NS_INTERFACE_MAP_BEGIN(nsMessengerFreeDesktopIntegration)
	NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIMessengerOSIntegration)
	NS_INTERFACE_MAP_ENTRY(nsIMessengerOSIntegration)
	NS_INTERFACE_MAP_ENTRY(nsIMessengerFreeDesktopIntegration)
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
	rv = rootBranchInternal->AddObserver(PREF_BIFF_SHOW_ASUS_LED, this, PR_TRUE);
	rv = rootBranchInternal->AddObserver(PREF_BIFF_USE_HW_INDICATOR, this, PR_TRUE);
	rv = rootBranchInternal->AddObserver(PREF_BIFF_HW_INDICATOR_FILE, this, PR_TRUE);
	rv = rootBranchInternal->AddObserver(PREF_BIFF_ALWAYS_SHOW_ICON, this, PR_TRUE);
	NS_ENSURE_SUCCESS(rv,rv);

	// because we care about biff notifications
	nsCOMPtr <nsIMsgMailSession> mailSession = do_GetService(NS_MSGMAILSESSION_CONTRACTID, &rv);
	NS_ENSURE_SUCCESS(rv,rv);
	rv = mailSession->AddFolderListener(this, nsIFolderListener::propertyFlagChanged);
	NS_ENSURE_SUCCESS(rv,rv);
	// In the future, we might want to add more properties here, to enable us to display
	// the unread message count etc.
	// For reference, see:
	// - nsMessengerWinIntegration.cpp
	// - nsStatusBarBiffManager.cpp

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
		
		if (prefName.Equals(PREF_BIFF_SHOW_ICON) ||
		    prefName.Equals(PREF_BIFF_USE_HW_INDICATOR) ||
		    prefName.Equals(PREF_BIFF_HW_INDICATOR_FILE) ||
		    // for backward compatibility:
		    prefName.Equals(PREF_BIFF_SHOW_ASUS_LED) ||
		    prefName.Equals(PREF_BIFF_ALWAYS_SHOW_ICON))
		{
			ApplyPrefs();
		}
	}
	return NS_OK;
}

void nsMessengerFreeDesktopIntegration::ApplyPrefs()
{
	mPrefBranch->GetBoolPref(PREF_BIFF_ALWAYS_SHOW_ICON, &mAlwaysShowBiffIcon);
	mPrefBranch->GetBoolPref(PREF_BIFF_SHOW_ICON, &mShowBiffIcon);
	mPrefBranch->GetBoolPref(PREF_BIFF_SHOW_ASUS_LED, &mUseHwIndicator);
	mPrefBranch->GetBoolPref(PREF_BIFF_USE_HW_INDICATOR, &mUseHwIndicator);
	if (mUseHwIndicator)
	{
		nsXPIDLCString newHwIndicatorFile;
		mPrefBranch->GetCharPref(PREF_BIFF_HW_INDICATOR_FILE, getter_Copies(newHwIndicatorFile));
		if (newHwIndicatorFile != mHwIndicatorFile)
		{
			HideHwIndicator();
			if (newHwIndicatorFile.IsEmpty())
			{
				mHwIndicatorFile = EmptyCString();
				// Autodetect
				for (unsigned int i=0;
				     i<sizeof(HW_INDICATOR_CONTROL_FILENAMES)/sizeof(HW_INDICATOR_CONTROL_FILENAMES[0]);
				     ++i)
				{
					const char* filename = HW_INDICATOR_CONTROL_FILENAMES[i];
					if ((access(filename, R_OK | W_OK) == 0) ||
					    ((errno != ENOENT) && (errno != ENOTDIR)))
					{
						mHwIndicatorFile = filename;
						break;
					}
				}
			}
		}
		else
		{
			mHwIndicatorFile = newHwIndicatorFile;
		}
		if (mHasBiff)
		{
			ShowHwIndicator();
		}
	}
	else
	{
		HideHwIndicator();
	}
	if (mAlwaysShowBiffIcon)
	{
		mShowBiffIcon = true; // force to true
		AddBiffIcon();
		ClearToolTip();
	}
	else if (!mShowBiffIcon || !mHasBiff)
	{
		RemoveBiffIcon();
	}
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

nsresult nsMessengerFreeDesktopIntegration::GetStringBundle(const char* src, nsIStringBundle **aBundle)
{
  nsresult rv = NS_OK;
  NS_ENSURE_ARG_POINTER(aBundle);
  nsCOMPtr<nsIStringBundleService> bundleService = do_GetService(NS_STRINGBUNDLE_CONTRACTID, &rv);
  nsCOMPtr<nsIStringBundle> bundle;
  if (bundleService && NS_SUCCEEDED(rv))
    bundleService->CreateBundle(src, getter_AddRefs(bundle));
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
  GetStringBundle(STRING_BUNDLE_MESSENGER, getter_AddRefs(bundle));
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
nsresult nsMessengerFreeDesktopIntegration::GetFirstFolderWithNewMail(char ** aFolderURI, char ** aMessageURI)
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
    nsString msgURI;
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
            if (numNewMessages > 0)
            {
              nsCOMPtr<nsIMsgDBHdr> msgHeader;
              msgFolder->GetFirstNewMessage(getter_AddRefs(msgHeader));
              msgFolder->GetUriForMsg(msgHeader, aMessageURI);
              break; // kick out of the while loop
            }
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
		// This should be ideally replaced by a completely libpr0n-based icon rendering.
		GError* err = NULL;
		GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(mBrandIconPath.get(), &err);
		if (pixbuf != NULL)
		{
			mTrayIcon = egg_status_icon_new_from_pixbuf(pixbuf);
			if (mTrayIcon != NULL)
			{
				g_signal_connect(G_OBJECT(mTrayIcon), "activate", G_CALLBACK(TrayIconActivate), this);
				g_signal_connect(G_OBJECT(mTrayIcon), "popup-menu", G_CALLBACK(TrayIconPopupMenu), this);
			}
			g_object_unref(G_OBJECT(pixbuf));
		}
		else
		{
			fprintf (stderr, "%s\n", err->message);
		}
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

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::GetMailHwIndicatorStatus(PRInt16 *aHwIndicatorStatus)
{
	if (access(mHwIndicatorFile, R_OK | W_OK) == 0)
	{
		*aHwIndicatorStatus = MAIL_HW_INDICATOR_OK;
	}
	else
	{
		switch(errno)
		{
			case ENOENT:
			case ENOTDIR:
				*aHwIndicatorStatus = MAIL_HW_INDICATOR_MISSING;
				break;
			default:
				*aHwIndicatorStatus = MAIL_HW_INDICATOR_INACCESSIBLE;
				break;
		}
	}
	
	return NS_OK;
} 

void nsMessengerFreeDesktopIntegration::ShowHwIndicator()
{
	if (!mHwIndicatorFile.IsEmpty())
	{
		int fd = open(mHwIndicatorFile, O_WRONLY);
		if (fd != -1)
		{
			write(fd, "1", 1);
			close(fd);
		}
	}
}

void nsMessengerFreeDesktopIntegration::HideHwIndicator()
{
	if (!mHwIndicatorFile.IsEmpty())
	{
		int fd = open(mHwIndicatorFile, O_WRONLY);
		if (fd != -1)
		{
			write(fd, "0", 1);
			close(fd);
		}
	}
}

void nsMessengerFreeDesktopIntegration::OnBiffIconActivate()
{
	if (mHasBiff)
	{
		nsXPIDLCString folderURI, messageURI;
		GetFirstFolderWithNewMail(getter_Copies(folderURI), getter_Copies(messageURI));
		openMailWindow(NS_LITERAL_STRING("mail:3pane").get(), folderURI, messageURI);
	}
	else
	{
		toggleMailWindow(NS_LITERAL_STRING("mail:3pane").get());
	}
}

void nsMessengerFreeDesktopIntegration::OnBiffChange()
{
	if (mHasBiff)
	{
		if (mShowBiffIcon)
		{
			AddBiffIcon();
			FillToolTipInfo();
			GdkPixbuf* pixbuf = gdk_pixbuf_new_from_inline(-1, tray_biff_icon, FALSE, NULL);
			egg_status_icon_set_from_pixbuf(mTrayIcon, pixbuf);
			g_object_unref(G_OBJECT(pixbuf));
		}
	      
		if (mUseHwIndicator)
		{
			ShowHwIndicator();
		}
	}
	else
	{
		if (mAlwaysShowBiffIcon)
		{
			ClearToolTip();
			GError* err = NULL;
			GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(mBrandIconPath.get(), &err);
			if (pixbuf != NULL)
			{
				egg_status_icon_set_from_pixbuf(mTrayIcon, pixbuf);
				g_object_unref(G_OBJECT(pixbuf));
			}
			else
			{
				fprintf (stderr, "%s\n", err->message);
			}
		}
		else
		{
			RemoveBiffIcon();
		}
		HideHwIndicator();
	}
}

void nsMessengerFreeDesktopIntegration::OnBiffIconPopupMenu(unsigned int button, unsigned int activateTime)
{
  // Create the menu first time it's needed.
  if (mTrayMenu == NULL)
  {
     mTrayMenu = gtk_menu_new();
     GtkWidget *entry;

     // Default values for menu item strings
     nsXPIDLString menuItemReadMail;
     menuItemReadMail.Assign(NS_LITERAL_STRING("Read mail..."));
     nsXPIDLString menuItemHide;
     menuItemHide.Assign(NS_LITERAL_STRING("Hide"));

     // Retrieve menu item localizations from the string bundle, if possible
     nsCOMPtr<nsIStringBundle> bundle; 
     GetStringBundle(STRING_BUNDLE_TRAYBIFF, getter_AddRefs(bundle));
     if (bundle)
     { 
       bundle->GetStringFromName(NS_LITERAL_STRING("trayMenu_ReadMail").get(), getter_Copies(menuItemReadMail));
       bundle->GetStringFromName(NS_LITERAL_STRING("trayMenu_Hide").get(), getter_Copies(menuItemHide));
     }

     entry = gtk_menu_item_new_with_label(NS_ConvertUTF16toUTF8(menuItemReadMail).get());
     g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(TrayIconActivate), this);
     gtk_menu_shell_append(GTK_MENU_SHELL(mTrayMenu), entry);

     gtk_widget_show_all(mTrayMenu);
  }

  gtk_menu_popup(GTK_MENU(mTrayMenu), NULL, NULL, NULL, NULL, button, activateTime);
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

void nsMessengerFreeDesktopIntegration::SetToolTipString(const PRUnichar * aToolTipString)
{
  if ((aToolTipString == NULL) || 
      (mTrayIcon == NULL))
     return;

  NS_ConvertUCS2toUTF8 utf8TooltipString(aToolTipString);
  egg_status_icon_set_tooltip(mTrayIcon, static_cast<const gchar*>(utf8TooltipString.get()), NULL);
}

void nsMessengerFreeDesktopIntegration::ClearToolTip()
{
	if (mTrayIcon == NULL)
	{
		return;
	}
	
	// This hardcoded "Mozilla" should be changed once we move
	// to 1.8 branch and have nsIXULAppInfo.
	egg_status_icon_set_tooltip(mTrayIcon, "Mozilla", NULL);
}

#ifndef MOZ_TRUNK
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

			// Suggests the icon's default action (which is performed by OnBiffIconActivate)
			mHasBiff = true;
			OnBiffChange();
		}
		else if (newFlag == nsIMsgFolder::nsMsgBiffState_NoMail)
		{
			mFoldersWithNewMail->Clear();
			// Suggests the icon's default action (which is performed by OnBiffIconActivate)
			mHasBiff = false;
			OnBiffChange();
		}
	} // if the biff property changed
  
	return NS_OK;
}
#else
NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemPropertyFlagChanged(nsIMsgDBHdr *item, nsIAtom *property, PRUint32 oldFlag, PRUint32 newFlag)
{
	return NS_OK;
}
#endif

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
	#ifdef MOZ_TRUNK
	// if we got new mail show a icon in the system tray
	if (mBiffStateAtom == aProperty && mFoldersWithNewMail)
	{
		nsCOMPtr<nsIMsgFolder> folder = do_QueryInterface(aItem);
		NS_ENSURE_TRUE(folder, NS_OK);

		if (aNewValue == nsIMsgFolder::nsMsgBiffState_NewMail) 
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

			// Suggests the icon's default action (which is performed by OnBiffIconActivate)
			mHasBiff = true;
			OnBiffChange();
		}
		else if (aNewValue == nsIMsgFolder::nsMsgBiffState_NoMail)
		{
			mFoldersWithNewMail->Clear();
			// Suggests the icon's default action (which is performed by OnBiffIconActivate)
			mHasBiff = false;
			OnBiffChange();
		}
	} // if the biff property changed
	#endif
	
	return NS_OK;
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemEvent(nsIMsgFolder *, nsIAtom *)
{
  return NS_OK;
}
