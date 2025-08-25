#if !defined(AFX_DLGWIENERFILTER_H__90E30A4B_A719_4BEF_80B9_10372D1F8F6D__INCLUDED_)
#define AFX_DLGWIENERFILTER_H__90E30A4B_A719_4BEF_80B9_10372D1F8F6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgWienerFilter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgWienerFilter dialog

class CdlgWienerFilter : public CDialog
{
// Construction
public:
	CdlgWienerFilter(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgWienerFilter)
	enum { IDD = IDD_DLG_WIENER_FILTER };
	float	m_maxlag;
	float	m_dt;
	float	m_maxcorr;
	float	m_mincorr;
	float	m_minlag;
	float	m_pnoise;
	BOOL	m_show_wiener;
	float	m_tmax;
	float	m_tmin;
	CString	m_nameIn;
	CString	m_nameOut;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgWienerFilter)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgWienerFilter)
	afx_msg void OnButtonBrowser1();
	afx_msg void OnButtonBrowser2();
	afx_msg void OnChangeProcnameIn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGWIENERFILTER2_H__90E30A4B_A719_4BEF_80B9_10372D1F8F6D__INCLUDED_)
