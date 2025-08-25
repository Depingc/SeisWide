// dlgSegyColor.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgSegyColor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgSegyColor dialog


CdlgSegyColor::CdlgSegyColor(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgSegyColor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgSegyColor)
	m_maxAmpG = 0;
	m_negAmpG = 0;
	m_maxAmpB = 0;
	m_maxAmpR = 0;
	m_negAmpB = 0;
	m_negAmpR = 0;
	//}}AFX_DATA_INIT
}


void CdlgSegyColor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgSegyColor)
	DDX_Control(pDX, IDC_BUTTON1, m_button1);
	DDX_Text(pDX, IDC_COL_MAXAMP_G, m_maxAmpG);
	DDV_MinMaxInt(pDX, m_maxAmpG, 0, 255);
	DDX_Text(pDX, IDC_COL_NEGAMP_G, m_negAmpG);
	DDV_MinMaxInt(pDX, m_negAmpG, 0, 255);
	DDX_Text(pDX, IDC_COL_MAXAMP_B, m_maxAmpB);
	DDV_MinMaxInt(pDX, m_maxAmpB, 0, 255);
	DDX_Text(pDX, IDC_COL_MAXAMP_R, m_maxAmpR);
	DDV_MinMaxInt(pDX, m_maxAmpR, 0, 255);
	DDX_Text(pDX, IDC_COL_NEGAMP_B, m_negAmpB);
	DDV_MinMaxInt(pDX, m_negAmpB, 0, 255);
	DDX_Text(pDX, IDC_COL_NEGAMP_R, m_negAmpR);
	DDV_MinMaxInt(pDX, m_negAmpR, 0, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgSegyColor, CDialog)
	//{{AFX_MSG_MAP(CdlgSegyColor)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnButton2)
	ON_WM_DRAWITEM()
	ON_EN_KILLFOCUS(IDC_COL_MAXAMP_R, OnKillfocusColMaxampR)
	ON_EN_KILLFOCUS(IDC_COL_MAXAMP_G, OnKillfocusColMaxampG)
	ON_EN_KILLFOCUS(IDC_COL_MAXAMP_B, OnKillfocusColMaxampB)
	ON_EN_KILLFOCUS(IDC_COL_NEGAMP_B, OnKillfocusColNegampB)
	ON_EN_KILLFOCUS(IDC_COL_NEGAMP_G, OnKillfocusColNegampG)
	ON_EN_KILLFOCUS(IDC_COL_NEGAMP_R, OnKillfocusColNegampR)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_NEWBW, &CdlgSegyColor::OnBnClickedButtonNewbw)
	ON_BN_CLICKED(IDC_BUTTON_NEWCOLOR, &CdlgSegyColor::OnBnClickedButtonNewcolor)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgSegyColor message handlers


void CdlgSegyColor::OnButton1() 
{
	CColorDialog dlgColor(RGB(m_maxAmpR, m_maxAmpG, m_maxAmpB));
//	dlgColor.SetCurrentColor(RGB(m_maxAmpR, m_maxAmpG, m_maxAmpB));
	if (dlgColor.DoModal() == IDOK)
	{
		m_maxAmpR = GetRValue(dlgColor.GetColor());
		m_maxAmpG = GetGValue(dlgColor.GetColor());
		m_maxAmpB = GetBValue(dlgColor.GetColor());
	//	CColorButton* button1 = (CColorButton*)GetDlgItem(IDC_BUTTON1);
	//	button1->m_clrBkgnd = dlgColor.GetColor();

		UpdateData(FALSE);
		Invalidate();
	}
	
}

void CdlgSegyColor::OnButton2() 
{
	CColorDialog dlgColor(RGB(m_negAmpR, m_negAmpG, m_negAmpB));
//	dlgColor.SetCurrentColor(RGB(m_negAmpR, m_negAmpR, m_negAmpR));
	if (dlgColor.DoModal() == IDOK)
	{
		m_negAmpR = GetRValue(dlgColor.GetColor());
		m_negAmpG = GetGValue(dlgColor.GetColor());
		m_negAmpB = GetBValue(dlgColor.GetColor());
		UpdateData(FALSE);
		Invalidate();
	}
	
}


void CdlgSegyColor::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if(nIDCtl == IDC_BUTTON_POS)
	{
		CRect rect(lpDrawItemStruct->rcItem);
		int delta = 0;
		// (int)(rect.Width( )*0.01);
		CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		CBrush *oldBrush;
		CBrush brush(RGB(m_maxAmpR,m_maxAmpG,m_maxAmpB));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left+delta, rect.top+delta,rect.right-delta, rect.bottom-delta);
	//	pDC->Draw3dRect( &rect, RGB(25,25,25), RGB(m_maxAmpR,m_maxAmpG,m_maxAmpB));
	//	pDC->DrawFrameControl(&rect, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_PUSHED|DFCS_BUTTON3STATE   );

		pDC->SelectObject(oldBrush);

		InvalidateRect(&rect);
	//	lpDrawItemStruct->itemState = ODS_FOCUS|ODS_DEFAULT;
		return;
//	UpdateWindow();
	}
	if(nIDCtl == IDC_BUTTON_NEG)
	{
		CRect rect(lpDrawItemStruct->rcItem);
		int delta = 0;
		// (int)(rect.Width( )*0.01);
		CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		CBrush *oldBrush;
		CBrush brush(RGB(m_negAmpR,m_negAmpG,m_negAmpB));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left+delta, rect.top+delta,rect.right-delta, rect.bottom-delta);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
	//	lpDrawItemStruct->itemState = ODS_FOCUS|ODS_DEFAULT;
		return;
//	UpdateWindow();
	}
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CdlgSegyColor::OnKillfocusColMaxampR() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}
void CdlgSegyColor::OnKillfocusColMaxampG() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
	
}

void CdlgSegyColor::OnKillfocusColMaxampB() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
	
}

void CdlgSegyColor::OnKillfocusColNegampB() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
	
}

void CdlgSegyColor::OnKillfocusColNegampG() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
	
}

void CdlgSegyColor::OnKillfocusColNegampR() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
	
}

void CdlgSegyColor::OnBnClickedButtonNewbw()
{
		m_maxAmpR = 0;
		m_maxAmpG = 0;
		m_maxAmpB = 0;
		m_negAmpR = 230;
		m_negAmpG = 230;
		m_negAmpB = 230;
		UpdateData(FALSE);
		Invalidate();
}

void CdlgSegyColor::OnBnClickedButtonNewcolor()
{
		m_maxAmpR = 255;
		m_maxAmpG = 0;
		m_maxAmpB = 0;
		m_negAmpR = 0;
		m_negAmpG = 0;
		m_negAmpB = 255;
		UpdateData(FALSE);
		Invalidate();
}
