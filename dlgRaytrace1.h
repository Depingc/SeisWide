// dlgRaytrace1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgRaytrace1 dialog

class CdlgRaytrace1 : public CDialog
{
// Construction
public:
	CdlgRaytrace1(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgRaytrace1)
	enum { IDD = IDD_DLGRAYTRACING1 };
	CString	m_rayTracing;
	CString	m_velName;
	CString	m_floatName;
	CString	m_txName;
	//}}AFX_DATA
	float tmax,tmin,vred,xmax,xmin,xshot,zshot,zmax,zmin;
	CString timeSecName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgRaytrace1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgRaytrace1)
	afx_msg void OnButtonBrowserin();
	afx_msg void OnButtonBrowsev();
	afx_msg void OnButtonAuto();
	afx_msg void OnButtonBrowseFloat();
	afx_msg void OnButtonEdit();
	afx_msg void OnButtonBrowseTx();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	// BOOL m_isTraceAuto;
	float m_kmShift;
	BOOL m_isPrompt;
};
