#include "StdAfx.h"
#include "MetroPackageEngineer.h"
#include "ASWinrtAddtional.h"
#include "ApplicationItem.h"
#include "SmartPtr.h"
#include "uautil.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>


using namespace std;

const wstring WS_LogoPre_In_Properties = L"<Logo>";
const wstring WS_LogoSuf_In_Properties = L"</Logo>";

const wstring WS_Applications_Pre = L"<Applications>";
const wstring WS_Applications_Suf = L"</Applications>";

const wstring WS_ApplicationPre_In_Applications = L"<Application";
const wstring WS_ApplicationSuf_In_Applications = L"</Application>";

const wstring WS_Id_In_Application = L"Id=";
const wstring WS_BackgroundColor_In_Application = L"BackgroundColor=";
const wstring WS_SmallLogo_In_Application = L"SmallLogo=";
const wstring WS_Logo_In_Application = L"Logo=";
const wstring WS_Square30x30Logo_In_Application = L"Square30x30Logo=";
const wstring WS_DisplayName_In_Application = L"DisplayName=";
const wstring WS_DoubleQuoteMark = L"\"";

CMetroPackageEngineer CMetroPackageEngineer::g_PMetroPackageEngineerInstance;

CMetroPackageEngineer& CMetroPackageEngineer::GetInstance()
{
	return g_PMetroPackageEngineerInstance;
}

CMetroPackageEngineer::CMetroPackageEngineer(void)
{
	g_lVersion = GetOSVersion();

	ClearMetroApplicationList();
}

CMetroPackageEngineer::~CMetroPackageEngineer(void)
{
	ClearMetroApplicationList();
}

BOOL CMetroPackageEngineer::ClearMetroApplicationList()
{
	for (int i=0; i<m_MetroApplicationList.GetCount(); i++)
	{
		CMetroApplicationItem * pMetroApplcaitionItem =(CMetroApplicationItem *)m_MetroApplicationList[i];
		if (pMetroApplcaitionItem)
		{
			delete pMetroApplcaitionItem;
			pMetroApplcaitionItem = NULL;
		}	
	}
	m_MetroApplicationList.RemoveAll();
	return TRUE;
}

BOOL CMetroPackageEngineer::DoLoadLibrary()
{
	return FALSE;
}

VOID CMetroPackageEngineer::DoFreeLibrary()
{
}

BOOL CMetroPackageEngineer::DoLoadCombaseLibrary()
{
	BOOL bRet(FALSE);

	m_hComModule = LoadLibrary(L"combase.dll");

	if(NULL != m_hComModule)
	{
		pftRoActivateInstance = (ASRoActivateInstance)GetProcAddress( m_hComModule , "RoActivateInstance");
		pfnWindowsCreateString = (ASWINDOWSCREATESTRING)GetProcAddress( m_hComModule , "WindowsCreateString");
		pfnWindowsDeleteString = (ASWINDOWSDELETESTRING)GetProcAddress( m_hComModule , "WindowsDeleteString");
		pfnWindowsGetStringRawBuffer = (ASWindowsGetStringRawBuffer)GetProcAddress( m_hComModule , "WindowsGetStringRawBuffer");

		if (  NULL == pftRoActivateInstance 
			||NULL == pfnWindowsCreateString
			||NULL == pfnWindowsDeleteString
			||NULL == pfnWindowsGetStringRawBuffer)
		{
			DoFreeCombaseLibrary();
		}
		else
		{
			bRet = TRUE;
		}
	}

	return bRet;
}

VOID CMetroPackageEngineer::DoFreeCombaseLibrary()
{
	if(NULL != m_hComModule)
	{
		::FreeLibrary(m_hComModule);
	}
}

