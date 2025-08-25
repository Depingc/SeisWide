#if !defined(AFX_DLGMIGPS_H__89D6E120_1358_11D1_84D4_261164000000__INCLUDED_)
#define AFX_DLGMIGPS_H__89D6E120_1358_11D1_84D4_261164000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgMigPS.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgMigPS dialog

class CdlgMigPS : public CDialog
{
// Construction
public:
	CdlgMigPS(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgMigPS)
	enum { IDD = IDD_DLG_MIGPS };
	float	m_dz;
	int		m_nx;
	int		m_ltaper;
	CString	m_outFile;
	int		m_nxPad;
	CString	m_inFile;
	CString	m_velSecName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgMigPS)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgMigPS)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonBrowset();
	afx_msg void OnBnClickedButtonBrowsev();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMIGPS_H__89D6E120_1358_11D1_84D4_261164000000__INCLUDED_)
