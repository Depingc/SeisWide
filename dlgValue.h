#pragma once


// CdlgValue dialog

class CdlgValue : public CDialog
{
	DECLARE_DYNAMIC(CdlgValue)

public:
	CdlgValue(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgValue();

// Dialog Data
	enum { IDD = IDD_DLG_FLOAT_VALUE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_fvalue;
};
