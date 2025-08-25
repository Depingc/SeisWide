#pragma once


// CdlgTaupInv dialog

class CdlgTaupInv : public CDialog
{
	DECLARE_DYNAMIC(CdlgTaupInv)

public:
	CdlgTaupInv(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgTaupInv();

// Dialog Data
	enum { IDD = IDD_DLG_TAUP_INV };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_timeSecName;
	CString m_taupName;
	CString m_FiltTaupName;
	int m_npoints;

};
