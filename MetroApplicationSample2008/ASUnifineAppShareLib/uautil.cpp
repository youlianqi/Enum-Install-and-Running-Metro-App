#include "stdafx.h"
#include "uautil.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>
using namespace std;

LONG			g_lVersion = 0;

static const GUID CLSID_WICPngDecoder = 
{ 0x389ea17b, 0x5078, 0x4cde, { 0xb6, 0xef, 0x25, 0xc1, 0x51, 0x75, 0xc7, 0x51 } };

static const GUID CLSID_WICJpegDecoder = 
{ 0x9456a480, 0xe88b, 0x43ea, { 0x9e, 0x73, 0x0b, 0x2d, 0x9b, 0x71, 0xb1, 0xca } };

static const GUID GUID_WICPixelFormat32bppPBGRA = 
{ 0x6fddc324, 0x4e03, 0x4bfe, { 0xb1, 0x85, 0x3d, 0x77, 0x76, 0x8d, 0xc9, 0x10 } };

static const GUID CLSID_WICImagingFactory = 
{ 0xcacaf262, 0x9370, 0x4615, { 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0xa } };

int GetTextType(const wstring sPath)
{
	int nReturn = -1;
	unsigned char uniTxt[] = { 0xFF, 0xFE };// Unicode file header
	unsigned char endianTxt[] = { 0xFE, 0xFF };// Unicode big endian file header
	unsigned char utf8Txt[] = { 0xEF, 0xBB };// UTF_8 file header
	DWORD dwBytesRead = 0;

	//string sAppxManifestXmlPathA = std::string(fPath.begin(), fPath.end());
	
	HANDLE hFile = CreateFileW(sPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		hFile = NULL;
		CloseHandle(hFile);
		return -1;
	}
	BYTE *lpHeader = new BYTE[2];
	ReadFile(hFile, lpHeader, 2, &dwBytesRead, NULL);
	CloseHandle(hFile);

	if (lpHeader[0] == uniTxt[0] && lpHeader[1] == uniTxt[1])// Unicode file
		nReturn = 1;
	else if (lpHeader[0] == endianTxt[0] && lpHeader[1] == endianTxt[1])//  Unicode big endian file
		nReturn = 2;
	else if (lpHeader[0] == utf8Txt[0] && lpHeader[1] == utf8Txt[1])// UTF-8 file
		nReturn = 3;
	else
		nReturn = 0;   //Ascii

	delete[]lpHeader;
	return nReturn;
}

BOOL ReadUTF16(const wstring sPath,wstring &sFileContent)
{
	// std::ios::binary for Windows
	ifstream file(sPath.c_str(), std::ios::binary);
	stringstream ss;
	ss << file.rdbuf() << '\0';
	sFileContent = wstring((wchar_t *)ss.str().c_str());

	return TRUE;

	//size_t length=::wcslen((wchar_t *)ss.str().c_str())+1;
	//sFileContent = new WCHAR[length];
	//_ASSERTE(sFileContent);

	//if (!sFileContent) return FALSE;
	//wcsncpy_s(sFileContent,length, (wchar_t *)ss.str().c_str(),length-1);

	//return wstring((wchar_t *)ss.str().c_str());
}

BOOL EqualVersion(DWORD majorVersion, DWORD minorVersion)
{
	OSVERSIONINFOEX osVersionInfo;
	::ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFOEX));
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osVersionInfo.dwMajorVersion = majorVersion;
	osVersionInfo.dwMinorVersion = minorVersion;
	ULONGLONG maskMajorCondition = ::VerSetConditionMask(0, VER_MAJORVERSION, VER_EQUAL);
	ULONGLONG maskMinorCondition = ::VerSetConditionMask(0, VER_MINORVERSION, VER_EQUAL);
	return ::VerifyVersionInfo(&osVersionInfo, VER_MAJORVERSION, maskMajorCondition) 
		&& ::VerifyVersionInfo(&osVersionInfo, VER_MINORVERSION, maskMinorCondition);
}

LONG GetOSVersion()
{
	OSVERSIONINFOEX osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if (!GetVersionEx((OSVERSIONINFO*)&osvi))
	{
		return 0;
	}

	if(EqualVersion(6, 3)) //for win8.1
	{
		osvi.dwMajorVersion = 6;
		osvi.dwMinorVersion = 3;
	}

	return MAKELPARAM(osvi.dwMajorVersion, osvi.dwMinorVersion);
}

BOOL EqualString(LPCTSTR lpStr1, LPCTSTR lpStr2)
{
	if((LOWORD(g_lVersion)==WINDOWS_2K)&&(HIWORD(g_lVersion)==0))
		return (CompareString(
		LOCALE_SYSTEM_DEFAULT,
		NORM_IGNORECASE, 
		lpStr1,
		-1,
		lpStr2,
		-1) == CSTR_EQUAL);
	else
		return (CompareString(
		LOCALE_INVARIANT,
		NORM_IGNORECASE, 
		lpStr1,
		-1,
		lpStr2,
		-1) == CSTR_EQUAL);
}

BOOL IsFileExistStat(const std::wstring name) 
{
	struct stat buffer;   
	string sname = string(name.begin(),name.end());
	return (stat (sname.c_str(), &buffer) == 0); 
}

LONG WINAPI ASGetPackageId(
						   _In_       HANDLE hProcess,
						   _Inout_    UINT32 *bufferLength,
						   _Out_opt_  BYTE *pBuffer
						   )
{
	LONG rc = 0L;

	HMODULE hMod = LoadLibrary(_T("Kernel32.dll"));
	if(!hMod) return rc;

	PFN_GetPackageId tGetPackageId  = (PFN_GetPackageId)GetProcAddress(hMod, "GetPackageId");
	if (!tGetPackageId)
		return rc;

	rc = tGetPackageId(hProcess, bufferLength,pBuffer);

	if (hMod)
	{
		FreeLibrary(hMod);
	}

	return rc;
}

LONG WINAPI ASGetPackagePath(
							 _In_        const ASPACKAGE_ID *packageId,
							 _Reserved_  const UINT32 reserved,
							 _Inout_     UINT32 *pathLength,
							 _Out_opt_   PWSTR path
							 )
{
	LONG rc = 0L;

	HMODULE hMod = LoadLibrary(_T("Kernel32.dll"));
	if(!hMod) return rc;

	PFN_GetPackagePath tGetPackagePath  = (PFN_GetPackagePath)GetProcAddress(hMod, "GetPackagePath");
	if (!tGetPackagePath)
		return rc;

	rc = tGetPackagePath(packageId, reserved,pathLength,path);

	if (hMod)
	{
		FreeLibrary(hMod);
	}

	return rc;
}

