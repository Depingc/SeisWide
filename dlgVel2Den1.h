#if !defined(AFX_DLGVEL2DEN1_H__A2ACBBC1_8219_11D2_98A1_00C04FAE94E9__INCLUDED_)
#define AFX_DLGVEL2DEN1_H__A2ACBBC1_8219_11D2_98A1_00C04FAE94E9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgVel2Den1.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgVel2Den1 dialog

class CdlgVel2Den1 : public CDialog
{
// Construction
public:
	CdlgVel2Den1(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgVel2Den1)
	enum { IDD = IDD_DLG_VEL2DEN1 };
	float	m_d1;
	float	m_d2;
	float	m_d3;
	float	m_d4;
	float	m_d5;
	float	m_d6;
	float	m_d7;
	float	m_d8;
	float	m_v1;
	float	m_v2;
	float	m_v3;
	float	m_v4;
	float	m_v5;
	float	m_v6;
	float	m_v7;
	float	m_v8;
	float	m_d9;
	float	m_v9;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgVel2Den1)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgVel2Den1)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_isSaveBlocks;
	float m_denMax;
	float m_depthMax;
	float m_gravDC;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGVEL2DEN1_H__A2ACBBC1_8219_11D2_98A1_00C04FAE94E9__INCLUDED_)
