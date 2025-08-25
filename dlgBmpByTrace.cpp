// dlgBmpByTrace.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgBmpByTrace.h"
#include "afxdialogex.h"
#include "dlgImportbitmap.h"

// CdlgBmpByTrace dialog

IMPLEMENT_DYNAMIC(CdlgBmpByTrace, CDialog)

CdlgBmpByTrace::CdlgBmpByTrace(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgBmpByTrace::IDD, pParent)
	, m_isSwapX(FALSE)
{
	m_shot1 = 0;
	m_shot2 = 0;
	m_tmax = 0.0f;
	m_tmin = 0.0f;

}

CdlgBmpByTrace::~CdlgBmpByTrace()
{
}

void CdlgBmpByTrace::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_VIEW_UPDATE_SHOT1, m_shot1);
	DDX_Text(pDX, IDC_VIEW_UPDATE_SHOT2, m_shot2);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMAX, m_tmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMIN, m_tmin);
	DDX_Check(pDX, IDC_IS_SWAPX, m_isSwapX);
}


BEGIN_MESSAGE_MAP(CdlgBmpByTrace, CDialog)
END_MESSAGE_MAP()


// CdlgBmpByTrace message handlers


