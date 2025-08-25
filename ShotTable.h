#pragma once


// CShotTable dialog

class CShotTable : public CDialog
{
	DECLARE_DYNAMIC(CShotTable)

public:
	CShotTable(CWnd* pParent = NULL);   // standard constructor
	virtual ~CShotTable();

// Dialog Data
	enum { IDD = IDD_DLG_SHOTTABLE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	double m_long;
	double m_lat;
};
