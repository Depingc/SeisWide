// dlgTim2Dpth.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgTim2Dpth dialog
//{{AFX_INCLUDES()
//#include "webbrowser2.h"
//}}AFX_INCLUDES

class CdlgTim2Dpth : public CDialog
{
// Construction
public:
	CdlgTim2Dpth(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgTim2Dpth)
	enum { IDD = IDD_DLG_TIM2DPTH };
	float	m_dz;
	float	m_zmax;
	float	m_zmin;
	CString	m_depthSecName;
	float	m_sphericOrder;
	CString	m_timeSecName;
	float	m_xmax;
	float	m_xmin;
	CString	m_velSecName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgTim2Dpth)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgTim2Dpth)
	afx_msg void OnButtonBrowset();
	afx_msg void OnButtonBrowsed();
	afx_msg void OnButtonBrowsev();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	float m_distShift;
	CString m_modelsEx;
};
