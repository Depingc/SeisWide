// dlgDigitizeShift.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgDigitizeShift.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgDigitizeShift dialog


CdlgDigitizeShift::CdlgDigitizeShift(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgDigitizeShift::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgDigitizeShift)
	m_dx = 0.0f;
	m_dt = 0.0f;
	m_x1 = 0.0f;
	m_x2 = 0.0f;
	//}}AFX_DATA_INIT
}


void CdlgDigitizeShift::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgDigitizeShift)
	DDX_Text(pDX, IDC_EDIT1, m_dx);
	DDX_Text(pDX, IDC_EDIT2, m_dt);
	DDX_Text(pDX, IDC_EDIT3, m_x1);
	DDX_Text(pDX, IDC_EDIT4, m_x2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgDigitizeShift, CDialog)
	//{{AFX_MSG_MAP(CdlgDigitizeShift)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgDigitizeShift message handlers
