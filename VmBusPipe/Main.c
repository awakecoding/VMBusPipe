
#include <stdio.h>
#include <stdlib.h>
#include <initguid.h>

#include <windows.h>

#include "VmBusPipe.h"

// {1DAE6E2C-D5EF-4085-8C3C-00FC462F181A}
DEFINE_GUID(GUID_VMBUS_PIPE_TEST, 0x1dae6e2c, 0xd5ef, 0x4085, 0x8c, 0x3c, 0x0, 0xfc, 0x46, 0x2f, 0x18, 0x1a);

static BYTE RdvVmEndPointInstanceGuidUnifiedApi[16] =
	"\xCE\x7C\xE2\xAB\x02\x0E\x97\x41\x8E\x45\x5E\x92\x12\x4E\x1C\x2A";

static BYTE RdvVmEndPointInstanceGuidAllowList[16] =
	"\x67\x97\x16\x6E\xF8\x53\xB7\x49\xBF\xE1\xB4\x99\x16\x80\x57\xC4";

int test_VmBusPipeHost(VmBusPipeHost* host)
{
	int status;
	BOOL bSuccess;
	printf("VmBusPipeHost\n");

	/**
	 * VmbusPipeServerOfferChannel
	 * VmbusPipeServerConnectPipe
	 */ 

	bSuccess = host->VmbusPipeClientEnumeratePipes((const GUID*) RdvVmEndPointInstanceGuidUnifiedApi, 0xFFFF, 0xFFFF);

	printf("VmbusPipeClientEnumeratePipes: %d\n", bSuccess);

	status = host->VmbusPipeServerOfferChannel(NULL, 0xFFFF, 0xFFFF);

	printf("VmbusPipeServerOfferChannel: %d\n", status);

	return 0;
}

int test_VmBusPipeGuest(VmBusPipeGuest* guest)
{
	int status;
	BOOL bSuccess;
	HANDLE hChannel;

	printf("VmBusPipeGuest\n");

	/**
	 * VmbusPipeClientOpenChannel
	 * VmbusPipeServerOfferChannel
	 * VmbusPipeServerConnectPipe
	 * VmbusPipeClientEnumeratePipes
	 */ 

	bSuccess = guest->VmbusPipeClientEnumeratePipes((const GUID*) RdvVmEndPointInstanceGuidUnifiedApi, 0xFFFF, 0xFFFF);

	printf("VmbusPipeClientEnumeratePipes: %d\n", bSuccess);

	status = guest->VmbusPipeServerOfferChannel(NULL, 0xFFFF, 0xFFFF);

	printf("VmbusPipeServerOfferChannel: %d\n", status);

	/**
	 * First argument is a 526-byte structure
	 * Second argument is a set of flags, with an observed value of 0x40000000
	 */

	hChannel = guest->VmbusPipeClientOpenChannel(NULL, 0x40000000);

	printf("VmbusPipeClientOpenChannel: 0x%04X\n", hChannel);

	return 0;
}

int main(int argc, char** argv)
{
	VmBusPipeHost* host;
	VmBusPipeGuest* guest;

	host = VmBusPipeHostInit();
	guest = VmBusPipeGuestInit();

	if (host)
	{
		test_VmBusPipeHost(host);
	}
	else if (guest)
	{
		test_VmBusPipeGuest(guest);
	}
	else
	{
		printf("VmBusPipe not found!\n");
	}

	VmBusPipeHostFree(host);
	VmBusPipeGuestFree(guest);

	return 0;
}
