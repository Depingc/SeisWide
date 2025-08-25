// dlgChangeRanges.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgChangeRanges dialog

class CdlgChangeRanges : public CDialog
{
// Construction
public:
	CdlgChangeRanges(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgChangeRanges)
	enum { IDD = IDD_CHANGE_RANGES };
	CString	m_refName;
	int		m_refShot;
	int		m_refShot2;
	BOOL	m_distance;
	BOOL	m_offset;
	int		m_option;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgChangeRanges)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgChangeRanges)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
