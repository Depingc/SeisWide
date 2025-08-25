// dlgTraceDub.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgTraceDub.h"


// CdlgTraceDub dialog

IMPLEMENT_DYNAMIC(CdlgTraceDub, CDialog)

CdlgTraceDub::CdlgTraceDub(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgTraceDub::IDD, pParent)
	, m_fireRate1(0)
	, m_fireRate2(0)
	, m_traceSplit(0)
{

}

CdlgTraceDub::~CdlgTraceDub()
{
}

void CdlgTraceDub::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FIRERATE1, m_fireRate1);
	DDX_Text(pDX, IDC_EDIT_FIRERATE2, m_fireRate2);
	DDX_Text(pDX, IDC_EDIT_TRACESPLIT, m_traceSplit);
}


BEGIN_MESSAGE_MAP(CdlgTraceDub, CDialog)
END_MESSAGE_MAP()


// CdlgTraceDub message handlers
