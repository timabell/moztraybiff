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
#include <nsIMsgMailSession.h> // Compatible with tb1.0
#include <nsIMsgIncomingServer.h>
#include "nsIMsgIncomingServer10.h" // For compatibility with tb1.0
#include <nsIMsgAccount.h> // Compatible with tb1.0
#include <nsIRDFResource.h> // Compatible with tb1.0
#include <nsIMsgFolder.h>
#include "nsIMsgFolder10.h" // For compatibility with tb1.0
#include <nsIMsgHdr.h> // nsMsgDBHdr compatible with tb1.0
#include <nsMsgBaseCID.h>
#include <nsMsgFolderFlags.h>
#include <nsIProfile.h>
#include <nsIRDFService.h> // Compatible with tb1.0
#include <nsIPrefBranchInternal.h> // Only for NS_PREFBRANCH_PREFCHANGE_TOPIC_ID

#include <nsIWindowMediator.h> // Compatible with tb1.0
#include <nsIDOMWindowInternal.h> // Compatible with tb1.0
#include <nsPIDOMWindow.h>
#ifndef MOZ_TRUNK_FINAL
#include <nsIScriptGlobalObject.h>
#endif
#include "nsIScriptGlobalObject10.h"
#include <nsIDocShell.h>
#include "nsIDocShell10.h"
#include <nsIBaseWindow.h> // Compatible with tb1.0
#include <nsIWidget.h>
#include "nsIWidget10.h"

#include <nsIMessengerWindowService.h> // Compatible with 1.0
#include <nsMsgUtils.h>
#include <prprf.h>
#include <nsIWeakReference.h>
#include <nsIStringBundle.h>
#include <nsIPrefService.h>
#include <nsIPref.h>
#include <nsStringAPI.h>

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
const char* PREF_BIFF_USE_KEYBOARD_LED = "mail.biff.use_keyboard_led";

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
	nsCOMPtr<nsIBaseWindow> ppBaseWindow;
	
#ifndef MOZ_TRUNK_FINAL
	nsCOMPtr<nsIScriptGlobalObject> ppScriptGlobalObj = do_QueryInterface(window);
	if (ppScriptGlobalObj)
	{
		ppBaseWindow = do_QueryInterface( ppScriptGlobalObj->GetDocShell() );
	}
	else
	{
		nsCOMPtr<nsIScriptGlobalObject10> ppScriptGlobalObj10 = do_QueryInterface(window);
		if (ppScriptGlobalObj10)
		ppBaseWindow = do_QueryInterface( ppScriptGlobalObj->GetDocShell() );
	}
#else
	nsCOMPtr<nsPIDOMWindow> win( do_QueryInterface(window) );
	if (win)
		ppBaseWindow = do_QueryInterface( win->GetDocShell() );
