#pragma once


// CdlgAsciiHead dialog

class CdlgAsciiHead : public CDialog
{
	DECLARE_DYNAMIC(CdlgAsciiHead)

public:
	CdlgAsciiHead(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgAsciiHead();

// Dialog Data
	enum { IDD = IDD_DLG_ASCIIHEAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_text;
};
