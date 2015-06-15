#include "StdAfx.h"
#include "ApplicationItem.h"

//////////////////////////////////////////CRunningApplicationItem
CRunningApplicationItem::CRunningApplicationItem(void)
{
	m_bMetro = FALSE;

	m_hWnd = NULL;
	m_hBitmap = NULL;
	m_hIcon = NULL;

	m_sFileDescription = L"";
	m_sWindowText = L"";
	m_sClassName = L"";
	m_sDisplayName = L"";
	m_sInstallPath = L"";
	m_sExeName = L"";
}

CRunningApplicationItem::~CRunningApplicationItem(void)
{
	if (m_hIcon)
		DestroyIcon(m_hIcon);
	m_hIcon = NULL;

	if (m_hBitmap)
		DeleteObject(m_hBitmap);
	m_hBitmap = NULL;

	if (m_hWnd)
		DeleteObject(m_hWnd);
	m_hWnd = NULL;
}

BOOL CRunningApplicationItem::GetBMetro()
{
	return m_bMetro;
}

VOID CRunningApplicationItem::SetBMetro(const BOOL bMetro)
{
	m_bMetro = bMetro;
}

HWND CRunningApplicationItem::GetHWND()
{
	return m_hWnd;
}

VOID CRunningApplicationItem::SetHWND(const HWND hWnd)
{
	m_hWnd = hWnd;
}

VOID CRunningApplicationItem::SetIcon(const HICON hIcon)
{
	m_hIcon = hIcon;
}

HICON CRunningApplicationItem::GetIcon()
{
	return m_hIcon;
}

HBITMAP CRunningApplicationItem::GetBITMAP()
{
	return m_hBitmap;
}

VOID CRunningApplicationItem::SetBITMAP(const HBITMAP hBitmap)
{
	m_hBitmap = hBitmap;
}

VOID CRunningApplicationItem::SetProcessId(const DWORD dProcessId)
{
	m_dwPorcessid = dProcessId;
}

DWORD CRunningApplicationItem::GetProcessId()
{
	return m_dwPorcessid;
}

VOID CRunningApplicationItem::SetFileDescription(const wstring sFileDescription)
{
	m_sFileDescription = sFileDescription;
}

wstring CRunningApplicationItem::GetFileDescription()
{
	return m_sFileDescription;
}

VOID CRunningApplicationItem::SetWindowText(const wstring sWindowText)
{
	m_sWindowText = sWindowText;
}

wstring CRunningApplicationItem::GetWindowText()
{
	return m_sWindowText;
}

VOID CRunningApplicationItem::SetClassName(const wstring sClassName)
{
	m_sClassName = sClassName;
}

wstring CRunningApplicationItem::GetClassName()
{
	return m_sClassName;
}

VOID CRunningApplicationItem::SetDisplayName(const wstring sDisplayName)
{
	m_sDisplayName = sDisplayName;
}

wstring CRunningApplicationItem::GetDisplayName()
{
	return m_sDisplayName;
}

VOID CRunningApplicationItem::SetInstallPath(const wstring sInstallPath)
{
	m_sInstallPath = sInstallPath;
}

wstring CRunningApplicationItem::GetInstallPath()
{
	return m_sInstallPath;
}

VOID CRunningApplicationItem::SetExeName(const wstring sExeName)
{
	m_sExeName = sExeName;
}

wstring CRunningApplicationItem::GetExeName()
{
	return m_sExeName;
}


/////////////////////////////////////////////////////////////////////////////////

CApplicationItem::CApplicationItem(void)
{
	m_hIcon = NULL;
	m_hBitmap = NULL;
	m_hWnd = NULL;

	m_sFileDescription = L"";
	m_sApplicationName = L"";
	m_sInstallPath = L"";
}

CApplicationItem::~CApplicationItem(void)
{
	if (m_hIcon)
		DestroyIcon(m_hIcon);
	m_hIcon = NULL;

	if (m_hBitmap)
		DeleteObject(m_hBitmap);
	m_hBitmap = NULL;

	if (m_hWnd)
		DeleteObject(m_hWnd);
	m_hWnd = NULL;
}

VOID CApplicationItem::SetFileDescription(const wstring sFileDescription)
{
	m_sFileDescription = sFileDescription;
}

wstring CApplicationItem::GetFileDescription()
{
	return m_sFileDescription;
}

VOID CApplicationItem::SetApplicationName(const wstring sApplicationName)
{
	m_sApplicationName = sApplicationName;
}

wstring CApplicationItem::GetApplicationName()
{
	return m_sApplicationName;
}

VOID CApplicationItem::SetInstallPath(const wstring sInstallPath)
{
	m_sInstallPath = sInstallPath;
}

wstring CApplicationItem::GetInstallPath()
{
	return m_sInstallPath;
}

VOID CApplicationItem::SetIcon(const HICON hIcon)
{
	m_hIcon = hIcon;
	//m_hIcon = CopyIcon(hIcon);
}

HICON CApplicationItem::GetIcon()
{
	return m_hIcon;
}

VOID CApplicationItem::SetBITMAP(const HBITMAP hBitmap)
{
	m_hBitmap = hBitmap;
}

HBITMAP CApplicationItem::GetBITMAP()
{
	return m_hBitmap;
}

/////////////////////////////////////////////////////////////////////////////////

//CMetroPackageInfo::CMetroPackageInfo(void)
//{
//}
//
//CMetroPackageInfo::~CMetroPackageInfo(void)
//{
//}

/////////////////////////////////////////////////////////////////////////////////

CMetroApplicationItem::CMetroApplicationItem(void)
{
	m_SAppUserModelId = L"";
}

CMetroApplicationItem::~CMetroApplicationItem(void)
{
}

VOID CMetroApplicationItem::SetAppUserModelId(const wstring sAppUserModelId)
{
	m_SAppUserModelId = sAppUserModelId;
}

wstring CMetroApplicationItem::GetAppUserModelId()
{
	return m_SAppUserModelId;
}

