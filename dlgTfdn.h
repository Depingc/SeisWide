#pragma once


// CdlgTfdn dialog

class CdlgTfdn : public CDialog
{
	DECLARE_DYNAMIC(CdlgTfdn)

public:
	CdlgTfdn(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgTfdn();

// Dialog Data
	enum { IDD = IDD_DLG_TFDN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_infile;
	CString m_outFile;
	CString m_outFile2;
	float m_specThreshald;
	int m_numMax;
	float m_xWinMix;
	float m_tWinMix;
	BOOL m_isAllTraces;
};
