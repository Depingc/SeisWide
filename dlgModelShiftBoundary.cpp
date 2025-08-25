// dlgModelShiftBoundary.cpp : implementation file
//

#include "stdafx.h"
#include "seiswide.h"
#include "dlgModelShiftBoundary.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgModelShiftBoundary dialog


CdlgModelShiftBoundary::CdlgModelShiftBoundary(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgModelShiftBoundary::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgModelShiftBoundary)
	m_xmax = 0.0f;
	m_xmin = 0.0f;
	m_zinc = 0.0f;
	//}}AFX_DATA_INIT
}


void CdlgModelShiftBoundary::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgModelShiftBoundary)
	DDX_Text(pDX, IDC_MODEL_XMAX, m_xmax);
	DDX_Text(pDX, IDC_MODEL_XMIN, m_xmin);
	DDX_Text(pDX, IDC_MODEL_ZINC, m_zinc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgModelShiftBoundary, CDialog)
	//{{AFX_MSG_MAP(CdlgModelShiftBoundary)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgModelShiftBoundary message handlers
