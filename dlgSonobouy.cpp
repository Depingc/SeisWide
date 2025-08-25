// dlgSonobouy.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgSonobouy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgSonobouy dialog


CdlgSonobouy::CdlgSonobouy(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgSonobouy::IDD, pParent)
	, m_srcDepth(0)
	, m_buoydepth(0)
{
	//{{AFX_DATA_INIT(CdlgSonobouy)
	m_vred = 1.44f;
	m_srcDepth = 0.011f;
	m_buoydepth = 0.060f;

	//}}AFX_DATA_INIT
}


void CdlgSonobouy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgSonobouy)
	DDX_Text(pDX, IDC_VIEW_VRED, m_vred);
	DDV_MinMaxFloat(pDX, m_vred, 1.e-002f, 99999.f);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_VIEW_SRCDEPTH, m_srcDepth);
	DDX_Text(pDX, IDC_VIEW_BUOYDEPTH, m_buoydepth);
}


BEGIN_MESSAGE_MAP(CdlgSonobouy, CDialog)
	//{{AFX_MSG_MAP(CdlgSonobouy)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_VIEW_VRED, &CdlgSonobouy::OnEnChangeViewVred)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgSonobouy message handlers

void CdlgSonobouy::OnEnChangeViewVred()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
