#if !defined(AFX_DLGIMPORTBITMAP_H__D61E0313_0191_11D3_98F9_00C04FAE94E9__INCLUDED_)
#define AFX_DLGIMPORTBITMAP_H__D61E0313_0191_11D3_98F9_00C04FAE94E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgImportbitmap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgImportbitmap dialog

class CdlgImportbitmap : public CDialog
{
// Construction
public:
	CdlgImportbitmap(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgImportbitmap)
	enum { IDD = IDD_DLG_IMPORT_BITMAP };
	float	m_Z2;
	float	m_Z1;
	float	m_X2;
	float	m_X1;
	CString	m_name;
//	int	m_isDepth;
	//}}AFX_DATA
	// BOOL isLoading;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgImportbitmap)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgImportbitmap)
	afx_msg void OnButtonBrowset();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_isOffset;
	BOOL m_isDigitizing;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGIMPORTBITMAP_H__D61E0313_0191_11D3_98F9_00C04FAE94E9__INCLUDED_)
