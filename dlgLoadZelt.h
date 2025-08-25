// dlgLoadZelt.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgLoadZelt dialog

class CdlgLoadZelt : public CDialog
{
// Construction
public:
	CdlgLoadZelt(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgLoadZelt)
	enum { IDD = IDD_DLG_LOADZELT };
	CString	m_velName;
	CString	m_ZeltName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgLoadZelt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgLoadZelt)
	afx_msg void OnButtonBrowsevascii();
	afx_msg void OnButtonBrowsev();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_is4Decimal;
};
