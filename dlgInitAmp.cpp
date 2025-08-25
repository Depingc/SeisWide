// dlgInitAmp.cpp : implementation file
//

#include "stdafx.h"
#include "seiswide.h"
#include "dlgInitAmp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgInitAmp dialog


CdlgInitAmp::CdlgInitAmp(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgInitAmp::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgInitAmp)
	m_ampMax = 0.0f;
	//}}AFX_DATA_INIT
}


void CdlgInitAmp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgInitAmp)
	DDX_Text(pDX, IDC_EDIT1, m_ampMax);
	DDV_MinMaxFloat(pDX, m_ampMax, 1.e-005f, 1.e+009f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgInitAmp, CDialog)
	//{{AFX_MSG_MAP(CdlgInitAmp)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgInitAmp message handlers
