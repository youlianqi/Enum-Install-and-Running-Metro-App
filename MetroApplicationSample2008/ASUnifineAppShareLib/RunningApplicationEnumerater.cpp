#include "StdAfx.h"
#include "RunningApplicationEnumerater.h"

CRunningApplicationEnumerater CRunningApplicationEnumerater::g_RunningApplicationEnumeraterInstance;

CRunningApplicationEnumerater& CRunningApplicationEnumerater::GetInstance()
{
	return g_RunningApplicationEnumeraterInstance;
}

CRunningApplicationEnumerater::CRunningApplicationEnumerater(void)
{
	m_bDesktopEnuming = FALSE;
}

CRunningApplicationEnumerater::~CRunningApplicationEnumerater(void)
{
	ClearRunningApplicationList();
}

BOOL CRunningApplicationEnumerater::ClearRunningApplicationList()
{
	for (int i=0; i<m_RunningApplicationList.GetCount(); i++)
	{
		CRunningApplicationItem * pMetroApplcaitionItem =(CRunningApplicationItem *)m_RunningApplicationList[i];
		if (pMetroApplcaitionItem)
		{
			delete pMetroApplcaitionItem;
			pMetroApplcaitionItem = NULL;
		}	
	}
	m_RunningApplicationList.RemoveAll();
	return TRUE;
}

BOOL CRunningApplicationEnumerater::IsSkipCtrlWin(HWND hwnd)
{
	if (::IsSkipCtrlWin(hwnd))
		return TRUE;

	return FALSE;
}

BOOL CRunningApplicationEnumerater::GetDvASWindowItemFromHWnd(CRunningApplicationItem *pRunningApplicationItem, HWND hWnd, UINT nType, DWORD Expid, DWORD Currentpid)
{
	TCHAR szClassName[MAX_CLASS_NAME+1];
	UINT uLength = ::GetClassName(hWnd, szClassName, MAX_CLASS_NAME+1);

	TCHAR szWindowText[1024];
	GetWindowText(hWnd, szWindowText, 1024);
	INT nWindowTextLen = wcsnlen_s((LPTSTR)szWindowText, _MAX_FNAME);

	if (!m_bDesktopEnuming)
	{
		if(	Expid == Currentpid
			|| EqualString(_T("Shell_Dialog"), szClassName)
			|| EqualString(_T("Shell_Dim"), szClassName)
			|| EqualString(_T("IPTip_Main_Window"), szClassName)
			|| EqualString(_T("PredictionWnd"), szClassName)
			|| EqualString(_T("ApplicationHostBridgeWindow"), szClassName)
			|| EqualString(_T("Shell_HostingContainer"), szClassName)
			|| EqualString(_T("HostWindow"), szClassName)
			)
		{
			return FALSE;
		}

		DWORD dwClassStyle = GetClassLong(hWnd, GCL_STYLE);
		if(dwClassStyle&CS_IME)
			return FALSE;
	}

	pRunningApplicationItem->SetBMetro(!m_bDesktopEnuming);

	pRunningApplicationItem->SetClassName(szClassName);

	pRunningApplicationItem->SetHWND(hWnd);

	if (nWindowTextLen>0)
	{
		pRunningApplicationItem->SetWindowText(szWindowText);
	}	

	HICON hIcon=NULL, hSmallIcon=NULL;
	TCHAR szExePath[_MAX_PATH+1];

	if (GetImageFileFromWindow(hWnd,szExePath, _MAX_PATH+1))
	{

		if (!m_bDesktopEnuming)
		{
			wstring sPath = L"";
			wstring sPackageFullName = L"";
			wstring sPackageIdName = L"";
			wstring sApplicationIdName = L"";
			GetMetroPackageInfoFromHWnd(hWnd,sPath,sPackageFullName,sPackageIdName,sApplicationIdName);
			pRunningApplicationItem->SetInstallPath(sPath);
		}
		else
		{
			pRunningApplicationItem->SetInstallPath(szExePath);
		}
		
		

		

		//HBITMAP hBitmap = NULL;
		//GetWindowDisplayIcon(hWnd,szExePath,!m_bDesktopEnuming,hBitmap);
		
		HICON hIcon =NULL;
		hIcon = GetWindowIcon(hWnd);
		if (hIcon)
		{
			pRunningApplicationItem->SetIcon(hIcon);
		}
	}

	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];

	_wsplitpath_s(szExePath, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
	////size_t lenExt = wcsnlen_s(ext,_MAX_EXT);

	//TCHAR szAppName[_MAX_FNAME];
	//_tsplitpath_s(szExePath, NULL, NULL, szAppName, NULL );
	//_wsplitpath_s(szExePath, NULL, _MAX_DRIVE, NULL, _MAX_DIR, szAppName, _MAX_FNAME, NULL, _MAX_EXT);

	INT nExeNameLen = wcsnlen_s((LPTSTR)fname, _MAX_FNAME);
	if (nExeNameLen>0)
	{
		pRunningApplicationItem->SetExeName(fname);
	}

	return TRUE;
}