LONG WINAPI ASPackageFullNameFromId(
									_In_       const ASPACKAGE_ID *packageId,
									_Inout_    UINT32 *packageFullNameLength,
									_Out_opt_  PWSTR packageFullName
									)
{
	LONG rc = 0L;

	HMODULE hMod = LoadLibrary(_T("Kernel32.dll"));
	if(!hMod) return rc;

	PFN_PackageFullNameFromId tPackageFullNameFromId  = (PFN_PackageFullNameFromId)GetProcAddress(hMod, "PackageFullNameFromId");
	if (!tPackageFullNameFromId)
		return rc;

	rc = tPackageFullNameFromId(packageId, packageFullNameLength,packageFullName);

	if (hMod)
	{
		FreeLibrary(hMod);
	}

	return rc;
}

typedef BOOL (WINAPI *_NtQueryInformationProcess)(
	HANDLE ProcessHandle,
	DWORD ProcessInformationClass,
	PVOID ProcessInformation,
	DWORD ProcessInformationLength,
	PDWORD ReturnLength
	);

typedef struct _UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;	
	PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _PROCESS_BASIC_INFORMATION
{
	LONG ExitStatus;	
	PVOID PebBaseAddress;	
	ULONG_PTR AffinityMask;	
	LONG BasePriority;	
	ULONG_PTR UniqueProcessId;
	ULONG_PTR ParentProcessId;
} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;


PVOID GetPebAddress(HANDLE ProcessHandle)
{
	_NtQueryInformationProcess NtQueryInformationProcess =		
	(_NtQueryInformationProcess)GetProcAddress(
	GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
	PROCESS_BASIC_INFORMATION pbi;
	NtQueryInformationProcess(ProcessHandle, 0, &pbi, sizeof(pbi), NULL);
	return pbi.PebBaseAddress;
}

#define ProcessBasicInformation 0

typedef struct
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} ASUNICODE_STRING, *ASPUNICODE_STRING;

typedef struct
{
	ULONG          AllocationSize;
	ULONG          ActualSize;
	ULONG          Flags;
	ULONG          Unknown1;
	ASUNICODE_STRING Unknown2;
	HANDLE         InputHandle;
	HANDLE         OutputHandle;
	HANDLE         ErrorHandle;
	ASUNICODE_STRING CurrentDirectory;
	HANDLE         CurrentDirectoryHandle;
	ASUNICODE_STRING SearchPaths;
	ASUNICODE_STRING ApplicationName;
	ASUNICODE_STRING CommandLine;
	PVOID          EnvironmentBlock;
	ULONG          Unknown[9];
	ASUNICODE_STRING Unknown3;
	ASUNICODE_STRING Unknown4;
	ASUNICODE_STRING Unknown5;
	ASUNICODE_STRING Unknown6;
} ASPROCESS_PARAMETERS, *ASPPROCESS_PARAMETERS;

typedef struct
{
	ULONG               AllocationSize;
	ULONG               Unknown1;
	HINSTANCE           ProcessHinstance;
	PVOID               ListDlls;
	ASPPROCESS_PARAMETERS ProcessParameters;
	ULONG               Unknown2;
	HANDLE              Heap;
} PEB, *PPEB;

typedef struct
{
	DWORD ExitStatus;
	PPEB  PebBaseAddress;
	DWORD AffinityMask;
	DWORD BasePriority;
	ULONG UniqueProcessId;
	ULONG InheritedFromUniqueProcessId;
}   ASPROCESS_BASIC_INFORMATION;


// ntdll!NtQueryInformationProcess (NT specific!)
//
// The function copies the process information of the
// specified type into a buffer
//
// NTSYSAPI
// NTSTATUS
// NTAPI
// NtQueryInformationProcess(
//    IN HANDLE ProcessHandle,              // handle to process
//    IN PROCESSINFOCLASS InformationClass, // information type
//    OUT PVOID ProcessInformation,         // pointer to buffer
//    IN ULONG ProcessInformationLength,    // buffer size in bytes
//    OUT PULONG ReturnLength OPTIONAL      // pointer to a 32-bit
//                                          // variable that receives
//                                          // the number of bytes
//                                          // written to the buffer 
// );
typedef LONG (WINAPI *PROCNTQSIP)(HANDLE,UINT,PVOID,ULONG,PULONG);


PROCNTQSIP NtQueryInformationProcess;

BOOL GetProcessCmdLine(DWORD dwId,LPWSTR wBuf,DWORD dwBufLen);

BOOL GetProcessCmdLine(DWORD dwId,LPWSTR wBuf,DWORD dwBufLen)
{
	LONG                      status;
	HANDLE                    hProcess;
	ASPROCESS_BASIC_INFORMATION pbi;
	PEB                       Peb;
	ASPROCESS_PARAMETERS        ProcParam;
	DWORD                     dwDummy;
	DWORD                     dwSize;
	LPVOID                    lpAddress;
	BOOL                      bRet = FALSE;

	// Get process handle
	hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION,FALSE,dwId);
	if (!hProcess)
		return FALSE;

	// Retrieve information
	status = NtQueryInformationProcess( hProcess,
		ProcessBasicInformation,
		(PVOID)&pbi,
		sizeof(ASPROCESS_BASIC_INFORMATION),
		NULL
		);


	if (status)
		goto cleanup;

	if (!ReadProcessMemory( hProcess,
		pbi.PebBaseAddress,
		&Peb,
		sizeof(PEB),
		&dwDummy
		)
		)
		goto cleanup;

	if (!ReadProcessMemory( hProcess,
		Peb.ProcessParameters,
		&ProcParam,
		sizeof(ASPROCESS_PARAMETERS),
		&dwDummy
		)
		)
		goto cleanup;

	lpAddress = ProcParam.CommandLine.Buffer;
	dwSize = ProcParam.CommandLine.Length;

	if (dwBufLen<dwSize)
		goto cleanup;

	if (!ReadProcessMemory( hProcess,
		lpAddress,
		wBuf,
		dwSize,
		&dwDummy
		)
		)
		goto cleanup;


	bRet = TRUE;

cleanup:

	CloseHandle (hProcess);


	return bRet;
} 

