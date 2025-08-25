#pragma once


// CdlgLabelOverwrite dialog

class CdlgLabelOverwrite : public CDialog
{
	DECLARE_DYNAMIC(CdlgLabelOverwrite)

public:
	CdlgLabelOverwrite(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgLabelOverwrite();

// Dialog Data
	enum { IDD = IDD_DLG_XLABEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_xLabelNew;
	CString m_yLabelNew;
};
