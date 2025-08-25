#pragma once
#include "afxwin.h"


// CdlgWide2Velocity dialog

class CdlgWide2Velocity : public CDialog
{
	DECLARE_DYNAMIC(CdlgWide2Velocity)

public:
	CdlgWide2Velocity(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgWide2Velocity();

// Dialog Data
	enum { IDD = IDD_DLG_WIDE2VELOCITY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowset();
	afx_msg void OnBnClickedButtonBrowsed();
	afx_msg void OnBnClickedButtonBrowsev();
	afx_msg void OnBnClickedButtonBrowserin();
	afx_msg void OnBnClickedButtonEdit();
	CString m_rayTracing;
	CString m_velname;
	CString m_depthSecName;
	CString m_timeSecName;
	float m_zmin;
	float m_zmax;
	float m_dz;
	float m_vPerturbMin;
	float m_vPerturbMax;
	float m_dv;
	int m_iLayerFocus;
	float m_secsmooth;
	BOOL m_iscoh;  // BOOL seems to be needed instead of bool for GUI controls
	int m_numDisplay;
	float m_basePower;
	// mode of raytracing, can be P or converted waves
	int m_rayMode;
	int m_sumMethod;
	afx_msg void OnBnClickedButtonBrowsev2();
	CString m_floatName;
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadioMode();
	int m_xpercent;
	BOOL m_isTwtt;
	float m_offsetMax;
	float m_tmin;
	float m_tmax;
};
