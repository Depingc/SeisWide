// dlgTTCurve.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgTTCurve.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgTTCurve dialog


CdlgTTCurve::CdlgTTCurve(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgTTCurve::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgTTCurve)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CdlgTTCurve::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgTTCurve)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgTTCurve, CDialog)
	//{{AFX_MSG_MAP(CdlgTTCurve)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgTTCurve message handlers
