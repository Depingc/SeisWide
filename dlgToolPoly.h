#pragma once


// CdlgToolPoly dialog

class CdlgToolPoly : public CDialog
{
	DECLARE_DYNAMIC(CdlgToolPoly)

public:
	CdlgToolPoly(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgToolPoly();

// Dialog Data
	enum { IDD = IDD_DLG_POLYPOINTS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_polyA;
	float m_polyB;
	float m_polyC;
	float m_dt;
	float m_tmax;
	float m_tsf;
};
