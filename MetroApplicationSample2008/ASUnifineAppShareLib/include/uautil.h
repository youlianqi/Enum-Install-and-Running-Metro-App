#pragma once
#include "ASWinrtCommon.h"
#include "AsQueue.h"
#include <windows.h>
#include <string>
#include <tchar.h>
#include <wincodec.h> 
#include <wincodecsdk.h> 

//#pragma comment(lib, "Msimg32.lib")

using namespace std;

typedef struct ASPACKAGE_VERSION {
	union {
		UINT64 Version;
		struct {
			USHORT Revision;
			USHORT Build;
			USHORT Minor;
			USHORT Major;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} ASPACKAGE_VERSION;

typedef struct ASPACKAGE_ID {
	UINT32          reserved;
	UINT32          processorArchitecture;
	ASPACKAGE_VERSION version;
	PWSTR           name;
	PWSTR           publisher;
	PWSTR           resourceId;
	PWSTR           publisherId;
} ASPACKAGE_ID;


typedef  CONST WCHAR *PCNZWCH;
typedef HRESULT (STDAPICALLTYPE *ASRoActivateInstance)( HSTRING pvReserved,IInspectable ** instance);
typedef HRESULT (STDAPICALLTYPE *ASWINDOWSCREATESTRING)(PCNZWCH,UINT32,HSTRING*);
typedef HRESULT (STDAPICALLTYPE *ASWINDOWSDELETESTRING)(HSTRING);
typedef PCWSTR (STDAPICALLTYPE *ASWindowsGetStringRawBuffer)(_In_opt_ HSTRING string,_Out_opt_ UINT32 * length);

typedef DWORD (WINAPI *PFN_GetProcessImageFileName)(HANDLE hProcess, LPTSTR lpImageFileName, DWORD nSize);

typedef DWORD (WINAPI* WIN32FUNCNOPARA)();
typedef DWORD (WINAPI* WIN32FUNCWITHONEPARA)(DWORD dwPara1);
typedef DWORD (WINAPI* WIN32FUNCWITHTWOPARA)(DWORD dwPara1, DWORD dwPara2);
typedef DWORD (WINAPI* WIN32FUNCWITHTHREEPARA)(DWORD dwPara1, DWORD dwPara2, DWORD dwPara3);

typedef LONG (WINAPI* PFN_GetPackageId)(
										_In_       HANDLE hProcess,
										_Inout_    UINT32 *bufferLength,
										_Out_opt_  BYTE *pBuffer
										);

typedef LONG (WINAPI* PFN_GetPackagePath)(
						   _In_        const ASPACKAGE_ID *packageId,
						   _Reserved_  const UINT32 reserved,
						   _Inout_     UINT32 *pathLength,
						   _Out_opt_   PWSTR path
						   );

typedef LONG (WINAPI* PFN_PackageFullNameFromId)(
								  _In_       const ASPACKAGE_ID *packageId,
								  _Inout_    UINT32 *packageFullNameLength,
								  _Out_opt_  PWSTR packageFullName
								  );

LONG WINAPI ASGetPackageId(
							_In_       HANDLE hProcess,
							_Inout_    UINT32 *bufferLength,
							_Out_opt_  BYTE *pBuffer
						 );

LONG WINAPI ASGetPackagePath(
						   _In_        const ASPACKAGE_ID *packageId,
						   _Reserved_  const UINT32 reserved,
						   _Inout_     UINT32 *pathLength,
						   _Out_opt_   PWSTR path
						   );

LONG WINAPI ASPackageFullNameFromId(
								  _In_       const ASPACKAGE_ID *packageId,
								  _Inout_    UINT32 *packageFullNameLength,
								  _Out_opt_  PWSTR packageFullName
								  );

BOOL GetMetroPackageInfoFromHWnd(_In_ HWND hWnd,
								 _Out_opt_ wstring &sPath,
								 _Out_opt_ wstring &sPackageFullName, 
								 _Out_opt_ wstring &sPackageIdName,
								 _Out_opt_ wstring &sApplicationIdName);

BOOL IsWOW64Process(HANDLE hProcess, DWORD dwProcessId);
int GetLinkInfoFromShell(HWND hWnd, LPCTSTR lpszLinkName, LPTSTR lpszExeName, int nExeSize, 
									   LPTSTR lpszWorkingDir, int nDirSize, LPTSTR lpszArg, int nArgSize, LPTSTR lpszDescription, int nDescSize);
BOOL ResolveLinkFile(TCHAR *name, TCHAR *resolution, 
									  TCHAR *arguments, TCHAR *working_dir, HWND hwnd);

BOOL ASSHLoadIndirectString(LPCWSTR pszSource, 
							LPWSTR pszOutBuf, 
							UINT cchOutBuf, 
							PVOID *ppvReserved);



BOOL ASWICConvertBitmapSource(
							  __in REFWICPixelFormatGUID dstFormat, // Destination pixel format
							  __in_ecount(1) IWICBitmapSource  *pISrc,    // Source bitmap
							  __deref_out_ecount(1) IWICBitmapSource **ppIDst   // Destination bitmap, a copy or addrefed source
							  );
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
				   __in BLENDFUNCTION ftn);

HBITMAP GetBitmapFromFile(LPCTSTR p_pchFileName, COLORREF color, DWORD BitWidth, DWORD BitHeight);

enum _tagENUM_OSVER{
	WINDOWS_2K = 5,
	WINDOWS_VISTA = 6,
	WINDOWS_7	= 0x10006,
	WINDOWS_8	= 0x20006
};

#define IS_WINDOW_VISTA_LATER(Version) (LOWORD(Version) >= 6)
#define IS_WINDOW_7_LATER(Version) ((LOWORD((Version)) == 6 && HIWORD((Version)) >= 1) || LOWORD((Version)) > 6)
#define IS_WINDOW_8_LATER(Version) ((LOWORD((Version)) == 6 && HIWORD((Version)) >= 2) || LOWORD((Version)) > 6 )
#define IS_WINDOW_8_1_LATER(Version) ((LOWORD((Version)) == 6 && HIWORD((Version)) >= 3) || LOWORD((Version)) > 6 )

#define IS_WINDOW_8(Version) ((LOWORD(Version)) == 6 && (HIWORD(Version)) == 2)
#define IS_WINDOW_8_1(Version) ((LOWORD(Version)) == 6 && (HIWORD(Version)) == 3)

extern		LONG				g_lVersion;

int GetTextType(const wstring sPath);
BOOL ReadUTF16(const wstring sPath,wstring &sFileContent);
BOOL EqualString(LPCTSTR lpStr1, LPCTSTR lpStr2);
LONG GetOSVersion();

BOOL IsFileExistStat(const std::wstring name);

//set/get/remove prop from desktop window or shell window
BOOL AsSetProp(HWND hWnd,LPCTSTR lpString,HANDLE hData);
HANDLE AsGetProp(HWND hWnd,LPCTSTR lpString);
HANDLE AsRemoveProp(HWND hWnd,LPCTSTR lpString);

BOOL IsSkipCtrlWin(HWND hWnd);

#ifdef UNICODE
#define SZ_GetProcessImageFileName "GetProcessImageFileNameW"
#else
#define SZ_GetProcessImageFileName "GetProcessImageFileNameA"
#endif



enum _tagOsVer
{
	NT_WINDOWS2000	=	0x0500,
	NT_WINDOWSXP	=	0x0501,
	NT_WINDOWSXP64	=   0x0502,
	NT_WINDOWS2003	=	0x0512,
	NT_WINDOWS2003R2=	0x0522,
	NT_WINDOWSHOMESVR=	0x0532,
	NT_VISTA		=	0x0600,
	NT_WINDOWS2008	=	0x0610,
	NT_WINDOWS2008R2=	0x0601,
	NT_WINDOWS7		=	0x0611
};

typedef struct _XP_OSVERSIONINFOEXW {
	DWORD dwOSVersionInfoSize;
	DWORD dwMajorVersion;
	DWORD dwMinorVersion;
	DWORD dwBuildNumber;
	DWORD dwPlatformId;
	WCHAR  szCSDVersion[ 128 ];     // Maintenance string for PSS usage
	WORD   wServicePackMajor;
	WORD   wServicePackMinor;
	WORD   wSuiteMask;
	BYTE  wProductType;
	BYTE  wReserved;
} XP_OSVERSIONINFOEXW, *XP_POSVERSIONINFOEXW;

#define VER_SUITE_WH_SERVER			0x00008000

DWORD IsWinNT();

__inline BOOL IsVistaOrLater()
{
	return (IsWinNT() >= NT_VISTA);
}

BOOL _VT_QueryFullProcessImageName(__in DWORD dwProcessId, __out_ecount(dwBufSize) LPTSTR lpszPath, __in DWORD dwBufSize);

BOOL WINAPI GetImageFileFromWindow(__in HWND hWnd, __out_ecount(dwSize) LPTSTR szApp, __in DWORD dwSize);

HICON	GetWindowIcon(HWND hWnd);
BOOL GetWindowDisplayIcon(HWND hWnd, LPTSTR szExePath, BOOL bMetro, HBITMAP &hBitmap);

struct ComInitializer
{
	ComInitializer()
	{
		m_hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
		//	if (FAILED(m_hr))
		//		;
	}
	ComInitializer(COINIT conType)
	{
		m_hr = CoInitializeEx(NULL, conType);
		//	if (FAILED(m_hr))
		//		;
	}
	ComInitializer(LONG n)
	{
		m_hr = CoInitialize(NULL);
	}
	~ComInitializer()
	{
		if (isValid())
			CoUninitialize();
	}
	bool isValid() const
	{
		return SUCCEEDED(m_hr);
	}
private:
	HRESULT m_hr;
};



