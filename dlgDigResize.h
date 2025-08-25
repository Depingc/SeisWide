#if !defined(AFX_DLGDIGRESIZE_H__F4A9B77B_80E8_4C98_8650_2E0892DAEA6E__INCLUDED_)
#define AFX_DLGDIGRESIZE_H__F4A9B77B_80E8_4C98_8650_2E0892DAEA6E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgDigResize.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgDigResize dialog

class CdlgDigResize : public CDialog
{
// Construction
public:
	CdlgDigResize(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgDigResize)
	enum { IDD = IDD_DLG_DIGRESIZE };
	float	m_ratioResize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgDigResize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgDigResize)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDIGRESIZE_H__F4A9B77B_80E8_4C98_8650_2E0892DAEA6E__INCLUDED_)
