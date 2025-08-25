// dlgOffset1Trc.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgOffset1Trc.h"
#include "afxdialogex.h"


// CdlgOffset1Trc dialog

IMPLEMENT_DYNAMIC(CdlgOffset1Trc, CDialog)

CdlgOffset1Trc::CdlgOffset1Trc(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgOffset1Trc::IDD, pParent)
	, m_trace(0)
	, m_offset(0)
{

}

CdlgOffset1Trc::~CdlgOffset1Trc()
{
}

void CdlgOffset1Trc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TRACE1, m_trace);
	DDX_Text(pDX, IDC_EDIT_OFFSET, m_offset);
}


BEGIN_MESSAGE_MAP(CdlgOffset1Trc, CDialog)
END_MESSAGE_MAP()


// CdlgOffset1Trc message handlers
