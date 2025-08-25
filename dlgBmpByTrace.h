#pragma once


// CdlgBmpByTrace dialog

class CdlgBmpByTrace : public CDialog
{
	DECLARE_DYNAMIC(CdlgBmpByTrace)

public:
	CdlgBmpByTrace(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgBmpByTrace();

// Dialog Data
	enum { IDD = IDD_DLG_TIMEBYTRC_BMP };
	int	m_shot1;
	int	m_shot2;
	float	m_tmax;
	float	m_tmin;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_isSwapX;
};
