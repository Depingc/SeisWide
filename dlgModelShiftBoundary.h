#if !defined(AFX_DLGMODELSHIFTBOUNDARY_H__8FBE6755_9B3E_4D9F_AA3D_1327550E9CED__INCLUDED_)
#define AFX_DLGMODELSHIFTBOUNDARY_H__8FBE6755_9B3E_4D9F_AA3D_1327550E9CED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgModelShiftBoundary.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgModelShiftBoundary dialog

class CdlgModelShiftBoundary : public CDialog
{
// Construction
public:
	CdlgModelShiftBoundary(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgModelShiftBoundary)
	enum { IDD = IDD_DLG_MODEL_SHIFTBOUNDARY };
	float	m_xmax;
	float	m_xmin;
	float	m_zinc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgModelShiftBoundary)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgModelShiftBoundary)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMODELSHIFTBOUNDARY_H__8FBE6755_9B3E_4D9F_AA3D_1327550E9CED__INCLUDED_)
