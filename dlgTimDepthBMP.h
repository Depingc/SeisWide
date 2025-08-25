#if !defined(AFX_DLGTIMDEPTHBMP_H__0980ADE4_05F9_11D3_874A_9FFDF328A337__INCLUDED_)
#define AFX_DLGTIMDEPTHBMP_H__0980ADE4_05F9_11D3_874A_9FFDF328A337__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgTimDepthBMP.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgTimDepthBMP dialog

class CdlgTimDepthBMP : public CDialog
{
// Construction
public:
	CdlgTimDepthBMP(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgTimDepthBMP)
	enum { IDD = IDD_DLG_TIM2DPTH_BMP };
	float	m_zmax;
	float	m_zmin;
	float	m_xmax;
	float	m_xmin;
	CString	m_velSecName;
	CString	m_bmpIN;
	CString	m_bmpOUT;
	int		m_col;
	int		m_row;
	int		m_bitSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgTimDepthBMP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgTimDepthBMP)
	afx_msg void OnButtonBrowsev();
	afx_msg void OnButtonBrowsed();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTIMDEPTHBMP_H__0980ADE4_05F9_11D3_874A_9FFDF328A337__INCLUDED_)
