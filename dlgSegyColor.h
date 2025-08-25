#if !defined(AFX_DLGSEGYCOLOR_H__9224C581_F324_11D0_BA6C_444553540000__INCLUDED_)
#define AFX_DLGSEGYCOLOR_H__9224C581_F324_11D0_BA6C_444553540000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgSegyColor.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgSegyColor dialog

class CdlgSegyColor : public CDialog
{
// Construction
public:
	CdlgSegyColor(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgSegyColor)
	enum { IDD = IDD_DLG_SEGYCOLOR };
	CButton	m_button1;
	BYTE		m_maxAmpG;
	BYTE		m_negAmpG;
	BYTE		m_maxAmpB;
	BYTE		m_maxAmpR;
	BYTE		m_negAmpB;
	BYTE		m_negAmpR;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgSegyColor)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgSegyColor)
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnKillfocusColMaxampR();
	afx_msg void OnKillfocusColMaxampG();
	afx_msg void OnKillfocusColMaxampB();
	afx_msg void OnKillfocusColNegampB();
	afx_msg void OnKillfocusColNegampG();
	afx_msg void OnKillfocusColNegampR();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonNewbw();
	afx_msg void OnBnClickedButtonNewcolor();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSEGYCOLOR_H__9224C581_F324_11D0_BA6C_444553540000__INCLUDED_)
