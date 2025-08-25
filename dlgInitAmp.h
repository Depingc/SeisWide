#if !defined(AFX_DLGINITAMP_H__BE510163_00A2_11D3_98F8_00C04FAE94E9__INCLUDED_)
#define AFX_DLGINITAMP_H__BE510163_00A2_11D3_98F8_00C04FAE94E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgInitAmp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgInitAmp dialog

class CdlgInitAmp : public CDialog
{
// Construction
public:
	CdlgInitAmp(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgInitAmp)
	enum { IDD = IDD_DLG_INITAMP };
	float	m_ampMax;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgInitAmp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgInitAmp)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGINITAMP_H__BE510163_00A2_11D3_98F8_00C04FAE94E9__INCLUDED_)
