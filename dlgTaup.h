#pragma once


// CdlgTaup dialog

class CdlgTaup : public CDialog
{
	DECLARE_DYNAMIC(CdlgTaup)

public:
	CdlgTaup(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgTaup();

// Dialog Data
	enum { IDD = IDD_DLG_TAUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_timeSecName;
	CString m_taupName;
	CString m_FiltTaupName;
	float m_ppMin;
	float m_pp1;
	float m_pp2;
	float m_pp3;
	float m_pp4;
	float m_ppMax;
	float m_dp;
	int m_npoints;
//	float m_freqMax;
	float m_tmin;
	float m_tmax;
	BOOL m_isForwardOnly;
	BOOL m_isInBetween;
	float m_dx;
	float m_fMax;
	afx_msg void OnEnChangeEditFreqmax();
};
