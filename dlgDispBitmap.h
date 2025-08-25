#if !defined(AFX_DLGDISPBITMAP_H__CBADFF62_1037_11D3_874A_AE135AB1C09D__INCLUDED_)
#define AFX_DLGDISPBITMAP_H__CBADFF62_1037_11D3_874A_AE135AB1C09D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// dlgDispBitmap.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgDispBitmap dialog

class CdlgDispBitmap : public CDialog
{
// Construction
public:
	CdlgDispBitmap(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgDispBitmap)
	enum { IDD = IDD_DLG_DISP_BITMAP };
	float	m_yMax;
	float	m_yMin;
	float	m_xMax;
	float	m_xMin;
	BOOL m_offset;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgDispBitmap)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CdlgDispBitmap)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
public:
//	float m_vred;
	BOOL m_isFaceUp;
	BOOL m_travelTime;
	BOOL m_isSwapX;
	BOOL m_isSwapYLabel;
	float m_vred;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGDISPBITMAP_H__CBADFF62_1037_11D3_874A_AE135AB1C09D__INCLUDED_)
