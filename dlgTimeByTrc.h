#if !defined(AFX_DLGTIMEBYTRC_H__773EBCF1_488C_11D1_84FF_644BF0000000__INCLUDED_)
#define AFX_DLGTIMEBYTRC_H__773EBCF1_488C_11D1_84FF_644BF0000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgTimeByTrc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgTimeByTrc dialog

class CdlgTimeByTrc : public CDialog
{
// Construction
public:
	CdlgTimeByTrc(CWnd* pParent = NULL);   // standard constructor
	// BOOL CalcSegyInfoOld();
	BOOL CalcSegyInfo();

// Dialog Data
	//{{AFX_DATA(CdlgTimeByTrc)
	enum { IDD = IDD_DLG_TIMEBYTRC };
	int		m_faceDown;
	double	m_clip;
	double	m_gain;
	int	m_shot1;
	int	m_shot2;

	int		m_wiggle;
	CString	m_timeSecTitle;
	double	m_fillOff;
	CString	m_timeSecName;
	float	m_tmax;
	float	m_tmin;
	float	m_tTick;
	BOOL	m_overWrite;
	int		m_tstart;
	//}}AFX_DATA

	struct SEISBICOLOR{
		int	maxAmpR, maxAmpG, maxAmpB, negAmpR, negAmpG, negAmpB;
		int zeroAmpR,zeroAmpG,zeroAmpB;  //not usedd.
		float colClipRatio; //not used.
	} seisBiColor;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgTimeByTrc)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgTimeByTrc)
	afx_msg void OnButtonBrowset();
	afx_msg void OnButtonAuto();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonColor();

	// BOOL m_isAvgAmp;
	float m_vred;
	afx_msg void OnBnClickedButtonColorbar();
	// CString m_colPalette;
	int m_logScaleThreshold;
	BOOL m_isSwapX;
	float m_ampPower;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTIMEBYTRC_H__773EBCF1_488C_11D1_84FF_644BF0000000__INCLUDED_)
