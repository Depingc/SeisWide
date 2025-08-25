#if !defined(AFX_DLGLABELSIZES_H__571B11BD_3DFC_4126_9037_F05B9BBF097E__INCLUDED_)
#define AFX_DLGLABELSIZES_H__571B11BD_3DFC_4126_9037_F05B9BBF097E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgLabelSizes.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgLabelSizes dialog

class CdlgLabelSizes : public CDialog
{
// Construction
public:
	CdlgLabelSizes(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgLabelSizes)
	enum { IDD = IDD_DLG_LABEL_SIZES };
	float		m_LabelNumberHeigth;
	float		m_LabelNumberWidth;
	float		m_LabelSize;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgLabelSizes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgLabelSizes)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLABELSIZES_H__571B11BD_3DFC_4126_9037_F05B9BBF097E__INCLUDED_)
