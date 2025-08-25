#if !defined(AFX_DLGMERGEMODELS_H__75EF01F1_09F0_11D1_84CC_2CD620000000__INCLUDED_)
#define AFX_DLGMERGEMODELS_H__75EF01F1_09F0_11D1_84CC_2CD620000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgMergeModels.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgMergeModels dialog

class CdlgMergeModels : public CDialog
{
// Construction
public:
	CdlgMergeModels(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgMergeModels)
	enum { IDD = IDD_DLG_MERGEMODELS };
	CString	m_velSecName;
	CString	m_velSecName2;
	float m_shiftKm2;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgMergeModels)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgMergeModels)
	afx_msg void OnButtonBrowset();
	afx_msg void OnButtonBrowset2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	float m_vTolerancePerc;
	float m_shiftKm;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGMERGEMODELS_H__75EF01F1_09F0_11D1_84CC_2CD620000000__INCLUDED_)
