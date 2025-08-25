// dlgSineModulate.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgSineModulate.h"


// CdlgSineModulate dialog

IMPLEMENT_DYNAMIC(CdlgSineModulate, CDialog)

CdlgSineModulate::CdlgSineModulate(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgSineModulate::IDD, pParent)
	, m_lumda(0)
	, m_modDepth(0)
	, m_xmin(0)
	, m_xmax(0)
{

}

CdlgSineModulate::~CdlgSineModulate()
{
}

void CdlgSineModulate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_WAVELENGTH, m_lumda);
	DDX_Text(pDX, IDC_EDIT_MODULATION_DEPTH, m_modDepth);
	DDX_Text(pDX, IDC_EDIT_XMIN, m_xmin);
	DDX_Text(pDX, IDC_EDIT_XMAX, m_xmax);
}


BEGIN_MESSAGE_MAP(CdlgSineModulate, CDialog)
END_MESSAGE_MAP()


// CdlgSineModulate message handlers
