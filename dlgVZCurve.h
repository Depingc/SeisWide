#if !defined(AFX_DLGVZCURVE_H__ABB554A1_0E55_11D1_AFD9_0000C04E5DE8__INCLUDED_)
#define AFX_DLGVZCURVE_H__ABB554A1_0E55_11D1_AFD9_0000C04E5DE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgVZCurve.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgVZCurve dialog

class CdlgVZCurve : public CDialog
{
// Construction
public:
	CdlgVZCurve(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgVZCurve)
	enum { IDD = IDD_DLG_VZCURVE };
	float	m_xShot;
	float m_basementVel;
	float m_basementDepth;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgVZCurve)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgVZCurve)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGVZCURVE_H__ABB554A1_0E55_11D1_AFD9_0000C04E5DE8__INCLUDED_)
