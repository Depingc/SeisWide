// dlgReorganize.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgReorganize dialog

class CdlgReorganize : public CDialog
{
// Construction
public:
	CdlgReorganize(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgReorganize)
	enum { IDD = IDD_DLGREORGANIZE };
	CString	m_procNameIn;
	CString	m_procNameOut;
	int		m_trcReverse;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgReorganize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgReorganize)
	afx_msg void OnButtonBrowset();
	afx_msg void OnButtonBrowsetcoh();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