BOOL GetMetroPackageInfoFromHWnd(_In_ HWND hWnd,
								 _Out_opt_ wstring &sPath,
								 _Out_opt_ wstring &sPackageFullName, 
								 _Out_opt_ wstring &sPackageIdName,
								 _Out_opt_ wstring &sApplicationIdName)
{
	//int GetLinkInfoFromShell(HWND hWnd, LPCTSTR lpszLinkName, LPTSTR lpszExeName, int nExeSize, 
	//	LPTSTR lpszWorkingDir, int nDirSize, LPTSTR lpszArg, int nArgSize, LPTSTR lpszDescription, int nDescSize)
//
//	TCHAR szDocument[MAX_PATH] = {0};
//	TCHAR szParam[MAX_PATH] = {0};
//	TCHAR szDirectory[MAX_PATH]={0};
////	ResolveLinkFile(NULL,NULL,arguments,NULL,hWnd);
//	GetLinkInfoFromShell(hWnd,NULL,NULL,0,NULL,0,arguments,MAX_PATH,NULL,0);
//	sApplicationIdName = wstring(arguments);

	DWORD pid = 0L;
	GetWindowThreadProcessId(hWnd, &pid);

	//NtQueryInformationProcess = (PROCNTQSIP)GetProcAddress(
	//	GetModuleHandle(_T("ntdll")),
	//	"NtQueryInformationProcess"
	//	);

	//if (!NtQueryInformationProcess)
	//	return FALSE;

	////DWORD dwId;
	////(argv[1],"%lu",&dwId);

	//WCHAR wstr[255] = {0};

	//if (GetProcessCmdLine(pid,wstr,sizeof(wstr)))
	//	wprintf(L"Command line for process %lu is:/n%s/n",pid,wstr);
	//else
	//	wprintf(L"Could not get command line!");

	//sApplicationIdName = wstring(wstr);

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);

	//HANDLE processHandle;
	PVOID pebAddress;
	PVOID rtlUserProcParamsAddress;
	UNICODE_STRING commandLine;
	WCHAR *commandLineContents;
	pebAddress = GetPebAddress(hProcess);

	/* get the address of ProcessParameters */
	if (!ReadProcessMemory(hProcess, (PCHAR)pebAddress + 0x10,
		&rtlUserProcParamsAddress, sizeof(PVOID), NULL))
	{
		printf("Could not read the address of ProcessParameters!\n");
		return GetLastError();
	}


	if (!ReadProcessMemory(hProcess, (PCHAR)rtlUserProcParamsAddress + 0x40,
		&commandLine, sizeof(commandLine), NULL))
	{
		printf("Could not read CommandLine!\n");
		return GetLastError();
	}

	commandLineContents = (WCHAR *)malloc(commandLine.Length);


	if (!ReadProcessMemory(hProcess, commandLine.Buffer,
		commandLineContents, commandLine.Length, NULL))
	{
		//printf("Could not read the command line string!\n");
		return FALSE;
	}
		

	//printf("%.*S\n", commandLine.Length / 2, commandLineContents);
	
	sApplicationIdName = wstring(commandLineContents);
	free(commandLineContents);



	

	UINT32 length = 0;
	LONG rc = ASGetPackageId(hProcess, &length, NULL);
	if (rc != 122L/*ERROR_INSUFFICIENT_BUFFER*/)
	{
		if (rc == 15700L/*APPMODEL_ERROR_NO_PACKAGE*/)
			wprintf(L"Process has no package identity\n");
		else
			wprintf(L"GetPackageId failed with error %d\n", rc);
		return FALSE;
	}

	BYTE * buffer = (PBYTE)malloc(length);
	if (buffer == NULL)
	{
		wprintf(L"Memory allocation failed\n");
		return FALSE;
	}

	rc = ASGetPackageId(hProcess, &length, buffer);
	if (rc != ERROR_SUCCESS)
	{
		wprintf(L"GetPackageId failed with error %d\n", rc);
		return FALSE;
	}

	ASPACKAGE_ID * packageID = (ASPACKAGE_ID *)buffer;
	sPackageIdName = wstring(packageID->name);
	
	length = 0;
	rc = ASGetPackagePath(packageID, 0, &length, NULL);
	if (rc != ERROR_INSUFFICIENT_BUFFER)
	{
		wprintf(L"GetPackagePath failed with error %d\n", rc);
		return FALSE;
	}

	PWSTR path = (PWSTR)malloc((length + 1) * sizeof(WCHAR));
	if (path == NULL)
	{
		wprintf(L"Memory allocation failed\n");
		return FALSE;
	}

	rc = ASGetPackagePath(packageID, 0, &length, path);
	if (rc != ERROR_SUCCESS)
	{
		wprintf(L"GetPackagePath failed with error %d\n", rc);
		return FALSE;
	}
	else
	{
		wprintf(L"Path = %s\n", path);
		sPath = wstring(path);
	}

	free(path);

	length = 0;
	rc = ASPackageFullNameFromId(packageID, &length, NULL);
	if (rc == ERROR_SUCCESS)
	{
		wprintf(L"PackageFullNameFromId unexpectedly succeeded\n");
		return 4;
	}
	else if (rc != ERROR_INSUFFICIENT_BUFFER)
	{
		wprintf(L"PackageFullNameFromId failed with error %d\n", rc);
		return 5;
	}

	PWSTR fullName = (PWSTR) malloc((length + 1) * sizeof(WCHAR));
	if (fullName == NULL)
	{
		wprintf(L"Memory allocation failed\n");
		return 6;
	}

	rc = ASPackageFullNameFromId(packageID, &length, fullName);
	if (rc != ERROR_SUCCESS)
		wprintf(L"PackageFullNameFromId failed with error %d\n", rc);
	else
	{
		wprintf(L"Package full name = %s\n", fullName);
		sPackageFullName = wstring(fullName);
	}

	CloseHandle(hProcess);
	free(fullName);

	free(buffer);

	return TRUE;

}

