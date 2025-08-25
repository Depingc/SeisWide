#if !defined(AFX_DLGCHANGEREDUCINGBMP_H__0A951FC5_9639_4410_B690_CB7AF243C271__INCLUDED_)
#define AFX_DLGCHANGEREDUCINGBMP_H__0A951FC5_9639_4410_B690_CB7AF243C271__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgChangeReducingBMP.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgChangeReducingBMP dialog

class CdlgChangeReducingBMP : public CDialog
{
// Construction
public:
	CdlgChangeReducingBMP(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgChangeReducingBMP)
	enum { IDD = IDD_DLG_CHANGEREDUCE_BMP };
	CString	m_bmpIn;
	CString	m_bmpOut;
	int		m_bitSize;
	float	m_velOut;
	float	m_velIn;
	int		m_col;
	int		m_row;
	float	m_sourceKm;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgChangeReducingBMP)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgChangeReducingBMP)
	afx_msg void OnButtonBrowsed();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCHANGEREDUCINGBMP_H__0A951FC5_9639_4410_B690_CB7AF243C271__INCLUDED_)
