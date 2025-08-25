#pragma once


// CdlgNormTrcInt dialog

class CdlgNormTrcInt : public CDialog
{
	DECLARE_DYNAMIC(CdlgNormTrcInt)

public:
	CdlgNormTrcInt(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgNormTrcInt();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SEGY_ADD };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_timeSecName;
	float m_multFactor;
	BOOL m_isAutoScaling;
	CString m_outputSecName;
	afx_msg void OnBnClickedButtonBrowset();
};
