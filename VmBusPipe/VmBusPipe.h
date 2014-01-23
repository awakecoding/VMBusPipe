#ifndef VMBUS_PIPE_H
#define VMBUS_PIPE_H

#include <windows.h>

typedef BOOL (WINAPI * fnVmbusPipeClientEnumeratePipes)(const GUID* ClassGuid, DWORD flags, int arg3);
typedef HANDLE (WINAPI * fnVmbusPipeClientOpenChannel)(void* arg1, int arg2);

typedef void (WINAPI * fnVmbusPipeClientReadyForChannelNotification)(void* arg1, int flags);
typedef LPVOID (WINAPI * fnVmbusPipeClientRegisterChannelNotification)(void* arg1, void* arg2, int arg3, int arg4, int arg5);
typedef int (WINAPI * fnVmbusPipeClientUnregisterChannelNotification)(LPVOID arg1, int arg2);

typedef BOOL (WINAPI * fnVmbusPipeClientWaitChannel)(int arg1, int arg2, int arg3, UINT_PTR param);
typedef BOOL (WINAPI * fnVmbusPipeServerConnectPipe)(HANDLE hDevice, LPOVERLAPPED lpOverlapped);
typedef int (WINAPI * fnVmbusPipeServerOfferChannel)(void* arg1, int arg2, int arg3);

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

VmBusPipeHost* VmBusPipeHostInit();
void VmBusPipeHostFree(VmBusPipeHost* host);

VmBusPipeGuest* VmBusPipeGuestInit();
void VmBusPipeGuestFree(VmBusPipeGuest* guest);

#endif
