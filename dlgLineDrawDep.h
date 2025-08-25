#if !defined(AFX_DLGLINEDRAWDEP_H__1A896CA1_6C17_11D2_989D_00C04FAE94E9__INCLUDED_)
#define AFX_DLGLINEDRAWDEP_H__1A896CA1_6C17_11D2_989D_00C04FAE94E9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgLineDrawDep.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgLineDrawDep dialog

class CdlgLineDrawDep : public CDialog
{
// Construction
public:
	CdlgLineDrawDep(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgLineDrawDep)
	enum { IDD = IDD_DLG_LINEDRAWDEP };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgLineDrawDep)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgLineDrawDep)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLINEDRAWDEP_H__1A896CA1_6C17_11D2_989D_00C04FAE94E9__INCLUDED_)
