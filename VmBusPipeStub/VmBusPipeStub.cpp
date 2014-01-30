
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

void VMBUSAPI VmbusPipeClientReadyForChannelNotification(VMBUS_CHANNEL_NOTIFICATION* pChannelNotification, BOOL bEnumeratePipes)
{
	log.Write("<VmbusPipeClientReadyForChannelNotification>");

	DWORD test = WM_CREATE;

	if (!guest || !guest->VmbusPipeClientReadyForChannelNotification)
		return;

	guest->VmbusPipeClientReadyForChannelNotification(pChannelNotification, bEnumeratePipes);

	log.Write("</VmbusPipeClientReadyForChannelNotification>");
}

VMBUS_CHANNEL_NOTIFICATION* VMBUSAPI VmbusPipeClientRegisterChannelNotification(GUID* ClassGuid, GUID* Guid2,
				DWORD notificationFlags, fnVmbusPipeClientChannelNotification pfnDispatchCallback, void* pDispatchContext)
{
	GUID* pGuid;
	VMBUS_CHANNEL_NOTIFICATION* pChannelNotification;

	log.Write("<VmbusPipeClientRegisterChannelNotification>");

	if (!guest || !guest->VmbusPipeClientRegisterChannelNotification)
		return NULL;

	pGuid = ClassGuid;

	log.Write("\tpClassGuid: {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		pGuid->Data1, pGuid->Data2, pGuid->Data3,
		pGuid->Data4[0], pGuid->Data4[1],
		pGuid->Data4[2], pGuid->Data4[3], pGuid->Data4[4],
		pGuid->Data4[5], pGuid->Data4[6], pGuid->Data4[7]);

	pGuid = Guid2;

	log.Write("\tpGuid2: {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		pGuid->Data1, pGuid->Data2, pGuid->Data3,
		pGuid->Data4[0], pGuid->Data4[1],
		pGuid->Data4[2], pGuid->Data4[3], pGuid->Data4[4],
		pGuid->Data4[5], pGuid->Data4[6], pGuid->Data4[7]);

	log.Write("\tnotificationFlags: 0x%04X", notificationFlags);

	log.Write("\tpfnDispatchCallback: %p pDispatchContext: %p", pfnDispatchCallback, pDispatchContext);

	/**
	 * Using Device Interfaces:
	 * http://msdn.microsoft.com/en-us/library/windows/hardware/ff545432/
	 */

	pChannelNotification = guest->VmbusPipeClientRegisterChannelNotification(ClassGuid, Guid2, notificationFlags,
		pfnDispatchCallback, pDispatchContext);

	log.Write("</VmbusPipeClientRegisterChannelNotification>");

	return pChannelNotification;
}

int VMBUSAPI VmbusPipeClientUnregisterChannelNotification(VMBUS_CHANNEL_NOTIFICATION* pChannelNotification, BOOL bWait)
{
	int status;

	log.Write("<VmbusPipeClientUnregisterChannelNotification>");

	if (!guest || !guest->VmbusPipeClientUnregisterChannelNotification)
		return -1;

	log.Write("\tbWait: %d", bWait);

	status = guest->VmbusPipeClientUnregisterChannelNotification(pChannelNotification, bWait);

	log.Write("</VmbusPipeClientUnregisterChannelNotification>");

	return status;
}

BOOL VMBUSAPI VmbusPipeClientWaitChannel(GUID* pGuid1, DWORD arg2, DWORD uElapse, UINT_PTR param)
{
	GUID* pGuid;
	BOOL bSuccess;

	log.Write("<VmbusPipeClientWaitChannel>");

	if (!guest || !guest->VmbusPipeClientWaitChannel)
		return FALSE;

	pGuid = pGuid1;

	log.Write("\tpGuid1: {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		pGuid->Data1, pGuid->Data2, pGuid->Data3,
		pGuid->Data4[0], pGuid->Data4[1],
		pGuid->Data4[2], pGuid->Data4[3], pGuid->Data4[4],
		pGuid->Data4[5], pGuid->Data4[6], pGuid->Data4[7]);

	log.Write("\targ2: 0x%4X uElapse: 0x%04X param: 0x%04X\n", arg2, uElapse, param);

	bSuccess = guest->VmbusPipeClientWaitChannel(pGuid1, arg2, uElapse, param);

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

	pGuid = &(pVmbusChannelInfo->VirtualMachineGuid);
	pBufferString = BinToHexString((BYTE*) pVmbusChannelInfo->UserDefined, 116);

	log.Write("\tpGuid: {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		pGuid->Data1, pGuid->Data2, pGuid->Data3,
		pGuid->Data4[0], pGuid->Data4[1],
		pGuid->Data4[2], pGuid->Data4[3], pGuid->Data4[4],
		pGuid->Data4[5], pGuid->Data4[6], pGuid->Data4[7]);

	/**
	 * GUID Present on the Hyper-V Host:
	 * {cfa8b69e-5b4a-4cc0-b98b-8ba1a1f3f95a} Interface Type?
	 * HKEY_LOCAL_MACHINE\SYSTEM\DriverDatabase\DeviceIds\VMBUS\{CFA8B69E-5B4A-4cc0-B98B-8BA1A1F3F95A}
	 */

	pGuid = (GUID*) &(pVmbusChannelInfo->InterfaceTypeGuid);

	log.Write("\tInterfaceTypeGuid: {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		pGuid->Data1, pGuid->Data2, pGuid->Data3,
		pGuid->Data4[0], pGuid->Data4[1],
		pGuid->Data4[2], pGuid->Data4[3], pGuid->Data4[4],
		pGuid->Data4[5], pGuid->Data4[6], pGuid->Data4[7]);

	/**
	 * GUID Present on the Hyper-V Guest:
	 * {58f75a6d-d949-4320-99e1-a2a2576d581c} Interface Instance?
	 */

	pGuid = (GUID*) &(pVmbusChannelInfo->InterfaceInstanceGuid);

	log.Write("\tInterfaceInstanceGuid: {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		pGuid->Data1, pGuid->Data2, pGuid->Data3,
		pGuid->Data4[0], pGuid->Data4[1],
		pGuid->Data4[2], pGuid->Data4[3], pGuid->Data4[4],
		pGuid->Data4[5], pGuid->Data4[6], pGuid->Data4[7]);

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
