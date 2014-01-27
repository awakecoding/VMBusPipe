#ifndef VMBUS_PIPE_H
#define VMBUS_PIPE_H

#include <windows.h>

/* Based on SP_DEVICE_INTERFACE_DETAIL_DATA */

struct _VMBUS_DEVICE_INTERFACE_DETAIL_DATA
{
	DWORD cbSize;
	WCHAR DevicePath[260 + 1];
};
typedef struct _VMBUS_DEVICE_INTERFACE_DETAIL_DATA VMBUS_DEVICE_INTERFACE_DETAIL_DATA;
typedef struct _VMBUS_DEVICE_INTERFACE_DETAIL_DATA* PVMBUS_DEVICE_INTERFACE_DETAIL_DATA;

typedef void (CALLBACK * fnVmbusPipeClientEnumeratePipe)(void* pContext, BYTE* pUserDefined, PVMBUS_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData, char* arg4);
typedef BOOL (WINAPI * fnVmbusPipeClientEnumeratePipes)(const GUID* ClassGuid, void* pContext, fnVmbusPipeClientEnumeratePipe pfn);
typedef HANDLE (WINAPI * fnVmbusPipeClientOpenChannel)(PVMBUS_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData, DWORD flags);

typedef void (WINAPI * fnVmbusPipeClientReadyForChannelNotification)(void* arg1, int flags);
typedef LPVOID (WINAPI * fnVmbusPipeClientRegisterChannelNotification)(void* arg1, void* arg2, int arg3, int arg4, int arg5);
typedef int (WINAPI * fnVmbusPipeClientUnregisterChannelNotification)(LPVOID arg1, int arg2);

typedef BOOL (WINAPI * fnVmbusPipeClientWaitChannel)(int arg1, int arg2, int arg3, UINT_PTR param);
typedef BOOL (WINAPI * fnVmbusPipeServerConnectPipe)(HANDLE hDevice, LPOVERLAPPED lpOverlapped);
typedef int (WINAPI * fnVmbusPipeServerOfferChannel)(PVMBUS_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData, DWORD dwFlagsAndAttributes, PHANDLE phChannel);

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