int GetLinkInfoFromShell(HWND hWnd, LPCTSTR lpszLinkName, LPTSTR lpszExeName, int nExeSize, 
									 LPTSTR lpszWorkingDir, int nDirSize, LPTSTR lpszArg, int nArgSize, LPTSTR lpszDescription, int nDescSize)
{

	IShellLink *pShLink = NULL;
	WIN32_FIND_DATA wfd;
	memset(&wfd,0,sizeof(WIN32_FIND_DATA));

	BOOL bIsWOW64 = IsWOW64Process(GetCurrentProcess(), 0);

	// Initialize the return parameters to null strings.
	if(lpszExeName) *lpszExeName = '\0';
	if(lpszWorkingDir) *lpszWorkingDir = '\0';
	if(lpszArg) *lpszArg = '\0';
	if(lpszDescription) *lpszDescription = '\0';

	// Call CoCreateInstance to obtain the IShellLink
	// Interface pointer. This call fails if
	// CoInitialize is not called, so it is assumed that
	// CoInitialize has been called.
	HRESULT hres = CoCreateInstance( CLSID_ShellLink,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IShellLink,
		(LPVOID *)&pShLink );

	if (SUCCEEDED(hres))
	{
		IPersistFile *ppf;

		// The IShellLink Interface supports the IPersistFile
		// interface. Get an interface pointer to it.
		hres = pShLink->QueryInterface(IID_IPersistFile, (LPVOID *)&ppf );
		if (SUCCEEDED(hres))
		{
			//WORD wsz[MAX_PATH*2] = {0};
			TCHAR wsz[MAX_PATH*2] = {0};

			// Convert the given link name string to a wide character string.
#if defined(UNICODE) || defined(_UNICODE)
			_tcscpy_s(wsz, MAX_PATH*2, lpszLinkName);
#else
			MultiByteToWideChar( CP_ACP, 0,
				lpszLinkName,
				-1, wsz, MAX_PATH );
#endif			
			// Load the file.
			hres = ppf->Load( wsz, STGM_READ );
			if (SUCCEEDED(hres))
			{

				if (SUCCEEDED(hres))
				{
					// Get the path of the file the link points to.
					if(lpszExeName) 

					pShLink->GetPath(lpszExeName, nExeSize, &wfd, (bIsWOW64 ? SLGP_RAWPATH : 0) | /*SLGP_SHORTPATH*/SLGP_UNCPRIORITY);

					if(lpszWorkingDir)
						pShLink->GetWorkingDirectory(lpszWorkingDir, nDirSize);

					if(lpszArg)
						pShLink->GetArguments(lpszArg, nArgSize);

					if(lpszDescription)
						pShLink->GetDescription(lpszDescription, nDescSize);

				}
			}
			ppf->Release();
		}
		pShLink->Release();
	}
	else
	{
		

	}



	//Jonathan Gu: 64-bit support
	//Expand environment variable to 64-bit version on 64-bit OS
	if (lpszExeName && *lpszExeName && bIsWOW64)
	{
		//TCHAR szTest[1024] = _T("%ProgramFiles%1%%2%CommonProgramFiles%3%SystemRoot%4%ProgramFiles%");
		//lpszExeName = szTest;
	//	m_appEnv.UpdateWOW64EnvWithW64Env(lpszExeName, nExeSize);
	}


	return SUCCEEDED(hres) ? 1 : 0;
}

BOOL ResolveLinkFile(TCHAR *name, TCHAR *resolution, 
					 TCHAR *arguments, TCHAR *working_dir, HWND hwnd)
{
	ComInitializer ccom(100L);

	HRESULT hres; 
	IShellLink* psl; 
	WIN32_FIND_DATA wfd; 

//	*resolution = 0; // assume failure 

	// Get a pointer to the IShellLink interface.
	hres = CoCreateInstance (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
		IID_IShellLink, (void **)&psl);
	if (SUCCEEDED (hres)) {
		IPersistFile *ppf;

		// Get a pointer to the IPersistFile interface.
		hres = psl->QueryInterface (IID_IPersistFile, (void **)&ppf);

		if (SUCCEEDED (hres)) {
#ifndef UNICODE
			WORD wsz [MAX_PATH]; // buffer for Unicode string

			// Ensure that the string consists of Unicode TCHARacters.
			MultiByteToWideChar(CP_ACP, 0, name, -1, wsz, MAX_PATH);
#else
			TCHAR *wsz = name;
#endif

			// Load the shortcut.
			hres = ppf->Load(wsz, STGM_READ);

			if (SUCCEEDED(hres)) {
				// Resolve the shortcut.
				hres = psl->Resolve(hwnd, SLR_ANY_MATCH);
				if (SUCCEEDED(hres)) {
				//	_tcscpy(resolution, name);
					// Get the path to the shortcut target.
			/*		hres = psl->GetPath(resolution, MAX_PATH,
						(WIN32_FIND_DATA *)&wfd, SLGP_SHORTPATH);
					if (! SUCCEEDED(hres))
						goto cleanup;*/
					hres = psl->GetArguments(arguments, MAX_PATH);
					if (! SUCCEEDED(hres))
						goto cleanup;
					/*hres = psl->GetWorkingDirectory(working_dir, MAX_PATH);
					if (! SUCCEEDED(hres))
						goto cleanup;*/
				}
			}
cleanup:
			// Release the pointer to IPersistFile.
			ppf->Release ();
		}
		// Release the pointer to IShellLink.
		psl->Release ();
	}
	return (SUCCEEDED(hres) ? TRUE : FALSE);

}

BOOL ASSHLoadIndirectString(LPCWSTR pszSource, 
							LPWSTR pszOutBuf, 
							UINT cchOutBuf, 
							PVOID *ppvReserved)
{
	typedef HRESULT (STDAPICALLTYPE *PFNASSHLoadIndirectString) (
		LPCWSTR pszSource, 
		LPWSTR pszOutBuf, 
		UINT cchOutBuf, 
		PVOID *ppvReserved);

	HMODULE hMod = GetModuleHandle(_T("Shlwapi.dll"));
	if(!hMod) return FALSE;

	PFNASSHLoadIndirectString pfnSHLoadIndirectString  = (PFNASSHLoadIndirectString)GetProcAddress(hMod, "SHLoadIndirectString");
	if (!pfnSHLoadIndirectString)
		return FALSE;

	HRESULT bRet;
	bRet = pfnSHLoadIndirectString(pszSource, pszOutBuf,cchOutBuf,ppvReserved);

	if (hMod)
	{
		FreeLibrary(hMod);
	}

	if (FAILED(bRet))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

BOOL IsWOW64Process(HANDLE hProcess, DWORD dwProcessId)
{
	BOOL bRes = FALSE;
	typedef BOOL (WINAPI *PFN_IsWow64Process)(HANDLE hProcess, PBOOL Wow64Process);
	HMODULE hMod = GetModuleHandle(_T("kernel32"));
	if(NULL==hMod) return bRes;

	PFN_IsWow64Process pfnIsWow64Process = (PFN_IsWow64Process)GetProcAddress(hMod, "IsWow64Process");
	if (!pfnIsWow64Process)
		return bRes;

	HANDLE hProc = NULL;
	if(hProcess == NULL)
	{
		hProcess = hProc = OpenProcess(PROCESS_QUERY_INFORMATION, 0, dwProcessId);
		if (!hProc) return TRUE; //if the process can't be opened like elevated process, treat it as a 32 bit app.
	}

	pfnIsWow64Process(hProcess, &bRes);

	if(hProc) CloseHandle(hProc);

	return bRes; //32bits process == run in wow64
}

BOOL PfnAlphaBlend( __in HDC hdcDest, 
				   __in int xoriginDest, 
				   __in int yoriginDest, 
				   __in int wDest,
				   __in int hDest, 
				   __in HDC hdcSrc, 
				   __in int xoriginSrc, 
				   __in int yoriginSrc, 
				   __in int wSrc, 
				   __in int hSrc, 
				   __in BLENDFUNCTION ftn)
{
	typedef BOOL (WINAPI *t_fn_AlphaBlend)(
		HDC ,                 // handle to destination DC
		int ,            // x-coord of upper-left corner
		int ,            // y-coord of upper-left corner
		int ,              // destination width
		int ,             // destination height
		HDC ,                  // handle to source DC
		int ,             // x-coord of upper-left corner
		int ,             // y-coord of upper-left corner
		int ,               // source width
		int ,              // source height
		BLENDFUNCTION   // alpha-blending function
		);

	HMODULE hMod = LoadLibrary(_T("msimg32.dll"));
	if(!hMod) return FALSE;

	t_fn_AlphaBlend tAlphaBlend  = (t_fn_AlphaBlend)GetProcAddress(hMod, "AlphaBlend");
	if (!tAlphaBlend)
		return FALSE;

	HRESULT bRet;
	bRet = tAlphaBlend(hdcDest, xoriginDest,yoriginDest,wDest,hDest,hdcSrc,xoriginSrc,yoriginSrc,wSrc,hSrc,ftn);

	if (hMod)
	{
		FreeLibrary(hMod);
	}

	if (FAILED(bRet))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}

}

