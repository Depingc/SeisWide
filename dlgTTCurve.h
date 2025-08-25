#if !defined(AFX_DLGTTCURVE_H__F5721691_1F0B_11D1_AF92_0000C04E5DE8__INCLUDED_)
#define AFX_DLGTTCURVE_H__F5721691_1F0B_11D1_AF92_0000C04E5DE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgTTCurve.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgTTCurve dialog

class CdlgTTCurve : public CDialog
{
// Construction
public:
	CdlgTTCurve(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgTTCurve)
	enum { IDD = IDD_DLG_TTCURVE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgTTCurve)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgTTCurve)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTTCURVE_H__F5721691_1F0B_11D1_AF92_0000C04E5DE8__INCLUDED_)
