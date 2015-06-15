// InstallMetroAppListDialog.cpp : implementation file
//

#include "stdafx.h"
#include "MFCTestASUnfineLib.h"
#include "InstallMetroAppListDialog.h"
#include "MetroPackageEngineer.h"
#include "ApplicationItem.h"


// CInstallMetroAppListDialog dialog

IMPLEMENT_DYNAMIC(CInstallMetroAppListDialog, CDialog)

CInstallMetroAppListDialog::CInstallMetroAppListDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInstallMetroAppListDialog::IDD, pParent)
{

}

CInstallMetroAppListDialog::~CInstallMetroAppListDialog()
{
}

HICON CreateIcon(HBITMAP hBitmap)
{
	Gdiplus::Bitmap* pTmpBitmap=Gdiplus::Bitmap::FromHBITMAP(hBitmap,NULL);
	HICON hIcon=NULL;
	pTmpBitmap->GetHICON(&hIcon);
	delete pTmpBitmap;
	return hIcon;
}

HICON HICONFromCBitmap( CBitmap& bitmap)
{
	BITMAP bmp;
	bitmap.GetBitmap(&bmp);

	HBITMAP hbmMask = ::CreateCompatibleBitmap(::GetDC(NULL), 
		bmp.bmWidth, bmp.bmHeight);

	ICONINFO ii = {0};
	ii.fIcon = TRUE;
	ii.hbmColor = bitmap;
	ii.hbmMask = hbmMask;

	HICON hIcon = ::CreateIconIndirect(&ii);
	::DeleteObject(hbmMask);

	return hIcon;
}

HICON CreateAppIcon( HBITMAP bmp)
{
	// create a memory device context
	HDC memdc = ::CreateCompatibleDC( NULL );

	// create compatible bitmaps
	HBITMAP mask = ::CreateCompatibleBitmap( memdc, 32, 32 );
//	HBITMAP bmp = ::LoadBitmap( hmod, MAKEINTRESOURCE(PIBMP) );
//	HBITMAP mask = CreateBitmap(32, 32, 1, 1, NULL);

	// create the icon
	ICONINFO ii;
	ii.fIcon = TRUE;
	ii.hbmMask = mask;
	ii.hbmColor = bmp;
	HICON icon = ::CreateIconIndirect( &ii );

	// free resources
	::DeleteDC( memdc );
	::DeleteObject( mask );
	//::DeleteObject( bmp );

	// return the resulting icon
	return icon;
}

BOOL CInstallMetroAppListDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	BOOL bGetInstallMetroList = CMetroPackageEngineer::GetInstance().GetInstallMetroApplicationList();
	if (bGetInstallMetroList)
	{
		CDvArray metroAppStringList = CMetroPackageEngineer::GetInstance().m_MetroApplicationList;
		for (int i = 0; i < metroAppStringList.GetCount(); ++i)
		{
			CMetroApplicationItem * metroItem =(CMetroApplicationItem *)metroAppStringList[i];

			/*HICON hIcon = CreateAppIcon(metroItem->GetBITMAP());
			m_InstallMetroListBox.AddItem(metroItem->GetApplicationName().c_str(),hIcon);
			DeleteObject(hIcon);*/
			m_InstallMetroListBox.AddItem2(metroItem->GetApplicationName().c_str(),metroItem->GetBITMAP());
		}

	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInstallMetroAppListDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INSTALLMETRO, m_InstallMetroListBox);
}


BEGIN_MESSAGE_MAP(CInstallMetroAppListDialog, CDialog)
//	ON_LBN_DBLCLK(IDC_LIST_INSTALLMETRO, &CInstallMetroAppListDialog::OnLbnDblclkListInstallmetro)
//	ON_LBN_SELCHANGE(IDC_LIST_INSTALLMETRO, &CInstallMetroAppListDialog::OnLbnSelchangeListInstallmetro)
ON_LBN_DBLCLK(IDC_LIST_INSTALLMETRO, &CInstallMetroAppListDialog::OnLbnDblclkListInstallmetro)
ON_LBN_SELCHANGE(IDC_LIST_INSTALLMETRO, &CInstallMetroAppListDialog::OnLbnSelchangeListInstallmetro)
END_MESSAGE_MAP()

void CInstallMetroAppListDialog::DrawDisplayIcon(HBITMAP hBitmapA)
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


void CInstallMetroAppListDialog::OnLbnDblclkListInstallmetro()
{
	// TODO: Add your control notification handler code here
	int row = m_InstallMetroListBox.GetCurSel();
	if (row < 0)
		return;

	CDvArray metroAppStringList = CMetroPackageEngineer::GetInstance().m_MetroApplicationList;
	if (row < metroAppStringList.GetCount())
	{
		CMetroApplicationItem * metroItem =(CMetroApplicationItem *)metroAppStringList[row];
		CMetroPackageEngineer::GetInstance().ActiveApplication(metroItem->GetAppUserModelId());
	}
}

void CInstallMetroAppListDialog::OnLbnSelchangeListInstallmetro()
{
	return ;

	int row = m_InstallMetroListBox.GetCurSel();
	if (row < 0)
		return;

	CDvArray metroAppStringList = CMetroPackageEngineer::GetInstance().m_MetroApplicationList;
	if (row < metroAppStringList.GetCount())
	{
		CMetroApplicationItem * metroItem =(CMetroApplicationItem *)metroAppStringList[row];
		DrawDisplayIcon(metroItem->GetBITMAP());
	}
}
