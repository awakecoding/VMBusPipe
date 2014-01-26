
#include <stdio.h>
#include <stdlib.h>
#include <initguid.h>

#include <windows.h>

#include <VmBusPipe.h>

// {f9e9c0d3-b511-4a48-8046-d38079a8830c} "Microsoft Hyper-V Remote Desktop Data Channel"
DEFINE_GUID(GUID_VMBUS_RDP_DATA_CHANNEL, 0xf9e9c0d3, 0xb511, 0x4a48, 0x80, 0x46, 0xd3, 0x80, 0x79, 0xa8, 0x83, 0x0c);

// {1DAE6E2C-D5EF-4085-8C3C-00FC462F181A}
DEFINE_GUID(GUID_VMBUS_PIPE_TEST, 0x1dae6e2c, 0xd5ef, 0x4085, 0x8c, 0x3c, 0x0, 0xfc, 0x46, 0x2f, 0x18, 0x1a);

static BYTE RdvVmEndPointInstanceGuidUnifiedApi[16] =
	"\xCE\x7C\xE2\xAB\x02\x0E\x97\x41\x8E\x45\x5E\x92\x12\x4E\x1C\x2A";

static BYTE RdvVmEndPointInstanceGuidAllowList[16] =
	"\x67\x97\x16\x6E\xF8\x53\xB7\x49\xBF\xE1\xB4\x99\x16\x80\x57\xC4";

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

void VmbusPipeClientEnumeratePipe(DWORD arg1, BYTE* arg2, int* arg3, char* arg4)
{
	int length;
	char* VmBusStringA;
	WCHAR* VmBusStringW;

	printf("VmbusPipeClientEnumeratePipe: arg1: 0x%04X arg2: %p arg3: %p arg4: %p\n", arg1, arg2, arg3, arg4);

	/**
	 * arg1: observed value 0
	 * possibly the index in the pipe array
	 */

	/**
	 * Unknown buffer
	 */

	//printf("arg2:\n");
	//HexDump((BYTE*) arg2, 16 * 8);

	/**
	 * First 4 bytes: 80 00 00 00
	 * Following bytes is:
	 * \\?\vmbus#
	 * {f9e9c0d3-b511-4a48-8046-d38079a8830c}#
	 * {99221fa3-24ad-11e2-be98-001aa01bbf6e}#
	 * {f9e9c0d3-b511-4a48-8046-d38079a8830c}
	 */
	VmBusStringW = (WCHAR*) (arg3 + 1);
	ConvertFromUnicode(CP_UTF8, 0, VmBusStringW, -1, &VmBusStringA, 0, NULL, NULL);

	printf("arg3: 0x%04X / %s\n", *arg3, VmBusStringA);

	length = (int) strlen(arg4);
	printf("arg4: (length = %d)\n", length);
	HexDump((BYTE*) arg4, length);
}

int test_VmBusPipeHost(VmBusPipeHost* host)
{
	int status;
	BOOL bSuccess;
	HANDLE hChannel;
	DWORD dwFlagsAndAttributes;
	VMBUS_PIPE_CHANNEL_INFO* pChannelInfo;

	printf("VmBusPipeHost: %d\n", host ? 1 : 0);

	/**
	 * VmbusPipeServerOfferChannel
	 * VmbusPipeServerConnectPipe
	 */ 

	pChannelInfo = (VMBUS_PIPE_CHANNEL_INFO*) malloc(sizeof(VMBUS_PIPE_CHANNEL_INFO));
	ZeroMemory(pChannelInfo, sizeof(VMBUS_PIPE_CHANNEL_INFO));

	bSuccess = host->VmbusPipeClientEnumeratePipes(&GUID_VMBUS_RDP_DATA_CHANNEL, 0, VmbusPipeClientEnumeratePipe);

	printf("VmbusPipeClientEnumeratePipes: %d\n", bSuccess);

	pChannelInfo->flags3A = 0x1000;
	dwFlagsAndAttributes = 0;
	hChannel = NULL;

	status = host->VmbusPipeServerOfferChannel(pChannelInfo, dwFlagsAndAttributes, &hChannel);

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

char* BinToHexString(BYTE* data, int length)
{
	int i;
	char* p;
	int ln, hn;
	char bin2hex[] = "0123456789ABCDEF";

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

int test_VmBusPipeGuest(VmBusPipeGuest* guest)
{
	int status;
	BOOL bSuccess;
	HANDLE hChannel;
	DWORD dwFlagsAndAttributes;
	VMBUS_PIPE_CHANNEL_INFO* pChannelInfo;

	printf("VmBusPipeGuest: %d\n", guest ? 1 : 0);

	/**
	 * VmbusPipeClientOpenChannel
	 * VmbusPipeServerOfferChannel
	 * VmbusPipeServerConnectPipe
	 * VmbusPipeClientEnumeratePipes
	 */ 

	pChannelInfo = (VMBUS_PIPE_CHANNEL_INFO*) malloc(sizeof(VMBUS_PIPE_CHANNEL_INFO));
	ZeroMemory(pChannelInfo, sizeof(VMBUS_PIPE_CHANNEL_INFO));

	bSuccess = guest->VmbusPipeClientEnumeratePipes(&GUID_VMBUS_RDP_DATA_CHANNEL, 0, VmbusPipeClientEnumeratePipe);

	printf("VmbusPipeClientEnumeratePipes: %d\n", bSuccess);

	pChannelInfo->flags3A = 0x1000;
	dwFlagsAndAttributes = 0;
	hChannel = NULL;

	/**
	 * CreateFile dwFlagsAndAttributes:
	 * http://msdn.microsoft.com/en-us/library/windows/desktop/aa363858/
	 * FILE_FLAG_POSIX_SEMANTICS (0x0100000) is added
	 */

	status = guest->VmbusPipeServerOfferChannel(pChannelInfo, dwFlagsAndAttributes, &hChannel);

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

	/**
	 * First argument is a 526-byte structure
	 * Second argument is a set of flags, with an observed value of 0x40000000
	 */

	hChannel = guest->VmbusPipeClientOpenChannel(pChannelInfo, 0x40000000);

	printf("VmbusPipeClientOpenChannel: 0x%04X\n", hChannel);

	free(pChannelInfo);

	return 0;
}

int main(int argc, char** argv)
{
	BOOL bGuestMode;
	VmBusPipeHost* host;
	VmBusPipeGuest* guest;

	VMBUS_PIPE_CHANNEL_INFO* pChannelInfo;
	pChannelInfo = (VMBUS_PIPE_CHANNEL_INFO*) malloc(sizeof(VMBUS_PIPE_CHANNEL_INFO));
	ZeroMemory(pChannelInfo, sizeof(VMBUS_PIPE_CHANNEL_INFO));

	printf("test: %s\n", BinToHexString((BYTE*) pChannelInfo, sizeof(VMBUS_PIPE_CHANNEL_INFO)));

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
