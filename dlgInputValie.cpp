// dlgInputValie.cpp : implementation file
//

#include "stdafx.h"
#include "seiswide.h"
#include "dlgInputValie.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgInputValie dialog


CdlgInputValie::CdlgInputValie(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgInputValie::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgInputValie)
	m_value = 0.0f;
	//}}AFX_DATA_INIT
}


void CdlgInputValie::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgInputValie)
	DDX_Text(pDX, IDC_EDIT1, m_value);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgInputValie, CDialog)
	//{{AFX_MSG_MAP(CdlgInputValie)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgInputValie message handlers
