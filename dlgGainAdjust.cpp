// dlgGainAdjust.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgGainAdjust.h"


// CdlgGainAdjust dialog

IMPLEMENT_DYNAMIC(CdlgGainAdjust, CDialog)

CdlgGainAdjust::CdlgGainAdjust(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgGainAdjust::IDD, pParent)
	, m_multiply(0)
	, m_trace1(0)
	, m_trace2(0)
{

}

CdlgGainAdjust::~CdlgGainAdjust()
{
}

void CdlgGainAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MULTIPLY, m_multiply);
	DDX_Text(pDX, IDC_VIEW_UPDATE_SHOT1, m_trace1);
	DDX_Text(pDX, IDC_VIEW_UPDATE_SHOT2, m_trace2);
}


BEGIN_MESSAGE_MAP(CdlgGainAdjust, CDialog)
END_MESSAGE_MAP()


// CdlgGainAdjust message handlers
