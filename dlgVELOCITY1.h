// dlgVELOCITY1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgVELOCITY1 dialog

class CdlgVELOCITY1 : public CPropertyPage
{
	DECLARE_DYNCREATE(CdlgVELOCITY1)

// Construction
public:
	CdlgVELOCITY1();
	~CdlgVELOCITY1();

	const static int NUMCOLOR = 11;

// Dialog Data
	//{{AFX_DATA(CdlgVELOCITY1)
	enum { IDD = IDD_VELOCITY };
	double	m_vmax;
	UINT	m_vmaxB;
	UINT	m_vmaxG;
	UINT	m_vmaxR;
	double	m_vmin;
	UINT	m_vminB;
	UINT	m_vminG;
	UINT	m_vminR;
	double	m_dxV;
	CString	m_velSecName;
	UINT	m_vmidB;
	UINT	m_vmidG;
	UINT	m_vmidR;
	double	m_vmid;
	UINT	m_vmidB2;
	UINT	m_vmidB3;
	UINT	m_vmidB4;
	UINT	m_vmidB5;
	UINT	m_vmidB6;
	UINT	m_vmidB7;
	UINT	m_vmidB8;
	UINT	m_vmidB9;

	UINT	m_vmidG2;
	UINT	m_vmidG3;
	UINT	m_vmidG4;
	UINT	m_vmidG5;
	UINT	m_vmidG6;
	UINT	m_vmidG7;
	UINT	m_vmidG8;
	UINT	m_vmidG9;

	UINT	m_vmidR2;
	UINT	m_vmidR3;
	UINT	m_vmidR4;
	UINT	m_vmidR5;
	UINT	m_vmidR6;
	UINT	m_vmidR7;
	UINT	m_vmidR8;
	UINT	m_vmidR9;

	float	m_cxExt;
	float	m_cxPos;
	float	m_cyExt;
	float	m_cyPos;
	BOOL	m_colBar;
	double	m_vmid2;
	double	m_vmid3;
	double	m_vmid4;
	double	m_vmid5;
	double	m_vmid6;
	double	m_vmid7;
	double	m_vmid8;
	double	m_vmid9;
	float	m_dv1;
	//float	m_dv2;
	//float	m_dv3;
	//float	m_dv4;
	//float	m_dv5;
	CString	m_floatName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CdlgVELOCITY1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CdlgVELOCITY1)
	afx_msg void OnButton1();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	afx_msg void OnButton5();
	afx_msg void OnButton6();
	afx_msg void OnButton7();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnKillfocusConversionVminb();
	afx_msg void OnKillfocusConversionVming();
	afx_msg void OnKillfocusConversionVminr();
	afx_msg void OnKillfocusConversionVmaxb();
	afx_msg void OnKillfocusConversionVmaxg();
	afx_msg void OnKillfocusConversionVmaxr();
	afx_msg void OnKillfocusConversionVmidb();
	afx_msg void OnKillfocusConversionVmidb2();
	afx_msg void OnKillfocusConversionVmidb3();
	afx_msg void OnKillfocusConversionVmidb4();
	afx_msg void OnKillfocusConversionVmidg();
	afx_msg void OnKillfocusConversionVmidg2();
	afx_msg void OnKillfocusConversionVmidg3();
	afx_msg void OnKillfocusConversionVmidg4();
	afx_msg void OnKillfocusConversionVmidr();
	afx_msg void OnKillfocusConversionVmidr2();
	afx_msg void OnKillfocusConversionVmidr3();
	afx_msg void OnKillfocusConversionVmidr4();
	afx_msg void OnButtonBrowseFloat();
	//afx_msg void OnButtonBrowsev3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	//void OnBrowseV();

public:
	afx_msg void OnBnClickedButtonBrowsev();
	CString m_vrmsTauName;
	afx_msg void OnBnClickedButtonBrowsevrms();
	void setColorArray(int *colR, int *colG, int *colB);
	BOOL getColorArray(int iColorPattern);
	afx_msg void OnBnClickedButtonColpattern();
	afx_msg void OnBnClickedButtonColpattern2();
//	afx_msg void OnEnChangeConversionVmid5();
	afx_msg void OnBnClickedButton11();
	afx_msg void OnBnClickedButton21();
	afx_msg void OnBnClickedButton22();
	afx_msg void OnBnClickedButton23();
	afx_msg void OnEnKillfocusConversionVmidr5();
	afx_msg void OnEnKillfocusConversionVmidg5();
	afx_msg void OnEnKillfocusConversionVmidb5();
	afx_msg void OnEnKillfocusConversionVmidr6();
	afx_msg void OnEnKillfocusConversionVmidg6();
	afx_msg void OnEnKillfocusConversionVmidb6();
	afx_msg void OnEnKillfocusConversionVmidr7();
	afx_msg void OnEnKillfocusConversionVmidg7();
	afx_msg void OnEnKillfocusConversionVmidb7();
	afx_msg void OnEnKillfocusConversionVmidr8();
	afx_msg void OnEnKillfocusConversionVmidg8();
	afx_msg void OnEnKillfocusConversionVmidb8();
	float m_colBarVmin;
	float m_colBarVmax;
	afx_msg void OnBnClickedButtonColpattern3();
	float m_xWell;
	CString m_velModExtras;
	afx_msg void OnBnClickedButtonColpattern4();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonLoad();
};
