#ifndef VMBUS_PIPE_H
#define VMBUS_PIPE_H

#include <windows.h>

struct _VMBUS_PIPE_CHANNEL_INFO
{
 	UINT32 field_0;
 	UINT32 field_4;
 	UINT32 field_8;
 	UINT32 field_C;
 	UINT32 field_10;
 	UINT32 field_14;
 	UINT32 field_18;
 	UINT32 field_1C;
 	UINT32 field_20;
 	UINT32 field_24;
 	UINT32 field_28;
 	UINT32 field_2C;
 	UINT32 field_30;
 	UINT32 field_34;
	UINT16 field_38;
	UINT16 flags3A;
	BYTE buffer[116];
 	UINT32 field_B0;
 	UINT32 field_B4;
 	UINT32 field_B8;
 	UINT32 field_BC;
 	UINT32 field_C0;
 	UINT32 field_C4;
 	UINT32 field_C8;
 	UINT32 field_CC;
 	UINT32 field_D0;
 	UINT32 field_D4;
 	UINT32 field_D8;
 	UINT32 field_DC;
 	UINT32 field_E0;
 	UINT32 field_E4;
 	UINT32 field_E8;
 	UINT32 field_EC;
 	UINT32 field_F0;
 	UINT32 field_F4;
 	UINT32 field_F8;
 	UINT32 field_FC;
 	UINT32 field_100;
 	UINT32 field_104;
 	UINT32 field_108;
 	UINT32 field_10C;
 	UINT32 field_110;
 	UINT32 field_114;
 	UINT32 field_118;
 	UINT32 field_11C;
 	UINT32 field_120;
 	UINT32 field_124;
 	UINT32 field_128;
 	UINT32 field_12C;
 	UINT32 field_130;
 	UINT32 field_134;
 	UINT32 field_138;
 	UINT32 field_13C;
 	UINT32 field_140;
 	UINT32 field_144;
 	UINT32 field_148;
 	UINT32 field_14C;
 	UINT32 field_150;
 	UINT32 field_154;
 	UINT32 field_158;
 	UINT32 field_15C;
 	UINT32 field_160;
 	UINT32 field_164;
 	UINT32 field_168;
 	UINT32 field_16C;
 	UINT32 field_170;
 	UINT32 field_174;
 	UINT32 field_178;
 	UINT32 field_17C;
 	UINT32 field_180;
 	UINT32 field_184;
 	UINT32 field_188;
 	UINT32 field_18C;
 	UINT32 field_190;
 	UINT32 field_194;
 	UINT32 field_198;
 	UINT32 field_19C;
 	UINT32 field_1A0;
 	UINT32 field_1A4;
 	UINT32 field_1A8;
 	UINT32 field_1AC;
 	UINT32 field_1B0;
 	UINT32 field_1B4;
 	UINT32 field_1B8;
 	UINT32 field_1BC;
 	UINT32 field_1C0;
 	UINT32 field_1C4;
 	UINT32 field_1C8;
 	UINT32 field_1CC;
 	UINT32 field_1D0;
 	UINT32 field_1D4;
 	UINT32 field_1D8;
 	UINT32 field_1DC;
 	UINT32 field_1E0;
 	UINT32 field_1E4;
 	UINT32 field_1E8;
 	UINT32 field_1EC;
 	UINT32 field_1F0;
 	UINT32 field_1F4;
 	UINT32 field_1F8;
 	UINT32 field_1FC;
 	UINT32 field_200;
 	UINT32 field_204;
 	UINT32 field_208;
 	UINT32 field_20C;
	UINT32 field_210;
};
typedef struct _VMBUS_PIPE_CHANNEL_INFO VMBUS_PIPE_CHANNEL_INFO;

typedef void (WINAPI * fnVmbusPipeClientEnumeratePipe)(DWORD arg1, BYTE* arg2, int* arg3, char* arg4);
typedef BOOL (WINAPI * fnVmbusPipeClientEnumeratePipes)(const GUID* ClassGuid, DWORD flags, void* pfn);
typedef HANDLE (WINAPI * fnVmbusPipeClientOpenChannel)(void* arg1, int arg2);

typedef void (WINAPI * fnVmbusPipeClientReadyForChannelNotification)(void* arg1, int flags);
typedef LPVOID (WINAPI * fnVmbusPipeClientRegisterChannelNotification)(void* arg1, void* arg2, int arg3, int arg4, int arg5);
typedef int (WINAPI * fnVmbusPipeClientUnregisterChannelNotification)(LPVOID arg1, int arg2);

typedef BOOL (WINAPI * fnVmbusPipeClientWaitChannel)(int arg1, int arg2, int arg3, UINT_PTR param);
typedef BOOL (WINAPI * fnVmbusPipeServerConnectPipe)(HANDLE hDevice, LPOVERLAPPED lpOverlapped);
typedef int (WINAPI * fnVmbusPipeServerOfferChannel)(void* arg1, DWORD dwFlagsAndAttributes, int arg3);

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
