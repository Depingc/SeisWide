// dlgDesimate.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgDesimate.h"


// CdlgDesimate dialog

IMPLEMENT_DYNAMIC(CdlgDesimate, CDialog)

CdlgDesimate::CdlgDesimate(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgDesimate::IDD, pParent)
	, m_skip(0)
	, m_decimate(0)
{

}

CdlgDesimate::~CdlgDesimate()
{
}

void CdlgDesimate::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SKIP, m_skip);
	DDV_MinMaxInt(pDX, m_skip, 0, 99999);
	DDX_Text(pDX, IDC_EDIT_DECIMATE, m_decimate);
	DDV_MinMaxInt(pDX, m_decimate, 0, 99999);
}


BEGIN_MESSAGE_MAP(CdlgDesimate, CDialog)
END_MESSAGE_MAP()


// CdlgDesimate message handlers
