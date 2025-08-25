#pragma once


// CdlgSineModulate dialog

class CdlgSineModulate : public CDialog
{
	DECLARE_DYNAMIC(CdlgSineModulate)

public:
	CdlgSineModulate(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgSineModulate();

// Dialog Data
	enum { IDD = IDD_DLG_SINE_MODULATE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_lumda;
	float m_modDepth;
	float m_xmin;
	float m_xmax;
};
