#pragma once


// CdlgSonobuoyPoly dialog

class CdlgSonobuoyPoly : public CDialog
{
	DECLARE_DYNAMIC(CdlgSonobuoyPoly)

public:
	CdlgSonobuoyPoly(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgSonobuoyPoly();

// Dialog Data
	enum { IDD = IDD_DLG_SONOBOUY_POLY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_gunDelay;
	float	m_vred;
	float m_buoydepth;
	float m_xHyperbola;

	float m_polyA;
	float m_polyB;
	float m_polyC;
	BOOL m_isGunDelayCorr;
	afx_msg void OnBnClickedHelpPoly();
	BOOL m_isIntopol4Zero;
	BOOL m_isMono;
};
