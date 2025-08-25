#if !defined(AFX_DLGINOUT_H__B5BBEB40_2619_11D1_AF95_0000C04E5DE8__INCLUDED_)
#define AFX_DLGINOUT_H__B5BBEB40_2619_11D1_AF95_0000C04E5DE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgInOut.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgInOut dialog

class CdlgInOut : public CDialog
{
// Construction
public:
	CdlgInOut(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgInOut)
	enum { IDD = IDD_DLD_INOUT };
	CString	m_inFile;
	CString	m_outFile;
	int		m_trace1;
	int		m_trace2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgInOut)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgInOut)
	afx_msg void OnButtonBrowset();
	afx_msg void OnButtonBrowsed();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	float m_dac;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINOUT_H__B5BBEB40_2619_11D1_AF95_0000C04E5DE8__INCLUDED_)
