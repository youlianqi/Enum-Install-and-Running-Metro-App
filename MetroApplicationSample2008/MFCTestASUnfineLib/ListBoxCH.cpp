// ListBoxCH.cpp : implementation file
//

#include "stdafx.h"
#include "MFCTestASUnfineLib.h"
#include "ListBoxCH.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListBoxCH

CListBoxCH::CListBoxCH()
{
	m_clrSel  = RGB(51,153,255);//RGB(0,0,255);		// default selected color (blue)
	m_clrBg   = RGB(255,255,255);	// default background color (white)
	m_clrText = RGB(255,255,255);	// default text color (white)
	m_bEdge   = FALSE;//TRUE;				// default edge is enabled
}

CListBoxCH::~CListBoxCH()
{
	// release all list item.
	while( m_pItemList.GetCount() )
	{
		CExtItem* pItem = m_pItemList.GetHead();
		delete pItem;
		m_pItemList.RemoveHead();
	}
}


BEGIN_MESSAGE_MAP(CListBoxCH, CListBox)
	//{{AFX_MSG_MAP(CListBoxCH)
	ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxCH message handlers
void CListBoxCH::AddItem(LPCWSTR lpszItemName,HICON hIcon)
{
	CExtItem* pItem = new CExtItem(lpszItemName,hIcon);
	m_pItemList.AddTail(pItem);

	int nIndex = AddString(L"");
	SetItemDataPtr(nIndex,pItem);
}

void CListBoxCH::AddItem2(LPCWSTR lpszItemName,HBITMAP hBitmap)
{
	CExtItem* pItem = new CExtItem(lpszItemName,hBitmap);
	m_pItemList.AddTail(pItem);

	int nIndex = AddString(L"");
	SetItemDataPtr(nIndex,pItem);
}

void CListBoxCH::SetSelColor(COLORREF clr)
{
	m_clrSel = clr;
}
void CListBoxCH::SetBgColor(COLORREF clr)
{
	m_clrBg = clr;
}
void CListBoxCH::SetTextColor(COLORREF clr)
{
	m_clrText = clr;
}
void CListBoxCH::EnableEdge(BOOL bEnable)
{
	m_bEdge = bEnable;
}
void CListBoxCH::SetCurSel(int curSel)
{
	m_curSel = curSel;
	CListBox::SetCurSel(curSel);
	OnSelchange();
}
int CListBoxCH::GetCurSel()
{	
	return m_curSel;
}
void CListBoxCH::MeasureItem(LPMEASUREITEMSTRUCT lpMIS) 
{
	// TODO: Add your code to determine the size of specified item
	lpMIS->itemWidth   = 100;
	lpMIS->itemHeight  = g_lHeightList;
}

void DrawBitmap(HDC hdc, HBITMAP hBitmap,  CRect rcDes, RECT rcSrc)
{
	HDC dcDraw = ::CreateCompatibleDC(hdc);
	HBITMAP hBmpOld = (HBITMAP)::SelectObject(dcDraw, hBitmap);

	//BITMAP bm;
	//hBitmap.GetBitmap( &bm );


	BITMAP bitmap = { 0 };
	GetObject(hBitmap, sizeof(BITMAP), &bitmap);
	LONG bmWidth = bitmap.bmWidth;
	LONG bmHeight = bitmap.bmHeight;

	//::BitBlt(
	//	hdc, ptDest.x, ptDest.y, rcSrc.right - rcSrc.left, rcSrc.bottom - rcSrc.top, 
	//	dcDraw, rcSrc.left, rcSrc.top, SRCCOPY
	//	);

	int space = ((rcDes.bottom-rcDes.top)-(rcSrc.bottom-rcSrc.top))/2;
	::StretchBlt(hdc, rcDes.left+2, rcDes.top+space, rcSrc.right-rcSrc.left, rcSrc.bottom-rcSrc.top,
		dcDraw, 0, 0, bmWidth, bmHeight, SRCCOPY);

	::SelectObject(dcDraw, hBmpOld);
	::DeleteDC(dcDraw);
}

