// dlgAddRangeFile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgAddRangeFile dialog

class CdlgAddRangeFile : public CDialog
{
// Construction
public:
	CdlgAddRangeFile(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgAddRangeFile)
	enum { IDD = IDD_ADD_RANGES_FILE };
	CString	m_refName;
	//int		m_refCDPTrue;
	int     m_assignMethod;
	CString	m_nameOffset;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgAddRangeFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgAddRangeFile)
	afx_msg void OnButtonBrowset();
	afx_msg void OnButton1Offset();
	afx_msg void OnRadio1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRadio14();
};