BOOL ASWICConvertBitmapSource(
							  __in REFWICPixelFormatGUID dstFormat, // Destination pixel format
							  __in_ecount(1) IWICBitmapSource  *pISrc,    // Source bitmap
							  __deref_out_ecount(1) IWICBitmapSource **ppIDst   // Destination bitmap, a copy or addrefed source
							  )
{
	typedef HRESULT (WINAPI * t_fn_WICConvertBitmapSource)(
		__in REFWICPixelFormatGUID , // Destination pixel format
		__in_ecount(1) IWICBitmapSource  *,    // Source bitmap
		__deref_out_ecount(1) IWICBitmapSource **   // Destination bitmap, a copy or addrefed source
		);

	HMODULE hMod = LoadLibrary(_T("Windowscodecs.dll"));
	if(!hMod) return FALSE;

	t_fn_WICConvertBitmapSource pWICConvertBitmapSource  = (t_fn_WICConvertBitmapSource)GetProcAddress(hMod, "WICConvertBitmapSource");
	if (!pWICConvertBitmapSource)
		return FALSE;

	HRESULT bRet;
	bRet = pWICConvertBitmapSource(dstFormat, pISrc,ppIDst);

	if (hMod)
	{
		FreeLibrary(hMod);
	}

	if (FAILED(bRet))
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

// Loads a PNG image from the specified stream, using Windows Imaging Component).
IWICBitmapSource * LoadBitmapFromStream(IStream * pImageStream, GUID GUID_WIC_DECODER)
{
	IWICBitmapSource * pBitmapSource = NULL;

	// Load WIC's PNG decoder
	IWICBitmapDecoder * pDecoder = NULL;
	if (FAILED(CoCreateInstance(GUID_WIC_DECODER, NULL, CLSCTX_INPROC_SERVER, __uuidof(pDecoder), reinterpret_cast<void**>(&pDecoder))))
		return NULL;

	// Load the PNG
	if (FAILED(pDecoder->Initialize(pImageStream, WICDecodeMetadataCacheOnLoad)))
	{
		pDecoder->Release();
		return NULL;
	}

	UINT uFrameCount = 0;
	if (FAILED(pDecoder->GetFrameCount(&uFrameCount)) || uFrameCount != 1)
	{
		pDecoder->Release();
		return NULL;
	}

	IWICBitmapFrameDecode * pFrame = NULL;
	if (FAILED(pDecoder->GetFrame(0, &pFrame)))
	{
		pDecoder->Release();
		return NULL;
	}

	ASWICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, pFrame, &pBitmapSource);
	
	pFrame->Release();
	pDecoder->Release();

	return pBitmapSource;
}

// Creates a 32-bit DIB from the specified WIC bitmap.
HBITMAP CreateBitmapHandle(IWICBitmapSource * pBitmapSource)
{
	HBITMAP hBitmap = NULL;

	UINT uWidth = 0;
	UINT uHeight = 0;
	if (FAILED(pBitmapSource->GetSize(&uWidth, &uHeight)) || uWidth == 0 || uHeight == 0)
		return NULL;

	BITMAPINFO bmInfo  = { 0 };
	bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmInfo.bmiHeader.biWidth = uWidth;
	bmInfo.bmiHeader.biHeight = -(int)uHeight;
	bmInfo.bmiHeader.biPlanes = 1;
	bmInfo.bmiHeader.biBitCount = 32;
	bmInfo.bmiHeader.biCompression = BI_RGB;

	// Create a DIB section
	void * pImageBits = NULL;
	HDC hDCScreen = GetDC(NULL);
	hBitmap = CreateDIBSection(hDCScreen, &bmInfo, DIB_RGB_COLORS, &pImageBits, NULL, 0);
	ReleaseDC(NULL, hDCScreen);

	if (hBitmap == NULL)
		return NULL;

	const UINT cbStride = uWidth * 4;
	const UINT cbImage = cbStride * uWidth;

	if (FAILED(pBitmapSource->CopyPixels(NULL, cbStride, cbImage, static_cast<BYTE *>(pImageBits))))
	{
		DeleteObject(hBitmap);
		hBitmap = NULL;
	}

	return hBitmap;
}

typedef enum _tagENUM_Image_Type {
	PNG_Decoder = 0,
	JPEG_Decoder,
	OTHER_Decoder
} Image_Decoder_Type;

#define ISBIGAZ(c)      (c >= L'A' &&  c <= L'Z')
#define ISSMALLAZ(c)    (c >= L'a' &&  c <= L'z')

BOOL AsWcsncmpEuqal(const wchar_t* ch1, const wchar_t *ch2, long len)
{
	long i=0, j=0 ;
	for (i=0; i<len; i++)
	{
		if (ch1[i] == ch2[i] )
			j = j + 1;
		else if( ISBIGAZ(ch1[i]) || ISSMALLAZ(ch2[i]) )
		{
			if (ch1[i] + 32 == ch2[i] )
				j = j + 1;
		}
		else if ( ISBIGAZ(ch2[i]) || ISSMALLAZ(ch1[i]) )
		{
			if (ch2[i] + 32 == ch1[i])
				j = j + 1;
		}
	}
	if (j == len)
		return  TRUE;
	else
		return FALSE ; 
}
BOOL GetImageDecoderType(wstring sImgaeFilePath,Image_Decoder_Type &eImageType)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

	_wsplitpath_s(sImgaeFilePath.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	size_t lenExt = wcsnlen_s(ext,_MAX_EXT);
 
	if (AsWcsncmpEuqal(ext,_T(".png"),lenExt))
	{
		eImageType = PNG_Decoder;
		return TRUE;
	}
	else if (AsWcsncmpEuqal(ext,_T(".jpg"),lenExt)
		|| AsWcsncmpEuqal(ext,_T(".jpeg"),lenExt))
	{
		eImageType = JPEG_Decoder;
		return TRUE;
	}
	
	return FALSE;
}


