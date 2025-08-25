#if !defined(AFX_DLGINPUTVALIE_H__AC5D9BA2_0EB0_4E68_B7ED_69DD457979F0__INCLUDED_)
#define AFX_DLGINPUTVALIE_H__AC5D9BA2_0EB0_4E68_B7ED_69DD457979F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgInputValie.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgInputValie dialog

class CdlgInputValie : public CDialog
{
// Construction
public:
	CdlgInputValie(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgInputValie)
	enum { IDD = IDD_DLG_INPUTVALUE };
	float	m_value;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgInputValie)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgInputValie)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINPUTVALIE_H__AC5D9BA2_0EB0_4E68_B7ED_69DD457979F0__INCLUDED_)
