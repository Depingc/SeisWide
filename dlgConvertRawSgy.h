#if !defined(AFX_DLGCONVERTRAWSGY_H__E90254D2_3D0E_11D2_9894_00C04FAE94E9__INCLUDED_)
#define AFX_DLGCONVERTRAWSGY_H__E90254D2_3D0E_11D2_9894_00C04FAE94E9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgConvertRawSgy.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgConvertRawSgy dialog

class CdlgConvertRawSgy : public CDialog
{
// Construction
public:
	CdlgConvertRawSgy(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgConvertRawSgy)
	enum { IDD = IDD_DLG_CONVERTDATSEGY };
	CString	m_outName;
	CString	m_tabName;
	float	m_tmax;
	float	m_tmin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgConvertRawSgy)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgConvertRawSgy)
	afx_msg void OnButtonBrowseo();
	afx_msg void OnButtonBrowset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGCONVERTRAWSGY_H__E90254D2_3D0E_11D2_9894_00C04FAE94E9__INCLUDED_)