BOOL CMetroPackageEngineer::GetInstallMetroApplicationList()
{
	BOOL bRet(FALSE);

	bRet = DoLoadCombaseLibrary();

	if (!bRet) 
	{
		return FALSE;
	}

	ClearMetroApplicationList();

	ComInitializer com(COINIT_APARTMENTTHREADED);
	if (!com.isValid())
		return FALSE;

	LPCTSTR str = _T("Windows.Management.Deployment.PackageManager");
	HSTRING hstr;

	HRESULT hr = pfnWindowsCreateString(str, _tcslen(str), &hstr);
	if (FAILED(hr))
	{
		//log here
		return FALSE;
	}

	CSmartPtr<IInspectable> pInspectObj;
	hr = pftRoActivateInstance(hstr,(IInspectable**)&pInspectObj);
	if (FAILED(hr))
	{
		//log here
		return FALSE;
	}

	CSmartPtr<IPackageManager> pIPackageManager;
	if ( pInspectObj )
	{
		pInspectObj->QueryInterface(IID_IPackageManager,(void**)&pIPackageManager);
	}

	hr = pfnWindowsDeleteString(hstr);
	if (FAILED(hr))
	{
		//log here
		return FALSE;
	}

	CSmartPtr<IIterable<Package *>> pPackageCollection;
	pIPackageManager->FindPackagesByUserSecurityId(NULL,&pPackageCollection);


	CSmartPtr<IIterator<Package *>> pIteratorPackage;
	hr = pPackageCollection->First(&pIteratorPackage);
	if (FAILED(hr))
	{
	}

	boolean hasCurrent;
	hr = pIteratorPackage->get_HasCurrent(&hasCurrent);

	while (SUCCEEDED(hr) && hasCurrent) 
	{
		CSmartPtr<IPackage> package;
		hr = pIteratorPackage->get_Current(&package);
		if (FAILED(hr)) {
			hr = pIteratorPackage->MoveNext(&hasCurrent);
			continue;
		}

		CSmartPtr<IPackageId> packageId;
		hr = package->get_Id(&packageId);
		if (FAILED(hr)) {
			hr = pIteratorPackage->MoveNext(&hasCurrent);
			continue;
		}

		CSmartPtr<IStorageFolder> storageFolder;
		hr = package->get_InstalledLocation(&storageFolder);
		if (FAILED(hr))
		{
			hr = pIteratorPackage->MoveNext(&hasCurrent);
			continue;
		}

		CSmartPtr<IStorageItem> storageItem;
		hr = storageFolder->QueryInterface(IID_IStorageItem,(void**)&storageItem);
		if (FAILED(hr))
		{
			hr = pIteratorPackage->MoveNext(&hasCurrent);
			continue;
		}

		std::string result = "";
		HSTRING hsPath;
		storageItem->get_Path(&hsPath);

		std::wstring sInstallPath = std::wstring(pfnWindowsGetStringRawBuffer(hsPath, 0));

		HSTRING fullName;
		hr = packageId->get_FullName(&fullName);
		if (FAILED(hr)) 
		{
			if (hsPath)
				pfnWindowsDeleteString(hsPath);
			hr = pIteratorPackage->MoveNext(&hasCurrent);
			continue;
		}

		HSTRING packageIdName;
		hr = packageId->get_Name(&packageIdName);
		if (FAILED(hr)) 
		{
			if (hsPath)
				pfnWindowsDeleteString(hsPath);
			hr = pIteratorPackage->MoveNext(&hasCurrent);
			continue;
		}
		
		std::wstring sFullName = std::wstring(pfnWindowsGetStringRawBuffer(fullName, 0));
		std::wstring sIdName = std::wstring(pfnWindowsGetStringRawBuffer(packageIdName, 0));

		if (!GetMetroInstallPackageListFromInstallPath(sInstallPath,sFullName,sIdName))
		{
			//log here
		}

		if (hsPath)
			pfnWindowsDeleteString(hsPath);

		hr = pIteratorPackage->MoveNext(&hasCurrent);
	}

	//// //Check install metro application list data
	//for (int i=0; i<m_MetroApplicationList.GetCount(); i++)
	//{
	//	CMetroApplicationItem * metroItem =(CMetroApplicationItem *)m_MetroApplicationList[i];
	//	wstring packageUserId = metroItem->GetAppUserModelId();
	//}

	return bRet;
}

SMetroAppParam gSMetroAppParam;

DWORD WINAPI ActivateApplicationInOtherThread(LPVOID lpParamter)
{
	PMetroAppParam sparam;
	sparam = (PMetroAppParam)lpParamter;
	try
	{
		ComInitializer com(COINIT_MULTITHREADED);
		if (!com.isValid())
			return 0;

		CSmartPtr<IApplicationActivationManager> spAppActivationManager;

		HRESULT hr;

		if (FAILED(CoCreateInstance(CLSID_ApplicationActivationManager,
			NULL,
			CLSCTX_LOCAL_SERVER,
			IID_IApplicationActivationManager,
			(LPVOID*)&spAppActivationManager)))
		{
			return 0;
		}

		hr = CoAllowSetForegroundWindow(spAppActivationManager, NULL);

		if (FAILED(hr)) return 0;

		// Launch it!
		if (SUCCEEDED(hr))
		{
			DWORD dwProcessId = 0;
			hr = spAppActivationManager->ActivateApplication(sparam->sAppUserModelId.c_str(),
				//hr = spAppActivationManager->ActivateApplication(s_appUserId.c_str(),
				NULL,
				AO_NONE,
				&dwProcessId);
		}
	}
	catch (...)
	{

	}
	return 1;
}

BOOL CMetroPackageEngineer::ActiveApplication(const wstring sAppUserModelId)
{
	BOOL bRet(FALSE);

	if (sAppUserModelId.empty()) return bRet;

	gSMetroAppParam.sAppUserModelId = sAppUserModelId;
	
	HANDLE ThreadHandle = NULL;
	DWORD ThreadID;
	ThreadHandle = (HANDLE)CreateThread(NULL, 0, ActivateApplicationInOtherThread, (LPVOID)&gSMetroAppParam, 0, &ThreadID);
	bRet = CloseHandle(ThreadHandle);

	return bRet;
}

