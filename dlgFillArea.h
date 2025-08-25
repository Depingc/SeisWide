#if !defined(AFX_DLGFILLAREA_H__254E0771_E8A4_11D0_AFBF_0000C04E5DE8__INCLUDED_)
#define AFX_DLGFILLAREA_H__254E0771_E8A4_11D0_AFBF_0000C04E5DE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgFillArea.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgFillArea dialog

class CdlgFillArea : public CDialog
{
// Construction
public:
	CdlgFillArea(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgFillArea)
	enum { IDD = IDD_DLG_FILLAREA };
	BOOL	m_noLablesOutside;
//	BOOL	m_CopyAsBitmap;
	int		m_XLen;
	int		m_YLen;
	//}}AFX_DATA
	int m_mode; //0: show all items; 1:disable IDC_RAD_ONEOBJECT and IDC_RADIO4.


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgFillArea)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgFillArea)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckBitmap();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_isNoLabelsInside;
	BOOL m_isNoLines;
	BOOL m_isNoNodes;
	BOOL m_isSeismicPlot;
	float m_XDecimate;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGFILLAREA_H__254E0771_E8A4_11D0_AFBF_0000C04E5DE8__INCLUDED_)
