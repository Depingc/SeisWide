#pragma once

// CdlgVelocityPerturb dialog

class CdlgVelocityPerturb : public CDialog
{
	DECLARE_DYNCREATE(CdlgVelocityPerturb)

public:
	CdlgVelocityPerturb(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgVelocityPerturb();
// Overrides


// Dialog Data
	enum { IDD = IDD_DLG_VEL_PERTURB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	// float m_vValue;
	int m_layer1;
	int m_layer2;
	float m_vValue;
	CString m_strLines;
	float m_xshot;
};
