#if !defined(AFX_DLGRAYAUTO_H__26BA2B83_9E33_11D1_AFF3_0000C04E5DE8__INCLUDED_)
#define AFX_DLGRAYAUTO_H__26BA2B83_9E33_11D1_AFF3_0000C04E5DE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgRayAuto.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgRayAuto dialog

class CdlgRayAuto : public CDialog
{
// Construction
public:
	CdlgRayAuto(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgRayAuto)
	enum { IDD = IDD_DLGRAYTRACING_AUTO };
	float	m_zmax;
	float	m_zmin;
	float	m_tmax;
	float	m_tmin;
	float	m_xmax;
	float	m_xmin;
	float	m_xshot;
	float	m_zshot;
	float	m_vred;
	CString	m_newName;
	CString	m_timeSecName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgRayAuto)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgRayAuto)
	afx_msg void OnButtonBrowset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRAYAUTO_H__26BA2B83_9E33_11D1_AFF3_0000C04E5DE8__INCLUDED_)
