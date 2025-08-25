// BoundNodes.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "BoundNodes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBoundNodes dialog


CBoundNodes::CBoundNodes(CWnd* pParent /*=NULL*/)
	: CDialog(CBoundNodes::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBoundNodes)
	m_layerFocus = 0;
	m_xOld = 0.0;
	m_yOld = 0.0;
	m_xNew = 0.0;
	m_yNew = 0.0;		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBoundNodes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBoundNodes)
	DDX_Text(pDX, IDC_MOUSE_DOWN_X, m_xOld);
	DDX_Text(pDX, IDC_MOUSE_DOWNY, m_yOld);
	DDX_Text(pDX, IDC_MOUSE_UPX, m_xNew);
	DDX_Text(pDX, IDC_MOUSE_UPY, m_yNew);
	DDX_Text(pDX, IDC_BOUND_NUM, m_layerFocus);
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBoundNodes, CDialog)
	//{{AFX_MSG_MAP(CBoundNodes)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBoundNodes message handlers
