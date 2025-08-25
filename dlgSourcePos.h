// dlgSourcePos.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgSourcePos dialog

class CdlgSourcePos : public CDialog
{
// Construction
public:
	CdlgSourcePos(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgSourcePos)
	enum { IDD = IDD_DLG_SOURCEPOS };
	CString	m_timeSecName;
	float	m_xShot;
	int		m_options;
//	CString	m_xyFileName;
//	int		m_recPlus;
	int		m_isCalcFromUTM;
	float	m_sx;
	float	m_sy;
	BOOL	m_isSxsy;
	BOOL	m_sxOverwrite;
	int		m_sxsyOverwrite;
	BOOL	m_isNegativeFromLat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgSourcePos)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgSourcePos)
	afx_msg void OnButtonBrowset();
	afx_msg void OnButtonRefresh();
//	afx_msg void OnButtonBrowsetxt();
//	afx_msg void OnSetfocusXshot();
	afx_msg void OnRadioCalc();
	afx_msg void OnRadio6();
	afx_msg void OnChangeDlgProcessingSx();
	afx_msg void OnChangeDlgProcessingSy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
