// dlgBitmap2sgy.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgBitmap2sgy.h"
#include "afxdialogex.h"


// CdlgBitmap2sgy dialog

IMPLEMENT_DYNAMIC(CdlgBitmap2sgy, CDialog)

CdlgBitmap2sgy::CdlgBitmap2sgy(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgBitmap2sgy::IDD, pParent)
	, m_tmin(0)
	, m_tmax(0)
//	, m_isNotShowAgain(FALSE)
, m_isNotShowAgain(FALSE)
{

}

CdlgBitmap2sgy::~CdlgBitmap2sgy()
{
}

void CdlgBitmap2sgy::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_TMIN, m_tmin);
	DDX_Text(pDX, IDC_EDIT_TMAX, m_tmax);
	//	DDX_Check(pDX, IDC_CHECK_NOSHOW_AGAIN, m_isNotShowAgain);
	DDX_Check(pDX, IDC_CHECK_NOSHOW_AGAIN, m_isNotShowAgain);
}


BEGIN_MESSAGE_MAP(CdlgBitmap2sgy, CDialog)
END_MESSAGE_MAP()


// CdlgBitmap2sgy message handlers
