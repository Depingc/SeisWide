// LogPathDlg.h : header file
//

#if !defined(AFX_LOGPATHDLG_H__548A26F7_DC22_46E1_9185_70123AB9D19B__INCLUDED_)
#define AFX_LOGPATHDLG_H__548A26F7_DC22_46E1_9185_70123AB9D19B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _DEBUG_LOG	TRUE
#include "logfile.h"

/////////////////////////////////////////////////////////////////////////////
// CLogPathDlg dialog

class CLogPathDlg : public CDialog
{
// Construction
public:
	void LogEvent(CString event);
	CLogPathDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CLogPathDlg)
	enum { IDD = IDD_LOGPATH_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogPathDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CLogFile	m_log;

	// Generated message map functions
	//{{AFX_MSG(CLogPathDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGPATHDLG_H__548A26F7_DC22_46E1_9185_70123AB9D19B__INCLUDED_)
