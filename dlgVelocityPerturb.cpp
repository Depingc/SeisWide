// dlgVelocityPerturb.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgVelocityPerturb.h"


// CdlgVelocityPerturb dialog

IMPLEMENT_DYNCREATE(CdlgVelocityPerturb, CDialog)

CdlgVelocityPerturb::CdlgVelocityPerturb(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgVelocityPerturb::IDD, pParent)
//	, m_vValue(3.0f)
	, m_layer1(1)
	, m_layer2(9)
	, m_vValue(0)
	, m_strLines(_T(""))
	, m_xshot(0)
{

}

CdlgVelocityPerturb::~CdlgVelocityPerturb()
{
}

void CdlgVelocityPerturb::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//DDX_Text(pDX, IDC_VALUE, m_vValue);
	//DDV_MinMaxInt(pDX, m_layer1, -100, 100);
	DDX_Text(pDX, IDC_LAYER1, m_layer1);
	DDV_MinMaxInt(pDX, m_layer1, 1, 999);
	DDX_Text(pDX, IDC_LAYER2, m_layer2);
	DDV_MinMaxInt(pDX, m_layer2, 1, 999);
	DDX_Text(pDX, IDC_VALUE, m_vValue);
	DDV_MinMaxFloat(pDX, m_vValue, -100, 100);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_strLines);
	DDX_Text(pDX, IDC_XSHOT, m_xshot);
}

BOOL CdlgVelocityPerturb::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CdlgVelocityPerturb, CDialog)
END_MESSAGE_MAP()




// CdlgVelocityPerturb message handlers
