// dlgNewLayer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgNewLayer dialog

class CdlgNewLayer : public CDialog
{
// Construction
public:
	CdlgNewLayer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgNewLayer)
	enum { IDD = IDD_DLGNEWLAYER };
	int		m_numNewLayer;
	float	m_vTopMax;
	float	m_vTopMin;
	float	m_vBotMax;
	float	m_vBotMin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgNewLayer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgNewLayer)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
