// dlgTwoValues.cpp : implementation file
//
#include "stdafx.h"
#include "SeisWide.h"
#include "dlgTwoValues.h"
#include "afxdialogex.h"


// CdlgTwoValues dialog

IMPLEMENT_DYNAMIC(CdlgTwoValues, CDialog)

CdlgTwoValues::CdlgTwoValues(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgTwoValues::IDD, pParent)
	, m_value1(0)
	, m_value2(0)
{

}

CdlgTwoValues::~CdlgTwoValues()
{
}

void CdlgTwoValues::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_VALUE1, m_value1);
	DDX_Text(pDX, IDC_EDIT_VALUE2, m_value2);
}


BEGIN_MESSAGE_MAP(CdlgTwoValues, CDialog)
END_MESSAGE_MAP()


// CdlgTwoValues message handlers
