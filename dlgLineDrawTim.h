#if !defined(AFX_DLGLINEDRAWTIM_H__20C63A91_6A7D_11D2_989D_00C04FAE94E9__INCLUDED_)
#define AFX_DLGLINEDRAWTIM_H__20C63A91_6A7D_11D2_989D_00C04FAE94E9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgLineDrawTim.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgLineDrawTim dialog

class CdlgLineDrawTim : public CDialog
{
// Construction
public:
	CdlgLineDrawTim(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgLineDrawTim)
	enum { IDD = IDD_DLG_LINEDRAWTIM };
	int		m_option;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgLineDrawTim)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgLineDrawTim)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLINEDRAWTIM_H__20C63A91_6A7D_11D2_989D_00C04FAE94E9__INCLUDED_)
