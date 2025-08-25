#if !defined(AFX_DLGVRED_H__3FD0C941_F892_11D0_84BE_0000C04E5DE8__INCLUDED_)
#define AFX_DLGVRED_H__3FD0C941_F892_11D0_84BE_0000C04E5DE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgVred.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgVred dialog

class CdlgVred : public CDialog
{
// Construction
public:
	CdlgVred(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgVred)
	enum { IDD = IDD_DLG_VRED };
	CString	m_timeSecName;
	float	m_vred;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgVred)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgVred)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowset();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGVRED_H__3FD0C941_F892_11D0_84BE_0000C04E5DE8__INCLUDED_)
