// dlgDigitize.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDigitize dialog

class CdlgDigitize : public CDialog
{
// Construction
public:
	CdlgDigitize(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDigitize)
	enum { IDD = IDD_DIALOG_DIGITIZE };
	CString	m_digName;
	int		m_digFileFormat;
	int		m_points;
	int		m_curves;
	float	m_xs;
	CString	m_comments;
	//}}AFX_DATA
	int m_mode;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDigitize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDigitize)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowserin();
	afx_msg void OnBnClickedDigFileformat();
	afx_msg void OnBnClickedRadio2();
	int m_isSort;
	float m_tlag;
	float m_tmin;
	BOOL m_isEnergy;
	float m_tmax;
	int m_numSubdivide;
	float m_lagWeight;
	afx_msg void OnBnClickedHelp();



	BOOL m_isMinusXShot;
};
