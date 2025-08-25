// dlgTaupInv.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgTaupInv.h"
#include "afxdialogex.h"


// CdlgTaupInv dialog

IMPLEMENT_DYNAMIC(CdlgTaupInv, CDialog)

CdlgTaupInv::CdlgTaupInv(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgTaupInv::IDD, pParent)
	, m_timeSecName(_T(""))
	, m_taupName(_T(""))
	, m_FiltTaupName(_T(""))
	, m_npoints(0)
{

}

CdlgTaupInv::~CdlgTaupInv()
{
}

void CdlgTaupInv::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_2NDSECNAME, m_taupName);
	DDX_Text(pDX, IDC_2NDSECNAME2, m_FiltTaupName);
	DDX_Text(pDX, IDC_EDIT_NP, m_npoints);
	DDV_MinMaxInt(pDX, m_npoints, 10, 1000);
}


BEGIN_MESSAGE_MAP(CdlgTaupInv, CDialog)
END_MESSAGE_MAP()


// CdlgTaupInv message handlers
