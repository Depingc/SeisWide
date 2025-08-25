#pragma once


// CdlgBitmap2sgy dialog

class CdlgBitmap2sgy : public CDialog
{
	DECLARE_DYNAMIC(CdlgBitmap2sgy)

public:
	CdlgBitmap2sgy(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgBitmap2sgy();

// Dialog Data
	enum { IDD = IDD_DLG_BITMAP2SGY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	float m_tmin;
	float m_tmax;
	// BOOL m_isNotShowAgain;
	BOOL m_isNotShowAgain;
};