// Loads the PNG into a HBITMAP.
HBITMAP LoadPNGImage(LPCTSTR lpszFilePath)
{	
	HBITMAP hBitmap = NULL;

	IWICImagingFactory *pImagingFactory = NULL;
	if (FAILED(CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, 
		__uuidof(pImagingFactory), reinterpret_cast<void**>(&pImagingFactory))))
		return NULL;

	IWICStream *pImageStream;
	if (FAILED(pImagingFactory->CreateStream(&pImageStream)))
	{
		pImagingFactory->Release();
		return NULL;
	}

	if (FAILED(pImageStream->InitializeFromFilename(lpszFilePath, GENERIC_READ)))
	{
		pImageStream->Release();
		pImagingFactory->Release();
		return NULL;
	}

	Image_Decoder_Type eImageType = OTHER_Decoder;
	GetImageDecoderType(lpszFilePath,eImageType);
	// Load the bitmap with WIC

	GUID gCLSID;
	if (PNG_Decoder == eImageType)
	{
		gCLSID = CLSID_WICPngDecoder;
	}
	else if (JPEG_Decoder == eImageType)
	{
		gCLSID = CLSID_WICJpegDecoder;
	}
	else
	{
		pImageStream->Release();
		pImagingFactory->Release();
		return NULL;
	}

	IWICBitmapSource * pBitmap = LoadBitmapFromStream(pImageStream,gCLSID);
	if (pBitmap == NULL)
	{
		pImageStream->Release();
		pImagingFactory->Release();
		return NULL;
	}

	// Create a HBITMAP containing the image
	hBitmap = CreateBitmapHandle(pBitmap);

	pBitmap->Release();
	pImageStream->Release();
	pImagingFactory->Release();

	return hBitmap;
}

int DrawBitmap(HDC hDrawDC, HBITMAP hBitmap, COLORREF clrBK)
{
	HDC hMemDC = NULL;
	HBITMAP hOldBitmap = NULL;
	BITMAP bmpInfo = {0};

	GetObject(hBitmap,sizeof(BITMAP), &bmpInfo);

	hMemDC = CreateCompatibleDC(hDrawDC);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);

	RECT rcBitmap = {0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight};
	FillRect(hDrawDC, &rcBitmap, CreateSolidBrush(clrBK));

	BLENDFUNCTION bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = 255;
	bf.AlphaFormat = AC_SRC_ALPHA;

	BOOL nRet = /*::AlphaBlend*/PfnAlphaBlend(hDrawDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, 
		hMemDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, bf);

	SelectObject(hMemDC, hOldBitmap);

	DeleteDC(hMemDC);
	DeleteObject(hOldBitmap);

	return 0;
}

HBITMAP GetTransparentBitmap(HBITMAP image, DWORD BitWidth, DWORD BitHeight)
{
	HDC hDC = ::GetDC(NULL);

	HDC hTempDC;
	hTempDC = CreateCompatibleDC(hDC);

	HBITMAP hBitmapTemp = NULL;
	HBITMAP hBmpOld = NULL; 

	if (image)
	{
		HDC hMemDC = NULL;
		HBITMAP hOldBitmap = NULL;

		BITMAP bmpInfo = {0};
		GetObject(image,sizeof(BITMAP), &bmpInfo);

		hMemDC = CreateCompatibleDC(hTempDC);
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, image);

		hBitmapTemp = CreateCompatibleBitmap(hDC, BitWidth, BitHeight);
		hBmpOld = (HBITMAP)SelectObject(hTempDC, hBitmapTemp);

		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 255;
		bf.AlphaFormat = AC_SRC_ALPHA;

		BOOL nRet = /*::AlphaBlend*/PfnAlphaBlend(hTempDC, 0, 0, BitHeight, BitHeight, 
			hMemDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, bf);

		SelectObject(hMemDC, hOldBitmap);

		DeleteDC(hMemDC);
		DeleteObject(hOldBitmap);
	}

	//Display bitmap on desktop
	/*StretchBlt(hDC, 55, 55, BitWidth, BitHeight,
		hTempDC, 0, 0, BitWidth, BitHeight, SRCCOPY);*/
	DeleteObject(image);
	SelectObject(hTempDC, hBmpOld);
	DeleteObject(hBmpOld);
	::DeleteDC(hTempDC);
	::ReleaseDC(NULL, hDC);

	return hBitmapTemp;

}

HBITMAP GetBitmapFromFile(LPCTSTR p_pchFileName, COLORREF color, DWORD BitWidth, DWORD BitHeight)
{
	HDC hDC = ::GetDC(NULL);

	HDC hTempDC;
	hTempDC = CreateCompatibleDC(hDC);

	HBITMAP hBitmapTemp = NULL;
	HBITMAP hBmpOld = NULL; 

	if (!color) color = RGB(255, 255, 255);

	HBITMAP image = LoadPNGImage(p_pchFileName);
	if (image)
	{
		HDC hMemDC = NULL;
		HBITMAP hOldBitmap = NULL;

		BITMAP bmpInfo = {0};
		GetObject(image,sizeof(BITMAP), &bmpInfo);

		hMemDC = CreateCompatibleDC(hTempDC);
		hOldBitmap = (HBITMAP)SelectObject(hMemDC, image);

		hBitmapTemp = CreateCompatibleBitmap(hDC, BitWidth, BitHeight);
		hBmpOld = (HBITMAP)SelectObject(hTempDC, hBitmapTemp);

		RECT rcBitmap = {0, 0, BitHeight,BitHeight};
		FillRect(hTempDC, &rcBitmap, CreateSolidBrush(color));

		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 255;
		bf.AlphaFormat = AC_SRC_ALPHA;

		BOOL nRet = /*::AlphaBlend*/PfnAlphaBlend(hTempDC, 0, 0, BitHeight, BitHeight, 
			hMemDC, 0, 0, bmpInfo.bmWidth, bmpInfo.bmHeight, bf);

		SelectObject(hMemDC, hOldBitmap);

		DeleteDC(hMemDC);
		DeleteObject(hOldBitmap);
	}

	//Display bitmap on desktop
	/*StretchBlt(hDC, 55, 55, BitWidth, BitHeight,
		hTempDC, 0, 0, BitWidth, BitHeight, SRCCOPY);*/
	DeleteObject(image);
	SelectObject(hTempDC, hBmpOld);
	DeleteObject(hBmpOld);
	::DeleteDC(hTempDC);
	::ReleaseDC(NULL, hDC);

	return hBitmapTemp;
}

