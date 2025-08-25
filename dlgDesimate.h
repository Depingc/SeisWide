#pragma once


// CdlgDesimate dialog

class CdlgDesimate : public CDialog
{
	DECLARE_DYNAMIC(CdlgDesimate)

public:
	CdlgDesimate(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgDesimate();

// Dialog Data
	enum { IDD = IDD_DIALOG_DECIMATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_skip;
	int m_decimate;
};
