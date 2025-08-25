#pragma once


// CdlgDespikingLTZ dialog

class CdlgDespikingLTZ : public CDialog
{
	DECLARE_DYNAMIC(CdlgDespikingLTZ)

public:
	CdlgDespikingLTZ(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgDespikingLTZ();

// Dialog Data
	enum { IDD = IDD_DLG_DESPIKING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
