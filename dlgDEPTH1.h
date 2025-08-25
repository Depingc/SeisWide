// dlgDEPTH1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgDEPTH1 dialog

class CdlgDEPTH1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CdlgDEPTH1)

// Construction
public:
	CdlgDEPTH1();
	~CdlgDEPTH1();

// Dialog Data
	//{{AFX_DATA(CdlgDEPTH1)
	enum { IDD = IDD_DEPTH };
	//double	m_dz;  // this must be double or more, otherwise we have wrap around errors which accumulates
	float	m_zmax;
	float	m_zmin;
	CString	m_depthSecName;
	float	m_zTick;
	BOOL	m_gainDist;
	BOOL	m_travelTime;
	float	m_tmax;
	float	m_tmin;
	float	m_tTick;
	float	m_vred;
	float	m_baseGain;
	float	m_basePower;
	//CString	m_headInfo;
	//}}AFX_DATA
	int mode;
	CString	m_markerName;
	float m_xMultFactor;
	float m_xShift;
	float m_yMultFactor;
	float m_yShift;
	CString m_rayTracing;
	CString m_digName;
	float m_distShift;
	BOOL m_isShowLabel4LineDrawing;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CdlgDEPTH1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CdlgDEPTH1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonBrowserin2();
	// Ray parameter file, for raytracing only. Optional
	afx_msg void OnBnClickedButtonBrowsedig();
	afx_msg void OnBnClickedButtonBrowset();
	afx_msg void OnBnClickedButtonBrowsed();
	// BOOL m_isTWTT_Time;
	virtual BOOL OnSetActive();
	afx_msg void OnBnClickedButtonBrowsev3();
	BOOL m_markerRevYAxis;
};
