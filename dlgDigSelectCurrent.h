#if !defined(AFX_DLGDIGSELECTCURRENT_H__F1DC8FE9_0954_4EC7_AE35_C3E6B0787EE0__INCLUDED_)
#define AFX_DLGDIGSELECTCURRENT_H__F1DC8FE9_0954_4EC7_AE35_C3E6B0787EE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgDigSelectCurrent.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgDigSelectCurrent dialog

class CdlgDigSelectCurrent : public CDialog
{
// Construction
public:
	CdlgDigSelectCurrent(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgDigSelectCurrent)
	enum { IDD = IDD_DLG_DIG_SETCURRENT };
	int		m_selectCurrent;
	int		m_total;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgDigSelectCurrent)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgDigSelectCurrent)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDIGSELECTCURRENT_H__F1DC8FE9_0954_4EC7_AE35_C3E6B0787EE0__INCLUDED_)
