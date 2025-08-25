#pragma once

// CdlgTwoValuec dialog

class CdlgTwoValues : public CDialog
{
	DECLARE_DYNAMIC(CdlgTwoValues)

public:
	CdlgTwoValues(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgTwoValues();

	// Dialog Data
	enum { IDD = IDD_DLG_FLOAT_VALUES_TWO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_value1;
	float m_value2;
};
