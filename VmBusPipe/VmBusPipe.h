#ifndef VMBUS_PIPE_H
#define VMBUS_PIPE_H

#include <windows.h>

#define VMBUSAPI __stdcall

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
	GUID VirtualMachineId;
	DWORD unknown01;
	
	DWORD field_14;
	DWORD field_18;
	DWORD field_1C;
	DWORD field_20;

	DWORD field_24;
	DWORD field_28;
	DWORD field_2C;
	DWORD field_30;
	
	PHANDLE phChannel;
	UINT16 field_38;
	UINT16 flags3A;
	BYTE UserDefined[116];
};
typedef struct _VMBUS_CHANNEL_INFO VMBUS_CHANNEL_INFO;
typedef struct _VMBUS_CHANNEL_INFO* PVMBUS_CHANNEL_INFO;

typedef void (VMBUSAPI * fnVmbusPipeClientEnumeratePipe)(void* pContext, BYTE* pUserDefined, PVMBUS_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData, const GUID* pGuid);
typedef BOOL (VMBUSAPI * fnVmbusPipeClientEnumeratePipes)(const GUID* ClassGuid, void* pContext, fnVmbusPipeClientEnumeratePipe pfn);
typedef HANDLE (VMBUSAPI * fnVmbusPipeClientOpenChannel)(PVMBUS_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData, DWORD flags);

typedef void (VMBUSAPI * fnVmbusPipeClientReadyForChannelNotification)(void* arg1, int flags);
typedef LPVOID (VMBUSAPI * fnVmbusPipeClientRegisterChannelNotification)(void* arg1, void* arg2, int arg3, int arg4, int arg5);
typedef int (VMBUSAPI * fnVmbusPipeClientUnregisterChannelNotification)(LPVOID arg1, int arg2);

typedef BOOL (VMBUSAPI * fnVmbusPipeClientWaitChannel)(int arg1, int arg2, int arg3, UINT_PTR param);
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
