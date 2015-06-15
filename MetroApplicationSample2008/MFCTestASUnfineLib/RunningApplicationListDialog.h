#pragma once
#include "afxwin.h"
#include "ListBoxCH.h"
#include "ApplicationItem.h"
#include "RunningApplicationEnumerater.h"

// CRunningApplicationListDialog dialog

class CRunningApplicationListDialog : public CDialog
{
	DECLARE_DYNAMIC(CRunningApplicationListDialog)

public:
	CRunningApplicationListDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CRunningApplicationListDialog();

// Dialog Data
	enum { IDD = IDD_RUNNING_APPLICATION_DIALOG };

	void DrawDisplayIcon(HBITMAP hBitmap);
	BOOL ClearRunningApplicationList();

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnDblclkListRunningApplication();
	CListBoxCH m_RunningApplicationListBox;

	CDvArray m_RunningApplicationList;
};
