#pragma once


// CdlgTraceBalanceXT dialog

class CdlgTraceBalanceXT : public CDialog
{
	DECLARE_DYNAMIC(CdlgTraceBalanceXT)

public:
	CdlgTraceBalanceXT(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgTraceBalanceXT();

// Dialog Data
	enum { IDD = IDD_DIALOG_TRCBALANCEXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_inFile;
	CString m_outFile;
	float m_window_sliding;
	int m_numTraceMix;
	float m_dxMix;
};