void CListBoxCH::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	// TODO: Add your code to draw the specified item
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	CRect rectFull(lpDIS->rcItem);
	// icon and text rect

	CRect rectIcon(rectFull.left,rectFull.top,rectFull.left+g_lHeightList,rectFull.bottom); 
	CRect rectText(rectIcon.right,rectFull.top,rectFull.right,rectFull.bottom);	
	
	
	CExtItem* pItem = static_cast<CExtItem *>(GetItemDataPtr(lpDIS->itemID));
	pDC->SetBkMode(TRANSPARENT);

	if ( (lpDIS->itemAction & ODA_DRAWENTIRE) ||
	     ((!(lpDIS->itemState & ODS_SELECTED) && (lpDIS->itemAction & ODA_SELECT)))
	   )
	{
		// item has been selected or draw all item 
		// fill background for setted background color 
		CRect rect(rectFull);

		pDC->FillSolidRect(rect,m_clrBg);
		if( m_bEdge )
			pDC->DrawEdge(rect,EDGE_SUNKEN,BF_BOTTOM);

		if (pItem->m_bIcon)
		{
			//pDC->DrawIcon(rectIcon.left+2,rectIcon.top+4,pItem->m_hIcon);
			//DrawIconEx(pDC->m_hDC,
			//	0,
			//	0,
			//	pItem->m_hIcon,
			//	nWidthIcon,
			//	nHeightIcon,
			//	rectIcon.left+2,
			//	rectIcon.top+10,
			//	DI_NORMAL);
			//::DrawIconEx(pDC->m_hDC,0,0, pItem->m_hIcon, nWidthIcon,nHeightIcon,0,0,DI_NORMAL);
			DrawIconEx (pDC->m_hDC, rectIcon.left+2,rectIcon.top+(g_lHeightList-g_lHeightIcon)/2, pItem->m_hIcon, g_lHeightIcon, g_lHeightIcon, 0, NULL,
				DI_NORMAL ); 
		}
		else
		{
			RECT rc = {0,0,g_lHeightIcon,g_lHeightIcon};
			DrawBitmap(pDC->m_hDC,pItem->m_hBitmap,rectIcon,rc);
		}
		//pDC->DrawIcon(rectIcon.left+2,rectIcon.top+4,pItem->m_hIcon);
		

		/*HDC hBufDC = CreateCompatibleDC(pDC->m_hDC);
		HBITMAP hBitmapTemp;
		hBitmapTemp = CreateCompatibleBitmap(pDC->m_hDC, 32, 32);
		HBITMAP hBmpOld = (HBITMAP)SelectObject(pDC->m_hDC, hBitmapTemp);
		hBitmapTemp = (HBITMAP)SelectObject(hBufDC, pItem->m_hBitmap);
		StretchBlt(pDC->m_hDC, rectIcon.left+2, rectIcon.top+4, 32, 32,
			hBufDC, 0, 0, 32, 32, SRCCOPY);
		SelectObject(pDC->m_hDC, hBmpOld);
		DeleteObject(hBmpOld);
		DeleteObject(hBitmapTemp);
		::DeleteDC(hBufDC);*/

		pDC->SetTextColor(RGB(0,0,0));			// unselected text color

		rect.CopyRect(rectText);
		rect.DeflateRect(2,0);
		rect.OffsetRect(2,0);	
		if( pItem->m_szItemName != NULL )
			pDC->DrawText(pItem->m_szItemName,lstrlen(pItem->m_szItemName),
						  rect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}

	
	if ((lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{	
		// item has been selected - draw selected frame
		CRect rect(rectFull);

		CPen  Pen(PS_SOLID,1,RGB(0,0,0));				// draw dotted frame 
		CPen* pOldPen = pDC->SelectObject(&Pen);
		pDC->Rectangle(rect);
		pDC->SelectObject(pOldPen);
				
		rect.DeflateRect(1,1,1,1);
		pDC->FillRect(rect,&CBrush(m_clrSel));	// fill item frame
			
		if( m_bEdge )
			pDC->DrawEdge(rect,EDGE_SUNKEN,BF_BOTTOM);

		if (pItem->m_bIcon)
		{
			//pDC->DrawIcon(rectIcon.left+2,rectIcon.top+4,pItem->m_hIcon);
			/*::DrawIconEx(pDC->m_hDC,
				0,
				0,
				pItem->m_hIcon,
				nWidthIcon,
				nHeightIcon,
				rectIcon.left+2,
				rectIcon.top+10,
				DI_NORMAL);*/
			DrawIconEx (pDC->m_hDC, rectIcon.left+2,rectIcon.top+(g_lHeightList-g_lHeightIcon)/2, pItem->m_hIcon, g_lHeightIcon, g_lHeightIcon, 0, NULL,
				DI_NORMAL ); 
		}
		else
		{
			RECT rc = {0,0,g_lHeightIcon,g_lHeightIcon};
			DrawBitmap(pDC->m_hDC,pItem->m_hBitmap,rectIcon,rc);
		}
	/*	HDC hBufDC = CreateCompatibleDC(pDC->m_hDC);
		HBITMAP hBitmapTemp;
		hBitmapTemp = CreateCompatibleBitmap(pDC->m_hDC, 32, 32);
		HBITMAP hBmpOld = (HBITMAP)SelectObject(pDC->m_hDC, hBitmapTemp);
		hBitmapTemp = (HBITMAP)SelectObject(hBufDC, pItem->m_hBitmap);
		StretchBlt(pDC->m_hDC, rectIcon.left+2, rectIcon.top+4, 32, 32,
			hBufDC, 0, 0, 32, 32, SRCCOPY);
		SelectObject(pDC->m_hDC, hBmpOld);
		DeleteObject(hBmpOld);
		DeleteObject(hBitmapTemp);
		::DeleteDC(hBufDC);*/

		pDC->SetTextColor(m_clrText);			// selected text color
		rect.CopyRect(rectText);
		rect.DeflateRect(2,0);
		rect.OffsetRect(2,0);					// draw text
		if( pItem->m_szItemName != NULL )
			pDC->DrawText(pItem->m_szItemName,lstrlen(pItem->m_szItemName),
						  rect,DT_LEFT | DT_SINGLELINE | DT_VCENTER);
	}
}
void CListBoxCH::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	m_curSel = -1;
}
void CListBoxCH::OnSelchange() 
{
	static int oldSel = m_curSel;

	if( oldSel != m_curSel )
	{	// refresh two rectangle,one is old item and another is new.
		CRect rect;					
		GetItemRect(oldSel,rect);
		InvalidateRect(rect,TRUE);

		GetItemRect(m_curSel,rect);
		InvalidateRect(rect,TRUE);
		oldSel = m_curSel;
	}
}
void CListBoxCH::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	// get selected index from mouse position
	BOOL data;
	m_curSel = ItemFromPoint(point,data);	
	OnSelchange();
	CListBox::OnLButtonUp(nFlags, point);
}
int CListBoxCH::VKeyToItem(UINT nKey, UINT nIndex) 
{
	// TODO: Add your code to handle a particular virtual key
	// return -1 = default action
	// return -2 = no further action
	// return index = perform default action for keystroke on
	//                item specified by index
	if ((nKey == VK_UP) && (nIndex > 0))
		SetCurSel(nIndex);
	
   // On key down, move the caret down one item.
   else if ((nKey == VK_DOWN) && (nIndex < (UINT)GetCount()))
		SetCurSel(nIndex);
	return -1;
}

