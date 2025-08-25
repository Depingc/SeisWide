#pragma once


// CdlgAddData dialog

class CdlgAddData : public CDialog
{
	DECLARE_DYNAMIC(CdlgAddData)

public:
	CdlgAddData(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgAddData();

// Dialog Data
	enum { IDD = IDD_DLG_SEGYADD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_timeSecName;
	CString m_timeSecName2;
	BOOL m_isAutoScaling;
	CString m_outputSecName;
	afx_msg void OnBnClickedButtonBrowset();
	afx_msg void OnBnClickedButtonBrowsed();

};
