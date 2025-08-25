#if !defined(AFX_DLGKDMIG_H__B2FE8C61_2C92_11D1_84E6_F42710000000__INCLUDED_)
#define AFX_DLGKDMIG_H__B2FE8C61_2C92_11D1_84E6_F42710000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgKdmig.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgKdmig dialog

class CdlgKdmig : public CDialog
{
// Construction
public:
	CdlgKdmig(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgKdmig)
	enum { IDD = IDD_DLG_KDMIG };
	BOOL	m_ek;
	CString	m_depthSecName;
	float	m_amax;
	float	m_amin;
	float	m_angmax;
	float	m_aperx;
	float	m_da;
	float	m_doff;
	float	m_dt;
	float	m_dxo;
	float	m_dzo;
	float	m_fa;
	float	m_fac;
	float	m_fxo;
	float	m_fzo;
	int		m_na;
	float	m_off0;
	float	m_offmax;
	float	m_v0;
	CString	m_timeSecName;
	CString	m_velSecName;
	float	m_fmax;
	float	m_dxv;
	float	m_dzv;
	float	m_dvz;
	float	m_xmax;
	float	m_zmax;
	float	m_memMax;
	float	m_tmaxRay;
	int		m_ls;
	BOOL	m_useExist;
	float	m_dxm;
	float	m_dxs;
	float	m_dzt;
	float	m_srcDepth;
	float	m_dxt;
	float	m_fs;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgKdmig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgKdmig)
	afx_msg void OnButtonBrowsed();
	afx_msg void OnButtonBrowset();
	afx_msg void OnButtonBrowsev();
	afx_msg void OnButtonDxo();
	virtual BOOL OnInitDialog();
	//afx_msg void OnButtonDistsg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGKDMIG_H__B2FE8C61_2C92_11D1_84E6_F42710000000__INCLUDED_)
