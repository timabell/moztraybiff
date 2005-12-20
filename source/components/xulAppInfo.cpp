#include <nsCOMPtr.h>
#include <nsServiceManagerUtils.h>
#include <nsXULAppAPI.h>

int main()
{
	nsCOMPtr<nsIXULAppInfo> xulrun (do_GetService(XULAPPINFO_SERVICE_CONTRACTID));
}
