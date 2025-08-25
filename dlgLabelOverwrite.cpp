// dlgLabelOverwrite.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgLabelOverwrite.h"


// CdlgLabelOverwrite dialog

IMPLEMENT_DYNAMIC(CdlgLabelOverwrite, CDialog)

CdlgLabelOverwrite::CdlgLabelOverwrite(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgLabelOverwrite::IDD, pParent)
	, m_xLabelNew(_T(""))
	, m_yLabelNew(_T(""))
{

}

CdlgLabelOverwrite::~CdlgLabelOverwrite()
{
}

void CdlgLabelOverwrite::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LABEL, m_xLabelNew);
	DDX_Text(pDX, IDC_EDIT_YLABEL, m_yLabelNew);
}


BEGIN_MESSAGE_MAP(CdlgLabelOverwrite, CDialog)
END_MESSAGE_MAP()


// CdlgLabelOverwrite message handlers
