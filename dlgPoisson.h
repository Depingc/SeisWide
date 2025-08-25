#pragma once


// dlgPoisson dialog

class dlgPoisson : public CDialogEx
{
	DECLARE_DYNAMIC(dlgPoisson)

public:
	dlgPoisson(CWnd* pParent = NULL);   // standard constructor
	virtual ~dlgPoisson();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_POISSON };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_poisson;
	afx_msg void OnBnClickedOk();
};
