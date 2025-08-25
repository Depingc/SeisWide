#pragma once


// CdlgDespiking dialog

class CdlgDespiking : public CDialog
{
	DECLARE_DYNAMIC(CdlgDespiking)

public:
	CdlgDespiking(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgDespiking();

// Dialog Data
	enum { IDD = IDD_DLG_DESPIKING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowset();
	afx_msg void OnBnClickedButtonBrowsed();
	CString m_inFile;
	CString m_outFile;
	float m_tlength;
	float m_gw;
	int m_trace1;
	int m_trace2;
	BOOL m_isSpikeZeroing;
	float m_dac;
	BOOL m_isLTZ;
	float m_tlag;
};
