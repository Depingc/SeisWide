#if !defined(AFX_DLGHYDRATEDISP_H__7CB67621_634E_11D1_850F_A24A98000000__INCLUDED_)
#define AFX_DLGHYDRATEDISP_H__7CB67621_634E_11D1_850F_A24A98000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgHydrateDisp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgHydrateDisp dialog

class CdlgHydrateDisp : public CDialog
{
// Construction
public:
	CdlgHydrateDisp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgHydrateDisp)
	enum { IDD = IDD_DLG_HYDRATE };
	CString	m_timeSecTitle;
	CString	m_timeSecName;
	CString m_velName;
	CString m_digFileName;
	CString m_markerName;
	int		m_faceDown;
	double	m_clip;
	double	m_gain;
	double	m_xmax;
	double	m_xmin;
	int		m_wiggle;
	double	m_xTick;
	double	m_fillOff;
	float	m_obsDepth;
//	float m_waterDepth;
	float	m_tmax;
	float	m_tmin;
	float	m_tTick;
	float	m_vRed;
	int		m_viewAsOffset;
	BOOL	m_travelTime;
	BOOL m_gainDist;
	double m_baseGain;
	double m_basePower;
	//BOOL m_isAvgAmp;
	BOOL m_isNegativeFill;
	BOOL m_isAvgReducedNMO;
	float m_velWater;
	float m_gundepth;
	BOOL m_isSwapX;
	BOOL m_isSwapYLabel;
	int m_logScaleThreshold;
	//}}AFX_DATA
	float m_ampPower;

	struct SEISBICOLOR{
		int	maxAmpR, maxAmpG, maxAmpB, negAmpR, negAmpG, negAmpB;
		int zeroAmpR,zeroAmpG,zeroAmpB;  //not usedd.
		float colClipRatio; //not used.
	} seisBiColor;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgHydrateDisp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	afx_msg void OnButtonBrowset();
	afx_msg void OnBnClickedButtonBrowset2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonColor();
	afx_msg void OnBnClickedButtonColor2();
	afx_msg void OnBnClickedHelp();
	afx_msg void OnBnClickedRadio16();
	// Increase when water depth is too small for straightening hyperbola
	float m_targetDepth;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGHYDRATEDISP_H__7CB67621_634E_11D1_850F_A24A98000000__INCLUDED_)
