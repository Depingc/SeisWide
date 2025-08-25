#pragma once


// CDlgVelocityTwtt dialog

class CDlgVelocityTwtt : public CDialog
{
	DECLARE_DYNAMIC(CDlgVelocityTwtt)

public:
	CDlgVelocityTwtt(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVelocityTwtt();

// Dialog Data
	enum { IDD = IDD_DLG_VELOCITY_TWTT };
	CString	m_timeSecTitle;
	float	m_xmax;
	float	m_xmin;
	float m_xTick;
	float   m_zSource;
	CString	m_velSecName;
	float	m_tmax;
	float	m_tmin;
	float	m_tTick;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowsev();
	afx_msg void OnBnClickedButtonBrowset();
	CString m_timeSecName;
	CString m_digName;
	afx_msg void OnBnClickedButtonBrowsedig();
	CString m_vrmsTauName;
	afx_msg void OnBnClickedButtonBrowsevrms();
	int m_wiggle;
	BOOL m_isAveEnergy;
};
