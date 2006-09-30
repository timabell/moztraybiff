/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*-
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
 * Scott MacGregor <mscott@netscape.com>
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

#ifndef __nsMessengerFreeDesktopIntegration_h
#define __nsMessengerFreeDesktopIntegration_h

#include "nsIMessengerFreeDesktopIntegration.h"
#include <nsIMessengerOSIntegration.h>
#include <nsIFolderListener.h>
#include "nsIFolderListener10.h"
#include <nsIObserver.h>
#include <nsIAtom.h>
#include <nsITimer.h>
#include <nsCOMPtr.h>
#include <nsString.h>
#include <nsIPrefBranch.h>
#include <nsInt64.h>
#include <nsISupportsArray.h>
#include <nsWeakReference.h>

#include "eggstatusicon.h"
#include <gtk/gtk.h>

#define NS_MESSENGERFREEDESKTOPINTEGRATION_CID \
  {0xb7b5b532, 0x1d3b, 0x11d8, \
    {0x97, 0x1f, 0x00, 0x20, 0x18, 0x2e, 0x3f, 0x64}}

class nsIStringBundle;

class nsMessengerFreeDesktopIntegration :
	public nsIMessengerFreeDesktopIntegration,
	public nsIMessengerOSIntegration,
	public nsIFolderListener,
	public nsIFolderListener10,
	public nsIObserver,
	public nsSupportsWeakReference
{
public:
	nsMessengerFreeDesktopIntegration();
	virtual ~nsMessengerFreeDesktopIntegration();
	virtual nsresult Init();

	NS_DECL_ISUPPORTS
	NS_DECL_NSIMESSENGEROSINTEGRATION
	NS_DECL_NSIFOLDERLISTENER
	NS_DECL_NSIOBSERVER
	NS_DECL_NSIMESSENGERFREEDESKTOPINTEGRATION

// Instead of NS_DECL_NSIFOLDERLISTENER10
public:
	NS_IMETHOD OnItemAdded(nsISupports *parentItem, nsISupports *item, const char *viewString);
	NS_IMETHOD OnItemRemoved(nsISupports *parentItem, nsISupports *item, const char *viewString);
	NS_IMETHOD OnItemPropertyChanged(nsISupports *item, nsIAtom *property, const char *oldValue, const char *newValue);
	NS_IMETHOD OnItemIntPropertyChanged(nsISupports *item, nsIAtom *property, PRInt32 oldValue, PRInt32 newValue);
	NS_IMETHOD OnItemBoolPropertyChanged(nsISupports *item, nsIAtom *property, PRBool oldValue, PRBool newValue);
	NS_IMETHOD OnItemUnicharPropertyChanged(nsISupports *item, nsIAtom *property, const PRUnichar *oldValue, const PRUnichar *newValue);
	NS_IMETHOD OnItemPropertyFlagChanged(nsISupports *item, nsIAtom *property, PRUint32 oldFlag, PRUint32 newFlag);

private:
	void ApplyPrefs();
	void FillToolTipInfo();
	void SetToolTipString(const PRUnichar * aToolTipString);
	void ClearToolTip();
	void AddBiffIcon();
	void RemoveBiffIcon();
	void ShowHwIndicator();
	void HideHwIndicator();
	// Called when mHasBiff is changed and responsible for changing the visual cues.
	void OnBiffChange();
	nsresult GetFirstFolderWithNewMail(char ** aFolderURI, char ** aMessageURI);
	nsresult GetStringBundle(const char* src, nsIStringBundle **aBundle);

private:
	nsCOMPtr <nsIPrefBranch> mPrefBranch;
	PRBool mShowBiffIcon, mUseHwIndicator, mAlwaysShowBiffIcon;
	// Specifies which keyboard led should indicate new mail (0 = disable).
	PRInt32 mUseKeyboardLed;
	// Autodetected or manually-set file name for controlling the HW indicator.
	nsXPIDLCString mHwIndicatorFile;
	PRBool mHasBiff;
	nsCOMPtr<nsISupportsArray> mFoldersWithNewMail;	// keep track of all the root folders with pending new mail
	nsCOMPtr<nsIAtom> mBiffStateAtom;

// Event handlers which map directly to GTK+ events on the tray icon.
private:
	// Default action (tray icon left clicked etc.)
	void OnActionDefault();
	// 3rd button clicked on the tray icon.
	void OnActionPopupMenu(unsigned int button, unsigned int activateTime);
	// Read Mail chosen in popup menu.
	void OnActionReadMail();
	// Toggle Window chosen in popup menu.
	void OnActionToggleWindow();
	// Hide Icon chosen in popup menu.
	void OnActionHideIcon();

// GTK+ signal handlers
private:
	static void TrayIconActivate(GtkWidget *trayIcon, void *data);
	static void TrayIconPopupMenu(GtkWidget *trayIcon, guint button, guint activateTime, void *data);
	static void TrayIconPopupMenuSelectionDone(GtkMenuShell *menushell, void *data);
	static void MenuReadMail(GtkMenuItem *menuItem, void *data);
	static void MenuToggleWindow(GtkMenuItem *menuItem, void *data);
	static void MenuHideIcon(GtkMenuItem *menuItem, void *data);
  
// GTK+ tray icon-related members
private:
	EggStatusIcon* mTrayIcon;
	
protected:
	nsCAutoString mBrandIconPath;
};

#endif // __nsMessengerFreeDesktopIntegration_h
