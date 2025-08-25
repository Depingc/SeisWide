#if !defined(AFX_DLGKDMIGPOST_H__9B7758E1_3F4C_11D1_AFA4_0000C04E5DE8__INCLUDED_)
#define AFX_DLGKDMIGPOST_H__9B7758E1_3F4C_11D1_AFA4_0000C04E5DE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgKDMigPost.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgKDMigPost dialog

class CdlgKDMigPost : public CDialog
{
// Construction
public:
	CdlgKDMigPost(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgKDMigPost)
	enum { IDD = IDD_DLG_KDMIGPOST };
	BOOL	m_ek;
	CString	m_depthSecName;
	float	m_amax;
	float	m_amin;
	float	m_angmax;
	float	m_aperx;
	float	m_da;
	float	m_dt;
	float	m_dxo;
	float	m_dzo;
	float	m_fa;
	float	m_fac;
	float	m_fxo;
	float	m_fzo;
	int		m_na;
	float	m_v0;
	CString	m_timeSecName;
	CString	m_velSecName;
	float	m_fmax;
	float	m_dxv;
	float	m_dzv;
	float	m_dvz;
	float	m_xmax;
	float	m_zmax;
	int		m_ls;
	float	m_memMax;
	float	m_tmaxRay;
	float	m_dxs;
	BOOL	m_useExist;
	float	m_dxt;
	float	m_dzt;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgKDMigPost)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgKDMigPost)
	afx_msg void OnButtonBrowset();
	afx_msg void OnButtonBrowsev();
	afx_msg void OnButtonBrowsed();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGKDMIGPOST_H__9B7758E1_3F4C_11D1_AFA4_0000C04E5DE8__INCLUDED_)
