// dlgMagmod.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgMagmod.h"
#include "afxdialogex.h"


// CdlgMagmod dialog

IMPLEMENT_DYNAMIC(CdlgMagmod, CDialog)

CdlgMagmod::CdlgMagmod(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgMagmod::IDD, pParent)
	, m_suscept(0)
	, m_anginc(0)
	, m_angstr(0)
{

}

CdlgMagmod::~CdlgMagmod()
{
}

void CdlgMagmod::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SUSCEPT, m_suscept);
	DDV_MinMaxFloat(pDX, m_suscept, 0.000001f, 9.f);
	DDX_Text(pDX, IDC_EDIT_ANGINC, m_anginc);
	DDX_Text(pDX, IDC_EDIT_ANGSTR, m_angstr);
}


BEGIN_MESSAGE_MAP(CdlgMagmod, CDialog)
END_MESSAGE_MAP()


// CdlgMagmod message handlers
