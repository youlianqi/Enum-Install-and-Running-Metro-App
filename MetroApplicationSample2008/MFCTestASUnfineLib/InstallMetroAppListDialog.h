#pragma once
#include "afxwin.h"
#include "ListBoxCH.h"


// CInstallMetroAppListDialog dialog

class CInstallMetroAppListDialog : public CDialog
{
	DECLARE_DYNAMIC(CInstallMetroAppListDialog)

public:
	CInstallMetroAppListDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CInstallMetroAppListDialog();

// Dialog Data
	enum { IDD = IDD_INSTALL_METRO_DIALOG };

	void DrawDisplayIcon(HBITMAP hBitmap);

protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnLbnDblclkListInstallmetro();
//	afx_msg void OnLbnSelchangeListInstallmetro();
	//CListBox m_InstallMetroListBox;
	CListBoxCH m_InstallMetroListBox;
	afx_msg void OnLbnDblclkListInstallmetro();
	afx_msg void OnLbnSelchangeListInstallmetro();
};
