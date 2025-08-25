#pragma once


// dlgAgc dialog

class dlgAgc : public CDialog
{
	DECLARE_DYNAMIC(dlgAgc)

public:
	dlgAgc(CWnd* pParent = NULL);   // standard constructor
	virtual ~dlgAgc();

// Dialog Data
	enum { IDD = IDD_DLG_AGC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_inFile;
	CString m_outFile;
	float m_window_sliding;
};
