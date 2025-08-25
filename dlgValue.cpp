// dlgValue.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgValue.h"
#include "afxdialogex.h"


// CdlgValue dialog

IMPLEMENT_DYNAMIC(CdlgValue, CDialog)

CdlgValue::CdlgValue(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgValue::IDD, pParent)
	, m_fvalue(0)
{

}

CdlgValue::~CdlgValue()
{
}

void CdlgValue::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VALUE, m_fvalue);
}


BEGIN_MESSAGE_MAP(CdlgValue, CDialog)
END_MESSAGE_MAP()


// CdlgValue message handlers
