
#ifndef LOG_FILE_H
#define LOG_FILE_H

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>

#include <windows.h>

int ConvertToUnicode(UINT CodePage, DWORD dwFlags, LPCSTR lpMultiByteStr,
                int cbMultiByte, LPWSTR* lpWideCharStr, int cchWideChar);

int ConvertFromUnicode(UINT CodePage, DWORD dwFlags, LPCWSTR lpWideCharStr, int cchWideChar,
                LPSTR* lpMultiByteStr, int cbMultiByte, LPCSTR lpDefaultChar, LPBOOL lpUsedDefaultChar);

char* BinToHexString(BYTE* data, int length);

class CLogFile
{
public:
	CLogFile();
	~CLogFile();

	void OpenFile(LPCTSTR strFile, bool bAppend = TRUE, long lTruncate = 16384);
	void CloseFile();

	void Write(LPCTSTR pszFormat, ...);

private:
	void CreateDirectories(LPCTSTR filename);

	bool m_enabled;
	FILE* m_pLogFile;
	long m_lTruncate;
	CRITICAL_SECTION m_cs;
	char m_filename[MAX_PATH];
};

#endif
