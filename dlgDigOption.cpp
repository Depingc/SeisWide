// dlgDigOption.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgDigOption.h"
//#include "afxdialogex.h"


// dlgDigOption dialog

IMPLEMENT_DYNAMIC(dlgDigOption, CDialog)

dlgDigOption::dlgDigOption(CWnd* pParent /*=NULL*/)
	: CDialog(dlgDigOption::IDD, pParent)
	, m_nCurve(0)
	, m_label(_T(""))
	, m_isSort(FALSE)
	, m_isSortY(FALSE)
	, m_NotShow(FALSE)
{

}

dlgDigOption::~dlgDigOption()
{
}

void dlgDigOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DIG_NCURVE, m_nCurve);
	DDX_Text(pDX, IDC_DIG_LABEL, m_label);
	DDX_Check(pDX, IDC_CHECK_ISSORT, m_isSort);
	DDX_Check(pDX, IDC_CHECK_ISSORTY, m_isSortY);
	DDX_Check(pDX, IDC_CHECK1, m_NotShow);
}


BEGIN_MESSAGE_MAP(dlgDigOption, CDialog)
END_MESSAGE_MAP()


// dlgDigOption message handlers
