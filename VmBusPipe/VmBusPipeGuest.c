
#include <stdio.h>
#include <stdlib.h>

#include "VmBusPipe.h"

VmBusPipeGuest* VmBusPipeGuestInit()
{
	HMODULE hModule;
	VmBusPipeGuest* guest;

	hModule = LoadLibraryA("vmbuspipe.dll");

	if (!hModule)
	{
		printf("LoadLibrary failure: %d\n", (int) GetLastError());
		return NULL;
	}

	guest = (VmBusPipeGuest*) malloc(sizeof(VmBusPipeGuest));

	if (!guest)
		return NULL;

	ZeroMemory(guest, sizeof(VmBusPipeGuest));

	guest->VmBusPipeGuestModule = hModule;

	guest->VmbusPipeClientEnumeratePipes = (fnVmbusPipeClientEnumeratePipes) GetProcAddress(hModule, "VmbusPipeClientEnumeratePipes");
	guest->VmbusPipeClientOpenChannel = (fnVmbusPipeClientOpenChannel) GetProcAddress(hModule, "VmbusPipeClientOpenChannel");
	guest->VmbusPipeClientReadyForChannelNotification = (fnVmbusPipeClientReadyForChannelNotification) GetProcAddress(hModule, "VmbusPipeClientReadyForChannelNotification");
	guest->VmbusPipeClientRegisterChannelNotification = (fnVmbusPipeClientRegisterChannelNotification) GetProcAddress(hModule, "VmbusPipeClientRegisterChannelNotification");
	guest->VmbusPipeClientUnregisterChannelNotification = (fnVmbusPipeClientUnregisterChannelNotification) GetProcAddress(hModule, "VmbusPipeClientUnregisterChannelNotification");
	guest->VmbusPipeClientWaitChannel = (fnVmbusPipeClientWaitChannel) GetProcAddress(hModule, "VmbusPipeClientWaitChannel");
	guest->VmbusPipeServerConnectPipe = (fnVmbusPipeServerConnectPipe) GetProcAddress(hModule, "VmbusPipeServerConnectPipe");
	guest->VmbusPipeServerOfferChannel = (fnVmbusPipeServerOfferChannel) GetProcAddress(hModule, "VmbusPipeServerOfferChannel");

	return guest;
}

void VmBusPipeGuestFree(VmBusPipeGuest* guest)
{
	if (guest)
	{
		FreeLibrary(guest->VmBusPipeGuestModule);
		free(guest);
	}
}
