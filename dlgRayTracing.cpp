// dlgRayTracing.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgRayTracing.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgRayTracing dialog


CdlgRayTracing::CdlgRayTracing(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgRayTracing::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgRayTracing)
	m_rayTracing = _T("");
	m_velName = _T("");
	//}}AFX_DATA_INIT
}


void CdlgRayTracing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgRayTracing)
	DDX_Text(pDX, IDC_DLG_RAYTRACING, m_rayTracing);
	DDX_Text(pDX, IDC_DLG_VELNAME, m_velName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgRayTracing, CDialog)
	//{{AFX_MSG_MAP(CdlgRayTracing)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgRayTracing message handlers
