#if !defined(AFX_DLGRESAMPLE_H__EFF10DF1_28D0_11D3_9908_00C04FAE94E9__INCLUDED_)
#define AFX_DLGRESAMPLE_H__EFF10DF1_28D0_11D3_9908_00C04FAE94E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgResample.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgResample dialog

class CdlgResample : public CDialog
{
// Construction
public:
	CdlgResample(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgResample)
	enum { IDD = IDD_DLG_RESAMPLE };
	CString	m_in;
	CString	m_out;
	float	m_tmax;
	float	m_tmin;
	float	m_vRed;
	long	m_rate2;
	long	m_rate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgResample)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL sampRateOld(CString name);

	// Generated message map functions
	//{{AFX_MSG(CdlgResample)
	afx_msg void OnButtonBrowset();
	afx_msg void OnButtonBrowset2();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int m_numsampNew;
	int m_numsampOld;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGRESAMPLE_H__EFF10DF1_28D0_11D3_9908_00C04FAE94E9__INCLUDED_)
