#pragma once


// CdlgFilterOffset dialog

class CdlgFilterOffset : public CDialog
{
	DECLARE_DYNAMIC(CdlgFilterOffset)

public:
	CdlgFilterOffset(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgFilterOffset();

// Dialog Data
	enum { IDD = IDD_DLG_FILTER_OFFSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_fLowa;
	float m_fHigha;
	float m_ta;
	float m_fLowb;
	float m_fHighb;
	float m_tb;
	float m_x1;

	float m_fLowax2;
	float m_fHighax2;
	float m_tax2;
	float m_fLowbx2;
	float m_fHighbx2;
	float m_tbx2;
	float m_x2;

	float m_fLowax3;
	float m_fHighax3;
	float m_tax3;
	float m_fLowbx3;
	float m_fHighbx3;
	float m_tbx3;
	float m_x3;
	CString m_nameIn;
	CString m_nameOut;
	int m_numTFilter;
	afx_msg void OnBnClickedButtonBrowset();
};
