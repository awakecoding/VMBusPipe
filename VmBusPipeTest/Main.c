
#include <stdio.h>
#include <stdlib.h>
#include <initguid.h>

#include <windows.h>

#include <VmBusPipe.h>

struct _VmBusPipeContext
{
	BOOL bGuestMode;
	VmBusPipeHost* host;
	VmBusPipeGuest* guest;
};
typedef struct _VmBusPipeContext VmBusPipeContext;

static VmBusPipeContext* g_VmBusPipeContext;

// {f9e9c0d3-b511-4a48-8046-d38079a8830c} "Microsoft Hyper-V Remote Desktop Data Channel"
DEFINE_GUID(GUID_VMBUS_REMOTE_DESKTOP_DATA_CHANNEL, 0xf9e9c0d3, 0xb511, 0x4a48, 0x80, 0x46, 0xd3, 0x80, 0x79, 0xa8, 0x83, 0x0c);

// {276aacf4-ac15-426c-98dd-7521ad3f01fe} "Microsoft Hyper-V Remote Desktop Virtualization"
DEFINE_GUID(GUID_VMBUS_REMOTE_DESKTOP_VIRTUALIZATION, 0x276aacf4, 0xac15, 0x426c, 0x98, 0xdd, 0x75, 0x21, 0xad, 0x3f, 0x01, 0xfe);

// {57164f39-9115-4e78-ab55-382f3bd5422d} "Microsoft Hyper-V Heartbeat"
DEFINE_GUID(GUID_VMBUS_HEARTBEAT, 0x57164f39, 0x9115, 0x4e78, 0xab, 0x55, 0x38, 0x2f, 0x3b, 0xd5, 0x42, 0x2d);

// {1DAE6E2C-D5EF-4085-8C3C-00FC462F181A}
DEFINE_GUID(GUID_VMBUS_PIPE_TEST, 0x1dae6e2c, 0xd5ef, 0x4085, 0x8c, 0x3c, 0x0, 0xfc, 0x46, 0x2f, 0x18, 0x1a);

//DEFINE_GUID(GUID_VMBUS_, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x, 0x);

// {7563c5a0-025e-4a49-ad24-df8341a577bd} "Windows Server 2012 R2" Virtual Machine Id
DEFINE_GUID(GUID_VIRTUAL_MACHINE_ID, 0x7563c5a0, 0x025e, 0x4a49, 0xad, 0x24, 0xdf, 0x83, 0x41, 0xa5, 0x77, 0xbd);

void HexDump(BYTE* data, int length)
{
	BYTE* p = data;
	int i, line, offset = 0;

	while (offset < length)
	{
		fprintf(stderr, "%04x ", offset);

		line = length - offset;

		if (line > 16)
			line = 16;

		for (i = 0; i < line; i++)
			fprintf(stderr, "%02x ", p[i]);

		for (; i < 16; i++)
			fprintf(stderr, "   ");

		for (i = 0; i < line; i++)
			fprintf(stderr, "%c", (p[i] >= 0x20 && p[i] < 0x7F) ? p[i] : '.');

		fprintf(stderr, "\n");

		offset += line;
		p += line;
	}
}

char* BinToHexString(BYTE* data, int length)
{
	int i;
	char* p;
	int ln, hn;
	char bin2hex[] = "0123456789abcdef";

	p = (char*) malloc((length + 1) * 2);

	for (i = 0; i < length; i++)
	{
		ln = data[i] & 0xF;
		hn = (data[i] >> 4) & 0xF;
		p[i * 2] = bin2hex[hn];
		p[(i * 2) + 1] = bin2hex[ln];
	}

	p[length * 2] = '\0';

	return p;
}

int ConvertToUnicode(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr,
                int cbMultiByte, LPWSTR* lpWideCharStr, int cchWideChar)
{
        int status;
        BOOL allocate = FALSE;

        if (!lpMultiByteStr)
                return 0;

        if (!lpWideCharStr)
                return 0;

        if (cbMultiByte == -1)
                cbMultiByte = (int) strlen(lpMultiByteStr) + 1;

        if (cchWideChar == 0)
        {
                cchWideChar = MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, NULL, 0);
                allocate = TRUE;
        }

        if (cchWideChar < 1)
                return 0;

        if (!(*lpWideCharStr))
                allocate = TRUE;

        if (allocate)
                *lpWideCharStr = (LPWSTR) malloc(cchWideChar * sizeof(WCHAR));

        status = MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, *lpWideCharStr, cchWideChar);

        if (status != cchWideChar)
                status = 0;

        return status;
}

