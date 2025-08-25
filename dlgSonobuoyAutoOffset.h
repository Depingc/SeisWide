#pragma once


// CdlgSonobuoyAutoOffset dialog

class CdlgSonobuoyAutoOffset : public CDialog
{
	DECLARE_DYNAMIC(CdlgSonobuoyAutoOffset)

public:
	CdlgSonobuoyAutoOffset(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgSonobuoyAutoOffset();

// Dialog Data
	enum { IDD = IDD_DLG_SONOBOUY_AUTOOFFSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowset();
	CString m_timeSecName;
	double m_long;
	double m_lat;
	afx_msg void OnBnClickedButtonBrowset2();
	//BOOL m_isTwoSided;
	//BOOL m_isOffsetVaryWithLat;
	afx_msg void OnBnClickedHelp();
	float m_factor;
//	BOOL m_isFactorMIL;
//	afx_msg void OnBnClickedChkFactormil();
	BOOL m_markupError;
};
