#pragma once


// CdlgRegionalModel dialog

class CdlgRegionalModel : public CDialog
{
	DECLARE_DYNAMIC(CdlgRegionalModel)

public:
	CdlgRegionalModel(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgRegionalModel();

// Dialog Data
	enum { IDD = IDD_DLG_MODEL_THEORETICAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_regionalModelA;
	double m_regionalModelB;
	double m_regionalModelAlpha;
	double m_regionalModelBeta;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};
