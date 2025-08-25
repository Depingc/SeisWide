#pragma once

#ifdef _WIN32_WCE
#error "CDHtmlDialog is not supported for Windows CE."
#endif 

// CdlgHardwareId dialog

class CdlgHardwareId : public CDHtmlDialog
{
	DECLARE_DYNCREATE(CdlgHardwareId)

public:
	CdlgHardwareId(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgHardwareId();
// Overrides
	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);

// Dialog Data
	enum { IDD = IDD_DLG_HARDWAREID, IDH = IDR_HTML_DLGHARDWAREID };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
public:
	CString m_HardwareId;
	CString m_licenseDetectedInfo;
};
