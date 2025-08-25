// dlgLabelSizes.cpp : implementation file
//

#include "stdafx.h"
#include "seiswide.h"
#include "dlgLabelSizes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgLabelSizes dialog


CdlgLabelSizes::CdlgLabelSizes(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgLabelSizes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgLabelSizes)
	m_LabelNumberHeigth = 0;
	m_LabelNumberWidth = 0;
	m_LabelSize = 0;
	//}}AFX_DATA_INIT
}


void CdlgLabelSizes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgLabelSizes)
	DDX_Text(pDX, IDC_EDIT_LABEL_NUMHEIGTH, m_LabelNumberHeigth);
	DDV_MinMaxFloat(pDX, m_LabelNumberHeigth, 1, 99999);
	DDX_Text(pDX, IDC_EDIT_LABEL_NUMWIDTH, m_LabelNumberWidth);
	DDV_MinMaxFloat(pDX, m_LabelNumberWidth, 1, 99999);
	DDX_Text(pDX, IDC_EDIT_LABEL_SIZE, m_LabelSize);
	DDV_MinMaxFloat(pDX, m_LabelSize, 1, 99999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgLabelSizes, CDialog)
	//{{AFX_MSG_MAP(CdlgLabelSizes)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgLabelSizes message handlers
