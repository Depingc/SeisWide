#if !defined(AFX_DLGUNIX2PC_H__4A073263_23E6_11D3_9907_00C04FAE94E9__INCLUDED_)
#define AFX_DLGUNIX2PC_H__4A073263_23E6_11D3_9907_00C04FAE94E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgUnix2PC.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgUnix2PC dialog

class CdlgUnix2PC : public CDialog
{
// Construction
public:
	CdlgUnix2PC(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgUnix2PC)
	enum { IDD = IDD_DLG_UNIX2PC };
	BOOL	m_gsca;
	BOOL	m_reNum;
	BOOL	m_useReducedTime;
	BOOL	m_isDeadTrcExisting;
	BOOL m_isConvertEbc2Ascii;
	BOOL m_isOffsetMM;
	BOOL m_isSwapIEEE_IBM;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgUnix2PC)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgUnix2PC)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_isResetZero;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGUNIX2PC_H__4A073263_23E6_11D3_9907_00C04FAE94E9__INCLUDED_)
