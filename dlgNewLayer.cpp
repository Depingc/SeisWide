// dlgNewLayer.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgNewLayer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgNewLayer dialog


CdlgNewLayer::CdlgNewLayer(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgNewLayer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgNewLayer)
	m_numNewLayer = 0;
	m_vTopMax = 0.0f;
	m_vTopMin = 0.0f;
	m_vBotMax = 0.0f;
	m_vBotMin = 0.0f;
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CdlgNewLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgNewLayer)
	DDX_Text(pDX, IDC_DLGNEWLAYER, m_numNewLayer);
	DDV_MinMaxInt(pDX, m_numNewLayer, 0, 30);
	DDX_Text(pDX, IDC_DLGV1MIN, m_vTopMin);
	DDV_MinMaxFloat(pDX, m_vTopMin, 0.f, 30.f);
	DDX_Text(pDX, IDC_DLGV1MAX, m_vTopMax);
	DDV_MinMaxFloat(pDX, m_vTopMax, 0.f, 20.f);
	DDX_Text(pDX, IDC_DLGV2MIN, m_vBotMin);
	DDV_MinMaxFloat(pDX, m_vBotMin, 0.f, 30.f);
	DDX_Text(pDX, IDC_DLGV2MAX, m_vBotMax);
	DDV_MinMaxFloat(pDX, m_vBotMax, 0.f, 30.f);
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgNewLayer, CDialog)
	//{{AFX_MSG_MAP(CdlgNewLayer)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgNewLayer message handlers