int GetWindowItemIndex(CDvArray dvArray,CRunningApplicationItem * pRunningApplicationItemA) 
{
	int		i;

	for(i=dvArray.GetCount()-1; i>=0; i--)
	{
		CRunningApplicationItem * pRunningApplicationItem = (CRunningApplicationItem *)dvArray[i];

		if( pRunningApplicationItem->GetHWND() == pRunningApplicationItemA->GetHWND() &&
			pRunningApplicationItem->GetProcessId() == pRunningApplicationItemA->GetProcessId() )
		{
			break;
		}
	}

	return	i;
}

BOOL CRunningApplicationEnumerater::GetRunningApplicationList(CDvArray &runningApplicationList)
{
	int				i;
	int				nCount;
	BOOL			bRet;
	HWND			hWnd;

	DWORD Expid;
	HWND hWndCabine = ::FindWindow(NULL,_T("Program Manager"));
	GetWindowThreadProcessId(hWndCabine, &Expid);

	m_bDesktopEnuming = FALSE;
	hWnd = FindWindowEx(NULL,NULL,NULL,NULL);

	while( hWnd )
	{
		if (!IsWindow(hWnd))
			break;

		RECT rc;

		HWND hOwnerWnd=::GetWindow(hWnd, GW_OWNER);
		GetWindowRect(hOwnerWnd, &rc);

		TCHAR szClassName[MAX_CLASS_NAME+1];
		UINT uLength = ::GetClassName(hWnd, szClassName, MAX_CLASS_NAME+1);

		if (EqualString(szClassName,_T("ApplicationManager_ImmersiveShellWindow"))
			||EqualString(szClassName,_T("ImmersiveBackgroundWindow")))
		{
			m_bDesktopEnuming = TRUE;
			hWnd = FindWindowEx(NULL,hWnd,NULL,NULL);
			continue;
		}

		if( !hOwnerWnd || !::IsWindowVisible(hOwnerWnd) || IsRectEmpty(&rc))
		{
			if( ::IsWindowVisible(hWnd) )
			{
				BOOL bGo =FALSE;
				if( ::GetWindowTextLength(hWnd) ==0 )
				{
					GetWindowRect(hWnd, &rc);

					if( rc.right-rc.left>0 && rc.bottom-rc.top>0 )
					{
						if(EqualString(szClassName,_T("Shell_TrayWnd")))
						{
							bGo =TRUE;
							hWnd = FindWindowEx(NULL,hWnd,NULL,NULL);
							continue;
						}
							
					}
				}

				if( ::GetWindowTextLength(hWnd) !=0 || bGo )
				{
					if( !IsSkipCtrlWin(hWnd) )
					{
						if ((DWORD)AsGetProp(hWnd, _T("ActiveTouchMeeting"))!=0x53415441)
						{
							CRunningApplicationItem * pRunningApplicationItem = new CRunningApplicationItem;
							_ASSERTE(pRunningApplicationItem);

							if (pRunningApplicationItem)
							{
								DWORD pid;
								GetWindowThreadProcessId(hWnd, &pid);
								pRunningApplicationItem->SetProcessId(pid);
								if (GetDvASWindowItemFromHWnd(pRunningApplicationItem,hWnd,0,Expid,pid))
								{
									runningApplicationList.Add(pRunningApplicationItem);
								}
							}

							
						}
					}
				}

			}
		}
		else//has owner, but need be included
		{
		}

		//goto next hwnd
		hWnd = FindWindowEx(NULL,hWnd,NULL,NULL);
	}

	//CRunningApplicationItem * pRunningApplicationItem;
	//if( m_RunningApplicationList.GetCount() == runningApplicationList.GetCount() )
	//{
	//	nCount = m_RunningApplicationList.GetCount();
	//	for(i=0; i<nCount && !bRet; i++)
	//	{
	//		pRunningApplicationItem = (CRunningApplicationItem *)m_RunningApplicationList[i];

	//		if( GetWindowItemIndex(m_RunningApplicationList,pRunningApplicationItem) < 0)
	//			bRet = TRUE;
	//	}
	//}
	//else
	//	bRet = TRUE;

	//m_RunningApplicationList = runningApplicationList;

	return TRUE;
}