int ConvertFromUnicode(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar,
                LPSTR* lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar)
{
        int status;
        BOOL allocate = FALSE;

        if (!lpWideCharStr)
                return 0;

        if (!lpMultiByteStr)
                return 0;

        if (cchWideChar == -1)
                cchWideChar = (int) wcslen(lpWideCharStr) + 1;

        if (cbMultiByte == 0)
        {
                cbMultiByte = WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, NULL, 0, NULL, NULL);
                allocate = TRUE;
        }

        if (cbMultiByte < 1)
                return 0;

        if (!(*lpMultiByteStr))
                allocate = TRUE;

        if (allocate)
        {
                *lpMultiByteStr = (LPSTR) malloc(cbMultiByte + 1);
                ZeroMemory(*lpMultiByteStr, cbMultiByte + 1);
        }

        status = WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar,
                        *lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar);

        if (status != cbMultiByte)
                status = 0;

        if ((status <= 0) && allocate)
        {
                free(*lpMultiByteStr);
                *lpMultiByteStr = NULL;
        }

        return status;
}

static void VMBUSAPI VmbusPipeClientEnumeratePipe(void* pContext, BYTE* pUserDefined, PVMBUS_DEVICE_INTERFACE_DETAIL_DATA pDeviceInterfaceDetailData, const GUID* pGuid)
{
	HANDLE hChannel;
	char* VmBusStringA = NULL;
	WCHAR* VmBusStringW = NULL;
	char* pUserDefinedString = NULL;
	VmBusPipeContext* vmbus = (VmBusPipeContext*) pContext;

	/**
	 * pUserDefined: 116-byte UserDefined registry key
	 *
	 * pDeviceInterfaceDetailData: pointer to SP_DEVICE_INTERFACE_DETAIL_DATA (526-byte)
	 * http://msdn.microsoft.com/en-us/library/windows/hardware/ff552343/
	 */

	printf("VmbusPipeClientEnumeratePipe: pContext: %p\n", pContext);

	pUserDefinedString = BinToHexString(pUserDefined, 116);

	printf("\tpUserDefined: %s\n", pUserDefinedString);

	VmBusStringW = (WCHAR*) pDeviceInterfaceDetailData->DevicePath;
	ConvertFromUnicode(CP_UTF8, 0, VmBusStringW, -1, &VmBusStringA, 0, NULL, NULL);

	printf("\tpDeviceInterfaceDetailData: cbSize: %d DevicePath: %s\n",
		pDeviceInterfaceDetailData->cbSize, VmBusStringA);

	printf("\tpGuid: {%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}\n",
		pGuid->Data1, pGuid->Data2, pGuid->Data3,
		pGuid->Data4[0], pGuid->Data4[1],
		pGuid->Data4[2], pGuid->Data4[3], pGuid->Data4[4],
		pGuid->Data4[5], pGuid->Data4[6], pGuid->Data4[7]);

	free(pUserDefinedString);
	free(VmBusStringA);

	if (vmbus->bGuestMode)
	{
		hChannel = vmbus->guest->VmbusPipeClientOpenChannel(pDeviceInterfaceDetailData, 0x40000000);
	}
	else
	{
		hChannel = vmbus->host->VmbusPipeClientOpenChannel(pDeviceInterfaceDetailData, 0x40000000);
	}

	printf("VmbusPipeClientOpenChannel: 0x%04X\n", hChannel);
}

