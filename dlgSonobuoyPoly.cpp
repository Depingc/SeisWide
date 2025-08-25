// dlgSonobuoyPoly.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgSonobuoyPoly.h"
#include "util.h"


// CdlgSonobuoyPoly dialog

IMPLEMENT_DYNAMIC(CdlgSonobuoyPoly, CDialog)

CdlgSonobuoyPoly::CdlgSonobuoyPoly(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgSonobuoyPoly::IDD, pParent)
	, m_gunDelay(0)

	, m_buoydepth(0)
	, m_vred(0)
	, m_xHyperbola(0)

	, m_polyA(0)
	, m_polyB(0)
	, m_polyC(0)
	, m_isGunDelayCorr(FALSE)
	, m_isIntopol4Zero(FALSE)
	, m_isMono(FALSE)
{

}

CdlgSonobuoyPoly::~CdlgSonobuoyPoly()
{
}

void CdlgSonobuoyPoly::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PROC_A, m_polyA);
	DDX_Text(pDX, IDC_PROC_B, m_polyB);
	DDX_Text(pDX, IDC_PROC_C, m_polyC);
	DDX_Text(pDX, IDC_VIEW_XHYPERBOLA, m_xHyperbola);
	DDX_Text(pDX, IDC_VIEW_VRED, m_vred);
	DDV_MinMaxFloat(pDX, m_vred, 1.e-002f, 99999.f);
	DDX_Text(pDX, IDC_VIEW_BUOYDEPTH, m_buoydepth);
	DDX_Check(pDX, IDC_CHECK_GUNDELAY, m_isGunDelayCorr);
	DDX_Text(pDX, IDC_VIEW_GUNDELAY, m_gunDelay);

	DDX_Check(pDX, IDC_CHECK_INTZERO, m_isIntopol4Zero);
	DDX_Check(pDX, IDC_CHECK_ISMONO, m_isMono);
}


BEGIN_MESSAGE_MAP(CdlgSonobuoyPoly, CDialog)
	ON_BN_CLICKED(IDC_HELP_POLY, &CdlgSonobuoyPoly::OnBnClickedHelpPoly)
END_MESSAGE_MAP()


// CdlgSonobuoyPoly message handlers

void CdlgSonobuoyPoly::OnBnClickedHelpPoly()
{
	myShowHelp("index.html");
}
