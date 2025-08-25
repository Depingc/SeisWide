// LogPathDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LogPath.h"
#include "LogPathDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogPathDlg dialog

CLogPathDlg::CLogPathDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogPathDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogPathDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLogPathDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogPathDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLogPathDlg, CDialog)
	//{{AFX_MSG_MAP(CLogPathDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogPathDlg message handlers

BOOL CLogPathDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLogPathDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLogPathDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CLogPathDlg::LogEvent(CString event)
{
	CString name;
	SYSTEMTIME systime;

	GetLocalTime(&systime);

	name.Format("%i\\%02i\\log_%02i%02i%02i.txt",
		systime.wYear,
		systime.wMonth,
		systime.wYear,
		systime.wMonth,
		systime.wDay);
	m_log.ChangeFile(name);

	m_log.Write(event);
}

void CLogPathDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	LogEvent("An event");
}

void CLogPathDlg::OnButton2() 
{
	// TODO: Add your control notification handler code here
	LogEvent("Another event");

}
