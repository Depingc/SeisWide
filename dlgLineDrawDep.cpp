// dlgLineDrawDep.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgLineDrawDep.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgLineDrawDep dialog


CdlgLineDrawDep::CdlgLineDrawDep(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgLineDrawDep::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgLineDrawDep)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CdlgLineDrawDep::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgLineDrawDep)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgLineDrawDep, CDialog)
	//{{AFX_MSG_MAP(CdlgLineDrawDep)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgLineDrawDep message handlers
