#include <stdio.h>
#include "nsXPCOM.h"
#include "nsCOMPtr.h"
#include "nsServiceManagerUtils.h"
#include "nsIComponentRegistrar.h"
#include "nsXULAppAPI.h"
#include "nsIXULRuntime.h"
#include "nsStringAPI.h"

int main(int argc, char **argv)
	{
	nsresult rv;
	
	{
		nsCOMPtr<nsIServiceManager> servMan;
		rv = NS_InitXPCOM2(getter_AddRefs(servMan), nsnull, nsnull);
		if (NS_FAILED(rv))
			return -1;
		{
			nsCOMPtr<nsIComponentRegistrar> registrar = do_QueryInterface(servMan);
			NS_ASSERTION(registrar, "Null nsIComponentRegistrar");
			if (registrar)
				registrar->AutoRegister(nsnull);
		}
		{
			nsCOMPtr<nsIXULRuntime> xulrun = do_GetService(XULAPPINFO_SERVICE_CONTRACTID, &rv);
			if (xulrun)
			{
				nsCString xpcomAbi;
				if (!NS_FAILED(xulrun->GetXPCOMABI(xpcomAbi)))
				{
					printf("%s\n", xpcomAbi.get());
				}
			}
			else
			{
				printf("No nsIXULRuntime: %x.\n", rv);
			}
		}
	}
	rv = NS_ShutdownXPCOM(nsnull);
	NS_ASSERTION(NS_SUCCEEDED(rv), "NS_ShutdownXPCOM failed");
	return 0;
}
