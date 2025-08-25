// dlgNewVelFile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgNewVelFile dialog
#include "VelMod.h"

class CdlgNewVelFile : public CDialog
{
// Construction
public:
	CdlgNewVelFile(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgNewVelFile)
	enum { IDD = IDD_DLGNEWVELFILE };
	CString	m_velNewFileName;
	int		m_velFormat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgNewVelFile)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgNewVelFile)
	afx_msg void OnButtonBrowsev();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnBnClickedVelFormat();
	afx_msg void OnBnClickedRadio18();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio16();
	afx_msg void OnBnClickedRadio17();
	afx_msg void OnBnClickedButtonCalc();
	afx_msg void OnEnChangeVelWellDist();
//	afx_msg void OnEnChangeVelEnddist();


	CString m_title;
	CString m_cruise;
	CString m_author;
	CString m_company;
	CString m_date;
	CString m_secName;

	// we want these to be float instead of double, otherwise the display is too long
	// but, using float, it becomes inaccurate. So I change back to double
	double m_startLat;
	double m_startLong;
	double m_endLat;
	double m_endLong;
	double m_deployLat;
	double m_deployLong;
	double m_wellLat;
	double m_wellLong;
	double m_wellDist;

	double m_xShift;
		// this is to be added to the trace distance, must comes together with m_secName,
		// otherwise the distance info is incorrect

	CVelMod::VELMOD *m_velm;

	BOOL m_isNoNodeNav;
	BOOL m_isViewXml;
	BOOL m_isVelBelowValid;

	int m_maxHorizonValid;
	double m_deployDist;
	double m_deployDepth;
	float m_startDist;
	float m_endDist;
//	afx_msg void OnEnChangeVelStartdist();
	CString m_wellTitle;
	afx_msg void OnBnClickedButtonCalc2();
};
