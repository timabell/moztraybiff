#define MOZILLA_STRICT_API

// Headers required for module defintion.
#include <nsIGenericFactory.h>

// Headers of module's components.
#include "nsMessengerFreeDesktopIntegration.h"
#include <nsMsgBaseCID.h>

NS_GENERIC_FACTORY_CONSTRUCTOR_INIT(nsMessengerFreeDesktopIntegration, Init);

static const nsModuleComponentInfo gComponents[] = {
	{
		"Integration of Messenger into Free Desktop environments",
		NS_MESSENGERFREEDESKTOPINTEGRATION_CID,
		NS_MESSENGEROSINTEGRATION_CONTRACTID,
		nsMessengerFreeDesktopIntegrationConstructor
	}
};

NS_IMPL_NSGETMODULE(trayBiffModule, gComponents)
