#pragma once


// CdlgMagmod dialog

class CdlgMagmod : public CDialog
{
	DECLARE_DYNAMIC(CdlgMagmod)

public:
	CdlgMagmod(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgMagmod();

// Dialog Data
	enum { IDD = IDD_DLG_MAGMOD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_suscept;
	float m_anginc;
	float m_angstr;
};