TCHAR  *g_classSkipCtrlWin[]={
	_T("tooltips_class32"),
	_T("ATAnimationWndClass"),
	_T("ATAnnotationWndClass"),
	_T("AS_WND_BORDER"),
	_T("ATNothingWndClass"),
	_T("HostMouseControlTipWndClass"),//for 25641, Rick, 2002-10-15
	_T("#32768"),
	_T("SysShadow"),
	_T("ATAnnotationLayeredWndClass"), //for bug#412395
	_T("Progman"),
	_T("#32774"),  //Bug #: 416558, system ToolTip
	_T("WorkerW"),     // CSCud11691, exist in Win7/Win8 in Aero mode
	_T("EdgeUiInputWndClass"),     // CSCud11691, only exist in Win8
	_T("Shell_SecondaryTrayWnd"),
	_T("EdgeUiInputTopWndClass"),
	_T("MSO_BORDEREFFECT_WINDOW_CLASS")	// CSCul86227, add for Office 2013 board class
};

BOOL IsSkipCtrlWin(HWND hWnd)
{
	int	i;
	TCHAR szClassName[128];

	GetClassName(hWnd, szClassName, sizeof(szClassName) / sizeof(TCHAR));
	for (i = 0; i < sizeof(g_classSkipCtrlWin) / sizeof(TCHAR *); i++)
	{
		if (EqualString(szClassName, g_classSkipCtrlWin[i]))
			return TRUE;
	}

	return FALSE;
}

BOOL AsSetProp(HWND hWnd,LPCTSTR lpString,HANDLE hData)
{
	BOOL bSuccess= ::SetProp(hWnd,lpString,hData);
	if(bSuccess==FALSE)
	{
		if(hWnd == GetDesktopWindow())
		{
			bSuccess = ::SetProp(GetShellWindow(), lpString, hData);
		}
	}

	return bSuccess;
}

HANDLE AsGetProp(HWND hWnd,LPCTSTR lpString)
{
	HANDLE hProp=::GetProp(hWnd,lpString);
	if(hProp==NULL)
	{
		if(hWnd == GetDesktopWindow())
			hProp = ::GetProp(GetShellWindow(), lpString);
	}

	return hProp;
}

HANDLE AsRemoveProp(HWND hWnd,LPCTSTR lpString)
{
	HANDLE hProp = ::RemoveProp(hWnd,lpString);
	if(hProp==NULL)
	{
		if(hWnd == GetDesktopWindow())
			hProp = ::RemoveProp(GetShellWindow(), lpString);
	}

	return hProp;
}

DWORD IsWinNT()
{
	XP_OSVERSIONINFOEXW versionInfo;
	ZeroMemory(&versionInfo, sizeof(XP_OSVERSIONINFOEXW));

	versionInfo.dwOSVersionInfoSize = sizeof(XP_OSVERSIONINFOEXW);

	DWORD dwVersion = 0;
	if(FALSE == GetVersionEx((OSVERSIONINFO*)&versionInfo))
		return dwVersion;

	if(versionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		switch(versionInfo.dwMajorVersion)
		{
		case 5:
			{
				switch(versionInfo.dwMinorVersion)
				{
				case 0: return NT_WINDOWS2000;
				case 1: return NT_WINDOWSXP;
				case 2:
					{
						SYSTEM_INFO si;
						GetSystemInfo(&si);

						if((versionInfo.wProductType == VER_NT_WORKSTATION) 
							&& (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64))
							return NT_WINDOWSXP64;

						if(versionInfo.wSuiteMask == VER_SUITE_WH_SERVER)
							return NT_WINDOWSHOMESVR;

						if(GetSystemMetrics(SM_SERVERR2) == 0)
						{
							return NT_WINDOWS2003;
						}
						else
						{
							return NT_WINDOWS2003R2;
						}
					}
					break;
				}
			}
			break;
		case 6:
			{
				switch(versionInfo.dwMinorVersion)
				{
				case 0:
					{
						if(versionInfo.wProductType == VER_NT_WORKSTATION)
							return NT_VISTA;
						else
							return NT_WINDOWS2008;
					}
					break;
				case 1:
					{
						if(versionInfo.wProductType == VER_NT_WORKSTATION)
							return NT_WINDOWS7;
						else
							return NT_WINDOWS2008R2;
					}
					break;
				case 2:
					break;
				}
			}
			break;
		}

		if(dwVersion == 0)
			dwVersion = (versionInfo.dwMajorVersion<<8)|versionInfo.dwMinorVersion;
	}

	return dwVersion;
}

BOOL _VT_QueryFullProcessImageName(__in DWORD dwProcessId, __out_ecount(dwBufSize) LPTSTR lpszPath, __in DWORD dwBufSize)
{
	if(NULL  == lpszPath || 0 == dwBufSize)
		return FALSE;

	BOOL bRet = FALSE;
	HANDLE hKer32 = ::GetModuleHandle(_T("kernel32.dll"));
	if(hKer32)
	{
		typedef BOOL (WINAPI * QueryFullProcessImageNameProc)( HANDLE hProcess,DWORD dwFlags,LPTSTR lpExeName,PDWORD lpdwSize);
#ifdef _UNICODE
#define _QUERYFULLPROCESSIMAGENAME "QueryFullProcessImageNameW" //just is valid from vista
#else
#define _QUERYFULLPROCESSIMAGENAME "QueryFullProcessImageNameA"
#endif

		QueryFullProcessImageNameProc lpQF = (QueryFullProcessImageNameProc) ::GetProcAddress(( HMODULE)hKer32, _QUERYFULLPROCESSIMAGENAME);
		if(lpQF)  ////just is valid from vista, so we may not judge whether is VISTA
		{
			TCHAR szPath[_MAX_PATH] = {0};
			DWORD dwSize = _MAX_PATH;
			HANDLE hWndPro = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION  , FALSE, dwProcessId);
			if(hWndPro)
			{
				if(lpQF(hWndPro, 0, szPath, &dwSize))
				{

					if(0==_tcscpy_s(lpszPath, dwBufSize, szPath))
						bRet = TRUE;
				}

				::CloseHandle(hWndPro);
			}
		}
	}

	return bRet;
}

