
// MFCTestASUnfineLibDlg.h : header file
//

#pragma once
#include "afxwin.h"

class CRunningApplicationListDialog;
class CInstallMetroAppListDialog;

// CMFCTestASUnfineLibDlg dialog
class CMFCTestASUnfineLibDlg : public CDialog
{
// Construction
public:
	CMFCTestASUnfineLibDlg(CWnd* pParent = NULL);	// standard constructor
	~CMFCTestASUnfineLibDlg();

// Dialog Data
	enum { IDD = IDD_MFCTESTASUNFINELIB_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedShowInstallMetroButton();
	afx_msg void OnBnClickedShowRunningAppButton();

private:
	CRunningApplicationListDialog *m_pRunningApplicationListDialog;
	CInstallMetroAppListDialog *m_pInstallMetroAppListDialog;
public:
	CEdit m_EditList;
	CEdit m_EditIcon;
};
