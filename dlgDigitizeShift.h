#if !defined(AFX_DLGDIGITIZESHIFT_H__FE2E5493_C811_11D2_98C4_00C04FAE94E9__INCLUDED_)
#define AFX_DLGDIGITIZESHIFT_H__FE2E5493_C811_11D2_98C4_00C04FAE94E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgDigitizeShift.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgDigitizeShift dialog

class CdlgDigitizeShift : public CDialog
{
// Construction
public:
	CdlgDigitizeShift(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgDigitizeShift)
	enum { IDD = IDD_DLG_DIGITIZE_SHIFT };
	float	m_dx;
	float	m_dt;
	float	m_x1;
	float	m_x2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgDigitizeShift)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgDigitizeShift)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDIGITIZESHIFT_H__FE2E5493_C811_11D2_98C4_00C04FAE94E9__INCLUDED_)
