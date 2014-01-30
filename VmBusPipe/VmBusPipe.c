
#include "VmBusPipe.h"

DWORD VmbusPipeOfferChannelInternal(LPVOID lpInBuffer, DWORD dwIoControlCode, PHANDLE phHandle)
{
	HANDLE handle;
	BOOL bSuccess;
	DWORD error = 0;
	DWORD BytesReturned;

	handle = CreateFileW(L"\\\\.\\VMBus", 0x1F01FFu, 3, 0, 3, dwIoControlCode | 0x100000, 0);

	bSuccess = DeviceIoControl(handle, 0x3EC03Cu, lpInBuffer, 176, 0, 0, &BytesReturned, 0);

	if (bSuccess && (handle != INVALID_HANDLE_VALUE))
	{
		*phHandle = handle;
	}
	else
	{
		error = GetLastError();
	}

	if (error && (handle != INVALID_HANDLE_VALUE))
		CloseHandle(handle);

	return error;
}

