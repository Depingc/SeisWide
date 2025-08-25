// DlgNewModel.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgNewModel dialog

class CDlgNewModel : public CDialog
{
// Construction
public:
	CDlgNewModel(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgNewModel)
	enum { IDD = IDD_DLGNEWMODEL };
	int		m_nLayer;
	float	m_xmax;
	float	m_xmin;
	float	m_zmax;
	float	m_zmin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgNewModel)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgNewModel)
//	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
