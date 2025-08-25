// dlgLatShift.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgLatShift.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgLatShift dialog


CdlgLatShift::CdlgLatShift(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgLatShift::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgLatShift)
	m_refName = _T("");
	m_refKm = 0.0;
	m_refSec = 0.0;
	m_offset = 1;
	m_shot1 = 0;
	m_shot2 = 0;
	//}}AFX_DATA_INIT
}


void CdlgLatShift::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgLatShift)
	DDX_Text(pDX, IDC_REFNAME, m_refName);
	DDX_Radio(pDX, IDC_RADIO_OFFSET, m_offset);
	DDX_Text(pDX, IDC_REFKM, m_refKm);
	DDX_Text(pDX, IDC_VIEW_UPDATE_SHOT1, m_shot1);
	DDX_Text(pDX, IDC_VIEW_UPDATE_SHOT2, m_shot2);
	DDX_Text(pDX, IDC_REFSEC, m_refSec);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgLatShift, CDialog)
	//{{AFX_MSG_MAP(CdlgLatShift)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgLatShift message handlers
