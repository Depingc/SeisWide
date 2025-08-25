// dlgVel2Den1.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgVel2Den1.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgVel2Den1 dialog


CdlgVel2Den1::CdlgVel2Den1(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgVel2Den1::IDD, pParent)
	, m_isSaveBlocks(FALSE)
	, m_denMax(0)
	, m_depthMax(0)
	, m_gravDC(0)
{
	//{{AFX_DATA_INIT(CdlgVel2Den1)
	m_d1 = 0.0f;
	m_d2 = 0.0f;
	m_d3 = 0.0f;
	m_d4 = 0.0f;
	m_d5 = 0.0f;
	m_d6 = 0.0f;
	m_d7 = 0.0f;
	m_d8 = 0.0f;
	m_v1 = 0.0f;
	m_v2 = 0.0f;
	m_v3 = 0.0f;
	m_v4 = 0.0f;
	m_v5 = 0.0f;
	m_v6 = 0.0f;
	m_v7 = 0.0f;
	m_v8 = 0.0f;
	m_d9 = 0.0f;
	m_v9 = 0.0f;
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	double c1 = -.6997, c2 = 2.2302, c3 = -.598, c4 = .07036, c5 = -.0028311;
	float v;

	m_v1 = 1.6f;   m_d1 = 1.03f;
	m_v2 = 1.8f;   m_d2 = 1.8f;
	m_v3 = v = 3.0f;  m_d3 = (float)(c5*v*v*v*v + c4*v*v*v + c3*v*v + c2*v + c1); m_d3 = (int)(m_d3 * 100 + 0.5) * 0.01f;
	m_v4 = v = 4.0f;  m_d4 = (float)(c5*v*v*v*v + c4*v*v*v + c3*v*v + c2*v + c1); m_d4 = (int)(m_d4 * 100 + 0.5) * 0.01f;
	m_v5 = v = 5.0f;  m_d5 = (float)(c5*v*v*v*v + c4*v*v*v + c3*v*v + c2*v + c1); m_d5 = (int)(m_d5 * 100 + 0.5) * 0.01f;
	m_v6 = v = 6.0f;  m_d6 = (float)(c5*v*v*v*v + c4*v*v*v + c3*v*v + c2*v + c1); m_d6 = (int)(m_d6 * 100 + 0.5) * 0.01f;
	m_v7 = v = 7.0f;  m_d7 = (float)(c5*v*v*v*v + c4*v*v*v + c3*v*v + c2*v + c1); m_d7 = (int)(m_d7 * 100 + 0.5) * 0.01f;
	m_v8 = v = 8.0f;  m_d8 = (float)(c5*v*v*v*v + c4*v*v*v + c3*v*v + c2*v + c1); m_d8 = (int)(m_d8 * 100 + 0.5) * 0.01f;
	m_v9 = 8.5f;  m_d9 = m_d8;
}


void CdlgVel2Den1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgVel2Den1)
	DDX_Text(pDX, IDC_EDIT_V1, m_v1);
	DDV_MinMaxFloat(pDX, m_v1, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_D1, m_d1);
	DDV_MinMaxFloat(pDX, m_d1, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_V2, m_v2);
	DDV_MinMaxFloat(pDX, m_v2, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_D2, m_d2);
	DDV_MinMaxFloat(pDX, m_d2, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_V3, m_v3);
	DDV_MinMaxFloat(pDX, m_v3, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_D3, m_d3);
	DDV_MinMaxFloat(pDX, m_d3, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_V4, m_v4);
	DDV_MinMaxFloat(pDX, m_v4, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_D4, m_d4);
	DDV_MinMaxFloat(pDX, m_d4, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_V5, m_v5);
	DDV_MinMaxFloat(pDX, m_v5, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_D5, m_d5);
	DDV_MinMaxFloat(pDX, m_d5, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_V6, m_v6);
	DDV_MinMaxFloat(pDX, m_v6, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_D6, m_d6);
	DDV_MinMaxFloat(pDX, m_d6, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_V7, m_v7);
	DDV_MinMaxFloat(pDX, m_v7, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_D7, m_d7);
	DDV_MinMaxFloat(pDX, m_d7, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_V8, m_v8);
	DDV_MinMaxFloat(pDX, m_v8, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_D8, m_d8);
	DDV_MinMaxFloat(pDX, m_d8, 1.e-004f, 10.f);
	DDX_Text(pDX, IDC_EDIT_V9, m_v9);
	DDX_Text(pDX, IDC_EDIT_D9, m_d9);

	DDX_Text(pDX, IDC_EDIT_DEPTHMAX, m_depthMax);
	DDV_MinMaxFloat(pDX, m_depthMax, 1, 99999);
	DDX_Text(pDX, IDC_EDIT_GRAVDC, m_gravDC);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_EDIT_D10, m_denMax);
	DDV_MinMaxFloat(pDX, m_denMax, 0.001f, 5.f);
	DDX_Check(pDX, IDC_CHECK1, m_isSaveBlocks);
}


BEGIN_MESSAGE_MAP(CdlgVel2Den1, CDialog)
	//{{AFX_MSG_MAP(CdlgVel2Den1)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgVel2Den1 message handlers
