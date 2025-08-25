// dlgDistanceAdjust.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgDistanceAdjust.h"


// CdlgDistanceAdjust dialog

IMPLEMENT_DYNAMIC(CdlgDistanceAdjust, CDialog)

CdlgDistanceAdjust::CdlgDistanceAdjust(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgDistanceAdjust::IDD, pParent)
	, m_timeSecName(_T(""))
	, x0(0)
	, m_factor(0)
	, m_trace1(0)
	, m_trace2(0)
{

}

CdlgDistanceAdjust::~CdlgDistanceAdjust()
{
}

void CdlgDistanceAdjust::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_REFNAME, m_timeSecName);
	DDX_Text(pDX, IDC_DISTANCE0, x0);
	DDX_Text(pDX, IDC_FACTOR, m_factor);
	DDX_Text(pDX, IDC_VIEW_UPDATE_SHOT1, m_trace1);
	DDX_Text(pDX, IDC_VIEW_UPDATE_SHOT2, m_trace2);
}


BEGIN_MESSAGE_MAP(CdlgDistanceAdjust, CDialog)
END_MESSAGE_MAP()


// CdlgDistanceAdjust message handlers
