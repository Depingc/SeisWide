// ColorButton.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "ColorButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorButton

CColorButton::CColorButton()
{
	m_clrBkgnd = RGB(255,0,0);
	m_clrText = RGB(0,255,0);
	brush.CreateSolidBrush(m_clrBkgnd);
}

CColorButton::~CColorButton()
{
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers

HBRUSH CColorButton::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	// TODO: Change any attributes of the DC here
//	pDC->SetBkColor (m_clrBkgnd);
	pDC->SetBkColor (RGB(255,0,0));
//	brush.CreateSolidBrush(RGB(255,0,0));
	pDC->SetTextColor (m_clrText);
	CBrush* oldBrush;
	oldBrush = pDC->SelectObject(&brush);
	CRect rect;
	GetClientRect(&rect);
	pDC->Rectangle (rect);
	pDC->SelectObject(oldBrush);
	CBrush brush1;
	brush1.CreateSolidBrush(RGB(0,255,0));
	// TODO: Return a non-NULL brush if the parent's handler should not be called
	return brush1;
}