BOOL CMetroPackageEngineer::GetMetroApplicationItem(const wstring sPath, const wstring sFullName, const wstring sIdName,const wstring sApplication, CMetroApplicationItem *metroApplicationItem)
{
	wstring sAppxManifestXmlA = L"";

	if (!GetMetroPackageAppxManifestXml(sAppxManifestXmlA,sPath)) 
	{
		//log
		return FALSE;
	}

	BOOL bRet(FALSE);

	//Get Logo from <Properties> </Properties>, since logo, small logo,Square30x30Logo of  metro package  aren't true.
	size_t size_LogoPre = WS_LogoPre_In_Properties.length();
	size_t size_LogoSuf = WS_LogoSuf_In_Properties.length();

	std::wstring::size_type nFoundLogoPre = sAppxManifestXmlA.find(WS_LogoPre_In_Properties);
	std::wstring::size_type nFoundLogoSuf = sAppxManifestXmlA.find(WS_LogoSuf_In_Properties);

	wstring sLogoProperties = L"";
	if (nFoundLogoPre != std::wstring::npos
		&& nFoundLogoSuf != std::wstring::npos)
	{
		sLogoProperties = sAppxManifestXmlA.substr(nFoundLogoPre+size_LogoPre, nFoundLogoSuf - nFoundLogoPre - size_LogoPre);
	}

	//Get Applications
	size_t size_ApplicationsPre = WS_Applications_Pre.length();
	size_t size_ApplicationsSuf = WS_Applications_Suf.length();

	std::wstring::size_type nFoundApplicationsPre = sAppxManifestXmlA.find(WS_Applications_Pre);
	std::wstring::size_type nFoundApplicationsSuf = sAppxManifestXmlA.find(WS_Applications_Suf);

	if (nFoundApplicationsPre != std::wstring::npos
		&& nFoundApplicationsSuf != std::wstring::npos)
	{
		wstring sApplications = sAppxManifestXmlA.substr(nFoundApplicationsPre, nFoundApplicationsSuf - nFoundApplicationsPre + size_ApplicationsSuf);

		std::wstring::size_type nFoundApplicationPre_In_Applications = sApplications.find(WS_ApplicationPre_In_Applications, size_ApplicationsPre);
		std::wstring::size_type nFoundApplicationSuf_In_Applications = sApplications.find(WS_ApplicationSuf_In_Applications);

		while (nFoundApplicationPre_In_Applications != std::wstring::npos
			&& nFoundApplicationSuf_In_Applications != std::wstring::npos
			&& nFoundApplicationSuf_In_Applications > nFoundApplicationPre_In_Applications)
		{
			wstring sApplication = sApplications.substr(nFoundApplicationPre_In_Applications, 
				nFoundApplicationSuf_In_Applications - nFoundApplicationPre_In_Applications + WS_ApplicationSuf_In_Applications.length());

			CMetroPackageInfo *metroPackageInfo = new CMetroPackageInfo;
			if (GetMetroPackageInfo(sApplication, metroPackageInfo))
			{
				metroPackageInfo->m_SInstallPackagePath = sPath;
				metroPackageInfo->m_SPackageFullName = sFullName;
				metroPackageInfo->m_SPackageIdName = sIdName;
				metroPackageInfo->m_SLogoProperties = sLogoProperties;

				if (EqualString(sApplication.c_str(),metroPackageInfo->m_SApplicationID.c_str()))
				{
					ConvertMetroPackageToMetroApplication(metroPackageInfo,metroApplicationItem);
					bRet = TRUE;
				}
				
			}

			if (metroPackageInfo)
			{
				delete metroPackageInfo;
				metroPackageInfo = NULL;
			}

			nFoundApplicationPre_In_Applications = sApplications.find(WS_ApplicationPre_In_Applications, nFoundApplicationSuf_In_Applications + 1);
			nFoundApplicationSuf_In_Applications = sApplications.find(WS_ApplicationSuf_In_Applications, nFoundApplicationSuf_In_Applications + 1);
		}
		return bRet;
	}
	//WinStore and immersive control panel are specially in Windows 8
	else
	{
		//WinSotre & PC Setting aren't include <applications>, need special handle.
		const wstring sPackageIdStore = L"WinStore";
		const wstring sPackageIdimmersivecontrolpanel = L"windows.immersivecontrolpanel";

		if (EqualString(sPackageIdStore.c_str(), sIdName.c_str()) 
			|| EqualString(sPackageIdimmersivecontrolpanel.c_str(), sIdName.c_str()) )
		{
			//<DisplayName> </DisplayName>
			const wstring sDisplayNameBegin = L"<DisplayName>";
			const wstring sDisplayNameEnd = L"</DisplayName>";

			size_t size_DisplayNameBegin = sDisplayNameBegin.length();
			size_t size_DisplayNameEnd = sDisplayNameEnd.length();

			std::wstring::size_type nFoundDisplayNameBegin = sAppxManifestXmlA.find(sDisplayNameBegin);
			std::wstring::size_type nFoundDisplayNameEnd = sAppxManifestXmlA.find(sDisplayNameEnd);

			wstring sDisplayName = L"";
			if (nFoundDisplayNameBegin != std::wstring::npos
				&& nFoundDisplayNameEnd != std::wstring::npos)
			{
				wstring sDisplayName = sAppxManifestXmlA.substr(nFoundDisplayNameBegin + size_DisplayNameBegin, 
					nFoundDisplayNameEnd - nFoundDisplayNameBegin - size_DisplayNameBegin);

				CMetroPackageInfo *metroPackageInfo = new CMetroPackageInfo;
				metroPackageInfo->m_SDisplayName = sDisplayName;
				metroPackageInfo->m_SInstallPackagePath = sPath;
				metroPackageInfo->m_SPackageFullName = sFullName;
				metroPackageInfo->m_SPackageIdName = sIdName;
				metroPackageInfo->m_SLogoProperties = sLogoProperties;

				if (IS_WINDOW_8(g_lVersion))
				{
					if (EqualString(sPackageIdStore.c_str(), sIdName.c_str()))
					{
						metroPackageInfo->m_SApplicationID = L"Windows.Store";
					}
					else if (EqualString(sPackageIdimmersivecontrolpanel.c_str(), sIdName.c_str()))
					{
						metroPackageInfo->m_SApplicationID = L"microsoft.windows.immersivecontrolpanel";
					}
				}

				if (EqualString(sApplication.c_str(),metroPackageInfo->m_SApplicationID.c_str()))
				{
					ConvertMetroPackageToMetroApplication(metroPackageInfo,metroApplicationItem);
					bRet = TRUE;
				}

				if (metroPackageInfo)
				{
					delete metroPackageInfo;
					metroPackageInfo = NULL;
				}

				bRet = TRUE;
			}
		}
		return bRet;
	}

	return bRet;

}

