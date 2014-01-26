
#include <stdio.h>
#include <stdlib.h>

#include "VmBusPipe.h"

VmBusPipeHost* VmBusPipeHostInit(const char* filename)
{
	HMODULE hModule;
	VmBusPipeHost* host;

	hModule = LoadLibraryA(filename ? filename : "vmbuspiper.dll");

	if (!hModule)
	{
		printf("LoadLibrary failure: %d\n", (int) GetLastError());
		return NULL;
	}

	host = (VmBusPipeHost*) malloc(sizeof(VmBusPipeHost));

	if (!host)
		return NULL;

	ZeroMemory(host, sizeof(VmBusPipeHost));

	host->VmBusPipeHostModule = hModule;

	host->VmbusPipeClientEnumeratePipes = (fnVmbusPipeClientEnumeratePipes) GetProcAddress(hModule, "VmbusPipeClientEnumeratePipes");
	host->VmbusPipeClientOpenChannel = (fnVmbusPipeClientOpenChannel) GetProcAddress(hModule, "VmbusPipeClientOpenChannel");
	host->VmbusPipeClientWaitChannel = (fnVmbusPipeClientWaitChannel) GetProcAddress(hModule, "VmbusPipeClientWaitChannel");
	host->VmbusPipeServerConnectPipe = (fnVmbusPipeServerConnectPipe) GetProcAddress(hModule, "VmbusPipeServerConnectPipe");
	host->VmbusPipeServerOfferChannel = (fnVmbusPipeServerOfferChannel) GetProcAddress(hModule, "VmbusPipeServerOfferChannel");

	return host;
}

void VmBusPipeHostFree(VmBusPipeHost* host)
{
	if (host)
	{
		FreeLibrary(host->VmBusPipeHostModule);
		free(host);
	}
}
