#if !defined(AFX_DLGSONOBOUY_H__FE035BF1_77D3_11D1_AFCB_0000C04E5DE8__INCLUDED_)
#define AFX_DLGSONOBOUY_H__FE035BF1_77D3_11D1_AFCB_0000C04E5DE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgSonobouy.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgSonobouy dialog

class CdlgSonobouy : public CDialog
{
// Construction
public:
	CdlgSonobouy(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgSonobouy)
	enum { IDD = IDD_DLG_SONOBOUY };
	float	m_vred;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgSonobouy)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgSonobouy)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeViewVred();
	// Source depth below water surface
	float m_srcDepth;
	// Buoy depth in km
	float m_buoydepth;
	// Gun Delay in sec, marking the actual time delay for the T0 for each segy trace
//	float m_gunDelay;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSONOBOUY_H__FE035BF1_77D3_11D1_AFCB_0000C04E5DE8__INCLUDED_)
