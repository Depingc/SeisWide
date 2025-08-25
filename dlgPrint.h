// dlgPrint.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgPrint dialog

class CdlgPrint : public CDialog
{
// Construction
public:
	CdlgPrint(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgPrint)
	enum { IDD = IDD_PRINT };
	float	m_printX;
	float	m_printY;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgPrint)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgPrint)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_compactToOnePerTrace;
	int m_memoryMax;
	BOOL m_removeLabels;
	BOOL m_isNoLabelsInside;
	virtual void PreInitDialog();
	BOOL m_isNoLines;
};
