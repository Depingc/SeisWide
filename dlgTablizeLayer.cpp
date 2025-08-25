// dlgTablizeLayer.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgTablizeLayer.h"
#include "SeisWideDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgTablizeLayer dialog


CdlgTablizeLayer::CdlgTablizeLayer(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgTablizeLayer::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgTablizeLayer)
	m_x1 = 0.0f;
	m_x2 = 0.0f;
	m_y1 = 0.0f;
	m_y2 = 0.0f;
	m_mode = 1;
	m_top = 0;
	m_layerNum = 0;
	//}}AFX_DATA_INIT

}


void CdlgTablizeLayer::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgTablizeLayer)
	DDX_Text(pDX, IDC_EDIT_X1, m_x1);
	DDX_Text(pDX, IDC_EDIT_X2, m_x2);
	DDX_Text(pDX, IDC_EDIT_Y1, m_y1);
	DDX_Text(pDX, IDC_EDIT_Y2, m_y2);
	DDX_Text(pDX, IDC_EDIT_LAYERNO, m_layerNum);
	DDV_MinMaxByte(pDX, m_layerNum, 0, 50);

	DDX_Radio(pDX, IDC_RADIO1, m_mode);
	DDX_Radio(pDX, IDC_RADIO3, m_top);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgTablizeLayer, CDialog)
	//{{AFX_MSG_MAP(CdlgTablizeLayer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgTablizeLayer message handlers
