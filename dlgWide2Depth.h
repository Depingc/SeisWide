#pragma once


// CdlgWide2Depth dialog

class CdlgWide2Depth : public CDialog
{
	DECLARE_DYNAMIC(CdlgWide2Depth)

public:
	CdlgWide2Depth(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgWide2Depth();

// Dialog Data
	enum { IDD = IDD_DLG_WIDE2DPTH };
	float	m_dz;
	float	m_zmax;
	float	m_zmin;
	CString	m_depthSecName;
	float	m_sphericOrder;
	CString	m_timeSecName;
	float	m_xmax;
	float	m_xmin;
	CString	m_velSecName;

	
	CString m_floatName;
	CString m_rayTracing;
	int m_rayMode; // mode of raytracing, can be P or converted waves
	float m_offsetMax;
	BOOL m_isTwtt;

	BOOL m_isRaytrace;
	// stacking aperture window in km
	//float m_dx;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnButtonBrowset();
	afx_msg void OnButtonBrowsed();
	afx_msg void OnButtonBrowsev();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowserin();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonBrowsev2();
	float m_tmin;
	float m_tmax;
	float m_stackWidth;
	float m_stackWidth2;
};
