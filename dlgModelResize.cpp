// dlgModelResize.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgModelResize.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgModelResize dialog


CdlgModelResize::CdlgModelResize(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgModelResize::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgModelResize)
	m_xmax = 0.0f;
	m_xmin = 0.0f;
	//}}AFX_DATA_INIT
}


void CdlgModelResize::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgModelResize)
	DDX_Text(pDX, IDC_EDIT_XMAX, m_xmax);
	DDX_Text(pDX, IDC_EDIT_XMIN, m_xmin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgModelResize, CDialog)
	//{{AFX_MSG_MAP(CdlgModelResize)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgModelResize message handlers
