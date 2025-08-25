#pragma once


// CdlgTraceDub dialog

class CdlgTraceDub : public CDialog
{
	DECLARE_DYNAMIC(CdlgTraceDub)

public:
	CdlgTraceDub(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgTraceDub();

// Dialog Data
	enum { IDD = IDD_DLG_SONOBOUY_DUB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_fireRate1;
	float m_fireRate2;
	int m_traceSplit;
};
