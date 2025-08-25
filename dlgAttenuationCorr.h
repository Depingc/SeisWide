#pragma once


// CdlgAttenuationCorr dialog

class CdlgAttenuationCorr : public CDialog
{
	DECLARE_DYNAMIC(CdlgAttenuationCorr)

public:
	CdlgAttenuationCorr(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgAttenuationCorr();

// Dialog Data
	enum { IDD = IDD_DLG_ATTENUATERES };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_inFile;
	CString m_outFile;
	float m_attenuationSeafloor;
	float m_attenuationCorr;
	float m_sphericOrder;
};
