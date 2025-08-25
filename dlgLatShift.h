// dlgLatShift.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgLatShift dialog

class CdlgLatShift : public CDialog
{
// Construction
public:
	CdlgLatShift(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgLatShift)
	enum { IDD = IDD_DLG_LATSHIFT };
	CString	m_refName;
	double	m_refKm;
	double	m_refSec;
	int		m_offset;
	int		m_shot1;
	int		m_shot2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgLatShift)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgLatShift)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
