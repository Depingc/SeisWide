// dlgAsciiHead.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgAsciiHead.h"
#include "afxdialogex.h"


// CdlgAsciiHead dialog

IMPLEMENT_DYNAMIC(CdlgAsciiHead, CDialog)

CdlgAsciiHead::CdlgAsciiHead(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgAsciiHead::IDD, pParent)
	, m_text(_T(""))
{

}

CdlgAsciiHead::~CdlgAsciiHead()
{
}

void CdlgAsciiHead::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TEXT_MULTILINE, m_text);
}


BEGIN_MESSAGE_MAP(CdlgAsciiHead, CDialog)
END_MESSAGE_MAP()


// CdlgAsciiHead message handlers
