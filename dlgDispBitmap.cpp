// dlgDispBitmap.cpp : implementation file
//

#include "stdafx.h"
#include "seiswide.h"
// #include "dlgImportbitmap.h"
#include "dlgDispBitmap.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgDispBitmap dialog


CdlgDispBitmap::CdlgDispBitmap(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgDispBitmap::IDD, pParent)
	, m_offset(FALSE)
	, m_isFaceUp(FALSE)
	, m_travelTime(FALSE)
	, m_isSwapX(FALSE)
	, m_isSwapYLabel(FALSE)
	, m_vred(0)
{
	//{{AFX_DATA_INIT(CdlgDispBitmap)
	m_yMax = 0.0f;
	m_yMin = 0.0f;
	m_xMax = 0.0f;
	m_xMin = 0.0f;
	//}}AFX_DATA_INIT
}


void CdlgDispBitmap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgDispBitmap)
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMAX, m_yMax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMIN, m_yMin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMAX, m_xMax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_XMIN, m_xMin);
	//}}AFX_DATA_MAP
	DDX_Radio(pDX, IDC_VIEW_OFFSET, m_offset);
	DDX_Radio(pDX, IDC_VIEW_FACEUP, m_isFaceUp);
	DDX_Check(pDX, IDC_TRAVELTIME4, m_travelTime);
	DDX_Check(pDX, IDC_IS_SWAPX, m_isSwapX);
	DDX_Check(pDX, IDC_IS_SWAPYLABEL, m_isSwapYLabel);
	DDX_Text(pDX, IDC_VIEW_UPDATE_VRED, m_vred);
}


BEGIN_MESSAGE_MAP(CdlgDispBitmap, CDialog)
	//{{AFX_MSG_MAP(CdlgDispBitmap)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgDispBitmap message handlers

