#if !defined(AFX_DLGEXPORTBITMAP_H__E259BCC3_F9B5_11D2_98F5_00C04FAE94E9__INCLUDED_)
#define AFX_DLGEXPORTBITMAP_H__E259BCC3_F9B5_11D2_98F5_00C04FAE94E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgExportBitmap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgExportBitmap dialog

class CdlgExportBitmap : public CDialog
{
// Construction
public:
	CdlgExportBitmap(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgExportBitmap)
	enum { IDD = IDD_DLG_EXPORT_BITMAP };
	int		m_xPixels;
	int		m_yPixels;
	CString	m_nameBitmap;
	int		m_biForm;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgExportBitmap)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgExportBitmap)
	afx_msg void OnButtonBrowset();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_removeLabels;
	BOOL m_isVectorize;
	BOOL m_isNoLabelsInside;
	int m_numColors;

	int m_wiggle; // this is not visibly linked to dialog; 
		// if(m_wiggle==1 || m_wiggle==3) the numColors4Plot shall be disabled on the user's screen

	virtual BOOL OnInitDialog();
	BOOL m_isNoLines;
	BOOL m_isNonodes;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGEXPORTBITMAP_H__E259BCC3_F9B5_11D2_98F5_00C04FAE94E9__INCLUDED_)
