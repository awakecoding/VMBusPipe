
#include "Helpers.h"
#include "VmBusPipeStub.h"

CLogFile log;
static VmBusPipeGuest* guest = NULL;

#ifdef __cplusplus
extern "C" {
#endif

static fnVmbusPipeClientEnumeratePipe pfnVmbusPipeClientEnumeratePipe = NULL;

void VmbusPipeClientEnumeratePipe(void* pContext, BYTE* pUserDefined, PVMBUS_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData, char* arg4)
{
	char* VmBusStringA = NULL;
	WCHAR* VmBusStringW = NULL;
	char* pUserDefinedString = NULL;

	/**
	 * pUserDefined: 116-byte UserDefined registry key
	 *
	 * pDeviceInterfaceDetailData: pointer to SP_DEVICE_INTERFACE_DETAIL_DATA (526-byte)
	 * http://msdn.microsoft.com/en-us/library/windows/hardware/ff552343/
	 */

	log.Write("VmbusPipeClientEnumeratePipe: pContext: %p", pContext);

	pUserDefinedString = BinToHexString(pUserDefined, 116);

	log.Write("\tpUserDefined: %s", pUserDefinedString);

	VmBusStringW = (WCHAR*) pDeviceInterfaceDetailData->DevicePath;
	ConvertFromUnicode(CP_UTF8, 0, VmBusStringW, -1, &VmBusStringA, 0, NULL, NULL);

	log.Write("\tpDeviceInterfaceDetailData: cbSize: %d DevicePath: %s",
		pDeviceInterfaceDetailData->cbSize, VmBusStringA);

	log.Write("\targ4: %p", arg4);

	pfnVmbusPipeClientEnumeratePipe(pContext, pUserDefined, pDeviceInterfaceDetailData, arg4);

	free(pUserDefinedString);
	free(VmBusStringA);
}

BOOL VmbusPipeClientEnumeratePipes(const GUID* ClassGuid, void* pContext, fnVmbusPipeClientEnumeratePipe pfn)
{
	BOOL bSuccess;

	pfnVmbusPipeClientEnumeratePipe = (fnVmbusPipeClientEnumeratePipe) pfn;
	pfn = VmbusPipeClientEnumeratePipe;

	if (!guest || !guest->VmbusPipeClientEnumeratePipes)
		return FALSE;

	bSuccess = guest->VmbusPipeClientEnumeratePipes(ClassGuid, pContext, pfn);

	log.Write("VmbusPipeClientEnumeratePipes: ClassGuid: {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x} pContext: %p pfn: %p bSuccess: %d",
		ClassGuid->Data1, ClassGuid->Data2, ClassGuid->Data3,
		ClassGuid->Data4[0], ClassGuid->Data4[1],
		ClassGuid->Data4[2], ClassGuid->Data4[3], ClassGuid->Data4[4],
		ClassGuid->Data4[5], ClassGuid->Data4[6], ClassGuid->Data4[7],
		pContext, pfn, bSuccess);

	return bSuccess;
}

HANDLE VmbusPipeClientOpenChannel(PVMBUS_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData, DWORD flags)
{
	HANDLE hChannel;
	char* VmBusStringA = NULL;
	WCHAR* VmBusStringW = NULL;

	if (!guest || !guest->VmbusPipeClientOpenChannel)
		return INVALID_HANDLE_VALUE;

	hChannel = guest->VmbusPipeClientOpenChannel(pDeviceInterfaceDetailData, flags);

	VmBusStringW = (WCHAR*) pDeviceInterfaceDetailData->DevicePath;
	ConvertFromUnicode(CP_UTF8, 0, VmBusStringW, -1, &VmBusStringA, 0, NULL, NULL);

	log.Write("VmbusPipeClientOpenChannel: pDeviceInterfaceDetailData: cbSize: %d DevicePath: %s",
		pDeviceInterfaceDetailData->cbSize, VmBusStringA);
	log.Write("VmbusPipeClientOpenChannel: flags: 0x%04X", flags);

	free(VmBusStringA);

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

int VmbusPipeServerOfferChannel(PVMBUS_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData, DWORD dwFlagsAndAttributes, PHANDLE phChannel)
{
	int status;

	log.Write("VmbusPipeServerOfferChannel");

	if (!guest || !guest->VmbusPipeServerOfferChannel)
		return -1;

	status = guest->VmbusPipeServerOfferChannel(pDeviceInterfaceDetailData, dwFlagsAndAttributes, phChannel);

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
