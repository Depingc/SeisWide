// dlgSourcePosBulk.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgSourcePosBulk.h"
#include "afxdialogex.h"


// CdlgSourcePosBulk dialog

IMPLEMENT_DYNAMIC(CdlgSourcePosBulk, CDialog)

CdlgSourcePosBulk::CdlgSourcePosBulk(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DLG_SOURCEPOS_BULK, pParent)
	, m_xShot(0)
	, m_XsMinusOffset(FALSE)
{

}

CdlgSourcePosBulk::~CdlgSourcePosBulk()
{
}

void CdlgSourcePosBulk::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_XSHOT, m_xShot);
	DDX_Radio(pDX, IDC_RADIO_SXSY_OVERWRITE, m_XsMinusOffset);
}


BEGIN_MESSAGE_MAP(CdlgSourcePosBulk, CDialog)
END_MESSAGE_MAP()


// CdlgSourcePosBulk message handlers