BOOL CMetroPackageEngineer::GetMetroInstallPackageListFromInstallPath(const wstring sPath, const wstring sFullName, const wstring sIdName)
{
	wstring sAppxManifestXml = L"";

	if (!GetMetroPackageAppxManifestXml(sAppxManifestXml,sPath)) 
	{
		//log
		return FALSE;
	}
		
	if (!GetMetroInstallPackageListFromFromAppxManifestXml(sAppxManifestXml,sPath,sFullName,sIdName))
	{
		//log
		return FALSE;
	}
	
	return TRUE;
}


BOOL CMetroPackageEngineer::GetMetroPackageAppxManifestXml(wstring &sAppxManifestXmlA, wstring sPath)
{
	BOOL bRet(FALSE);

	wstring ws_Path = std::wstring(sPath);

	ws_Path = ws_Path + L"\\AppxManifest.xml";

	//utf-16
	if (1 == GetTextType(ws_Path))
	{
		ReadUTF16(sPath,sAppxManifestXmlA);
		return TRUE;
	}

	DWORD dwBytesWritten = 0;
	HANDLE hFile = CreateFile(ws_Path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwFileSize = GetFileSize(hFile, NULL);
		CHAR *pszFileBuffer = new CHAR[dwFileSize];
		ReadFile(hFile, (LPVOID)pszFileBuffer, dwFileSize, &dwBytesWritten, NULL);

		int len = MultiByteToWideChar(CP_UTF8, 0, pszFileBuffer, dwBytesWritten, 0, 0);
		wchar_t* buf = new wchar_t[len+1];
		MultiByteToWideChar(CP_UTF8, 0, pszFileBuffer, dwBytesWritten, buf, len);
		buf[len] = '\0';

		if (hFile)
		{
			CloseHandle(hFile);
			hFile = NULL;
		}

		sAppxManifestXmlA = std::wstring(buf);
		delete buf;
		buf = NULL;

		return TRUE;
	}
	return TRUE;
}

