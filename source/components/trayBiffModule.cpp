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
