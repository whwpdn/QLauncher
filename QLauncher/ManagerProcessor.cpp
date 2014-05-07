#include "ManageProcessor.h"

#include <windows.h>
#include <TlHelp32.h>


//------------------------------------------------------------------------------
// wchar -> char
void WCharToChar(const wchar_t* pwstrSrc, char* pChar, int iSize)
{
    if (! pwstrSrc)
		return;

//#if !defined _DEBUG
//    int len = 0;
//    len = (wcslen(pwstrSrc) + 1)*2;
//    char* pstr      = (char*) malloc ( sizeof( char) * len);
//
//    WideCharToMultiByte( 949, 0, pwstrSrc, -1, pstr, len, NULL, NULL);
//#else

	memset(pChar, 0, sizeof(char) * iSize);

    int nLen = wcslen(pwstrSrc);

	if (nLen > iSize)
		return;

    //char* pstr      = (char*) malloc ( sizeof( char) * nLen + 1);
    wcstombs(pChar, pwstrSrc, nLen+1);
//#endif
}
//------------------------------------------------------------------------------
void CharToWChar(const char* pstrSrc, wchar_t* pWChar, int iSize)
{
    //ASSERT(pstrSrc);
    int nLen = strlen(pstrSrc)+1;

    //wchar_t* pwstr      = (LPWSTR) malloc ( sizeof( wchar_t )* nLen);
    mbstowcs(pWChar, pstrSrc, nLen);

    //return pwstr;
}
//------------------------------------------------------------------------------
bool ExecuteProcessor(const string& strPath)
{
	DWORD dwSize = 250;
	HANDLE hSnapShot;
	PROCESSENTRY32 pEntry;
	bool bCurrent = false;

	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	pEntry.dwSize = sizeof(pEntry);
	Process32First(hSnapShot, &pEntry);

	char aExeFileStr[100];
	while(1)
	{
		bool bRes = Process32Next(hSnapShot, &pEntry);
		if(bRes == false)
			break;

		WCharToChar(pEntry.szExeFile, aExeFileStr, 100);

		if(!strncmp(aExeFileStr, strPath.c_str(), 15))
		{
			bCurrent = true;
			break;
		}
	}

	if(!bCurrent)
	{
		wchar_t aExeFileName[100];
		CharToWChar(strPath.c_str(), aExeFileName, 100);

		wchar_t aCmd[100];
		CharToWChar("open", aCmd, 100);

		//std::wstring wsTmp(strPath.begin(), strPath.end());

		//SendMsg(NETMSG_CREATE_ALL, "NULL");
		SHELLEXECUTEINFO sei;
		memset(&sei, 0, sizeof(SHELLEXECUTEINFO));
		sei.cbSize = sizeof(SHELLEXECUTEINFO);
		sei.lpFile = aExeFileName;
		sei.nShow = SW_SHOW;
		sei.fMask = SEE_MASK_INVOKEIDLIST;
		sei.lpVerb = aCmd;
		bool bRes = ShellExecuteEx(&sei);
		//ShellExecute(NULL, (LPCWSTR)"open", (LPCWSTR)strPath.c_str(), NULL, NULL, SW_SHOWNORMAL);

		DWORD aError = GetLastError();
		
		return true;
	}
	else
		return false;
}
//------------------------------------------------------------------------------
bool TerminateProcessor(const string& strPath)
{
	DWORD dwSize = 250;
	HANDLE hSnapShot;
	PROCESSENTRY32 pEntry;
	bool bCurrent = false;
	
	hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
	pEntry.dwSize = sizeof(pEntry);
	Process32First(hSnapShot, &pEntry);

	char aExeFileStr[100];

	while(1)
	{
		bool bRes = Process32Next(hSnapShot, &pEntry);
		if(bRes == false)
			return false;

		WCharToChar(pEntry.szExeFile, aExeFileStr, 100);

		//printf("Processor Name %s\n", aExeFileStr);
		if(!strncmp(aExeFileStr, strPath.c_str(), 15))
		{
			bCurrent = true;
		}
		if(bCurrent)
		{
			HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, pEntry.th32ProcessID);
			if(hProcess)
			{
				if(TerminateProcess(hProcess, 0))
				{
					//printf("Terminated %s\n", strPath.c_str());
					unsigned long nCode;
					GetExitCodeProcess(hProcess, &nCode);
				}
				CloseHandle(hProcess);
			}
			return true;
		}
		else
		{
			//printf("Not Found\n");
		}
	}
}
//------------------------------------------------------------------------------