// dlgLineDrawTim.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgLineDrawTim.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgLineDrawTim dialog


CdlgLineDrawTim::CdlgLineDrawTim(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgLineDrawTim::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgLineDrawTim)
	m_option = 0;
	//}}AFX_DATA_INIT
}


void CdlgLineDrawTim::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgLineDrawTim)
	DDX_Radio(pDX, IDC_RADIO1, m_option);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgLineDrawTim, CDialog)
	//{{AFX_MSG_MAP(CdlgLineDrawTim)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgLineDrawTim message handlers
