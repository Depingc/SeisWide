// dlgDigSelectCurrent.cpp : implementation file
//

#include "stdafx.h"
#include "seiswide.h"
#include "dlgDigSelectCurrent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgDigSelectCurrent dialog


CdlgDigSelectCurrent::CdlgDigSelectCurrent(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgDigSelectCurrent::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgDigSelectCurrent)
	m_selectCurrent = 0;
	m_total = 0;
	//}}AFX_DATA_INIT
}


void CdlgDigSelectCurrent::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgDigSelectCurrent)
	DDX_Text(pDX, IDC_EDIT_DIG_SELECTSEGMENT, m_selectCurrent);
	DDV_MinMaxInt(pDX, m_selectCurrent, 0, 99999);
	DDX_Text(pDX, IDC_EDIT_DIG_TOTALSEGMENTS, m_total);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgDigSelectCurrent, CDialog)
	//{{AFX_MSG_MAP(CdlgDigSelectCurrent)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgDigSelectCurrent message handlers
