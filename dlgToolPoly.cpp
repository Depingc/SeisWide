// dlgToolPoly.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgToolPoly.h"


// CdlgToolPoly dialog

IMPLEMENT_DYNAMIC(CdlgToolPoly, CDialog)

CdlgToolPoly::CdlgToolPoly(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgToolPoly::IDD, pParent)
	, m_polyA(0)
	, m_polyB(0)
	, m_polyC(0)
	, m_dt(0)
	, m_tmax(0)
	, m_tsf(0)
{

}

CdlgToolPoly::~CdlgToolPoly()
{
}

void CdlgToolPoly::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PROC_A, m_polyA);
	DDX_Text(pDX, IDC_PROC_B, m_polyB);
	DDX_Text(pDX, IDC_PROC_C, m_polyC);
	DDX_Text(pDX, IDC_CONVERSION_DZ2, m_dt);
	DDX_Text(pDX, IDC_CONVERSION_ZMAX, m_tmax);
	DDX_Text(pDX, IDC_CONVERSION_ZMAX2, m_tsf);
}


BEGIN_MESSAGE_MAP(CdlgToolPoly, CDialog)
END_MESSAGE_MAP()


// CdlgToolPoly message handlers
