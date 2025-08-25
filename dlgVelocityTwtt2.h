#pragma once


// dlgVelocityTwtt2 dialog

class dlgVelocityTwtt2 : public CDialog
{
	DECLARE_DYNAMIC(dlgVelocityTwtt2)

public:
	dlgVelocityTwtt2(CWnd* pParent = NULL);   // standard constructor
	virtual ~dlgVelocityTwtt2();

// Dialog Data
	enum { IDD = IDD_DLG_VELOCITY_TWTT2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	 CString m_plotTitle;
	CString m_timeSecName;
	CString m_digName;  // actually the line drawings
	CString	m_markerName;
	float m_xMultFactor;
	float m_xShift;
	float m_yMultFactor;
	float m_yShift;
	CString m_vrmsTauName;
	float m_xmin;
	float m_xmax;
	float m_xTick;
	float m_tmin;
	float m_tmax;
	float m_tTick;
	int m_wiggle;

	float	m_gain;
	float	m_clip;
	float	m_fillOff;

	//BOOL m_isAveEnergy;
	afx_msg void OnBnClickedButtonBrowsev();
	afx_msg void OnBnClickedButtonBrowset();
	CString m_velSecName;
	afx_msg void OnBnClickedButtonBrowsedig();
	afx_msg void OnBnClickedButtonBrowsevrms();
	afx_msg void OnBnClickedButtonColorbar();
	// CString m_colPalette;
	CString m_modelEx;
	float m_xWell;
	// Extra velocity models to display on the same figure as the main models
	CString m_velModExtras;
	float m_distShift;
	afx_msg void OnBnClickedButtonBrowsev3();
	BOOL m_markerRevYAxis;
	int m_logScaleThreshold;
	BOOL m_isSwapX;
	BOOL m_isSwapYLabel;

	BOOL m_isNegativeFill;
	float m_ampPower;
	BOOL m_travelTime;
};
