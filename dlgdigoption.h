#pragma once


// dlgDigOption dialog

class dlgDigOption : public CDialog
{
	DECLARE_DYNAMIC(dlgDigOption)

public:
	dlgDigOption(CWnd* pParent = NULL);   // standard constructor
	virtual ~dlgDigOption();

// Dialog Data
	enum { IDD = IDD_DLG_DIGOPTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_nCurve;
	CString m_label;
	BOOL m_isSort;
	BOOL m_isSortY;
	BOOL m_NotShow;
};