BOOL CMetroPackageEngineer::GenerateAppUserModelId(const CMetroPackageInfo *metroPackageInfo, wstring &sAppUserModelId)
{
	if (metroPackageInfo->m_SInstallPackagePath.empty() 
		|| metroPackageInfo->m_SPackageFullName.empty() 
		|| metroPackageInfo->m_SApplicationID.empty())
		return FALSE;

	wstring sFullpackagePathInRegistry
		= L"ActivatableClasses\\Package\\" +
		metroPackageInfo->m_SPackageFullName +
		L"\\Server";


	HKEY hKey = 0;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CLASSES_ROOT, sFullpackagePathInRegistry.c_str(), 0, KEY_READ, &hKey))
	{
		RegCloseKey(hKey);
		return FALSE;
	}

	DWORD n; // subkeys
	RegQueryInfoKey(hKey, NULL, NULL, NULL, &n, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

	for (DWORD i = 0; i < n; i++)
	{
		WCHAR dwValue[255];
		DWORD  dwSize = sizeof(dwValue);

		RegEnumKeyEx(hKey, i, dwValue, &dwSize, NULL, NULL, NULL, NULL);
		wstring sNewApplicationID = wstring(dwValue);
		sNewApplicationID = sNewApplicationID.substr(0,metroPackageInfo->m_SApplicationID.length());
		if (EqualString(metroPackageInfo->m_SApplicationID.c_str(),sNewApplicationID.c_str()))
		{
			DWORD dwType = REG_SZ;
			DWORD cbData = MAX_PATH;
			//		DWORD dwValue1 = 0;
			wchar_t wValue[1024];

			sFullpackagePathInRegistry = sFullpackagePathInRegistry + L"\\" + wstring(dwValue);

			if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CLASSES_ROOT, sFullpackagePathInRegistry.c_str(), 0, KEY_READ, &hKey))
			{
				RegCloseKey(hKey);
				return FALSE;
			}

			if (ERROR_SUCCESS != RegQueryValueExW(hKey, _T("AppUserModelId"), NULL, &dwType, (LPBYTE)&wValue, &cbData))
			{
				RegCloseKey(hKey);
				return FALSE;
			}

			sAppUserModelId = wstring(wValue);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CMetroPackageEngineer::GenerateBackgroundColorRef(const CMetroPackageInfo *metroPackageInfo, COLORREF &colorRefA)
{
	const wstring sApplicationIDimmersivecontrolpanel = L"microsoft.windows.immersivecontrolpanel";
	const wstring sApplicationIDStore = L"Windows.Store";
	const wstring sApplicationIDFileMangaer = L"Microsoft.Windows.FileManager";

	if (IS_WINDOW_8_LATER(g_lVersion))
	{
		if (EqualString(sApplicationIDimmersivecontrolpanel.c_str(),metroPackageInfo->m_SApplicationID.c_str()))
		{
			colorRefA = RGB(81, 51, 171);
			return TRUE;
		}
		else if (EqualString(sApplicationIDStore.c_str(),metroPackageInfo->m_SApplicationID.c_str()))
		{
			colorRefA = RGB(0, 138, 0);
			return TRUE;
		}

		if (IS_WINDOW_8_1(g_lVersion))
		{
			if (EqualString(sApplicationIDFileMangaer.c_str(),metroPackageInfo->m_SApplicationID.c_str()))
			{
				colorRefA = RGB(9, 74, 178);
				return TRUE;
			}
		}
	}

	wstring sRGB = L"";
	if (7 == metroPackageInfo->m_SBackgroundColor.length())
	{
		sRGB = metroPackageInfo->m_SBackgroundColor.substr(1, 6);
	}
	else
	{
		return FALSE;
	}

	DWORD dwColor = (DWORD)wcstol(sRGB.c_str(), NULL, 16);
	colorRefA =  RGB(dwColor>>16, (dwColor & 0xFF00)>>8, (dwColor & 0xFF));
	
	return TRUE;
}

BOOL CMetroPackageEngineer::GenerateDisplayIcon(const CMetroPackageInfo *metroPackageInfo,const COLORREF colorRefA,HBITMAP &hBitmap)
{
	if (metroPackageInfo->m_SInstallPackagePath.empty())
		return FALSE;

	DisplayImage_From_Type nImageFromType = DisplayImage_From_CombineImgaeAndBackgroundColor;

	wstring sImageDirectoryTemp = L"";

	//Get logo Directory
	if (!metroPackageInfo->m_SSmallLogo.empty())
	{
		sImageDirectoryTemp = metroPackageInfo->m_SInstallPackagePath + L"\\" + metroPackageInfo->m_SSmallLogo;
	}
	else if ( !metroPackageInfo->m_SSquare30x30Logo.empty())
	{
		sImageDirectoryTemp = metroPackageInfo->m_SInstallPackagePath + L"\\" + metroPackageInfo->m_SSquare30x30Logo;
	}
	else if (!metroPackageInfo->m_SLogoProperties.empty())
	{
		sImageDirectoryTemp = metroPackageInfo->m_SInstallPackagePath + L"\\" + metroPackageInfo->m_SLogoProperties;
	}
	else 
	{
		return FALSE;
	}

	std::wstring::size_type ndoc = sImageDirectoryTemp.find_last_of('.');
	wstring simageDirectoryOfRemovePostfix = sImageDirectoryTemp.substr(0, ndoc+1);

	wstring sDisplayImageDirectory = L"";

	//targetsize-32
	wstring sTargetsize_32 = simageDirectoryOfRemovePostfix + L"targetsize-32.png";
	//scale-100.png
	wstring sScale_100 = simageDirectoryOfRemovePostfix + L"scale-100.png";

	if (IsFileExistStat(sTargetsize_32))
	{
		sDisplayImageDirectory = sTargetsize_32;
		nImageFromType = DisplayImage_From_Targetsize;
		goto __SETDISPLAYIMAGEDIRECTORY;
	}

	//old
	if (IsFileExistStat(sImageDirectoryTemp))
	{
		sDisplayImageDirectory = sImageDirectoryTemp;
		goto __SETDISPLAYIMAGEDIRECTORY;
	}

	if (IsFileExistStat(sScale_100))
	{
		sDisplayImageDirectory = sScale_100;
		goto __SETDISPLAYIMAGEDIRECTORY;
	}

	//Because some application smalllogo string is not empty, but targetsize or scale-100 file is not exist.
	//logo from  <properties> <Logo> </Logo>
	sImageDirectoryTemp = metroPackageInfo->m_SInstallPackagePath + L"\\" + metroPackageInfo->m_SLogoProperties;
	ndoc = sImageDirectoryTemp.find_last_of('.');
	simageDirectoryOfRemovePostfix =  sImageDirectoryTemp.substr(0, ndoc+1);

	//old
	if (IsFileExistStat(sImageDirectoryTemp))
	{
		sDisplayImageDirectory = sImageDirectoryTemp;
		goto __SETDISPLAYIMAGEDIRECTORY;
	}

	sScale_100 = simageDirectoryOfRemovePostfix + L"scale-100.png";

	if (IsFileExistStat(sScale_100))
	{
		sDisplayImageDirectory = sScale_100;
		goto __SETDISPLAYIMAGEDIRECTORY;
	}

__SETDISPLAYIMAGEDIRECTORY:

	wstring sImageDirectory = L"";
	if (!sDisplayImageDirectory.empty())
	{
		sImageDirectory = sDisplayImageDirectory;
		if (IS_WINDOW_8_1(g_lVersion))
		{
			const wstring sApplicationIDPhotoManager = L"Microsoft.Windows.PhotoManager";//
			const wstring sPackageIDCamera = L"Microsoft.MoCamera";

			if (EqualString(metroPackageInfo->m_SPackageIdName.c_str(),sPackageIDCamera.c_str()))
			{
				sImageDirectoryTemp = metroPackageInfo->m_SInstallPackagePath + L"\\Assets\\SmallIcon.targetsize-32.png";
				if (IsFileExistStat(sImageDirectoryTemp))
				{
					sImageDirectory = sImageDirectoryTemp;
				}
			}
			else if (EqualString(metroPackageInfo->m_SApplicationID.c_str(),sApplicationIDPhotoManager.c_str()))
			{
				sImageDirectoryTemp = metroPackageInfo->m_SInstallPackagePath + L"\\Assets\\PhotosIcon.targetsize-32.png";
				if (IsFileExistStat(sImageDirectoryTemp))
				{
					sImageDirectory = sImageDirectoryTemp;
				}
			}
		}
	}
	else
	{
		if (IS_WINDOW_8_1(g_lVersion))
		{
			if (EqualString(metroPackageInfo->m_SPackageIdName.c_str(),TEXT("Microsoft.SkypeApp")))
			{
				sImageDirectoryTemp = metroPackageInfo->m_SInstallPackagePath + L"\\images\\targetsize-32\\skypeSmall.png";
				if (IsFileExistStat(sImageDirectoryTemp))
				{
					sImageDirectory = sImageDirectoryTemp;
				}
			}
		}
		else if (IS_WINDOW_8(g_lVersion))
		{
			if (EqualString(metroPackageInfo->m_SPackageIdName.c_str(),TEXT("Microsoft.SkypeApp")))
			{
				sImageDirectoryTemp = metroPackageInfo->m_SInstallPackagePath + L"\\images\\scale-100\\skypeSmall.png";
				if (IsFileExistStat(sImageDirectoryTemp))
				{
					sImageDirectory = sImageDirectoryTemp;
				}
			}
			//microsoft.windowsphotos
			else if ( EqualString(metroPackageInfo->m_SPackageIdName.c_str(),TEXT("microsoft.windowsphotos")))
			{
				nImageFromType = DisplayImage_From_Targetsize;
				sImageDirectoryTemp = metroPackageInfo->m_SInstallPackagePath + L"\\en-us\\ModernPhoto\\Images\\smalllogo.targetsize-32.png";
				if (IsFileExistStat(sImageDirectoryTemp))
				{
					sImageDirectory = sImageDirectoryTemp;
				}
			}
		}

	}

	hBitmap = GetBitmapFromFile(sImageDirectory.c_str(),colorRefA,32,32);

	if (!hBitmap) 
		return FALSE;
	
	return TRUE;
}

BOOL CMetroPackageEngineer::GenerateDisplayName(const CMetroPackageInfo *metroPackageInfo, wstring &sDisplayName)
{
	if (metroPackageInfo->m_SInstallPackagePath.empty() 
		|| metroPackageInfo->m_SDisplayName.empty()
		|| metroPackageInfo->m_SPackageIdName.empty())
		return FALSE;

	const wstring msresource = L"ms-resource:";
	const wstring mSlash = L"//";
	const wstring sDisplayNamePre = metroPackageInfo->m_SDisplayName.substr(0,msresource.length());
	if (EqualString(sDisplayNamePre.c_str(),msresource.c_str()))
	{
		wstring szSourceName = metroPackageInfo->m_SDisplayName.substr(msresource.length(), metroPackageInfo->m_SDisplayName.length() - msresource.length());

		if ('/' == szSourceName[0])
		{
			int nFoundSlash = 1;
			while ('/' == szSourceName[nFoundSlash])
			{
				nFoundSlash++;
			}
			szSourceName = szSourceName.substr(nFoundSlash, metroPackageInfo->m_SDisplayName.length() - nFoundSlash);

			//fix SkyDrive windows 8.1 bug
			if (EqualString(szSourceName.c_str(),TEXT("FileManager/MarketBranding/SkyDriveTileDisplayName")))
			{
				szSourceName = L"MarketBranding/SkyDriveTileDisplayName";
			}
		}

		wstring szSource = L"@{" + metroPackageInfo->m_SInstallPackagePath + L"\\resources.pri? ms-resource://" + metroPackageInfo->m_SPackageIdName + L"/resources/" + szSourceName + L"}";

		wchar_t wDis[1024] = {0};

		BOOL bRet = /*SHLoadIndirectString*/ASSHLoadIndirectString(szSource.c_str(), wDis, 1024, NULL);

		if (bRet)
		{
			sDisplayName = wstring(wDis);
		}
		else
		{
			//wstring szSource2 = L"@{" + m_metroAppString.s_InstallPackagePath + L"\\resources.pri? ms-resource://" + m_metroAppString.s_PackageIdName + L"/resources/" + szSourceName + L"}";;
			szSource = L"@{" + metroPackageInfo->m_SInstallPackagePath + L"\\resources.pri? ms-resource://" + metroPackageInfo->m_SPackageIdName + L"/" + szSourceName + L"}";
			bRet = /*SHLoadIndirectString*/ASSHLoadIndirectString(szSource.c_str(), wDis, 1024, NULL);
			if (bRet)
			{
				sDisplayName = wstring(wDis);
			}
			else
			{
				return FALSE;
			}
		}
	}
	else
	{
		sDisplayName = metroPackageInfo->m_SDisplayName;
	}

	return TRUE;
}

BOOL CMetroPackageEngineer::ConvertMetroPackageToMetroApplication(const CMetroPackageInfo *metroPackageInfo, CMetroApplicationItem *metroApplicationItem)
{
	BOOL bRet(FALSE);
	wstring sAppUserModelId = L"";
	wstring sDisplayName = L"";
	HBITMAP hBitmap;
	COLORREF colorRef = 0L;
	GenerateBackgroundColorRef(metroPackageInfo,colorRef);
	
	if(GenerateDisplayIcon(metroPackageInfo,colorRef,hBitmap))
	{
		metroApplicationItem->SetBITMAP(hBitmap);
	}
	
	if (GenerateAppUserModelId(metroPackageInfo,sAppUserModelId)
		&& GenerateDisplayName(metroPackageInfo,sDisplayName) 
		)
	{
		metroApplicationItem->SetAppUserModelId(sAppUserModelId);
		metroApplicationItem->SetApplicationName(sDisplayName);
		bRet = TRUE;
	}
	else
	{
		return FALSE;
	}
		
	return bRet;
}

BOOL CMetroPackageEngineer::GetMetroInstallPackageListFromFromAppxManifestXml(const wstring sAppxManifestXmlA, 
																			  const wstring sPath, 
																			  const wstring sFullName, 
																			  const wstring sIdName)
{
	BOOL bRet(FALSE);

	//Get Logo from <Properties> </Properties>, since logo, small logo,Square30x30Logo of  metro package  aren't true.
	size_t size_LogoPre = WS_LogoPre_In_Properties.length();
	size_t size_LogoSuf = WS_LogoSuf_In_Properties.length();

	std::wstring::size_type nFoundLogoPre = sAppxManifestXmlA.find(WS_LogoPre_In_Properties);
	std::wstring::size_type nFoundLogoSuf = sAppxManifestXmlA.find(WS_LogoSuf_In_Properties);

	wstring sLogoProperties = L"";
	if (nFoundLogoPre != std::wstring::npos
		&& nFoundLogoSuf != std::wstring::npos)
	{
		sLogoProperties = sAppxManifestXmlA.substr(nFoundLogoPre+size_LogoPre, nFoundLogoSuf - nFoundLogoPre - size_LogoPre);
	}

	//Get Applications
	size_t size_ApplicationsPre = WS_Applications_Pre.length();
	size_t size_ApplicationsSuf = WS_Applications_Suf.length();

	std::wstring::size_type nFoundApplicationsPre = sAppxManifestXmlA.find(WS_Applications_Pre);
	std::wstring::size_type nFoundApplicationsSuf = sAppxManifestXmlA.find(WS_Applications_Suf);

	if (nFoundApplicationsPre != std::wstring::npos
		&& nFoundApplicationsSuf != std::wstring::npos)
	{
		wstring sApplications = sAppxManifestXmlA.substr(nFoundApplicationsPre, nFoundApplicationsSuf - nFoundApplicationsPre + size_ApplicationsSuf);

		std::wstring::size_type nFoundApplicationPre_In_Applications = sApplications.find(WS_ApplicationPre_In_Applications, size_ApplicationsPre);
		std::wstring::size_type nFoundApplicationSuf_In_Applications = sApplications.find(WS_ApplicationSuf_In_Applications);

		while (nFoundApplicationPre_In_Applications != std::wstring::npos
			&& nFoundApplicationSuf_In_Applications != std::wstring::npos
			&& nFoundApplicationSuf_In_Applications > nFoundApplicationPre_In_Applications)
		{
			wstring sApplication = sApplications.substr(nFoundApplicationPre_In_Applications, 
				nFoundApplicationSuf_In_Applications - nFoundApplicationPre_In_Applications + WS_ApplicationSuf_In_Applications.length());

			CMetroPackageInfo *metroPackageInfo = new CMetroPackageInfo;
			if (GetMetroPackageInfo(sApplication, metroPackageInfo))
			{
				metroPackageInfo->m_SInstallPackagePath = sPath;
				metroPackageInfo->m_SPackageFullName = sFullName;
				metroPackageInfo->m_SPackageIdName = sIdName;
				metroPackageInfo->m_SLogoProperties = sLogoProperties;
				CMetroApplicationItem *metrpApplicationItem = new CMetroApplicationItem;
				ConvertMetroPackageToMetroApplication(metroPackageInfo,metrpApplicationItem);
				m_MetroApplicationList.Add(metrpApplicationItem);
				bRet = TRUE;
			}

			if (metroPackageInfo)
			{
				delete metroPackageInfo;
				metroPackageInfo = NULL;
			}
			
			nFoundApplicationPre_In_Applications = sApplications.find(WS_ApplicationPre_In_Applications, nFoundApplicationSuf_In_Applications + 1);
			nFoundApplicationSuf_In_Applications = sApplications.find(WS_ApplicationSuf_In_Applications, nFoundApplicationSuf_In_Applications + 1);
		}
		return bRet;
	}
	//WinStore and immersive control panel are specially in Windows 8
	else
	{
		//WinSotre & PC Setting aren't include <applications>, need special handle.
		const wstring sPackageIdStore = L"WinStore";
		const wstring sPackageIdimmersivecontrolpanel = L"windows.immersivecontrolpanel";

		if (EqualString(sPackageIdStore.c_str(), sIdName.c_str()) 
		 || EqualString(sPackageIdimmersivecontrolpanel.c_str(), sIdName.c_str()) )
		{
			//<DisplayName> </DisplayName>
			const wstring sDisplayNameBegin = L"<DisplayName>";
			const wstring sDisplayNameEnd = L"</DisplayName>";

			size_t size_DisplayNameBegin = sDisplayNameBegin.length();
			size_t size_DisplayNameEnd = sDisplayNameEnd.length();

			std::wstring::size_type nFoundDisplayNameBegin = sAppxManifestXmlA.find(sDisplayNameBegin);
			std::wstring::size_type nFoundDisplayNameEnd = sAppxManifestXmlA.find(sDisplayNameEnd);

			wstring sDisplayName = L"";
			if (nFoundDisplayNameBegin != std::wstring::npos
				&& nFoundDisplayNameEnd != std::wstring::npos)
			{
				wstring sDisplayName = sAppxManifestXmlA.substr(nFoundDisplayNameBegin + size_DisplayNameBegin, 
					nFoundDisplayNameEnd - nFoundDisplayNameBegin - size_DisplayNameBegin);

				CMetroPackageInfo *metroPackageInfo = new CMetroPackageInfo;
				metroPackageInfo->m_SDisplayName = sDisplayName;
				metroPackageInfo->m_SInstallPackagePath = sPath;
				metroPackageInfo->m_SPackageFullName = sFullName;
				metroPackageInfo->m_SPackageIdName = sIdName;
				metroPackageInfo->m_SLogoProperties = sLogoProperties;

				if (IS_WINDOW_8(g_lVersion))
				{
					if (EqualString(sPackageIdStore.c_str(), sIdName.c_str()))
					{
						metroPackageInfo->m_SApplicationID = L"Windows.Store";
					}
					else if (EqualString(sPackageIdimmersivecontrolpanel.c_str(), sIdName.c_str()))
					{
						metroPackageInfo->m_SApplicationID = L"microsoft.windows.immersivecontrolpanel";
					}
				}

				CMetroApplicationItem *metrpApplicationItem = new CMetroApplicationItem;
				ConvertMetroPackageToMetroApplication(metroPackageInfo,metrpApplicationItem);
				m_MetroApplicationList.Add(metrpApplicationItem);

				if (metroPackageInfo)
				{
					delete metroPackageInfo;
					metroPackageInfo = NULL;
				}
				
				bRet = TRUE;
			}
		}
		return bRet;
	}

	return bRet;
}

BOOL CMetroPackageEngineer::GetApplicationItemBase(wstring &sOut, const wstring sContentSrc, const wstring sFoundContentSrc)
{
	std::wstring::size_type nFound = sContentSrc.find(sFoundContentSrc);
	std::wstring::size_type nDoubleQuoteMark = -1;
	if (nFound != std::wstring::npos)
	{
		nDoubleQuoteMark = sContentSrc.find(WS_DoubleQuoteMark, nFound + sFoundContentSrc.length() + 1);
		if (nDoubleQuoteMark != std::string::npos)
		{
			int length = sFoundContentSrc.length();
			sOut = sContentSrc.substr(nFound + length + 1, nDoubleQuoteMark - nFound - length - 1);
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CMetroPackageEngineer::GetMetroPackageInfo(const wstring sApplication, CMetroPackageInfo *metroPackageInfo)
{
	wstring sApplicationID = L"";
	if (GetApplicationItemBase(sApplicationID,sApplication,WS_Id_In_Application))
	{
		metroPackageInfo->m_SApplicationID = sApplicationID;
	}

	wstring sBackgroundColor = L"";
	if (GetApplicationItemBase(sBackgroundColor,sApplication,WS_BackgroundColor_In_Application))
	{
		metroPackageInfo->m_SBackgroundColor = sBackgroundColor;
	}

	wstring sApplicationnSmallLogo = L"";
	if (GetApplicationItemBase(sApplicationnSmallLogo,sApplication,WS_SmallLogo_In_Application))
	{
		metroPackageInfo->m_SSmallLogo = sApplicationnSmallLogo;
	}

	wstring sApplicationnLogoApplication = L"";
	if (GetApplicationItemBase(sApplicationnLogoApplication,sApplication,WS_Logo_In_Application))
	{
		metroPackageInfo->m_SLogoApplication = sApplicationnLogoApplication;
	}

	wstring sApplicationnSquare30x30Logoo = L"";
	if (GetApplicationItemBase(sApplicationnSquare30x30Logoo,sApplication,WS_Square30x30Logo_In_Application))
	{
		metroPackageInfo->m_SSquare30x30Logo = sApplicationnSquare30x30Logoo;
	}

	wstring sDisplayName = L"";
	if (GetApplicationItemBase(sDisplayName,sApplication,WS_DisplayName_In_Application))
	{
		metroPackageInfo->m_SDisplayName = sDisplayName;
	}

	return TRUE;
}
