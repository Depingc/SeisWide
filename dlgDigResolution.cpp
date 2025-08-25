// dlgDigResolution.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgDigResolution.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgDigResolution dialog


CdlgDigResolution::CdlgDigResolution(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgDigResolution::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgDigResolution)
	m_digResolution = 0.0f;
	m_nCurve = 0;
	m_ivray = 0;
	//}}AFX_DATA_INIT
}


void CdlgDigResolution::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgDigResolution)
	DDX_Text(pDX, IDC_DIG_RESOLUTION, m_digResolution);
	DDV_MinMaxFloat(pDX, m_digResolution, 0.f, 10000.f);
	DDX_Text(pDX, IDC_DIG_NCURVE, m_nCurve);
	DDX_Text(pDX, IDC_DIG_IVRAY, m_ivray);
	DDV_MinMaxInt(pDX, m_ivray, 0, 99999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgDigResolution, CDialog)
	//{{AFX_MSG_MAP(CdlgDigResolution)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgDigResolution message handlers
