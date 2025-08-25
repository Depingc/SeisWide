// dlgCohMix.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgCohMix dialog

class CdlgCohMix : public CDialog
{
// Construction
public:
	CdlgCohMix(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgCohMix)
	enum { IDD = IDD_DLGCOHMIX };
	CString	m_procNameIn;
	CString	m_procNameOut;
	float	m_procTmax;
	float	m_procTmin;
	float	m_procTWindow;
	float	m_procVelmax;
	float	m_procVelmin;
	float	m_dxMix1;
	int m_outputMethod;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgCohMix)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgCohMix)
	afx_msg void OnButtonBrowset();
	afx_msg void OnButtonBrowsetcoh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	float m_xmin;
	float m_xmax;
	int m_numDivision;
	float m_f1;
	float m_f2;
	float m_dxMix2;
	int m_nBigLoop;
	int m_nSmooth;
};
