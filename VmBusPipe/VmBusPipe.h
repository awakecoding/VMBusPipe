#ifndef VMBUS_PIPE_H
#define VMBUS_PIPE_H

#include <windows.h>
#include <cfgmgr32.h>

#define VMBUSAPI __stdcall

/*
 * Based on CM_NOTIFY_FILTER
 * 416 bytes in 32-bit
 */

struct _VMBUS_CM_NOTIFY_FILTER
{
	DWORD cbSize;
	DWORD Flags;
	CM_NOTIFY_FILTER_TYPE FilterType;
	DWORD Reserved;
	const GUID ClassGuid;
	DWORD data[96];
};
typedef struct _VMBUS_CM_NOTIFY_FILTER VMBUS_CM_NOTIFY_FILTER;

struct _VMBUS_CHANNEL_NOTIFICATION
{
	DWORD flags00;
	HMODULE hModuleCfgmgr32;
	HMODULE hModuleKernel32;
	HANDLE CmNotificationHandle;
	void* pContext;
	GUID* pClassGuid;
	GUID* pGuid2;
	DWORD field_1C;
	DWORD field_20;
	DWORD field_24;
	DWORD field_28;
	DWORD context[130];
	DWORD field_234;
	DWORD field_238;
	const GUID ClassGuid;
	const GUID Guid2;
	DWORD notificationFlags;
	CRITICAL_SECTION lock01;
	DWORD ptrOrBool278;
	DWORD bBool27C;
	DWORD bThreadpoolWorkSubmitted;
	DWORD interlockedExch01;
	DWORD field_288;
	DWORD dispatchFlags;
	void* pfnCM_Register_Notification;
	void* pfnCM_Unregister_Notification;
	void* pfnCreateThreadpoolWork;
	void* pfnSubmitThreadpoolWork;
	void* pfnWaitForThreadpoolWorkCallbacks;
	void* pfnCloseThreadpoolWork;
	PTP_WORK pWorkObject;
	CRITICAL_SECTION lock02;
	void* pfnDispatchCallback;
	void* pDispatchContext;
};
typedef struct _VMBUS_CHANNEL_NOTIFICATION VMBUS_CHANNEL_NOTIFICATION;

/* Based on SP_DEVICE_INTERFACE_DETAIL_DATA */

struct _VMBUS_DEVICE_INTERFACE_DETAIL_DATA
{
	DWORD cbSize;
	WCHAR DevicePath[260 + 1];
};
typedef struct _VMBUS_DEVICE_INTERFACE_DETAIL_DATA VMBUS_DEVICE_INTERFACE_DETAIL_DATA;
typedef struct _VMBUS_DEVICE_INTERFACE_DETAIL_DATA* PVMBUS_DEVICE_INTERFACE_DETAIL_DATA;

struct _VMBUS_CHANNEL_INFO
{
	GUID VirtualMachineGuid;
	DWORD unknown01;
	GUID InterfaceTypeGuid;
	GUID InterfaceInstanceGuid;
	PHANDLE phChannel;
	UINT16 field_38;
	UINT16 flags3A;
	BYTE UserDefined[116];
};
typedef struct _VMBUS_CHANNEL_INFO VMBUS_CHANNEL_INFO;
typedef struct _VMBUS_CHANNEL_INFO* PVMBUS_CHANNEL_INFO;

typedef void (VMBUSAPI * fnVmbusPipeClientEnumeratePipe)(void* pContext, BYTE* pUserDefined, PVMBUS_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData, const GUID* pGuid);
typedef void (VMBUSAPI * fnVmbusPipeClientChannelNotification)(void* pDispatchContext, void* pContext, DWORD notificationFlags);

typedef BOOL (VMBUSAPI * fnVmbusPipeClientEnumeratePipes)(const GUID* ClassGuid, void* pContext, fnVmbusPipeClientEnumeratePipe pfn);
typedef HANDLE (VMBUSAPI * fnVmbusPipeClientOpenChannel)(PVMBUS_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData, DWORD flags);

typedef void (VMBUSAPI * fnVmbusPipeClientReadyForChannelNotification)(VMBUS_CHANNEL_NOTIFICATION* pChannelNotification, BOOL bEnumeratePipes);
typedef VMBUS_CHANNEL_NOTIFICATION* (VMBUSAPI * fnVmbusPipeClientRegisterChannelNotification)(GUID* ClassGuid, GUID* Guid2, DWORD notificationFlags,
											      fnVmbusPipeClientChannelNotification pfnDispatchCallback, void* pDispatchContext);
typedef int (VMBUSAPI * fnVmbusPipeClientUnregisterChannelNotification)(VMBUS_CHANNEL_NOTIFICATION* pChannelNotification, BOOL bWait);

typedef BOOL (VMBUSAPI * fnVmbusPipeClientWaitChannel)(GUID* pGuid1, DWORD arg2, DWORD uElapse, UINT_PTR param);
typedef BOOL (VMBUSAPI * fnVmbusPipeServerConnectPipe)(HANDLE hDevice, LPOVERLAPPED lpOverlapped);
typedef int (VMBUSAPI * fnVmbusPipeServerOfferChannel)(PVMBUS_CHANNEL_INFO pVmbusChannelInfo, DWORD dwFlagsAndAttributes, PHANDLE phChannel);

struct _VmBusPipeHost
{
	HMODULE VmBusPipeHostModule;
	fnVmbusPipeClientEnumeratePipes VmbusPipeClientEnumeratePipes;
	fnVmbusPipeClientOpenChannel VmbusPipeClientOpenChannel;
	fnVmbusPipeClientWaitChannel VmbusPipeClientWaitChannel;
	fnVmbusPipeServerConnectPipe VmbusPipeServerConnectPipe;
	fnVmbusPipeServerOfferChannel VmbusPipeServerOfferChannel;
};
typedef struct _VmBusPipeHost VmBusPipeHost;

struct _VmBusPipeGuest
{
	HMODULE VmBusPipeGuestModule;
	fnVmbusPipeClientEnumeratePipes VmbusPipeClientEnumeratePipes;
	fnVmbusPipeClientOpenChannel VmbusPipeClientOpenChannel;
	fnVmbusPipeClientReadyForChannelNotification VmbusPipeClientReadyForChannelNotification;
	fnVmbusPipeClientRegisterChannelNotification VmbusPipeClientRegisterChannelNotification;
	fnVmbusPipeClientUnregisterChannelNotification VmbusPipeClientUnregisterChannelNotification;
	fnVmbusPipeClientWaitChannel VmbusPipeClientWaitChannel;
	fnVmbusPipeServerConnectPipe VmbusPipeServerConnectPipe;
	fnVmbusPipeServerOfferChannel VmbusPipeServerOfferChannel;
};
typedef struct _VmBusPipeGuest VmBusPipeGuest;

#ifdef __cplusplus
extern "C" {
#endif

VmBusPipeHost* VmBusPipeHostInit(const char* filename);
void VmBusPipeHostFree(VmBusPipeHost* host);

VmBusPipeGuest* VmBusPipeGuestInit(const char* filename);
void VmBusPipeGuestFree(VmBusPipeGuest* guest);

#ifdef __cplusplus
}
#endif

#endif /* VMBUS_PIPE_H */
