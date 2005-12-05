/* Component registration code courtesy of FoxyTunes project. */

var extUuid = '{2e1b75f1-6b5a-4f1d-89b4-424f636e4fba}';

function getProfileDir()
{
	var dirServiceProp = Components.classes["@mozilla.org/file/directory_service;1"];
	dirServiceProp = dirServiceProp.getService(Components.interfaces.nsIProperties);
	var profileDir = dirServiceProp.get("ProfD", Components.interfaces.nsIFile);
	return profileDir;
}

function getExtensionHome()
{
	var profileDir = getProfileDir();
	var home = profileDir.clone();
	home.append('extensions');
	home.append(extUuid);
	return home;
}

function getComponentFile(filename)
{
	var home = getExtensionHome();
	var componentFile = home.clone();
	componentFile.append('components');
	componentFile.append(filename);
	return componentFile;
}

function init()
{
	// Check whether our binary component is registered.
	if (!Components.classes["@mozilla.org/messenger/osintegration;1"])
	{
		// In some peculiar cases, Thunderbird will not auto-register our shared object,
		// so we do it ourselves.
		var componentRegistrar = Components.manager.QueryInterface(Components.interfaces.nsIComponentRegistrar);
		try
		{
			var compFile = getComponentFile('libtraybiff.so');
			componentRegistrar.autoRegister(compFile);
			Components.classes["@mozilla.org/messenger/osintegration;1"].getService();
		}
		catch(err)
		{
			alert("Unable to register shared object:\n" + err + "\n\nThe New Mail Icon extension cannot be activated.");
		}
	}
}

addEventListener("load", init, false);
