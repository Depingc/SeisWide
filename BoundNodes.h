// BoundNodes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBoundNodes dialog

class CBoundNodes : public CDialog
{
// Construction
public:
	CBoundNodes(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBoundNodes)
	enum { IDD = IDD_BOUND_NODES };
	int		m_layerFocus;
	double	m_xOld;
	double	m_yOld;
	double	m_xNew;
	double	m_yNew;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBoundNodes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBoundNodes)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
