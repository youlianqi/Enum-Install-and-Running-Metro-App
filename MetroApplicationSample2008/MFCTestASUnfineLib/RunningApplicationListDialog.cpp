// RunningApplicationListDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MFCTestASUnfineLib.h"
#include "RunningApplicationListDialog.h"
#include <string>
#include <tchar.h>
#include <sstream>

using namespace std;

template <typename T>
std::string to_string(T value)
{
	//stringstream ss;
	//ss << value;
	//return wstring((wchar_t *)ss.str().c_str());

	std::ostringstream os ;
	os << value ;
	return os.str() ;
}

// CRunningApplicationListDialog dialog

IMPLEMENT_DYNAMIC(CRunningApplicationListDialog, CDialog)

CRunningApplicationListDialog::CRunningApplicationListDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CRunningApplicationListDialog::IDD, pParent)
{

}

CRunningApplicationListDialog::~CRunningApplicationListDialog()
{
	ClearRunningApplicationList();
}

void CRunningApplicationListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_RUNNING_APPLICATION, m_RunningApplicationListBox);
}
void CRunningApplicationListDialog::DrawDisplayIcon(HBITMAP hBitmapA)
{

	if (!hBitmapA) 
	{
		InvalidateRect(NULL,TRUE);
		return;
	}

	CDC *pDC = GetDC();

	HDC hBufDC = CreateCompatibleDC(pDC->m_hDC);

	HBITMAP hBitmapTemp;

	hBitmapTemp = CreateCompatibleBitmap(pDC->m_hDC, 32, 32);

	HBITMAP hBmpOld = (HBITMAP)SelectObject(pDC->m_hDC, hBitmapTemp);

	hBitmapTemp = (HBITMAP)SelectObject(hBufDC, hBitmapA);

	// BitBlt(hDC, 0, 0, BitWidth, BitHeight, hTempDC, 0, 0, SRCCOPY);

	StretchBlt(pDC->m_hDC, 0, 0, 32, 32,
		hBufDC, 0, 0, 32, 32, SRCCOPY);
	//	StretchBlt(hDC, 0, 0, BitWidth, BitHeight,
	//		hTempDC, 0, 0, BitWidth, BitHeight, SRCCOPY);
	//	hBitmap = (HBITMAP)SelectObject(hBufDC, hBitmapTemp)

	SelectObject(pDC->m_hDC, hBmpOld);
	DeleteObject(hBitmapTemp);
	::DeleteDC(hBufDC);
	::DeleteDC(pDC->m_hDC);

}

BEGIN_MESSAGE_MAP(CRunningApplicationListDialog, CDialog)
	ON_LBN_DBLCLK(IDC_LIST_RUNNING_APPLICATION, &CRunningApplicationListDialog::OnLbnDblclkListRunningApplication)
END_MESSAGE_MAP()


// CRunningApplicationListDialog message handlers

BOOL CRunningApplicationListDialog::ClearRunningApplicationList()
{
	for (int i=0; i<m_RunningApplicationList.GetCount(); i++)
	{
		CRunningApplicationItem * pRunningApplicationItem =(CRunningApplicationItem *)m_RunningApplicationList[i];
		if (pRunningApplicationItem)
		{
			delete pRunningApplicationItem;
			pRunningApplicationItem = NULL;
		}	
	}
	m_RunningApplicationList.RemoveAll();
	return TRUE;
}

BOOL CRunningApplicationListDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i;
	BOOL bGetInstallMetroList = CRunningApplicationEnumerater::GetInstance().GetRunningApplicationList(m_RunningApplicationList);
	if (bGetInstallMetroList)
	{
		//CDvArray runningApplicationList = CRunningApplicationEnumerater::GetInstance().m_RunningApplicationList;
		for ( i = 0; i < m_RunningApplicationList.GetCount(); ++i)
		{
			CRunningApplicationItem * pRunningApplicationItem =(CRunningApplicationItem *)m_RunningApplicationList[i];
			//m_RunningApplicationListBox.AddItem(metroItem->GetApplicationName().c_str(),hIcon);
			std::string sProcessID =  to_string(pRunningApplicationItem->GetProcessId()) ;//std::to_wstring(pRunningApplicationItem->GetProcessId());

			std::wstring wsProcessID = wstring(sProcessID.begin(),sProcessID.end());
			//HBITMAP hBitmap = ::LoadBitmap(AfxGetInstanceHandle(),_T("C:\\Program Files\\WindowsApps\\microsoft.windowscommunicationsapps_17.0.1119.516_x64__8wekyb3d8bbwe\\ModernChat\\Messaging.targetsize-32.png"));
			wstring sDisplayTitle = pRunningApplicationItem->GetWindowText() + L" id:" + wsProcessID + L"classname:" +  pRunningApplicationItem->GetClassName();

			if (pRunningApplicationItem->GetBMetro())
			{
				m_RunningApplicationListBox.AddItem2(sDisplayTitle.c_str(),pRunningApplicationItem->GetBITMAP());
			}
			else
			{
				m_RunningApplicationListBox.AddItem(sDisplayTitle.c_str(),pRunningApplicationItem->GetIcon());
			}
			
		}

	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void CRunningApplicationListDialog::OnLbnDblclkListRunningApplication()
{
	// TODO: Add your control notification handler code here
}
