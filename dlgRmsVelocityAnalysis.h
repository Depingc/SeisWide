#pragma once


// dlgRmsVelocityAnalysis dialog

class dlgRmsVelocityAnalysis : public CDialog
{
	DECLARE_DYNAMIC(dlgRmsVelocityAnalysis)

public:
	dlgRmsVelocityAnalysis(CWnd* pParent = NULL);   // standard constructor
	virtual ~dlgRmsVelocityAnalysis();

// Dialog Data
	enum { IDD = IDD_DLG_RMSVELOCITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_timeSecName;
	CString m_outfile;
	CString m_velSecName;
	float m_tmin;
	float m_tmax;
	int m_dtRatio;
	float m_vmin;
	float m_vmax;
	float m_dv;
	float m_secsmooth;
	float m_percMute;
	BOOL m_isNmoPanels;
	float m_xmax;
	float m_pwr;
	BOOL m_isNorm;
};
