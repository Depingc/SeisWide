// dlgTIMESEC1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgTIMESEC1 dialog

class CdlgTIMESEC1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CdlgTIMESEC1)

// Construction
public:
	CdlgTIMESEC1();
	~CdlgTIMESEC1();

// Dialog Data
	//{{AFX_DATA(CdlgTIMESEC1)
	enum { IDD = IDD_TIMESEC };
	int		m_faceDown;
	float	m_clip;
	float	m_gain;
	float	m_fillOff;
	CString	m_timeSecTitle;
	int		m_wiggle;
	CString	m_timeSecName;
	int		m_offset;
	float	m_xmax;
	float	m_xmin;
	float	m_xTick;
	BOOL	m_isNegativeFill;
	// BOOL m_isAvgAmp;
	BOOL m_isSwapX;
	BOOL m_isSwapYLabel; // swap position for the Y-label position on screen
	int m_logScaleThreshold;

	float	m_dist1;
	float	m_dist2;
	float	m_dist3;
	//}}AFX_DATA
	int mode;

	CString m_depthRefName;  // why do we need this? It is because color selection dialog will use m_timeSecName to locate the colPal.txt file.
		// if we do not supply m_depthRefName, it will use the directory of m_timeSecName  which can be wrong

/*
struct Date
{
    int day;
    int month;
    int year;
    Date(int d=0, int m=0, int y=0):day(d),
           month(m),
           year(y){}
};
Date d( 4, 2, 42 );
*/
	struct SEISBICOLOR{
		int	maxAmpR, maxAmpG, maxAmpB;
		int negAmpR, negAmpG, negAmpB;
		SEISBICOLOR(int r=255, int g=0, int b=0, int r2=0, int g2=0, int b2=255) :
			maxAmpR(r), maxAmpG(g), maxAmpB(b),
			negAmpR(r2), negAmpG(g2), negAmpB(b2) {}
	} seisBiColor;



// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CdlgTIMESEC1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CdlgTIMESEC1)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonColorbar();
	afx_msg void OnBnClickedButtonBrowset();
	afx_msg void OnBnClickedViewOffset();
	afx_msg void OnBnClickedViewDist();
	float m_ampPower;
};
