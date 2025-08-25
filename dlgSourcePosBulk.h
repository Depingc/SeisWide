#pragma once


// CdlgSourcePosBulk dialog

class CdlgSourcePosBulk : public CDialog
{
	DECLARE_DYNAMIC(CdlgSourcePosBulk)

public:
	CdlgSourcePosBulk(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgSourcePosBulk();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_SOURCEPOS_BULK };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_xShot;
	BOOL m_XsMinusOffset;
};
