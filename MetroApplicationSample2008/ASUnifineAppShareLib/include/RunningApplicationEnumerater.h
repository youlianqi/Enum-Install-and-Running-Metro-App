#pragma once

#include <windows.h>
#include "ApplicationItem.h"
#include "uautil.h"

class CRunningApplicationEnumerater
{
public:
	CRunningApplicationEnumerater(void);
	~CRunningApplicationEnumerater(void);

	static CRunningApplicationEnumerater& GetInstance();

	

	BOOL GetDvASWindowItemFromHWnd(CRunningApplicationItem *pRunningApplicationItem, HWND hWnd, UINT nType, DWORD Expid, DWORD Currentpid);
	BOOL IsSkipCtrlWin(HWND hwnd);
	BOOL GetRunningApplicationList(CDvArray &runningApplicationList);

	CDvArray	m_RunningApplicationList;

private:
	static CRunningApplicationEnumerater g_RunningApplicationEnumeraterInstance;

	BOOL ClearRunningApplicationList();

	BOOL m_bDesktopEnuming;
};
