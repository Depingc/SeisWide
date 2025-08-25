#if !defined(AFX_DLGTABLIZELAYER_H__F0E340D1_BE7B_11D1_B002_0000C04E5DE8__INCLUDED_)
#define AFX_DLGTABLIZELAYER_H__F0E340D1_BE7B_11D1_B002_0000C04E5DE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgTablizeLayer.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgTablizeLayer dialog

class CdlgTablizeLayer : public CDialog
{
// Construction
public:
	CdlgTablizeLayer(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgTablizeLayer)
	enum { IDD = IDD_DLG_TABLIZE_LAYER };
	float	m_x1;
	float	m_x2;
	float	m_y1;
	float	m_y2;
	int		m_mode;
	int		m_top;
	BYTE	m_layerNum;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgTablizeLayer)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgTablizeLayer)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTABLIZELAYER_H__F0E340D1_BE7B_11D1_B002_0000C04E5DE8__INCLUDED_)
