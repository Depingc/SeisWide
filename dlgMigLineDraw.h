#if !defined(AFX_DLGMIGLINEDRAW_H__6C8AF6A2_A3E1_11D2_98B0_00C04FAE94E9__INCLUDED_)
#define AFX_DLGMIGLINEDRAW_H__6C8AF6A2_A3E1_11D2_98B0_00C04FAE94E9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgMigLineDraw.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgMigLineDraw dialog

class CdlgMigLineDraw : public CDialog
{
// Construction
public:
	CdlgMigLineDraw(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgMigLineDraw)
	enum { IDD = IDD_DLG_MIGLINEDRAW };
	float	m_aamax;
	float	m_aamin;
	float	m_dv;
	float	m_dxv;
	int	m_maxTrc;
	int	m_nLayer;
	float	m_sDepth;
	float	m_sDist;
	float	m_tErr;
	float	m_xErr;
	CString	m_migName;
	CString	m_velName;
	BOOL	m_append;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgMigLineDraw)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgMigLineDraw)
	afx_msg void OnButtonBrowsev();
	afx_msg void OnButtonBrowsev2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMIGLINEDRAW_H__6C8AF6A2_A3E1_11D2_98B0_00C04FAE94E9__INCLUDED_)
