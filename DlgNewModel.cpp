// DlgNewModel.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "DlgNewModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgNewModel dialog


CDlgNewModel::CDlgNewModel(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgNewModel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgNewModel)
	m_nLayer = 0;
	m_xmax = 0.0f;
	m_xmin = 0.0f;
	m_zmax = 0.0f;
	m_zmin = 0.0f;
	//}}AFX_DATA_INIT
}


void CDlgNewModel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgNewModel)
	DDX_Text(pDX, IDC_DLG_NLAYER, m_nLayer);
	DDV_MinMaxInt(pDX, m_nLayer, 0, 30);
	DDX_Text(pDX, IDC_DLG_XMAX, m_xmax);
	DDX_Text(pDX, IDC_DLG_XMIN, m_xmin);
	DDX_Text(pDX, IDC_DLG_ZMAX, m_zmax);
	DDV_MinMaxFloat(pDX, m_zmax, 0.f, 10000.f);
	DDX_Text(pDX, IDC_DLG_ZMIN, m_zmin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgNewModel, CDialog)
	//{{AFX_MSG_MAP(CDlgNewModel)
//	ON_BN_CLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgNewModel message handlers

/*void CDlgNewModel::OnOk() 
{
	// TODO: Add your control notification handler code here
	
}
*/