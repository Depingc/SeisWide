#if !defined(AFX_DLGLATLONG_H__767A29F1_E5BF_11D1_B023_0000C04E5DE8__INCLUDED_)
#define AFX_DLGLATLONG_H__767A29F1_E5BF_11D1_B023_0000C04E5DE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgLatLong.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgLatLong dialog

class CdlgLatLong : public CDialog
{
// Construction
public:
	CdlgLatLong(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgLatLong)
	enum { IDD = IDD_DLG_LATLONG };
	CString	m_latLongMulti;
	BOOL	m_isLonglat;
	//BOOL	m_isOutputLatlong;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgLatLong)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgLatLong)
	afx_msg void OnButtonBrowset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_isDistanceInline;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLATLONG_H__767A29F1_E5BF_11D1_B023_0000C04E5DE8__INCLUDED_)
