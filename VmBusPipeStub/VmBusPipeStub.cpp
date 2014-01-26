
#include "Helpers.h"
#include "VmBusPipeStub.h"

CLogFile log;
static VmBusPipeGuest* guest = NULL;

#ifdef __cplusplus
extern "C" {
#endif

BOOL VmbusPipeClientEnumeratePipes(const GUID* ClassGuid, DWORD flags, void* pfn)
{
	BOOL bSuccess;

	if (!guest || !guest->VmbusPipeClientEnumeratePipes)
		return FALSE;

	bSuccess = guest->VmbusPipeClientEnumeratePipes(ClassGuid, flags, pfn);

	log.Write("VmbusPipeClientEnumeratePipes: ClassGuid: {%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X} flags: 0x%04X pfn: %p bSuccess: %d",
		ClassGuid->Data1, ClassGuid->Data2, ClassGuid->Data3,
		ClassGuid->Data4[0], ClassGuid->Data4[1],
		ClassGuid->Data4[2], ClassGuid->Data4[3], ClassGuid->Data4[4],
		ClassGuid->Data4[5], ClassGuid->Data4[6], ClassGuid->Data4[7],
		flags, pfn, bSuccess);

	return bSuccess;
}

HANDLE VmbusPipeClientOpenChannel(void* arg1, int arg2)
{
	char* hexstr;
	HANDLE hChannel;
	char* VmBusStringA;
	WCHAR* VmBusStringW;
	UINT32 unknownFlags;

	if (!guest || !guest->VmbusPipeClientOpenChannel)
		return INVALID_HANDLE_VALUE;

	hexstr = BinToHexString((BYTE*) arg1, 526);

	hChannel = guest->VmbusPipeClientOpenChannel(arg1, arg2);

	unknownFlags = *((UINT32*) arg1);

	VmBusStringW = (WCHAR*) (((BYTE*) arg1) + 4);
	ConvertFromUnicode(CP_UTF8, 0, VmBusStringW, -1, &VmBusStringA, 0, NULL, NULL);

	log.Write("VmbusPipeClientOpenChannel: flags: 0x%04X pChannelData: %s", arg2, hexstr);
	log.Write("VmbusPipeClientOpenChannel: unknownFlags: 0x%04X String: %s", unknownFlags, VmBusStringA);

	free(VmBusStringA);
	free(hexstr);

	return hChannel;
}

void VmbusPipeClientReadyForChannelNotification(void* arg1, int flags)
{
	log.Write("VmbusPipeClientReadyForChannelNotification");

	if (!guest || !guest->VmbusPipeClientReadyForChannelNotification)
		return;

	guest->VmbusPipeClientReadyForChannelNotification(arg1, flags);
}

LPVOID VmbusPipeClientRegisterChannelNotification(void* arg1, void* arg2, int arg3, int arg4, int arg5)
{
	LPVOID lptr;

	log.Write("VmbusPipeClientRegisterChannelNotification");

	if (!guest || !guest->VmbusPipeClientRegisterChannelNotification)
		return NULL;

	lptr = guest->VmbusPipeClientRegisterChannelNotification(arg1, arg2, arg3, arg4, arg5);

	return lptr;
}

int VmbusPipeClientUnregisterChannelNotification(LPVOID arg1, int arg2)
{
	int status;

	log.Write("VmbusPipeClientUnregisterChannelNotification");

	if (!guest || !guest->VmbusPipeClientUnregisterChannelNotification)
		return -1;

	status = guest->VmbusPipeClientUnregisterChannelNotification(arg1, arg2);

	return status;
}

BOOL VmbusPipeClientWaitChannel(int arg1, int arg2, int arg3, UINT_PTR param)
{
	BOOL bSuccess;

	log.Write("VmbusPipeClientWaitChannel");

	if (!guest || !guest->VmbusPipeClientWaitChannel)
		return FALSE;

	bSuccess = guest->VmbusPipeClientWaitChannel(arg1, arg2, arg3, param);

	return bSuccess;
}

BOOL VmbusPipeServerConnectPipe(HANDLE hDevice, LPOVERLAPPED lpOverlapped)
{
	BOOL bSuccess;

	log.Write("VmbusPipeServerConnectPipe");

	if (!guest || !guest->VmbusPipeServerConnectPipe)
		return FALSE;

	bSuccess = guest->VmbusPipeServerConnectPipe(hDevice, lpOverlapped);

	return bSuccess;
}

int VmbusPipeServerOfferChannel(void* arg1, DWORD dwFlagsAndAttributes, PHANDLE phChannel)
{
	int status;

	log.Write("VmbusPipeServerOfferChannel");

	if (!guest || !guest->VmbusPipeServerOfferChannel)
		return -1;

	status = guest->VmbusPipeServerOfferChannel(arg1, dwFlagsAndAttributes, phChannel);

	return status;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (!guest)
	{
		char logFileName[1024];
		char moduleFileName[1024];

		sprintf_s(logFileName, sizeof(logFileName), "VmBusPipeStub%04X.txt", GetProcessId(GetCurrentProcess()));

		GetModuleFileNameA(NULL, moduleFileName, sizeof(moduleFileName));

		guest = VmBusPipeGuestInit("vmbuspipe_.dll");

		log.OpenFile(logFileName, true);

		log.Write("DllMain: %s, fdwReason: %d", moduleFileName, fdwReason);
	}

	return TRUE;
}

#ifdef __cplusplus
}
#endif
