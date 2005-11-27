/*
   Copyright 2005 Ilya Konstantinov

   This file is part of mozTrayBiff.

   mozTrayBiff is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   mozTrayBiff is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with mozTrayBiff; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

function ensure_success(err)
{
	if (err != SUCCESS)
	{
		switch(err)
		{
			case -202:
				alert("No permission to install files for all users.\n\nUsually you'd have to use an administrator account to install this package.");
				break;
			case -215:
				alert("Could not overwrite existing component.\n\nUsually you'd have to use an administrator account to install this package.");
				break;
		}
		cancelInstall(err);
		throw 'Failed installation';
	}
}

try
{
	const APP_DISPLAY_NAME = "New Mail Icon";
	const APP_NAME = "traybiff";
	const APP_PACKAGE = "/TrayBiff";
	const APP_VERSION = "1.2";
	const APP_LOCALES = new Array("en-US", "he-IL", "fr-FR", "it-IT", "ca-AD", "de-DE", "es-ES", "cs-CZ");
	
	initInstall(APP_DISPLAY_NAME, APP_PACKAGE, APP_VERSION);

	// Add the Default Preferences
	var defaultPrefsPath = getFolder("Program", "defaults/pref"); // We cannot get 'Defaults' directly, as it's no longer supported (it seems)
	ensure_success( addFile(null, APP_VERSION, "defaults/preferences/tray-biff.js", defaultPrefsPath, "") );

	// Add the XPCOM component
	var componentsPath = getFolder("Components");
        ensure_success( addFile(null, APP_VERSION, "components/libtraybiff.so", componentsPath, "") );
	var oldComponentFile = getFolder(componentsPath, "mozFreeDesktopIntegration.so");
	File.remove(oldComponentFile);
	
	// Add the chrome
	var isProfile = false; 	// Cannot XPCOM components to profile anyway...
	var chromeFolder, regFlag = 0;
	if (isProfile)
	{
		chromeFolder = getFolder("Profile", "chrome");
		regFlag |= PROFILE_CHROME;
	}
	else
	{
		chromeFolder = getFolder("chrome");
	}
	ensure_success( addFile(null, APP_VERSION, "chrome/tray-biff.jar", chromeFolder, "") );
	File.remove( getFolder(chromeFolder, "mozFreeDesktopIntegration.jar") );
	var regSource = getFolder(chromeFolder, "tray-biff.jar");
	ensure_success( registerChrome(PACKAGE | regFlag, regSource, "content/" + APP_NAME + "/") );
	for (var i = 0; i < APP_LOCALES.length; ++i)
	{
		ensure_success( registerChrome(LOCALE | regFlag, regSource, "locale/" + APP_LOCALES[i] + "/" + APP_NAME + "/") );
	}

	// Perform the installation
	ensure_success( performInstall() );
	
	alert(APP_DISPLAY_NAME+" "+APP_VERSION+" has been succesfully installed.\nYou need to restart Mozilla / Thunderbird for the changes to take effect.");
	
	/*
	 This should hopefully refresh XPCOM components, just like navigator.plugins.reload().
	 Unfortunatelly, I cannot use the real XPCOM manager component from here, so we resort
	 to "known hacks".

	 In Mozilla 1.5, it doesn't do that, but it hopefully will in future revisions.
	*/
	refreshPlugins(true);
}
catch (e)
{
	
}
