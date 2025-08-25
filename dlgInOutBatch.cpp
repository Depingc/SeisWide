// dlgInOutBatch.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgInOutBatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgInOutBatch dialog


CdlgInOutBatch::CdlgInOutBatch(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgInOutBatch::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgInOutBatch)
	m_In = _T("");
	//}}AFX_DATA_INIT
}


void CdlgInOutBatch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgInOutBatch)
	DDX_Text(pDX, IDC_TIMESECNAME, m_In);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgInOutBatch, CDialog)
	//{{AFX_MSG_MAP(CdlgInOutBatch)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgInOutBatch message handlers

void CdlgInOutBatch::OnButtonBrowset() 
{


}

