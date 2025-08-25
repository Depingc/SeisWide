// dlgFilter1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgFilter1 dialog

class CdlgFilter1 : public CDialog
{
// Construction
public:
	CdlgFilter1(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgFilter1)
	enum { IDD = IDD_DLG_FILTER1 };
	BOOL	m_filter;
	float	m_f1;
	float	m_f2;
	//int		m_shot1;
	//int		m_shot2;
	//int		m_shotStep;
	//float	m_tmax;
	//float	m_tmin;
	//float	m_vRed;
	CString	m_nameIn;
	CString	m_nameOut;

	BOOL	m_norm;
	float	m_normValue;
	BOOL	m_multiply;
	float	m_multiplyValue;
	BOOL	m_filterF;
	float	m_ff0;
	float	m_ff1;
	float	m_ff2;
	float	m_ff3;

	BOOL m_isHilbert;
	int m_numTFilter;
	//float m_obsDepth;
	//float m_velWater;
	float m_ta;
	float m_tb;
	float m_f1b;
	float m_f2b;
	BOOL m_zerophase;
	float m_tmin;
	float m_tmax;
	float m_basePower;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgFilter1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CdlgFilter1)
	afx_msg void OnButtonBrowset();
	afx_msg void OnButtonBrowset2();
	afx_msg void OnProcFilter();
	afx_msg void OnProcFilter2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_isTranspose;
	BOOL m_isMixAmpPhase;
	BOOL m_isIEEE_Floats;
	BOOL m_isLimitTime;
};