#endif
  
	if (!ppBaseWindow)
		return NULL;

	GdkWindow* pWindow = NULL;
	nsCOMPtr<nsIWidget> pWidgetBase;
	ppBaseWindow->GetMainWidget( getter_AddRefs(pWidgetBase) );
	if (pWidgetBase)
	{
		nsCOMPtr<nsIWidget> pWidget;
		nsCOMPtr<nsIWidget10> pWidget10;

		if (pWidget = do_QueryInterface(pWidgetBase))
		{
			pWindow = reinterpret_cast<GdkWindow*>(pWidget->GetNativeData(NS_NATIVE_WIDGET));
		}
		else if (pWidget10 = do_QueryInterface(pWidgetBase))
		{
			pWindow = reinterpret_cast<GdkWindow*>(pWidget10->GetNativeData(NS_NATIVE_WIDGET));
		}
	}
 
	return pWindow;
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
			nsCOMPtr<nsPIDOMWindow> piDOMWindow(do_QueryInterface(domWindow));
			if (piDOMWindow)
			{
				if ((aFolderUri != NULL) && (aMessageUri != NULL))
				{
					nsCOMPtr<nsISupports> xpConnectObj;
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

enum MailWindowVisibilityEnum
{
	MAIL_WINDOW_NOT_FOUND,
	MAIL_WINDOW_HIDDEN,
	MAIL_WINDOW_VISIBLE
};

static MailWindowVisibilityEnum isMailWindowVisible(const PRUnichar * aMailWindowName)
{
	nsCOMPtr<nsIDOMWindowInternal> domWindow;
	
	nsCOMPtr<nsIWindowMediator> mediator ( do_GetService(NS_WINDOWMEDIATOR_CONTRACTID) );
	NS_ASSERTION(mediator, "no mediator");
	if (aMailWindowName != NULL)
	{
		mediator->GetMostRecentWindow(aMailWindowName, getter_AddRefs(domWindow));
		if (domWindow)
		{
			GdkWindow* pGdkWindow = GetGdkWindowForDOMWindow(domWindow);
			if (GDK_IS_WINDOW(pGdkWindow))
			{
				GdkWindow * root = gdk_window_get_toplevel(pGdkWindow);
				return gdk_window_is_visible(root) ? MAIL_WINDOW_VISIBLE : MAIL_WINDOW_HIDDEN;
			}
		}
	}
	return MAIL_WINDOW_NOT_FOUND;
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
	mShowBiffIcon(PR_TRUE),
	mHasBiff(PR_FALSE)
{
	mBiffStateAtom = do_GetAtom("BiffState");
	NS_NewISupportsArray(getter_AddRefs(mFoldersWithNewMail));

	nsCOMPtr<nsILocalFile> iconFile;
	ResolveIconName(NS_LITERAL_STRING("messengerWindow16"), NS_LITERAL_STRING(".png"), getter_AddRefs(iconFile));
	if (!iconFile)
		ResolveIconName(NS_LITERAL_STRING("messengerWindow16"), NS_LITERAL_STRING(".xpm"), getter_AddRefs(iconFile));
	if (!iconFile)
	{
		ResolveIconName(NS_LITERAL_STRING("mozicon16"), NS_LITERAL_STRING(".png"), getter_AddRefs(iconFile));
		if (!iconFile)
			ResolveIconName(NS_LITERAL_STRING("mozicon16"), NS_LITERAL_STRING(".xpm"), getter_AddRefs(iconFile));
	}
	if (iconFile)
	{
		iconFile->GetNativePath(mBrandIconPath);
	}
}

nsMessengerFreeDesktopIntegration::~nsMessengerFreeDesktopIntegration()
{
	RemoveBiffIcon();
	HideHwIndicator();
}

NS_IMPL_ADDREF(nsMessengerFreeDesktopIntegration)
NS_IMPL_RELEASE(nsMessengerFreeDesktopIntegration)

NS_INTERFACE_MAP_BEGIN(nsMessengerFreeDesktopIntegration)
	NS_INTERFACE_MAP_ENTRY_AMBIGUOUS(nsISupports, nsIMessengerOSIntegration)
	NS_INTERFACE_MAP_ENTRY(nsIMessengerOSIntegration)
	NS_INTERFACE_MAP_ENTRY(nsIMessengerFreeDesktopIntegration)
	NS_INTERFACE_MAP_ENTRY(nsIFolderListener)
	NS_INTERFACE_MAP_ENTRY(nsIFolderListener10)
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
	// Trunk builds no longer have rdf:chrome, and in addition, they avoid loading the component
	// when the extension is disabled so this check is no longer needed.
	{
		nsCOMPtr<nsIRDFService> rdfSvc = do_GetService("@mozilla.org/rdf/rdf-service;1", &rv);
		if (NS_SUCCEEDED(rv))
		{
			nsCOMPtr<nsIRDFDataSource> rdfDS;
			rv = rdfSvc->GetDataSource("rdf:chrome", getter_AddRefs(rdfDS));
			if (NS_SUCCEEDED(rv))
			{
				nsCOMPtr<nsIRDFResource> resSelf;
				rv = rdfSvc->GetResource(NS_LITERAL_CSTRING("urn:mozilla:package:traybiff"), getter_AddRefs(resSelf));
				if (NS_SUCCEEDED(rv))
				{
					nsCOMPtr<nsIRDFResource> resDisabled;
					rv = rdfSvc->GetResource(NS_LITERAL_CSTRING("http://www.mozilla.org/rdf/chrome#disabled"), getter_AddRefs(resDisabled));
					if (NS_SUCCEEDED(rv))
					{
						nsCOMPtr<nsIRDFNode> resNode;
						rv = rdfDS->GetTarget(resSelf, resDisabled, true, getter_AddRefs(resNode));
						if (NS_SUCCEEDED(rv) && (resNode != NULL))
						{
							// The chrome for this extension is marked as disabled.
							return NS_ERROR_FAILURE;
						}
					}
				}
			}
		}
	}

	// get pref service
	nsCOMPtr<nsIPrefService> prefService;
	prefService = do_GetService(NS_PREFSERVICE_CONTRACTID, &rv);
	NS_ENSURE_SUCCESS(rv,rv);
	
	// get mail.biff pref branch and watch it for interesting prefs changing
	prefService->GetBranch(nsnull, getter_AddRefs(mPrefBranch));
	nsCOMPtr<nsIPref> pref = do_QueryInterface(mPrefBranch);
	// ... with weak reference (PR_TRUE), to avoid cleaning up at shutdown.
	if (pref)
	{
		rv = pref->AddObserver(PREF_BIFF_SHOW_ICON, this, PR_TRUE);
		NS_ENSURE_SUCCESS(rv,rv);
		rv = pref->AddObserver(PREF_BIFF_SHOW_ASUS_LED, this, PR_TRUE);
		NS_ENSURE_SUCCESS(rv,rv);
		rv = pref->AddObserver(PREF_BIFF_USE_HW_INDICATOR, this, PR_TRUE);
		NS_ENSURE_SUCCESS(rv,rv);
		rv = pref->AddObserver(PREF_BIFF_HW_INDICATOR_FILE, this, PR_TRUE);
		NS_ENSURE_SUCCESS(rv,rv);
		rv = pref->AddObserver(PREF_BIFF_ALWAYS_SHOW_ICON, this, PR_TRUE);
		NS_ENSURE_SUCCESS(rv,rv);
		rv = pref->AddObserver(PREF_BIFF_USE_KEYBOARD_LED, this, PR_TRUE);
		NS_ENSURE_SUCCESS(rv,rv);
	}
	else
	{
		nsCOMPtr<nsIPrefBranchInternal> prefInternal = do_QueryInterface(mPrefBranch);
		NS_ENSURE_TRUE(prefInternal, NS_OK);
		rv = prefInternal->AddObserver(PREF_BIFF_SHOW_ICON, this, PR_TRUE);
		NS_ENSURE_SUCCESS(rv,rv);
		rv = prefInternal->AddObserver(PREF_BIFF_SHOW_ASUS_LED, this, PR_TRUE);
		NS_ENSURE_SUCCESS(rv,rv);
		rv = prefInternal->AddObserver(PREF_BIFF_USE_HW_INDICATOR, this, PR_TRUE);
		NS_ENSURE_SUCCESS(rv,rv);
		rv = prefInternal->AddObserver(PREF_BIFF_HW_INDICATOR_FILE, this, PR_TRUE);
		NS_ENSURE_SUCCESS(rv,rv);
		rv = prefInternal->AddObserver(PREF_BIFF_ALWAYS_SHOW_ICON, this, PR_TRUE);
		NS_ENSURE_SUCCESS(rv,rv);
		rv = prefInternal->AddObserver(PREF_BIFF_USE_KEYBOARD_LED, this, PR_TRUE);
		NS_ENSURE_SUCCESS(rv,rv);
	}

	// because we care about biff notifications
	nsCOMPtr <nsIMsgMailSession> mailSession = do_GetService(NS_MSGMAILSESSION_CONTRACTID, &rv);
	NS_ENSURE_SUCCESS(rv,rv);
	rv = mailSession->AddFolderListener(this, nsIFolderListener::propertyFlagChanged | nsIFolderListener::boolPropertyChanged | nsIFolderListener::intPropertyChanged | nsIFolderListener10::propertyFlagChanged | nsIFolderListener10::boolPropertyChanged | nsIFolderListener10::intPropertyChanged);
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
		nsCAutoString prefName;
		prefName.AppendWithConversion(aData);
		
		if (prefName.Equals(PREF_BIFF_SHOW_ICON) ||
		    prefName.Equals(PREF_BIFF_ALWAYS_SHOW_ICON) ||
		    prefName.Equals(PREF_BIFF_USE_HW_INDICATOR) ||
		    prefName.Equals(PREF_BIFF_HW_INDICATOR_FILE) ||
		    prefName.Equals(PREF_BIFF_USE_KEYBOARD_LED) ||
		    // for backward compatibility:
		    prefName.Equals(PREF_BIFF_SHOW_ASUS_LED))
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
	mPrefBranch->GetIntPref(PREF_BIFF_USE_KEYBOARD_LED, &mUseKeyboardLed);
	
	// Try to autodetect the hw indicator filename.
	// This still doesn't mean we'll use it; depends on mUseHwIndicator.
	nsXPIDLCString newHwIndicatorFile;
	mPrefBranch->GetCharPref(PREF_BIFF_HW_INDICATOR_FILE, getter_Copies(newHwIndicatorFile));
	if ((newHwIndicatorFile != mHwIndicatorFile) || mHwIndicatorFile.IsEmpty())
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
		else
		{
			mHwIndicatorFile = newHwIndicatorFile;
		}
	}

	if (mUseHwIndicator && mHasBiff)
	{
		ShowHwIndicator();
	}
	else
	{
		HideHwIndicator();
	}
	
	if (mUseKeyboardLed != 0)
	{
		XKeyboardControl kbControl;
		kbControl.led = mUseKeyboardLed;
		kbControl.led_mode = mHasBiff ? LedModeOn : LedModeOff;
		XChangeKeyboardControl(GDK_DISPLAY(), KBLed | KBLedMode, &kbControl);
	}
	
	if (mAlwaysShowBiffIcon)
	{
		mShowBiffIcon = true; // force to true
		AddBiffIcon();
		OnBiffChange();
		ClearToolTip();
	}
	else if (!mShowBiffIcon || !mHasBiff)
	{
		RemoveBiffIcon();
	}
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemPropertyChanged(nsIRDFResource *, nsIAtom *, char const *, char const *)
{
  return NS_OK;
}

// For compatibility with tb1.0
NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemPropertyChanged(nsISupports*, nsIAtom*, const char*, const char*)
{
  return NS_OK;
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemUnicharPropertyChanged(nsIRDFResource *, nsIAtom *, const PRUnichar *, const PRUnichar *)
{
  return NS_OK;
}

// For compatibility with tb1.0
NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemUnicharPropertyChanged(nsISupports*, nsIAtom*, const PRUnichar*, const PRUnichar*)
{
  return NS_OK;
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemRemoved(nsIRDFResource *, nsISupports *)
{
  return NS_OK;
}

// For compatibility with tb1.0
NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemRemoved(nsISupports*, nsISupports*, const char*)
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
	PRInt32 numNewMessages = 0;
	PRInt32 numNewMessagesTotal = 0;

	PRUint32 count = 0;
	mFoldersWithNewMail->Count(&count);

	for (PRUint32 index = 0; index < count; index++)
	{
		nsCOMPtr<nsISupports> supports;
		supports = getter_AddRefs(mFoldersWithNewMail->ElementAt(index));
		nsCOMPtr<nsIWeakReference> weakReference;
		weakReference = do_QueryInterface(supports);
		nsCOMPtr<nsIMsgFolder> folder;
		nsCOMPtr<nsIMsgFolder10> folder10;
		if (folder = do_QueryReferent(weakReference))
		{
			folder->GetPrettiestName(getter_Copies(accountName));
			numNewMessages = 0;   
			folder->GetNumNewMessages(PR_TRUE, &numNewMessages);
			numNewMessagesTotal += numNewMessages;
		}
		else if (folder10 = do_QueryReferent(weakReference))
		{
			folder10->GetPrettiestName(getter_Copies(accountName));
			numNewMessages = 0;   
			folder10->GetNumNewMessages(PR_TRUE, &numNewMessages);
			numNewMessagesTotal += numNewMessages;
		}
	}
	
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

	PRInt32 numNewMessages = 0;

	PRUint32 count = 0;
	mFoldersWithNewMail->Count(&count);

	if (count == 0)  // kick out if we don't have any folders with new mail
		return NS_OK;

	nsCOMPtr<nsISupports> supports = getter_AddRefs(mFoldersWithNewMail->ElementAt(0));
	nsCOMPtr<nsIWeakReference> weakReference = do_QueryInterface(supports);
	nsCOMPtr<nsIMsgFolder> folder;
	nsCOMPtr<nsIMsgFolder10> folder10;

	if (folder = do_QueryReferent(weakReference))
	{
		PRUint32 biffState = nsIMsgFolder::nsMsgBiffState_NoMail; 
		nsString msgURI;
		// enumerate over the folders under this root folder till we find one with new mail....
		nsCOMPtr<nsISupportsArray> allFolders;
		NS_NewISupportsArray(getter_AddRefs(allFolders));
		rv = folder->ListDescendents(allFolders);
		NS_ENSURE_SUCCESS(rv, rv);

		nsCOMPtr<nsIMsgFolder> msgFolder;
		nsCOMPtr<nsIEnumerator> enumerator;
		allFolders->Enumerate(getter_AddRefs(enumerator));
		if (enumerator)
		{
			nsresult more = enumerator->First();
			while (NS_SUCCEEDED(more))
			{
				nsCOMPtr<nsISupports> supports;
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
	else if (folder10 = do_QueryReferent(weakReference))
	{
		PRUint32 biffState = nsIMsgFolder10::nsMsgBiffState_NoMail; 
		nsString msgURI;
		// enumerate over the folders under this root folder till we find one with new mail....
		nsCOMPtr<nsISupportsArray> allFolders;
		NS_NewISupportsArray(getter_AddRefs(allFolders));
		rv = folder10->ListDescendents(allFolders);
		NS_ENSURE_SUCCESS(rv, rv);

		nsCOMPtr<nsIMsgFolder10> msgFolder;
		nsCOMPtr<nsIEnumerator> enumerator;
		allFolders->Enumerate(getter_AddRefs(enumerator));
		if (enumerator)
		{
			nsresult more = enumerator->First();
			while (NS_SUCCEEDED(more))
			{
				nsCOMPtr<nsISupports> supports;
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
		mTrayIcon = egg_status_icon_new();
		if (mTrayIcon != NULL)
		{
			g_signal_connect(G_OBJECT(mTrayIcon), "activate", G_CALLBACK(TrayIconActivate), this);
			g_signal_connect(G_OBJECT(mTrayIcon), "popup-menu", G_CALLBACK(TrayIconPopupMenu), this);
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

void nsMessengerFreeDesktopIntegration::OnActionDefault()
{
	if (mHasBiff)
	{
		OnActionReadMail();
	}
	else
	{
		OnActionToggleWindow();
	}
}

void nsMessengerFreeDesktopIntegration::OnActionReadMail()
{
	nsXPIDLCString folderURI, messageURI;
	GetFirstFolderWithNewMail(getter_Copies(folderURI), getter_Copies(messageURI));
	openMailWindow(NS_LITERAL_STRING("mail:3pane").get(), folderURI, messageURI);
}

void nsMessengerFreeDesktopIntegration::OnActionToggleWindow()
{
	toggleMailWindow(NS_LITERAL_STRING("mail:3pane").get());
}

void nsMessengerFreeDesktopIntegration::OnActionHideIcon()
{
	if (mHasBiff)
	{
		mPrefBranch->SetBoolPref(PREF_BIFF_SHOW_ICON, PR_FALSE);
	}
	else
	{
		mPrefBranch->SetBoolPref(PREF_BIFF_ALWAYS_SHOW_ICON, PR_FALSE);
	}
	// TODO: This should ultimately popup an informational message box.
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
			if (!mBrandIconPath.IsEmpty())
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
					fprintf (stderr, "mozTrayBiff: Error loading application icon: %s\n", err->message);
					RemoveBiffIcon();
				}
			}
			else
			{
				fprintf(stderr, "mozTrayBiff: No application icon found\n");
				RemoveBiffIcon();
			}
		}
		else
		{
			RemoveBiffIcon();
		}
		HideHwIndicator();
	}
}

void nsMessengerFreeDesktopIntegration::OnActionPopupMenu(unsigned int button, unsigned int activateTime)
{
	GtkWidget* pTrayMenu = gtk_menu_new();
	GtkWidget *entry;

	// Default values for menu item strings
	nsXPIDLString menuItemReadMail;
	menuItemReadMail.Assign(NS_LITERAL_STRING("Read mail..."));
	nsXPIDLString menuItemNewWindow;
	menuItemNewWindow.Assign(NS_LITERAL_STRING("New window"));
	nsXPIDLString menuItemRestoreWindow;
	menuItemRestoreWindow.Assign(NS_LITERAL_STRING("Restore window"));
	nsXPIDLString menuItemHideWindow;
	menuItemHideWindow.Assign(NS_LITERAL_STRING("Hide window"));
	nsXPIDLString menuItemHideIcon;
	menuItemHideIcon.Assign(NS_LITERAL_STRING("Hide icon"));

	// Retrieve menu item localizations from the string bundle, if possible
	nsCOMPtr<nsIStringBundle> bundle; 
	GetStringBundle(STRING_BUNDLE_TRAYBIFF, getter_AddRefs(bundle));
	NS_ASSERTION(bundle, "no string bundle");
	if (bundle)
	{ 
		bundle->GetStringFromName(NS_LITERAL_STRING("trayMenu_ReadMail").get(), getter_Copies(menuItemReadMail));
		bundle->GetStringFromName(NS_LITERAL_STRING("trayMenu_NewWindow").get(), getter_Copies(menuItemNewWindow));
		bundle->GetStringFromName(NS_LITERAL_STRING("trayMenu_RestoreWindow").get(), getter_Copies(menuItemRestoreWindow));
		bundle->GetStringFromName(NS_LITERAL_STRING("trayMenu_HideWindow").get(), getter_Copies(menuItemHideWindow));
		bundle->GetStringFromName(NS_LITERAL_STRING("trayMenu_HideIcon").get(), getter_Copies(menuItemHideIcon));
	}

	if (mHasBiff)
	{
		entry = gtk_menu_item_new_with_label(NS_ConvertUTF16toUTF8(menuItemReadMail).get());
		g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(MenuReadMail), this);
		gtk_menu_shell_append(GTK_MENU_SHELL(pTrayMenu), entry);
	}
	switch (isMailWindowVisible(NS_LITERAL_STRING("mail:3pane").get()))
	{
	case MAIL_WINDOW_NOT_FOUND:
		entry = gtk_menu_item_new_with_label(NS_ConvertUTF16toUTF8(menuItemNewWindow).get());
		break;
	case MAIL_WINDOW_HIDDEN:
		entry = gtk_menu_item_new_with_label(NS_ConvertUTF16toUTF8(menuItemRestoreWindow).get());
		break;
	case MAIL_WINDOW_VISIBLE:
		entry = gtk_menu_item_new_with_label(NS_ConvertUTF16toUTF8(menuItemHideWindow).get());
		break;
	}
	g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(MenuToggleWindow), this);
	gtk_menu_shell_append(GTK_MENU_SHELL(pTrayMenu), entry);
	entry = gtk_menu_item_new_with_label(NS_ConvertUTF16toUTF8(menuItemHideIcon).get());
	g_signal_connect(G_OBJECT(entry), "activate", G_CALLBACK(MenuHideIcon), this);
	gtk_menu_shell_append(GTK_MENU_SHELL(pTrayMenu), entry);

	gtk_widget_show_all(pTrayMenu);
	g_signal_connect(G_OBJECT(pTrayMenu), "selection-done", G_CALLBACK(TrayIconPopupMenuSelectionDone), this);
	gtk_menu_popup(GTK_MENU(pTrayMenu), NULL, NULL, NULL, NULL, button, activateTime);
}

/*
 Static functions which are GTK+ event handlers; minimal code to cast user-data
 into nsMessengerFreeDesktopIntegration and call the appropriate method.
 */
 
void nsMessengerFreeDesktopIntegration::TrayIconActivate(GtkWidget *trayIcon, void *data)
{
	reinterpret_cast<nsMessengerFreeDesktopIntegration*>(data)->OnActionDefault();
}

void nsMessengerFreeDesktopIntegration::TrayIconPopupMenu(GtkWidget *trayIcon, guint button, guint activateTime, void *data)
{
	reinterpret_cast<nsMessengerFreeDesktopIntegration*>(data)->OnActionPopupMenu(button, activateTime);
}

void nsMessengerFreeDesktopIntegration::TrayIconPopupMenuSelectionDone(GtkMenuShell *menushell, void *data)
{
	gtk_widget_destroy(GTK_WIDGET(menushell));
}

void nsMessengerFreeDesktopIntegration::MenuHideIcon(GtkMenuItem *menuItem, void *data)
{
	reinterpret_cast<nsMessengerFreeDesktopIntegration*>(data)->OnActionHideIcon();
}

void nsMessengerFreeDesktopIntegration::MenuReadMail(GtkMenuItem *menuItem, void *data)
{
	reinterpret_cast<nsMessengerFreeDesktopIntegration*>(data)->OnActionReadMail();
}

void nsMessengerFreeDesktopIntegration::MenuToggleWindow(GtkMenuItem *menuItem, void *data)
{
	reinterpret_cast<nsMessengerFreeDesktopIntegration*>(data)->OnActionToggleWindow();
}

void nsMessengerFreeDesktopIntegration::SetToolTipString(const PRUnichar * aToolTipString)
{
	if (aToolTipString == NULL || mTrayIcon == NULL)
	{
		return;
	}
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

// For compatibility with tb1.0
NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemPropertyFlagChanged(nsISupports *item, nsIAtom *aProperty, PRUint32 oldFlag, PRUint32 newFlag)
{
	// if we got new mail show a icon in the system tray
	if (mBiffStateAtom == aProperty && mFoldersWithNewMail)
	{
		nsCOMPtr<nsIMsgFolder10> folder = do_QueryInterface(item);
		NS_ENSURE_TRUE(folder, NS_OK);

		if (newFlag == nsIMsgFolder::nsMsgBiffState_NewMail) 
		{
			// if the icon is not already visible, only show a system tray icon iff 
			// we are performing biff (as opposed to the user getting new mail)
			PRBool performingBiff = PR_FALSE;
			nsCOMPtr<nsIMsgIncomingServer10> server;
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
			nsCOMPtr<nsIMsgFolder10> oldFolder;
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

// For compatibility with tb1.0
NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemPropertyFlagChanged(nsIMsgDBHdr *item, nsIAtom *property, PRUint32 oldFlag, PRUint32 newFlag)
{
	return NS_OK;
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemAdded(nsIRDFResource *, nsISupports *)
{
  return NS_OK;
}

// For compatibility with tb1.0
NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemAdded(nsISupports*, nsISupports*, const char*)
{
  return NS_OK;
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemBoolPropertyChanged(nsIRDFResource *aItem, nsIAtom *aProperty, PRBool aOldValue, PRBool aNewValue)
{
  return NS_OK;
}

// For compatibility with tb1.0
NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemBoolPropertyChanged(nsISupports*, nsIAtom*, int, int)
{
  return NS_OK;
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemIntPropertyChanged(nsIRDFResource *aItem, nsIAtom *aProperty, int aOldValue, int aNewValue)
{
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
	
	return NS_OK;
}

// For compatibility with tb1.0
NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemIntPropertyChanged(nsISupports*, nsIAtom*, int, int)
{
	return NS_OK;
}

NS_IMETHODIMP
nsMessengerFreeDesktopIntegration::OnItemEvent(nsIMsgFolder *, nsIAtom *)
{
  return NS_OK;
}
