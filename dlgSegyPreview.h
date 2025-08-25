#if !defined(AFX_DLGSEGYPREVIEW_H__3E7CEAE1_F7D3_11D0_84BC_0000C04E5DE8__INCLUDED_)
#define AFX_DLGSEGYPREVIEW_H__3E7CEAE1_F7D3_11D0_84BC_0000C04E5DE8__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// dlgSegyPreview.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgSegyPreview dialog

class CdlgSegyPreview : public CDialog
{
// Construction
public:
	CdlgSegyPreview(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgSegyPreview)
	enum { IDD = IDD_DLG_SEGYPREVIEW };
	CString	m_headInfo;
	CString	m_timeSecName;
	float	m_tmax;
	float	m_tmin;
	float	m_tTick;
	CString	m_timeSecTitle;
	int		m_faceDown;
	float	m_clip;
	float	m_fillOff;
	float	m_gain;
	long	m_shot1;
	long	m_shot2;
	long	m_shotStep;
	int		m_wiggle;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgSegyPreview)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgSegyPreview)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSEGYPREVIEW_H__3E7CEAE1_F7D3_11D0_84BC_0000C04E5DE8__INCLUDED_)
