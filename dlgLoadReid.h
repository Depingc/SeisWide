#if !defined(AFX_DLGLOADREID_H__81A212F1_DF6D_11D1_B017_0000C04E5DE8__INCLUDED_)
#define AFX_DLGLOADREID_H__81A212F1_DF6D_11D1_B017_0000C04E5DE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgLoadReid.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgLoadReid dialog

class CdlgLoadReid : public CDialog
{
// Construction
public:
	CdlgLoadReid(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgLoadReid)
	enum { IDD = IDD_DLG_LOADREID };
	CString	m_velName;
	CString	m_reidName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgLoadReid)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgLoadReid)
	afx_msg void OnButtonBrowsev();
	afx_msg void OnButtonBrowsevascii();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLOADREID_H__81A212F1_DF6D_11D1_B017_0000C04E5DE8__INCLUDED_)
