// dlgAddRange.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgAddRange dialog

class CdlgAddRange : public CDialog
{
// Construction
public:
	CdlgAddRange(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgAddRange)
	enum { IDD = IDD_ADD_RANGES };
	float	m_refDx;
	float	m_refRange0;
	CString	m_refName;
	//int		m_refCDPTrue;
	int     m_assignMethod;
	BOOL	m_deadkm;
	// CString	m_nameOffset;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgAddRange)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgAddRange)
	afx_msg void OnButtonBrowset();
	// afx_msg void OnButton1Offset();
	afx_msg void OnRadio1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadio14();
//	int m_refTrace;
	int m_trace1;
	BOOL m_isGs;
	afx_msg void OnBnClickedAssignMethod2();
};
