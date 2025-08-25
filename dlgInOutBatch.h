#if !defined(AFX_DLGINOUTBATCH_H__4B9B1D40_1CC4_11D2_9885_00C04FAE94E9__INCLUDED_)
#define AFX_DLGINOUTBATCH_H__4B9B1D40_1CC4_11D2_9885_00C04FAE94E9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgInOutBatch.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgInOutBatch dialog

class CdlgInOutBatch : public CDialog
{
// Construction
public:
	CdlgInOutBatch(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgInOutBatch)
	enum { IDD = IDD_DLG_INOUT1 };
	CString	m_In;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgInOutBatch)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgInOutBatch)
	afx_msg void OnButtonBrowset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINOUTBATCH_H__4B9B1D40_1CC4_11D2_9885_00C04FAE94E9__INCLUDED_)