int test_VmBusPipeHost(VmBusPipeHost* host)
{
	int status;
	BOOL bSuccess;
	HANDLE hChannel;
	DWORD dwFlagsAndAttributes;
	PVMBUS_CHANNEL_INFO pVmChannelInfo;

	printf("VmBusPipeHost: %d\n", host ? 1 : 0);

	/**
	 * VmbusPipeServerOfferChannel
	 * VmbusPipeServerConnectPipe
	 */ 

	pVmChannelInfo = (PVMBUS_CHANNEL_INFO) malloc(sizeof(VMBUS_CHANNEL_INFO));
	ZeroMemory(pVmChannelInfo, sizeof(VMBUS_CHANNEL_INFO));

	bSuccess = host->VmbusPipeClientEnumeratePipes(&GUID_VMBUS_REMOTE_DESKTOP_VIRTUALIZATION,
		g_VmBusPipeContext, VmbusPipeClientEnumeratePipe);

	printf("VmbusPipeClientEnumeratePipes: %d\n", bSuccess);

	hChannel = NULL;
	dwFlagsAndAttributes = 0x40000000;

	CopyMemory(&(pVmChannelInfo->VirtualMachineId), &GUID_VIRTUAL_MACHINE_ID, sizeof(GUID));

	status = host->VmbusPipeServerOfferChannel(pVmChannelInfo, dwFlagsAndAttributes, &hChannel);

	printf("VmbusPipeServerOfferChannel: %d\n", status);

	if (status < 0)
	{
		DWORD dwIoControlCode = 0x3EC03C;

		/**
		 * Host: ERROR_INVALID_FUNCTION (1)
		 * DeviceIoControl is called with dwIoControlCode 0x3EC03C
		 * 
		 * DeviceType: 0x003E (FILE_DEVICE_VMBUS)
		 * Access: 0x3
		 * Function: 0xF
		 * Method: 0
		 */

		printf("VmbusPipeServerOfferChannel: GetLastError() = %d\n", GetLastError());
	}

	return 0;
}

int test_VmBusPipeGuest(VmBusPipeGuest* guest)
{
	//int status;
	BOOL bSuccess;
	HANDLE hChannel;
	DWORD dwFlagsAndAttributes;
	PVMBUS_CHANNEL_INFO pVmChannelInfo;

	printf("VmBusPipeGuest: %d\n", guest ? 1 : 0);

	/**
	 * VmbusPipeClientOpenChannel
	 * VmbusPipeServerOfferChannel
	 * VmbusPipeServerConnectPipe
	 * VmbusPipeClientEnumeratePipes
	 */ 

	pVmChannelInfo = (PVMBUS_CHANNEL_INFO) malloc(sizeof(VMBUS_CHANNEL_INFO));
	ZeroMemory(pVmChannelInfo, sizeof(VMBUS_CHANNEL_INFO));

	bSuccess = guest->VmbusPipeClientEnumeratePipes(&GUID_VMBUS_REMOTE_DESKTOP_VIRTUALIZATION,
		g_VmBusPipeContext, VmbusPipeClientEnumeratePipe);

	printf("VmbusPipeClientEnumeratePipes: %d\n", bSuccess);

	dwFlagsAndAttributes = 0;
	hChannel = NULL;

#if 0
	/**
	 * CreateFile dwFlagsAndAttributes:
	 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa363858/
	 * FILE_FLAG_POSIX_SEMANTICS (0x0100000) is added
	 */

	status = guest->VmbusPipeServerOfferChannel(pDeviceInterfaceDetailData, dwFlagsAndAttributes, &hChannel);

	printf("VmbusPipeServerOfferChannel: %d\n", status);

	if (status < 0)
	{
		/**
		 * Guest: ERROR_FILE_NOT_FOUND (2)
		 *
		 * Host: ERROR_ACCESS_DENIED (5)
		 * Host + Admin: ERROR_INVALID_FUNCTION (1)
		 * ERROR_NOT_SUPPORTED (50)
		 */

		printf("VmbusPipeServerOfferChannel: GetLastError() = %d\n", GetLastError());
	}
#endif

	return 0;
}

int main(int argc, char** argv)
{
	BOOL bGuestMode;
	VmBusPipeHost* host;
	VmBusPipeGuest* guest;

	bGuestMode = FALSE;
	host = VmBusPipeHostInit(NULL);
	guest = VmBusPipeGuestInit(NULL);

	if (argc > 1)
	{
		if (strcmp(argv[1], "guest") == 0)
		{
			bGuestMode = TRUE;
		}
	}

	g_VmBusPipeContext = (VmBusPipeContext*) malloc(sizeof(VmBusPipeContext));
	g_VmBusPipeContext->bGuestMode = bGuestMode;
	g_VmBusPipeContext->host = host;
	g_VmBusPipeContext->guest = guest;

	if (host && !bGuestMode)
	{
		test_VmBusPipeHost(host);
	}
	else if (guest && bGuestMode)
	{
		test_VmBusPipeGuest(guest);
	}
	else
	{
		printf("VmBusPipe not found!\n");
	}

	VmBusPipeHostFree(host);
	VmBusPipeGuestFree(guest);

	return 0;
}
