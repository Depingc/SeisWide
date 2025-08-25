// dlgRayTracing.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgRayTracing dialog

class CdlgRayTracing : public CDialog
{
// Construction
public:
	CdlgRayTracing(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgRayTracing)
	enum { IDD = IDD_DLGRAYTRACING };
	CString	m_rayTracing;
	CString	m_velName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgRayTracing)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgRayTracing)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
