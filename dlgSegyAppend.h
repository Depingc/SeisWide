#if !defined(AFX_DLGSEGYAPPEND_H__976E3961_F462_11D0_BA6C_444553540000__INCLUDED_)
#define AFX_DLGSEGYAPPEND_H__976E3961_F462_11D0_BA6C_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgSegyAppend.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgSegyAppend dialog

class CdlgSegyAppend : public CDialog
{
// Construction
public:
	CdlgSegyAppend(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgSegyAppend)
	enum { IDD = IDD_DLG_SEGYAPPEND };
	CString	m_2ndName;
	CString	m_1stName;
	BOOL	m_reNumCDP;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgSegyAppend)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgSegyAppend)
	afx_msg void OnButtonBrowsed();
	afx_msg void OnButtonBrowset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int m_traces_insert;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSEGYAPPEND_H__976E3961_F462_11D0_BA6C_444553540000__INCLUDED_)
