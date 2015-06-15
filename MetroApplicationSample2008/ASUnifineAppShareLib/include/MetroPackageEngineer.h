#pragma once

#include <windows.h>
#include "ApplicationItem.h"
#include "uautil.h"

class CMetroPackageEngineer
{
public:
	CMetroPackageEngineer(void);
	~CMetroPackageEngineer(void);
	static CMetroPackageEngineer& GetInstance();

	BOOL GetInstallMetroApplicationList();
	BOOL ActiveApplication(const wstring pSAppUserModelId);

	//CDvArray	m_MetroPackageList;
	CDvArray	m_MetroApplicationList;

private:

	BOOL DoLoadLibrary();
	VOID DoFreeLibrary();
	BOOL DoLoadCombaseLibrary();
	VOID DoFreeCombaseLibrary();

	BOOL GetMetroInstallPackageListFromInstallPath(const wstring sPath, const wstring sFullName, const wstring sIdName);
	BOOL GetMetroPackageAppxManifestXml(wstring &sAppxManifestXmlA, wstring sPath);
	BOOL GetMetroInstallPackageListFromFromAppxManifestXml(const wstring sAppxManifestXmlA, const wstring sPath, const wstring sFullName, const wstring sIdName);
	BOOL GetMetroPackageInfo(const wstring sApplication, CMetroPackageInfo *metroPackageInfo);
	BOOL GetApplicationItemBase(wstring &sOut, const wstring sContentSrc, const wstring sFoundContentSrc);

	BOOL GetMetroApplicationItem(const wstring sPath, const wstring sFullName, const wstring sIdName, const wstring sApplication,CMetroApplicationItem *metroApplicationItem);
	

	BOOL ConvertMetroPackageToMetroApplication(const CMetroPackageInfo *metroPackageInfo, CMetroApplicationItem *metroApplicationItem);
	BOOL GenerateAppUserModelId(const CMetroPackageInfo *metroPackageInfo, wstring &sAppUserModelId);
	BOOL GenerateBackgroundColorRef(const CMetroPackageInfo *metroPackageInfo, COLORREF &colorRefA);
	BOOL GenerateDisplayIcon(const CMetroPackageInfo *metroPackageInfo,const COLORREF colorRefA, HBITMAP &hBitmap);
	BOOL GenerateDisplayName(const CMetroPackageInfo *metroPackageInfo, wstring &sDisplayName);

	static CMetroPackageEngineer g_PMetroPackageEngineerInstance;
	HMODULE m_hComModule;

	////CDvArray	m_MetroPackageList;
	//CDvArray	m_MetroApplicationList;
	//Clear m_MetroPackageList
	BOOL ClearMetroApplicationList();

	ASRoActivateInstance pftRoActivateInstance;
	ASWINDOWSCREATESTRING pfnWindowsCreateString;
	ASWINDOWSDELETESTRING pfnWindowsDeleteString;
	ASWindowsGetStringRawBuffer pfnWindowsGetStringRawBuffer;
	//ASSHLoadIndirectString pfnSHLoadIndirectString;
};