// dlgPoisson.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgPoisson.h"
#include "afxdialogex.h"


// dlgPoisson dialog

IMPLEMENT_DYNAMIC(dlgPoisson, CDialogEx)

dlgPoisson::dlgPoisson(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DLG_POISSON, pParent)
	, m_poisson(_T(""))
{

}

dlgPoisson::~dlgPoisson()
{
}

void dlgPoisson::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_LATLONG_MULTI, m_poisson);
}


BEGIN_MESSAGE_MAP(dlgPoisson, CDialogEx)
	ON_BN_CLICKED(IDOK, &dlgPoisson::OnBnClickedOk)
END_MESSAGE_MAP()


// dlgPoisson message handlers


void dlgPoisson::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here


	CDialogEx::OnOK();
}
