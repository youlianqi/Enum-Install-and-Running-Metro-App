
// MFCTestASUnfineLibDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MFCTestASUnfineLib.h"
#include "MFCTestASUnfineLibDlg.h"
#include "RunningApplicationListDialog.h"
#include "InstallMetroAppListDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMFCTestASUnfineLibDlg dialog




CMFCTestASUnfineLibDlg::CMFCTestASUnfineLibDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMFCTestASUnfineLibDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pInstallMetroAppListDialog = NULL;
	m_pRunningApplicationListDialog = NULL;
}

CMFCTestASUnfineLibDlg::~CMFCTestASUnfineLibDlg()
{
	if (m_pInstallMetroAppListDialog != NULL)
	{
		delete m_pInstallMetroAppListDialog;
		m_pInstallMetroAppListDialog = NULL;
	}

	if (m_pRunningApplicationListDialog != NULL)
	{
		delete m_pRunningApplicationListDialog;
		m_pRunningApplicationListDialog = NULL;
	}
}

void CMFCTestASUnfineLibDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LIST_HEIGHT, m_EditList);
	DDX_Control(pDX, IDC_EDIT_ICON_HEIGHT, m_EditIcon);
}

BEGIN_MESSAGE_MAP(CMFCTestASUnfineLibDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
//	ON_BN_CLICKED(IDC_BUTTON1, &CMFCTestASUnfineLibDlg::OnBnClickedButton1)
ON_BN_CLICKED(IDC_SHOW_INSTALL_METRO_BUTTON, &CMFCTestASUnfineLibDlg::OnBnClickedShowInstallMetroButton)
ON_BN_CLICKED(IDC_SHOW_RUNNING_APP_BUTTON, &CMFCTestASUnfineLibDlg::OnBnClickedShowRunningAppButton)
END_MESSAGE_MAP()


// CMFCTestASUnfineLibDlg message handlers

BOOL CMFCTestASUnfineLibDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_EditList.SetWindowText(_T("32"));
	m_EditIcon.SetWindowText(_T("24"));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMFCTestASUnfineLibDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMFCTestASUnfineLibDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMFCTestASUnfineLibDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//void CMFCTestASUnfineLibDlg::OnBnClickedButton1()
//{
//	// TODO: Add your control notification handler code here
//}

void CMFCTestASUnfineLibDlg::OnBnClickedShowInstallMetroButton()
{
	// TODO: Add your control notification handler code here
	/*CInstallMetroAppListDialog installMetroAppListDialog;
	installMetroAppListDialog.DoModal();*/

	CString text;
	m_EditList.GetWindowText(text);
	g_lHeightList= _wtoi( text );

	m_EditIcon.GetWindowText(text);
	g_lHeightIcon= _wtoi( text );

	

	if (g_lHeightList < 10)
	{
		g_lHeightList = 20;
	}

	if (g_lHeightIcon < 10)
	{
		g_lHeightIcon = 32;
	}

	if (g_lHeightIcon > g_lHeightList)
	{
		g_lHeightIcon = g_lHeightList;
	}

	if (!m_pInstallMetroAppListDialog)
	{
		m_pInstallMetroAppListDialog = new CInstallMetroAppListDialog(this);
		m_pInstallMetroAppListDialog->Create(IDD_INSTALL_METRO_DIALOG);
		m_pInstallMetroAppListDialog->ShowWindow(SW_SHOWNORMAL);
	}
	else
	{
		delete m_pInstallMetroAppListDialog;
		m_pInstallMetroAppListDialog = NULL;
		m_pInstallMetroAppListDialog = new CInstallMetroAppListDialog(this);
		m_pInstallMetroAppListDialog->Create(IDD_INSTALL_METRO_DIALOG);
		m_pInstallMetroAppListDialog->ShowWindow(SW_SHOWNORMAL);
	}
}

void CMFCTestASUnfineLibDlg::OnBnClickedShowRunningAppButton()
{
	// TODO: Add your control notification handler code here
	/*CRunningApplicationListDialog runningApplicationListDialog;
	runningApplicationListDialog.DoModal();*/

	CString text;
	m_EditList.GetWindowText(text);
	g_lHeightList= _wtoi( text );

	m_EditIcon.GetWindowText(text);
	g_lHeightIcon= _wtoi( text );

	if (g_lHeightIcon < 10)
	{
		g_lHeightIcon = 32;
	}

	if (g_lHeightList < 10)
	{
		g_lHeightList = 20;
	}

	if (g_lHeightIcon > g_lHeightList)
	{
		g_lHeightIcon = g_lHeightList;
	}


	if (!m_pRunningApplicationListDialog)
	{
		m_pRunningApplicationListDialog = new CRunningApplicationListDialog(this);
		m_pRunningApplicationListDialog->Create(IDD_RUNNING_APPLICATION_DIALOG);
		m_pRunningApplicationListDialog->ShowWindow(SW_SHOWNORMAL);
	}
	else
	{
		delete m_pRunningApplicationListDialog;
		m_pRunningApplicationListDialog = NULL;
		m_pRunningApplicationListDialog = new CRunningApplicationListDialog(this);
		m_pRunningApplicationListDialog->Create(IDD_RUNNING_APPLICATION_DIALOG);
		m_pRunningApplicationListDialog->ShowWindow(SW_SHOWNORMAL);
	}
}
