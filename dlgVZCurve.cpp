// dlgVZCurve.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgVZCurve.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgVZCurve dialog


CdlgVZCurve::CdlgVZCurve(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgVZCurve::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgVZCurve)
	m_xShot = 0.0f;
	m_basementVel = 0.0f;
	m_basementDepth = -1.f;
	//}}AFX_DATA_INIT
}


void CdlgVZCurve::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgVZCurve)
	DDX_Text(pDX, IDC_XSHOT, m_xShot);
	DDX_Text(pDX, IDC_BASEVEL, m_basementVel);
	DDX_Text(pDX, IDC_BASEDEPTH, m_basementDepth);
	
	
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgVZCurve, CDialog)
	//{{AFX_MSG_MAP(CdlgVZCurve)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgVZCurve message handlers