BOOL WINAPI GetImageFileFromWindow(__in HWND hWnd, __out_ecount(dwSize) LPTSTR szApp, __in DWORD dwSize)
{
	DWORD dwName = 0;

	if(NULL == szApp || dwSize == 0) return FALSE;

	DWORD	dwWindowProcessId = 0;
	GetWindowThreadProcessId(hWnd, &dwWindowProcessId);
	if(dwWindowProcessId == 0) return FALSE;

	//On vista, querying the information of higher level process will fail, no right.
	//so use new API to do it.
	//<<< fix bug#397216

	if(IsVistaOrLater())
	{
		return _VT_QueryFullProcessImageName(dwWindowProcessId,szApp, dwSize);
	}

	HMODULE hmod = LoadLibrary(_T("PSAPI.dll"));
	if(NULL == hmod) return FALSE;

	PFN_GetProcessImageFileName pfnGetProcessImageFileName = 
		(PFN_GetProcessImageFileName)GetProcAddress(hmod, SZ_GetProcessImageFileName);

	if (pfnGetProcessImageFileName)
	{
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, 0, dwWindowProcessId);
		if (hProcess)
		{
			dwName = pfnGetProcessImageFileName(hProcess, szApp, dwSize);
			CloseHandle(hProcess);

			FreeLibrary(hmod);

			return (dwName!=0);
		}
	}

	FreeLibrary(hmod);

	HINSTANCE hNTDLL = GetModuleHandle(_T("NTDLL"));
	if(NULL==hNTDLL) return FALSE;

	WIN32FUNCWITHTWOPARA RtlCreateQueryDebugBuffer = 
		(WIN32FUNCWITHTWOPARA)GetProcAddress(hNTDLL, "RtlCreateQueryDebugBuffer");

	WIN32FUNCWITHTHREEPARA	RtlQueryProcessDebugInformation = 
		(WIN32FUNCWITHTHREEPARA)GetProcAddress(hNTDLL, "RtlQueryProcessDebugInformation");

	WIN32FUNCWITHONEPARA	RtlDestroyQueryDebugBuffer = 
		(WIN32FUNCWITHONEPARA)GetProcAddress(hNTDLL, "RtlDestroyQueryDebugBuffer");

	if(!(RtlCreateQueryDebugBuffer &&
		RtlQueryProcessDebugInformation &&
		RtlDestroyQueryDebugBuffer))
		return FALSE;

	LPSTR pProcessBuffer = (LPSTR)RtlCreateQueryDebugBuffer(0, 0);

	if (!pProcessBuffer) return FALSE;

	if (RtlQueryProcessDebugInformation(dwWindowProcessId, 1, (DWORD)pProcessBuffer) == 0)
	{
		LPCSTR	pProcessInfo = *(LPCSTR *)(pProcessBuffer+0x30);

		if (*(LPDWORD)pProcessInfo != 0)
		{
			pProcessInfo += 0x20;

#if defined(UNICODE) || defined(_UNICODE)
			dwName = MultiByteToWideChar( CP_ACP, 0, (LPCSTR)pProcessInfo, -1, szApp, dwSize);	
#else			
			_tcsncpy_s(szApp, dwSize, pProcessInfo, MAX_PATH);
			dwName = 1;
#endif	
		}
	}

	RtlDestroyQueryDebugBuffer((DWORD)pProcessBuffer);

	return (dwName!=0);
}

HICON	GetWindowIcon(HWND hWnd)
{
	HICON		hIcon=NULL; 

	if(hWnd&&IsWindow(hWnd))
	{
		if (SendMessageTimeout(hWnd, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, 200, (LPDWORD)&hIcon))
		{
			if (hIcon)
				return hIcon;
		}

		hIcon=(HICON)GetClassLong(hWnd,GCL_HICONSM);
		if (hIcon)
			return hIcon;

		if (SendMessageTimeout(hWnd, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG|SMTO_BLOCK, 200, (LPDWORD)&hIcon))
		{
			if (hIcon)
				return hIcon;
		}

		hIcon=(HICON)GetClassLong(hWnd, GCL_HICON);
		if (hIcon)
			return hIcon;
	}

	hIcon=LoadIcon(NULL,IDI_WINLOGO);

	return hIcon;
}

HBITMAP BitmapFromIcon(HICON hIcon)
{
	HDC hDC = CreateCompatibleDC(NULL);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDC, hBitmap);
	DrawIcon(hDC, 0, 0, hIcon);
	SelectObject(hDC, hOldBitmap);
	DeleteDC(hDC);

	return hBitmap;
}

HBITMAP ConvertIconToBitmap(HICON hIcon)
{ 
	HDC	hScreenDC = NULL;
	HDC	hMemDC = NULL;
	HBITMAP	hResultBMP = NULL;
	HBITMAP	hMemBMP = NULL;
	HGDIOBJ	hOrgBMP = NULL;

	for(;;)
	{
		// prepare DC
		hScreenDC = GetDC(NULL);
		if(!hScreenDC) break;
		hMemDC = CreateCompatibleDC(hScreenDC);
		hMemBMP = CreateCompatibleBitmap(hScreenDC,32,32);
		if(!(hMemDC&&hMemBMP)) break;
		hOrgBMP = SelectObject(hMemDC,hMemBMP);
		if(!hOrgBMP) break;

		// draw icon to DC
		if(!DrawIconEx(hMemDC,0,0,hIcon,32,32,0,NULL,DI_NORMAL|DI_COMPAT)) break;

		// set result bitmap
		hResultBMP = hMemBMP;
		hMemBMP = NULL;

		// end
		break;
	}

	// cleanup
	if(hOrgBMP) SelectObject(hMemDC,hOrgBMP);
	if(hMemBMP) DeleteObject(hMemBMP);
	if(hMemDC) DeleteDC(hMemDC);
	if(hScreenDC) ReleaseDC(NULL,hScreenDC);
	//if(hIcon) DestroyIcon(hIcon);

	return hResultBMP;
}


BOOL GetWindowDisplayIcon(HWND hWnd, LPTSTR szExePath, BOOL bMetro, HBITMAP &hBitmap)
{
	//HICON hIcon = GetWindowIcon(hWnd);
	/*if (hIcon)
	{
		hBitmap = ConvertIconToBitmap(hIcon);
		DestroyIcon(hIcon);
		hIcon = NULL;
		return TRUE;
	}
	
	return FALSE;*/
	
	BOOL bRet(FALSE);

	HICON hIcon = GetWindowIcon(hWnd);
	if (hIcon)
	{
		ICONINFO iconinfo;
		GetIconInfo(hIcon, &iconinfo);
		if (iconinfo.hbmColor)
		{
			HBITMAP hBitmapTemp = NULL;
			hBitmapTemp = iconinfo.hbmColor;
			hBitmap = GetTransparentBitmap(hBitmapTemp,32,32);
			if (hBitmap)
			{
				bRet = TRUE;
			}
			if (hBitmapTemp)
			{
				DeleteObject(hBitmapTemp);
				hBitmapTemp = NULL;
			}
			
		}
		DestroyIcon(hIcon);
		hIcon = NULL;
	}
	
	
	
	
	return bRet;
}