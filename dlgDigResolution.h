#if !defined(AFX_DLGDIGRESOLUTION_H__F0FC4621_EC94_11D0_AFC3_0000C04E5DE8__INCLUDED_)
#define AFX_DLGDIGRESOLUTION_H__F0FC4621_EC94_11D0_AFC3_0000C04E5DE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgDigResolution.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgDigResolution dialog

class CdlgDigResolution : public CDialog
{
// Construction
public:
	CdlgDigResolution(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgDigResolution)
	enum { IDD = IDD_DLG_DIGRESOLUTION };
	float	m_digResolution;
	int		m_nCurve;
	int		m_ivray;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgDigResolution)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgDigResolution)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDIGRESOLUTION_H__F0FC4621_EC94_11D0_AFC3_0000C04E5DE8__INCLUDED_)
