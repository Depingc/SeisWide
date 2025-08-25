#pragma once


// CdlgDistanceAdjust dialog

class CdlgDistanceAdjust : public CDialog
{
	DECLARE_DYNAMIC(CdlgDistanceAdjust)

public:
	CdlgDistanceAdjust(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgDistanceAdjust();

// Dialog Data
	enum { IDD = IDD_DLG_DISTANCE_ADJUST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_timeSecName;
	float x0;
	float m_factor;
	int m_trace1;
	int m_trace2;
};
