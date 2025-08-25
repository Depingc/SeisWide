#pragma once


// CdlgOffset1Trc dialog

class CdlgOffset1Trc : public CDialog
{
	DECLARE_DYNAMIC(CdlgOffset1Trc)

public:
	CdlgOffset1Trc(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgOffset1Trc();

// Dialog Data
	enum { IDD = IDD_DLD_OFFSET_1TRC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_trace;
	float m_offset;
};
