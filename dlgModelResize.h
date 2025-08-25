#if !defined(AFX_DLGMODELRESIZE_H__3B210410_E68A_11D0_AFBF_0000C04E5DE8__INCLUDED_)
#define AFX_DLGMODELRESIZE_H__3B210410_E68A_11D0_AFBF_0000C04E5DE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgModelResize.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgModelResize dialog

class CdlgModelResize : public CDialog
{
// Construction
public:
	CdlgModelResize(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgModelResize)
	enum { IDD = IDD_DLG_MODEL_RESIZE };
	float	m_xmax;
	float	m_xmin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgModelResize)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgModelResize)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMODELRESIZE_H__3B210410_E68A_11D0_AFBF_0000C04E5DE8__INCLUDED_)
