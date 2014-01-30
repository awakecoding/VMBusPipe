
#include "Helpers.h"

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

/* Logger Class */

CLogFile::CLogFile()
{
	m_enabled = true;
}

CLogFile::~CLogFile()
{
	if (m_enabled)
		CloseFile();
}

void CLogFile::OpenFile(LPCTSTR strFile, bool bAppend, long lTruncate)
{
	m_lTruncate = lTruncate;

	if (!m_enabled)
		return;

	memcpy(m_filename, strFile, _tcslen(strFile));

	TCHAR szFile[MAX_PATH + 1];

	if (_tcslen(strFile) > 3 && strFile[1] != ':')
	{
		GetModuleFileName(NULL, szFile, MAX_PATH);
		
		long llength = (long) _tcslen(szFile);
		TCHAR* pcat = szFile + (llength - 1);

		while (llength--)
		{
			pcat--;

			if (*pcat == '\\')
				break;
		}

		if (*pcat == '\\')
		{
			pcat++;
			_tcscpy(pcat, strFile);
		}
		else
		{
			_tcscpy(szFile, strFile);
		}
	}
	else
	{
		_tcscpy(szFile, strFile);
	}

	m_pLogFile = fopen(szFile, bAppend ? "a" : "w");

	if (!m_pLogFile)
	{
		CreateDirectories(szFile);
		m_pLogFile = fopen(szFile, bAppend ? "a" : "w");
	}

	InitializeCriticalSection(&m_cs);
}

void CLogFile::CloseFile()
{
	if (!m_enabled)
		return;

	if (m_pLogFile)
	{
		fclose(m_pLogFile);
	}

	DeleteCriticalSection(&m_cs);
}

void CLogFile::CreateDirectories(LPCTSTR filename)
{
	char drivename[4];
	char path[MAX_PATH + 1];
	char name[MAX_PATH + 1];
	char ext[MAX_PATH + 1];
	char seps[] = "/\\";
	char* token;

	if (!m_enabled)
		return;

	_splitpath(filename, drivename, path, name, ext);

	sprintf(drivename, "%s\\", drivename);
	_chdir(drivename);

	token = strtok(path, seps);

	while (token != NULL)
	{
		if (_chdir(token) == -1)	
		{
			_mkdir(token);
			_chdir(token);
		}

		token = strtok(NULL, seps);
	}
}

void CLogFile::Write(LPCTSTR pszFormat, ...)
{
	if (!m_enabled)
		return;

	if (!m_pLogFile)
		return;

	EnterCriticalSection(&m_cs);
	
	TCHAR szLog[4096];
	va_list argList;
	va_start(argList, pszFormat);
	vsprintf(szLog, pszFormat, argList);
	va_end(argList);

	SYSTEMTIME time;
	::GetLocalTime(&time);
	TCHAR szLine[256];

	sprintf(szLine, "%04d/%02d/%02d %02d:%02d:%02d: %s\n",
		time.wYear, time.wMonth, time.wDay,
		time.wHour, time.wMinute, time.wSecond, szLog);

	fputs(szLine, m_pLogFile);

	fflush(m_pLogFile);

	LeaveCriticalSection(&m_cs);
}
