
#include "Helpers.h"
#include "VmBusPipeStub.h"

CLogFile log;
static VmBusPipeGuest* guest = NULL;

#ifdef __cplusplus
extern "C" {
#endif

struct _VmBusPipeEnumWrapper
{
	void* pContext;
	fnVmbusPipeClientEnumeratePipe pfnVmbusPipeClientEnumeratePipe;
};
typedef struct _VmBusPipeEnumWrapper VmBusPipeEnumWrapper;

static void VMBUSAPI VmbusPipeClientEnumeratePipe(void* pContext, BYTE* pUserDefined, PVMBUS_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData, const GUID* pGuid)
{
	char* VmBusStringA = NULL;
	WCHAR* VmBusStringW = NULL;
	char* pUserDefinedString = NULL;
	VmBusPipeEnumWrapper* wrapper = (VmBusPipeEnumWrapper*) pContext;

	/**
	 * pUserDefined: 116-byte UserDefined registry key
	 *
	 * pDeviceInterfaceDetailData: pointer to SP_DEVICE_INTERFACE_DETAIL_DATA (526 bytes)
	 * http://msdn.microsoft.com/en-us/library/windows/hardware/ff552343/
	 */

	log.Write("VmbusPipeClientEnumeratePipe: pContext: %p", wrapper->pContext);

	pUserDefinedString = BinToHexString(pUserDefined, 116);

	log.Write("\tpUserDefined: %s", pUserDefinedString);

	VmBusStringW = (WCHAR*) pDeviceInterfaceDetailData->DevicePath;
	ConvertFromUnicode(CP_UTF8, 0, VmBusStringW, -1, &VmBusStringA, 0, NULL, NULL);

	log.Write("\tpDeviceInterfaceDetailData: cbSize: %d DevicePath: %s",
		pDeviceInterfaceDetailData->cbSize, VmBusStringA);

	log.Write("\tpGuid: {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		pGuid->Data1, pGuid->Data2, pGuid->Data3,
		pGuid->Data4[0], pGuid->Data4[1],
		pGuid->Data4[2], pGuid->Data4[3], pGuid->Data4[4],
		pGuid->Data4[5], pGuid->Data4[6], pGuid->Data4[7]);

	wrapper->pfnVmbusPipeClientEnumeratePipe(wrapper->pContext, pUserDefined, pDeviceInterfaceDetailData, pGuid);

	free(pUserDefinedString);
	free(VmBusStringA);
}

BOOL VMBUSAPI VmbusPipeClientEnumeratePipes(const GUID* ClassGuid, void* pContext, fnVmbusPipeClientEnumeratePipe pfn)
{
	BOOL bSuccess;
	char* pContextString;
	VmBusPipeEnumWrapper wrapper;

	log.Write("<VmbusPipeClientEnumeratePipes>");

	wrapper.pContext = pContext;
	wrapper.pfnVmbusPipeClientEnumeratePipe = pfn;

	if (!guest || !guest->VmbusPipeClientEnumeratePipes)
		return FALSE;

	pContextString = BinToHexString((BYTE*) pContext, 64);

	log.Write("VmbusPipeClientEnumeratePipes: ClassGuid: {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x} pContext: %p / %s pfn: %p",
		ClassGuid->Data1, ClassGuid->Data2, ClassGuid->Data3,
		ClassGuid->Data4[0], ClassGuid->Data4[1],
		ClassGuid->Data4[2], ClassGuid->Data4[3], ClassGuid->Data4[4],
		ClassGuid->Data4[5], ClassGuid->Data4[6], ClassGuid->Data4[7],
		pContext, pContextString, pfn);

	//bSuccess = guest->VmbusPipeClientEnumeratePipes(ClassGuid, (void*) &wrapper, VmbusPipeClientEnumeratePipe);
	bSuccess = guest->VmbusPipeClientEnumeratePipes(ClassGuid, pContext, pfn);

	log.Write("VmbusPipeClientEnumeratePipes: bSuccess: %d", bSuccess);

	free(pContextString);

	log.Write("</VmbusPipeClientEnumeratePipes>");

	return bSuccess;
}

HANDLE VMBUSAPI VmbusPipeClientOpenChannel(PVMBUS_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData, DWORD flags)
{
	HANDLE hChannel;
	char* VmBusStringA = NULL;
	WCHAR* VmBusStringW = NULL;

	log.Write("<VmbusPipeClientOpenChannel>");

	if (!guest || !guest->VmbusPipeClientOpenChannel)
		return INVALID_HANDLE_VALUE;

	hChannel = guest->VmbusPipeClientOpenChannel(pDeviceInterfaceDetailData, flags);

	VmBusStringW = (WCHAR*) pDeviceInterfaceDetailData->DevicePath;
	ConvertFromUnicode(CP_UTF8, 0, VmBusStringW, -1, &VmBusStringA, 0, NULL, NULL);

	log.Write("VmbusPipeClientOpenChannel: pDeviceInterfaceDetailData: cbSize: %d DevicePath: %s",
		pDeviceInterfaceDetailData->cbSize, VmBusStringA);
	log.Write("VmbusPipeClientOpenChannel: flags: 0x%04X", flags);

	free(VmBusStringA);

	log.Write("</VmbusPipeClientOpenChannel>");

	return hChannel;
}

void VMBUSAPI VmbusPipeClientReadyForChannelNotification(void* arg1, int flags)
{
	log.Write("<VmbusPipeClientReadyForChannelNotification>");

	if (!guest || !guest->VmbusPipeClientReadyForChannelNotification)
		return;

	guest->VmbusPipeClientReadyForChannelNotification(arg1, flags);

	log.Write("</VmbusPipeClientReadyForChannelNotification>");
}

LPVOID VMBUSAPI VmbusPipeClientRegisterChannelNotification(void* arg1, void* arg2, int arg3, int arg4, int arg5)
{
	LPVOID lptr;

	log.Write("<VmbusPipeClientRegisterChannelNotification>");

	if (!guest || !guest->VmbusPipeClientRegisterChannelNotification)
		return NULL;

	lptr = guest->VmbusPipeClientRegisterChannelNotification(arg1, arg2, arg3, arg4, arg5);

	log.Write("</VmbusPipeClientRegisterChannelNotification>");

	return lptr;
}

int VMBUSAPI VmbusPipeClientUnregisterChannelNotification(LPVOID arg1, int arg2)
{
	int status;

	log.Write("<VmbusPipeClientUnregisterChannelNotification>");

	if (!guest || !guest->VmbusPipeClientUnregisterChannelNotification)
		return -1;

	status = guest->VmbusPipeClientUnregisterChannelNotification(arg1, arg2);

	log.Write("</VmbusPipeClientUnregisterChannelNotification>");

	return status;
}

BOOL VMBUSAPI VmbusPipeClientWaitChannel(int arg1, int arg2, int arg3, UINT_PTR param)
{
	BOOL bSuccess;

	log.Write("<VmbusPipeClientWaitChannel>");

	if (!guest || !guest->VmbusPipeClientWaitChannel)
		return FALSE;

	bSuccess = guest->VmbusPipeClientWaitChannel(arg1, arg2, arg3, param);

	log.Write("</VmbusPipeClientWaitChannel>");

	return bSuccess;
}

BOOL VMBUSAPI VmbusPipeServerConnectPipe(HANDLE hDevice, LPOVERLAPPED lpOverlapped)
{
	BOOL bSuccess;

	log.Write("<VmbusPipeServerConnectPipe>");

	if (!guest || !guest->VmbusPipeServerConnectPipe)
		return FALSE;

	log.Write("\thDevice: %p lpOverlapped: %p", hDevice, lpOverlapped);

	bSuccess = guest->VmbusPipeServerConnectPipe(hDevice, lpOverlapped);

	log.Write("</VmbusPipeServerConnectPipe>");

	return bSuccess;
}

int VMBUSAPI VmbusPipeServerOfferChannel(PVMBUS_CHANNEL_INFO pVmbusChannelInfo, DWORD dwFlagsAndAttributes, PHANDLE phChannel)
{
	int status;
	GUID* pGuid;
	char* pBufferString;
	char* VmBusStringA = NULL;
	WCHAR* VmBusStringW = NULL;

	log.Write("<VmbusPipeServerOfferChannel>");

	if (!guest || !guest->VmbusPipeServerOfferChannel)
		return -1;

	pGuid = &(pVmbusChannelInfo->VirtualMachineId);
	pBufferString = BinToHexString((BYTE*) pVmbusChannelInfo->UserDefined, 116);

	log.Write("\tpGuid: {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		pGuid->Data1, pGuid->Data2, pGuid->Data3,
		pGuid->Data4[0], pGuid->Data4[1],
		pGuid->Data4[2], pGuid->Data4[3], pGuid->Data4[4],
		pGuid->Data4[5], pGuid->Data4[6], pGuid->Data4[7]);

	pGuid = (GUID*) &(pVmbusChannelInfo->field_14);

	log.Write("\tPOSSIBLE pGuid1: {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		pGuid->Data1, pGuid->Data2, pGuid->Data3,
		pGuid->Data4[0], pGuid->Data4[1],
		pGuid->Data4[2], pGuid->Data4[3], pGuid->Data4[4],
		pGuid->Data4[5], pGuid->Data4[6], pGuid->Data4[7]);

	pGuid = (GUID*) &(pVmbusChannelInfo->field_24);

	log.Write("\tPOSSIBLE pGuid2: {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		pGuid->Data1, pGuid->Data2, pGuid->Data3,
		pGuid->Data4[0], pGuid->Data4[1],
		pGuid->Data4[2], pGuid->Data4[3], pGuid->Data4[4],
		pGuid->Data4[5], pGuid->Data4[6], pGuid->Data4[7]);

	log.Write("\t field_14: 0x%04X field_18: 0x%04X field_1C: 0x%04X",
		pVmbusChannelInfo->field_14, pVmbusChannelInfo->field_18, pVmbusChannelInfo->field_1C);

	log.Write("\t field_20: 0x%04X field_24: 0x%04X field_28: 0x%04X",
		pVmbusChannelInfo->field_20, pVmbusChannelInfo->field_24, pVmbusChannelInfo->field_28);

	log.Write("\t field_2C: 0x%04X field_30: 0x%04X field_38: 0x%04X",
		pVmbusChannelInfo->field_2C, pVmbusChannelInfo->field_30, pVmbusChannelInfo->field_38);

	log.Write("\tunknown01: 0x%04X flags3A: 0x%04X",
		pVmbusChannelInfo->unknown01, pVmbusChannelInfo->flags3A);

	log.Write("\tdwFlagsAndAttributes: %04X phChannel: %p", dwFlagsAndAttributes, phChannel);

	log.Write("\tpBufferString: %s", pBufferString);

	status = guest->VmbusPipeServerOfferChannel(pVmbusChannelInfo, dwFlagsAndAttributes, phChannel);

	log.Write("</VmbusPipeServerOfferChannel>");

	free(VmBusStringA);

	return status;
}

const char* DLL_MAIN_REASONS[4] =
{
	"DLL_PROCESS_DETACH",
	"DLL_PROCESS_ATTACH",
	"DLL_THREAD_ATTACH",
	"DLL_THREAD_DETACH"
};

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	char logFileName[1024];
	char moduleFileName[1024];

	DisableThreadLibraryCalls(hinstDLL);

#ifndef VMBUS_PIPER_STUB
	sprintf_s(logFileName, sizeof(logFileName), "%s\\VmBusPipeStub%04X.txt",
		"C:\\Windows\\Temp", GetProcessId(GetCurrentProcess()));
#else
	sprintf_s(logFileName, sizeof(logFileName), "%s\\VmBusPiperStub%04X.txt",
		"C:\\Windows\\Temp", GetProcessId(GetCurrentProcess()));
#endif

	GetModuleFileNameA(NULL, moduleFileName, sizeof(moduleFileName));

	if (!guest)
	{
#ifndef VMBUS_PIPER_STUB
		guest = VmBusPipeGuestInit("vmbuspipe_.dll");
#else
		guest = VmBusPipeGuestInit("vmbuspiper_.dll");
#endif

		log.OpenFile(logFileName, true);
	}

	log.Write("DllMain: %s, hModule: %p, fdwReason: %s (%d)",
		moduleFileName, hinstDLL, DLL_MAIN_REASONS[fdwReason], fdwReason);

	if (fdwReason == DLL_THREAD_DETACH)
	{
		log.CloseFile();
	}

	return TRUE;
}

#ifdef __cplusplus
}
#endif
