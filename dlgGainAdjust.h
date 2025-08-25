#pragma once


// CdlgGainAdjust dialog

class CdlgGainAdjust : public CDialog
{
	DECLARE_DYNAMIC(CdlgGainAdjust)

public:
	CdlgGainAdjust(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgGainAdjust();

// Dialog Data
	enum { IDD = IDD_DLG_GAIN_ADJUST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_multiply;
	int m_trace1;
	int m_trace2;
};
