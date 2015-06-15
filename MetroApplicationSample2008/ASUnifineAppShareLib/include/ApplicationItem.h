#pragma once

#include <windows.h>
#include <string>
using namespace std;

class CRunningApplicationItem
{
public:
	CRunningApplicationItem(void);
	~CRunningApplicationItem(void);

	BOOL GetBMetro();
	VOID SetBMetro(const BOOL bMetro);

	HWND GetHWND();
	VOID SetHWND(const HWND hWnd);

	VOID SetIcon(const HICON hIcon);
	HICON GetIcon();

	VOID SetBITMAP(const HBITMAP hBitmap);
	HBITMAP GetBITMAP();

	VOID SetProcessId(const DWORD dProcessId);
	DWORD GetProcessId();

	VOID SetFileDescription(const wstring sFileDescription);
	wstring GetFileDescription();

	VOID SetWindowText(const wstring sWindowText);
	wstring GetWindowText();

	VOID SetClassName(const wstring sClassName);
	wstring GetClassName();

	VOID SetDisplayName(const wstring sDisplayName);
	wstring GetDisplayName();

	VOID SetInstallPath(const wstring sInstallPath);
	wstring GetInstallPath();

	VOID SetExeName(const wstring sExeName);
	wstring GetExeName();

private:
	BOOL m_bMetro;
	HWND m_hWnd;
	HBITMAP m_hBitmap;
	HICON m_hIcon;
	DWORD m_dwPorcessid;

	wstring	m_sFileDescription;
	wstring	m_sWindowText;
	wstring m_sClassName;
	wstring m_sDisplayName;
	wstring	m_sInstallPath;
	wstring m_sExeName;
};

//The basic class of desktop application and metro application 
class CApplicationItem
{
public:
	CApplicationItem(void);
	~CApplicationItem(void);

	VOID SetFileDescription(const wstring sFileDescription);
	wstring GetFileDescription();

	VOID SetApplicationName(const wstring sApplicationName);
	wstring GetApplicationName();

	VOID SetInstallPath(const wstring sInstallPath);
	wstring GetInstallPath();

	VOID SetIcon(const HICON hIcon);
	HICON GetIcon();

	VOID SetBITMAP(const HBITMAP hBitmap);
	HBITMAP GetBITMAP();

private:
	wstring	m_sFileDescription;
	wstring	m_sApplicationName;
	wstring	m_sInstallPath;
	HICON	m_hIcon;
	HBITMAP m_hBitmap;
	HWND	m_hWnd;
};

//Save information to CMetroPackageInfo from AppxManifest.xml 
struct CMetroPackageInfo
{
//public:
//	CMetroPackageInfo(void);
//	~CMetroPackageInfo(void);
//private:
	wstring m_SInstallPackagePath;
	wstring m_SPackageFullName;
	wstring m_SPackageIdName;
	wstring m_SApplicationID;
	wstring m_SSmallLogo;
	wstring m_SLogoApplication;		 //from <application>
	wstring m_SLogoProperties;		 //from <Properties> 
	wstring m_SSquare30x30Logo;
	wstring m_SBackgroundColor;
	wstring m_SDisplayName;
};

//
class CMetroApplicationItem : public CApplicationItem
{
public:
	CMetroApplicationItem(void);
	~CMetroApplicationItem(void);

	VOID SetAppUserModelId(const wstring sAppUserModelId);
	wstring GetAppUserModelId();

private:
//	COLORREF m_ColorLogoRef;
	wstring m_SAppUserModelId;
};

//Use in for ActivateApplicationInOtherThread
typedef struct SMetroAppParam
{
	wstring sAppUserModelId;
}UMetroAppParam, *PMetroAppParam;

typedef enum _DisplayImage_From_Type {
	DisplayImage_From_CombineImgaeAndBackgroundColor = 0,
	DisplayImage_From_Targetsize,
} DisplayImage_From_Type;

