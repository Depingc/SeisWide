// dlgVELOCITY1.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgVELOCITY1.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgVELOCITY1 property page

IMPLEMENT_DYNCREATE(CdlgVELOCITY1, CPropertyPage)

CdlgVELOCITY1::CdlgVELOCITY1() : CPropertyPage(CdlgVELOCITY1::IDD)
, m_vrmsTauName(BLANK)
, m_colBarVmin(1.45f)
, m_colBarVmax(9)
, m_xWell(0)
, m_velModExtras(BLANK)
{
	//{{AFX_DATA_INIT(CdlgVELOCITY1)
	m_vmax = 0.0;
	m_vmaxB = 0;
	m_vmaxG = 0;
	m_vmaxR = 0;
	m_vmin = 0.0;
	m_vminB = 0;
	m_vminG = 0;
	m_vminR = 0;
	m_dxV = 0.0;
	m_velSecName = BLANK;
	m_vmidB = 0;
	m_vmidG = 0;
	m_vmidR = 0;
	m_vmid = 0.0;
	m_vmidB2 = 0;
	m_vmidB3 = 0;
	m_vmidB4 = 0;
	m_vmidG2 = 0;
	m_vmidG3 = 0;
	m_vmidG4 = 0;
	m_vmidR2 = 0;
	m_vmidR3 = 0;
	m_vmidR4 = 0;
	m_cxExt = 0.0f;
	m_cxPos = 0.0f;
	m_cyExt = 0.0f;
	m_cyPos = 0.0f;
	m_colBar = FALSE;
	m_vmid2 = 0.0;
	m_vmid3 = 0.0;
	m_vmid4 = 0.0;
	m_dv1 = 0.0f;
	//m_dv2 = 0.0f;
	//m_dv3 = 0.0f;
	//m_dv4 = 0.0f;
	//m_dv5 = 0.0f;
	m_floatName = BLANK;
	//}}AFX_DATA_INIT
}

CdlgVELOCITY1::~CdlgVELOCITY1()
{
}

void CdlgVELOCITY1::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgVELOCITY1)
	DDX_Text(pDX, IDC_VELSECNAME, m_velSecName);
	DDX_Check(pDX, IDC_VEL_COLBAR, m_colBar);
	DDX_Text(pDX, IDC_VEL_CXPOS, m_cxPos);
	DDX_Text(pDX, IDC_VEL_CYPOS, m_cyPos);
	DDX_Text(pDX, IDC_VEL_CYEXT, m_cyExt);
	DDX_Text(pDX, IDC_VEL_VBARMIN, m_colBarVmin);
	DDV_MinMaxFloat(pDX, m_colBarVmin, 0, 10);
	DDX_Text(pDX, IDC_VEL_VBARMAX, m_colBarVmax);
	DDV_MinMaxFloat(pDX, m_colBarVmax, 0, 10);
	DDX_Text(pDX, IDC_CONVERSIONDXV, m_dxV);
	DDV_MinMaxDouble(pDX, m_dxV, 3.e-005, 100.);
	DDX_Text(pDX, IDC_CONVERSION_DV1, m_dv1);
	DDV_MinMaxFloat(pDX, m_dv1, 0.f, 100.f);


	DDX_Text(pDX, IDC_CONVERSION_VMIN, m_vmin);
	DDX_Text(pDX, IDC_CONVERSION_VMID, m_vmid);
	DDX_Text(pDX, IDC_CONVERSION_VMAX, m_vmax);
	DDX_Text(pDX, IDC_CONVERSION_VMAXB, m_vmaxB);
	DDV_MinMaxUInt(pDX, m_vmaxB, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMINB, m_vminB);
	DDV_MinMaxUInt(pDX, m_vminB, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDB, m_vmidB);
	DDV_MinMaxUInt(pDX, m_vmidB, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDB2, m_vmidB2);
	DDV_MinMaxUInt(pDX, m_vmidB2, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDB3, m_vmidB3);
	DDV_MinMaxUInt(pDX, m_vmidB3, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDB4, m_vmidB4);
	DDV_MinMaxUInt(pDX, m_vmidB4, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDB5, m_vmidB5);
	DDV_MinMaxUInt(pDX, m_vmidB5, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDB6, m_vmidB6);
	DDV_MinMaxUInt(pDX, m_vmidB6, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDB7, m_vmidB7);
	DDV_MinMaxUInt(pDX, m_vmidB7, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDB8, m_vmidB8);
	DDV_MinMaxUInt(pDX, m_vmidB8, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDB9, m_vmidB9);
	DDV_MinMaxUInt(pDX, m_vmidB9, 0, 255);

	DDX_Text(pDX, IDC_CONVERSION_VMING, m_vminG);
	DDX_Text(pDX, IDC_CONVERSION_VMIDG, m_vmidG);
	DDV_MinMaxUInt(pDX, m_vmidG, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDG2, m_vmidG2);
	DDV_MinMaxUInt(pDX, m_vmidG2, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDG3, m_vmidG3);
	DDV_MinMaxUInt(pDX, m_vmidG3, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDG4, m_vmidG4);
	DDV_MinMaxUInt(pDX, m_vmidG4, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDG5, m_vmidG5);
	DDV_MinMaxUInt(pDX, m_vmidG5, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDG6, m_vmidG6);
	DDV_MinMaxUInt(pDX, m_vmidG6, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDG7, m_vmidG7);
	DDV_MinMaxUInt(pDX, m_vmidG7, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDG8, m_vmidG8);
	DDV_MinMaxUInt(pDX, m_vmidG8, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDG9, m_vmidG9);
	DDV_MinMaxUInt(pDX, m_vmidG9, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMAXG, m_vmaxG);
	DDV_MinMaxUInt(pDX, m_vmaxG, 0, 255);

	DDX_Text(pDX, IDC_CONVERSION_VMINR, m_vminR);
	DDX_Text(pDX, IDC_CONVERSION_VMIDR, m_vmidR);
	DDV_MinMaxUInt(pDX, m_vmidR, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDR2, m_vmidR2);
	DDV_MinMaxUInt(pDX, m_vmidR2, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDR3, m_vmidR3);
	DDV_MinMaxUInt(pDX, m_vmidR3, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDR4, m_vmidR4);
	DDV_MinMaxUInt(pDX, m_vmidR4, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDR5, m_vmidR5);
	DDV_MinMaxUInt(pDX, m_vmidR5, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDR6, m_vmidR6);
	DDV_MinMaxUInt(pDX, m_vmidR6, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDR7, m_vmidR7);
	DDV_MinMaxUInt(pDX, m_vmidR7, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDR8, m_vmidR8);
	DDV_MinMaxUInt(pDX, m_vmidR8, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMIDR9, m_vmidR9);
	DDV_MinMaxUInt(pDX, m_vmidR9, 0, 255);
	DDX_Text(pDX, IDC_CONVERSION_VMAXR, m_vmaxR);
	DDV_MinMaxUInt(pDX, m_vmaxR, 0, 255);


	DDX_Text(pDX, IDC_VEL_CXEXT, m_cxExt);
	DDV_MinMaxFloat(pDX, m_cxExt, 1.e-005f, 10000.f);
	DDX_Text(pDX, IDC_CONVERSION_VMID2, m_vmid2);
	DDX_Text(pDX, IDC_CONVERSION_VMID3, m_vmid3);
	DDX_Text(pDX, IDC_CONVERSION_VMID4, m_vmid4);
	DDX_Text(pDX, IDC_CONVERSION_VMID5, m_vmid5);
	DDX_Text(pDX, IDC_CONVERSION_VMID6, m_vmid6);
	DDX_Text(pDX, IDC_CONVERSION_VMID7, m_vmid7);
	DDX_Text(pDX, IDC_CONVERSION_VMID8, m_vmid8);
	DDX_Text(pDX, IDC_CONVERSION_VMID9, m_vmid9);

	DDX_Text(pDX, IDC_FLOATNAME, m_floatName);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_VRMSTAUNAME, m_vrmsTauName);

	DDX_Text(pDX, IDC_XSHOT3, m_xWell);
	DDX_Text(pDX, IDC_EDIT_VELMODEX, m_velModExtras);
}


BEGIN_MESSAGE_MAP(CdlgVELOCITY1, CPropertyPage)
	//{{AFX_MSG_MAP(CdlgVELOCITY1)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_BUTTON3, OnButton3)
	ON_BN_CLICKED(IDC_BUTTON4, OnButton4)
	ON_BN_CLICKED(IDC_BUTTON5, OnButton5)
	ON_BN_CLICKED(IDC_BUTTON6, OnButton6)
	ON_BN_CLICKED(IDC_BUTTON7, OnButton7)
	ON_WM_DRAWITEM()

	ON_EN_KILLFOCUS(IDC_CONVERSION_VMINB, OnKillfocusConversionVminb)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDB, OnKillfocusConversionVmidb)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDB2, OnKillfocusConversionVmidb2)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDB3, OnKillfocusConversionVmidb3)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDB4, OnKillfocusConversionVmidb4)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDB5, OnEnKillfocusConversionVmidb5)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDB6, OnEnKillfocusConversionVmidb6)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDB7, OnEnKillfocusConversionVmidb7)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDB8, OnEnKillfocusConversionVmidb8)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMAXB, OnKillfocusConversionVmaxb)

	ON_EN_KILLFOCUS(IDC_CONVERSION_VMING, OnKillfocusConversionVming)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDG, OnKillfocusConversionVmidg)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDG2, OnKillfocusConversionVmidg2)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDG3, OnKillfocusConversionVmidg3)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDG4, OnKillfocusConversionVmidg4)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDG5, OnEnKillfocusConversionVmidg5)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDG6, OnEnKillfocusConversionVmidg6)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDG7, OnEnKillfocusConversionVmidg7)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDG8, OnEnKillfocusConversionVmidg8)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMAXG, OnKillfocusConversionVmaxg)

	ON_EN_KILLFOCUS(IDC_CONVERSION_VMINR, OnKillfocusConversionVminr)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDR, OnKillfocusConversionVmidr)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDR2, OnKillfocusConversionVmidr2)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDR3, OnKillfocusConversionVmidr3)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDR4, OnKillfocusConversionVmidr4)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDR5, OnEnKillfocusConversionVmidr5)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDR6, OnEnKillfocusConversionVmidr6)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDR7, OnEnKillfocusConversionVmidr7)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMIDR8, OnEnKillfocusConversionVmidr8)
	ON_EN_KILLFOCUS(IDC_CONVERSION_VMAXR, OnKillfocusConversionVmaxr)


	ON_BN_CLICKED(IDC_BUTTON_BROWSEV2, OnButtonBrowseFloat)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, &CdlgVELOCITY1::OnBnClickedButtonBrowsev)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEVRMS, &CdlgVELOCITY1::OnBnClickedButtonBrowsevrms)
	ON_BN_CLICKED(IDC_BUTTON_COLPATTERN, &CdlgVELOCITY1::OnBnClickedButtonColpattern)
	ON_BN_CLICKED(IDC_BUTTON_COLPATTERN2, &CdlgVELOCITY1::OnBnClickedButtonColpattern2)
	ON_BN_CLICKED(IDC_BUTTON11, &CdlgVELOCITY1::OnBnClickedButton11)
	ON_BN_CLICKED(IDC_BUTTON21, &CdlgVELOCITY1::OnBnClickedButton21)
	ON_BN_CLICKED(IDC_BUTTON22, &CdlgVELOCITY1::OnBnClickedButton22)
	ON_BN_CLICKED(IDC_BUTTON23, &CdlgVELOCITY1::OnBnClickedButton23)




	ON_BN_CLICKED(IDC_BUTTON_COLPATTERN3, &CdlgVELOCITY1::OnBnClickedButtonColpattern3)
	ON_BN_CLICKED(IDC_BUTTON_COLPATTERN4, &CdlgVELOCITY1::OnBnClickedButtonColpattern4)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CdlgVELOCITY1::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_LOAD, &CdlgVELOCITY1::OnBnClickedButtonLoad)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgVELOCITY1 message handlers

void CdlgVELOCITY1::setColorArray(int *colR, int *colG, int *colB) 
{
	this->m_vminR = colR[0];
	this->m_vmidR = colR[1];
	this->m_vmidR2=	colR[2];
	this->m_vmidR3=	colR[3];
	this->m_vmidR4=	colR[4];
	this->m_vmidR5=	colR[5];
	this->m_vmidR6=	colR[6];
	this->m_vmidR7=	colR[7];
	this->m_vmidR8=	colR[8];
	this->m_vmidR9 = colR[9];
	this->m_vmaxR = colR[10];

	this->m_vminG = colG[0];
	this->m_vmidG = colG[1];
	this->m_vmidG2=	colG[2];
	this->m_vmidG3=	colG[3];
	this->m_vmidG4=	colG[4];
	this->m_vmidG5=	colG[5];
	this->m_vmidG6=	colG[6];
	this->m_vmidG7=	colG[7];
	this->m_vmidG8=	colG[8];
	this->m_vmidG9 = colG[9];
	this->m_vmaxG = colG[10];

	this->m_vminB = colB[0];
	this->m_vmidB = colB[1];
	this->m_vmidB2=	colB[2];
	this->m_vmidB3=	colB[3];
	this->m_vmidB4=	colB[4];
	this->m_vmidB5=	colB[5];
	this->m_vmidB6=	colB[6];
	this->m_vmidB7=	colB[7];
	this->m_vmidB8=	colB[8];
	this->m_vmidB9 = colB[9];
	this->m_vmaxB = colB[10];
}

BOOL CdlgVELOCITY1::getColorArray(int iColorPattern)
{
	// note:  NUMCOLOR=11

	if (iColorPattern == 0) {
		static int col_r[NUMCOLOR] = {0,  135,  210, 255, 255, 255, 255, 255, 255, 200, 170};
		static int col_g[NUMCOLOR] = {140, 255, 255, 135, 151, 220, 198, 171, 142, 100, 43};
		static int col_b[NUMCOLOR] = {255, 0,   0,   0,   0,   178, 140, 132, 140, 200, 255};
		setColorArray(&col_r[0], &col_g[0], &col_b[0]);
	}
	else if (iColorPattern == 1) {
		static int col_r[NUMCOLOR] = {255, 128, 181, 192, 255, 255, 255, 255, 255, 230, 217};
		static int col_g[NUMCOLOR] = {255, 128, 255, 245, 235, 220, 198, 171, 142, 160, 179};
		static int col_b[NUMCOLOR] = {0,    0,  0,   103, 205, 178, 140, 132, 140, 161, 255};
		setColorArray(&col_r[0], &col_g[0], &col_b[0]);
	}
	else if (iColorPattern == 2) {
		static int col_r[NUMCOLOR] = {128, 128, 181, 255, 255, 255, 255, 255, 255, 235, 217};
		static int col_g[NUMCOLOR] = {255, 0,   255, 0,   235, 220, 198, 171, 142, 160, 179};
		static int col_b[NUMCOLOR] = {255, 255,   0, 128, 205, 178, 140, 132, 140, 200, 255};
		setColorArray(&col_r[0], &col_g[0], &col_b[0]);
	}
	else if (iColorPattern == 3) {
		// color pattern for ocean-continent transition
		// this was my previous one
		//static int col_r[NUMCOLOR] = {255, 192, 150,  55,   0,   0, 200, 255, 217, 230};
		//static int col_g[NUMCOLOR] = {255, 255, 245, 235, 128, 100, 171,  50, 179, 210};
		//static int col_b[NUMCOLOR] = {  0,   0, 103, 205, 192, 255, 132,  50, 255, 255};

		// this is Thomas's color pallet
		static int col_r[NUMCOLOR] = { 210, 117,   3,  19, 252, 255, 255, 255, 170, 170, 170 };
		static int col_g[NUMCOLOR] = { 220, 200,   6, 255, 255, 151,  71,  30, 150, 80,  17 };
		static int col_b[NUMCOLOR] = { 255, 255, 255,   0,   0,   0,   0,  30, 250, 255, 255 };
		setColorArray(&col_r[0], &col_g[0], &col_b[0]);
	}


	return TRUE;

	//memcpy(ampColorDyn2.r, col_r, sizeof(int)*11);
	//memcpy(ampColorDyn2.g, col_g, sizeof(int)*11);
	//memcpy(ampColorDyn2.b, col_b, sizeof(int)*11);
	// m_ampColorref[i] = RGB(ampColorDyn2.r[i], ampColorDyn2.g[i], ampColorDyn2.b[i]);

}



void CdlgVELOCITY1::OnBnClickedButtonBrowsev()
{
	//new file dialog.
	CFileDialog dlg(TRUE, m_velSecName, _T("*.vxml;*.bin"),
			OFN_HIDEREADONLY|OFN_EXPLORER,
			_T("File name for model (*.vxml)|*.vxml|Older format (*.bin)|*.bin|"));

	if( dlg.DoModal()==IDOK )
	{
		m_velSecName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VELSECNAME));
		pEdit->SetWindowText(m_velSecName);
	}
	//t.ReleaseBuffer();
}



void CdlgVELOCITY1::OnButtonBrowseFloat() 
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("txt"),_T("*.txt"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("SeisWide Digitize Format (*.txt)|*.txt|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("SeisWide Digitize Format |txt |"));
#endif


	int pathLen = m_floatName.ReverseFind( '\\' );
	CString t = m_floatName.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_floatName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_FLOATNAME));
		pEdit->SetWindowText(m_floatName);
	}
	t.ReleaseBuffer();
}


void CdlgVELOCITY1::OnButton1() 
{
	CColorDialog dlgColor(RGB(m_vminR, m_vminG, m_vminB));
	if (dlgColor.DoModal() == IDOK)
	{
		m_vminR = GetRValue(dlgColor.GetColor());
		m_vminG = GetGValue(dlgColor.GetColor());
		m_vminB = GetBValue(dlgColor.GetColor());
		UpdateData(FALSE);
		Invalidate();
	}
	
}

void CdlgVELOCITY1::OnButton3() 
{
	CColorDialog dlgColor(RGB(m_vmidR, m_vmidG, m_vminB));
	if (dlgColor.DoModal() == IDOK)
	{
		m_vmidR = GetRValue(dlgColor.GetColor());
		m_vmidG = GetGValue(dlgColor.GetColor());
		m_vmidB = GetBValue(dlgColor.GetColor());
		UpdateData(FALSE);
		Invalidate();
	}
	
}

void CdlgVELOCITY1::OnButton4() 
{
	CColorDialog dlgColor(RGB(m_vmidR2, m_vmidG2, m_vmidB2));
	if (dlgColor.DoModal() == IDOK)
	{
		m_vmidR2 = GetRValue(dlgColor.GetColor());
		m_vmidG2 = GetGValue(dlgColor.GetColor());
		m_vmidB2 = GetBValue(dlgColor.GetColor());
		UpdateData(FALSE);
		Invalidate();
	}
	
}

void CdlgVELOCITY1::OnButton5() 
{
	CColorDialog dlgColor(RGB(m_vmidR3, m_vmidG3, m_vmidB3));
	if (dlgColor.DoModal() == IDOK)
	{
		m_vmidR3 = GetRValue(dlgColor.GetColor());
		m_vmidG3 = GetGValue(dlgColor.GetColor());
		m_vmidB3 = GetBValue(dlgColor.GetColor());
		UpdateData(FALSE);
		Invalidate();
	}
	
}

void CdlgVELOCITY1::OnButton6() 
{
	CColorDialog dlgColor(RGB(m_vmidR4, m_vmidG4, m_vmidB4));
	if (dlgColor.DoModal() == IDOK)
	{
		m_vmidR4 = GetRValue(dlgColor.GetColor());
		m_vmidG4 = GetGValue(dlgColor.GetColor());
		m_vmidB4 = GetBValue(dlgColor.GetColor());
		UpdateData(FALSE);
		Invalidate();
	}
	
}

void CdlgVELOCITY1::OnButton7() 
{
	CColorDialog dlgColor(RGB(m_vmaxR, m_vmaxG, m_vmaxB));
	if (dlgColor.DoModal() == IDOK)
	{
		m_vmaxR = GetRValue(dlgColor.GetColor());
		m_vmaxG = GetGValue(dlgColor.GetColor());
		m_vmaxB = GetBValue(dlgColor.GetColor());
		UpdateData(FALSE);
		Invalidate();
	}
	
}

void CdlgVELOCITY1::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// TODO: Add your message handler code here and/or call default
	CBrush *oldBrush, brush;
	CRect rect(lpDrawItemStruct->rcItem);
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	switch (nIDCtl) 
	{
	case IDC_BUTTON_MINC:
		brush.CreateSolidBrush(RGB(m_vminR,m_vminG,m_vminB));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left, rect.top,rect.right, rect.bottom);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	case IDC_BUTTON_MIDC:
		brush.CreateSolidBrush(RGB(m_vmidR,m_vmidG,m_vmidB));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left, rect.top,rect.right, rect.bottom);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	case IDC_BUTTON_MIDC2:
		brush.CreateSolidBrush(RGB(m_vmidR2,m_vmidG2,m_vmidB2));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left, rect.top,rect.right, rect.bottom);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	case IDC_BUTTON_MIDC3:
		brush.CreateSolidBrush(RGB(m_vmidR3,m_vmidG3,m_vmidB3));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left, rect.top,rect.right, rect.bottom);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	case IDC_BUTTON_MIDC4:
		brush.CreateSolidBrush(RGB(m_vmidR4,m_vmidG4,m_vmidB4));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left, rect.top,rect.right, rect.bottom);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	case IDC_BUTTON_MIDC5:
		brush.CreateSolidBrush(RGB(m_vmidR5,m_vmidG5,m_vmidB5));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left, rect.top,rect.right, rect.bottom);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	case IDC_BUTTON_MIDC6:
		brush.CreateSolidBrush(RGB(m_vmidR6,m_vmidG6,m_vmidB6));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left, rect.top,rect.right, rect.bottom);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	case IDC_BUTTON_MIDC7:
		brush.CreateSolidBrush(RGB(m_vmidR7,m_vmidG7,m_vmidB7));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left, rect.top,rect.right, rect.bottom);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	case IDC_BUTTON_MIDC8:
		brush.CreateSolidBrush(RGB(m_vmidR8,m_vmidG8,m_vmidB8));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left, rect.top,rect.right, rect.bottom);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;

	case IDC_BUTTON_MIDC9:
		brush.CreateSolidBrush(RGB(m_vmaxR,m_vmaxG,m_vmaxB));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left, rect.top,rect.right, rect.bottom);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	}
//	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
	
	CPropertyPage::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CdlgVELOCITY1::OnKillfocusConversionVminb() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
	
}

void CdlgVELOCITY1::OnKillfocusConversionVming() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVminr() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmaxb() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmaxg() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmaxr() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmidb() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmidb2() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmidb3() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmidb4() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmidg() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmidg2() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmidg3() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmidg4() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmidr() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmidr2() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmidr3() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnKillfocusConversionVmidr4() 
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnBnClickedButtonBrowsevrms()
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("txt"),_T("*.txt"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Vrms-Tau file; Digitize Format (*.txt)|*.txt|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Vrms-Tau file; Digitize Format |txt |"));
#endif


	int pathLen = this->m_vrmsTauName.ReverseFind( '\\' );
	CString t = m_vrmsTauName.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_vrmsTauName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VRMSTAUNAME));
		pEdit->SetWindowText(m_vrmsTauName);
	}
	t.ReleaseBuffer();	
}

void CdlgVELOCITY1::OnBnClickedButtonColpattern()
{
	this->getColorArray(0);
	UpdateData(FALSE); //the data is being updated from the UI to variables in memory.
	Invalidate();  // this will also update the dialog display as well.
}

void CdlgVELOCITY1::OnBnClickedButtonColpattern2()
{
	this->getColorArray(1);
	UpdateData(FALSE); //the data is being updated from the UI to variables in memory.
	Invalidate(); // this will also update the dialog display as well.
}

void CdlgVELOCITY1::OnBnClickedButtonColpattern3()
{
	this->getColorArray(2);
	UpdateData(FALSE); //the data is being updated from the UI to variables in memory.
	Invalidate(); // this will also update the dialog display as well.
}

void CdlgVELOCITY1::OnBnClickedButtonColpattern4()
{
	this->getColorArray(3);
	UpdateData(FALSE); //the data is being updated from the UI to variables in memory.
	Invalidate(); // this will also update the dialog display as well.
}


void CdlgVELOCITY1::OnBnClickedButton11()
{
	CColorDialog dlgColor(RGB(m_vmidR5, m_vmidG5, m_vmidB5));
	if (dlgColor.DoModal() == IDOK)
	{
		m_vmidR5 = GetRValue(dlgColor.GetColor());
		m_vmidG5 = GetGValue(dlgColor.GetColor());
		m_vmidB5 = GetBValue(dlgColor.GetColor());
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgVELOCITY1::OnBnClickedButton21()
{
	CColorDialog dlgColor(RGB(m_vmidR6, m_vmidG6, m_vmidB6));
	if (dlgColor.DoModal() == IDOK)
	{
		m_vmidR6 = GetRValue(dlgColor.GetColor());
		m_vmidG6 = GetGValue(dlgColor.GetColor());
		m_vmidB6 = GetBValue(dlgColor.GetColor());
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgVELOCITY1::OnBnClickedButton22()
{
	CColorDialog dlgColor(RGB(m_vmidR7, m_vmidG7, m_vmidB7));
	if (dlgColor.DoModal() == IDOK)
	{
		m_vmidR7 = GetRValue(dlgColor.GetColor());
		m_vmidG7 = GetGValue(dlgColor.GetColor());
		m_vmidB7 = GetBValue(dlgColor.GetColor());
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgVELOCITY1::OnBnClickedButton23()
{
	CColorDialog dlgColor(RGB(m_vmidR8, m_vmidG8, m_vmidB8));
	if (dlgColor.DoModal() == IDOK)
	{
		m_vmidR8 = GetRValue(dlgColor.GetColor());
		m_vmidG8 = GetGValue(dlgColor.GetColor());
		m_vmidB8 = GetBValue(dlgColor.GetColor());
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgVELOCITY1::OnEnKillfocusConversionVmidr5()
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnEnKillfocusConversionVmidg5()
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnEnKillfocusConversionVmidb5()
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnEnKillfocusConversionVmidr6()
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnEnKillfocusConversionVmidg6()
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnEnKillfocusConversionVmidb6()
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnEnKillfocusConversionVmidr7()
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnEnKillfocusConversionVmidg7()
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnEnKillfocusConversionVmidb7()
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnEnKillfocusConversionVmidr8()
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnEnKillfocusConversionVmidg8()
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}

void CdlgVELOCITY1::OnEnKillfocusConversionVmidb8()
{
	UpdateData(TRUE); //the data is being updated from the existing variables in memory.
	Invalidate();
}





void CdlgVELOCITY1::OnBnClickedButtonSave()
{
	CString tStr;
	CStringArray 	*arr = new CStringArray();
	tStr.Format(_T("#Color file for velocity model."));
	arr->Add(tStr);
	tStr.Format(_T("#Only the first 11 data lines are to be used."));
	arr->Add(tStr);
	tStr.Format(_T("#r    g    b    V(km/s) (You may manually edit these texts.)"));
	arr->Add(tStr);

	tStr.Format(_T("%3d  %3d  %3d %g"), m_vminR, m_vminG, m_vminB, this->m_vmin);
	arr->Add(tStr);
	tStr.Format(_T("%3d  %3d  %3d %g"), m_vmidR, m_vmidG, m_vmidB, this->m_vmid);
	arr->Add(tStr);
	tStr.Format(_T("%3d  %3d  %3d %g"), m_vmidR2, m_vmidG2, m_vmidB2, this->m_vmid2);
	arr->Add(tStr);
	tStr.Format(_T("%3d  %3d  %3d %g"), m_vmidR3, m_vmidG3, m_vmidB3, this->m_vmid3);
	arr->Add(tStr);
	tStr.Format(_T("%3d  %3d  %3d %g"), m_vmidR4, m_vmidG4, m_vmidB4, this->m_vmid4);
	arr->Add(tStr);
	tStr.Format(_T("%3d  %3d  %3d %g"), m_vmidR5, m_vmidG5, m_vmidB5, this->m_vmid5);
	arr->Add(tStr);
	tStr.Format(_T("%3d  %3d  %3d %g"), m_vmidR6, m_vmidG6, m_vmidB6, this->m_vmid6);
	arr->Add(tStr);
	tStr.Format(_T("%3d  %3d  %3d %g"), m_vmidR7, m_vmidG7, m_vmidB7, this->m_vmid7);
	arr->Add(tStr);
	tStr.Format(_T("%3d  %3d  %3d %g"), m_vmidR8, m_vmidG8, m_vmidB8, this->m_vmid8);
	arr->Add(tStr);
	tStr.Format(_T("%3d  %3d  %3d %g"), m_vmidR9, m_vmidG9, m_vmidB9, this->m_vmid9);
	arr->Add(tStr);
	tStr.Format(_T("%3d  %3d  %3d %g"), m_vmaxR, m_vmaxG, m_vmaxB, this->m_vmax);
	arr->Add(tStr);
	showStringArrayToFile(arr);
	arr->RemoveAll();
	delete arr;
}


void CdlgVELOCITY1::OnBnClickedButtonLoad()
{
	// TODO: Add your control notification handler code here
	CString tStr;
	CStringArray 	*arr = NULL;

	CFileDialog dlg(TRUE, _T("txt"), _T("*.txt"),
		OFN_READONLY, _T("Color Pallete File for Velocity Model (*.*)|*.*|"));
	if (dlg.DoModal() == IDOK) {
		tStr = dlg.GetPathName();
		arr = getLinesFromFile(tStr, false);
		if (arr) {
				CStringArray *arr2;
				arr2 = CString2LinesBySpace(arr->ElementAt(0));
				INT_PTR n2 = arr2->GetCount();
				if (arr2->GetCount() >= 4) { // only 4 is the correct number here
					m_vminR = (UINT)_ttoi(arr2->ElementAt(0));
					m_vminG = (UINT)_ttoi(arr2->ElementAt(1));
					m_vminB = (UINT)_ttoi(arr2->ElementAt(2));
					m_vmin = (double)_ttof(arr2->ElementAt(3));
				}
				arr2->RemoveAll();
				delete arr2; //this pointer must be deleted. If not deleted, and this pointer is re-assigned
				// to another one, we have memory exception!!!

				if (n2 < 4) {
					AfxMessageBox(_T("Unknonw format encountered."));
					arr->RemoveAll();
					delete arr;
					return;
				}

				arr2 = CString2LinesBySpace(arr->ElementAt(1));
				if (arr2->GetCount() >= 4) { // only 4 is the correct number here
					m_vmidR = (UINT)_ttoi(arr2->ElementAt(0));
					m_vmidG = (UINT)_ttoi(arr2->ElementAt(1));
					m_vmidB = (UINT)_ttoi(arr2->ElementAt(2));
					m_vmid = (double)_ttof(arr2->ElementAt(3));
				}
				arr2->RemoveAll();
				delete arr2;


				arr2 = CString2LinesBySpace(arr->ElementAt(2));
				if (arr2->GetCount() >= 4) { // only 4 is the correct number here
					m_vmidR2 = (UINT)_ttoi(arr2->ElementAt(0));
					m_vmidG2 = (UINT)_ttoi(arr2->ElementAt(1));
					m_vmidB2 = (UINT)_ttoi(arr2->ElementAt(2));
					m_vmid2 = (double)_ttof(arr2->ElementAt(3));
				}
				arr2->RemoveAll();
				delete arr2;

				arr2 = CString2LinesBySpace(arr->ElementAt(3));
				if (arr2->GetCount() >= 4) { // only 4 is the correct number here
					m_vmidR3 = (UINT)_ttoi(arr2->ElementAt(0));
					m_vmidG3 = (UINT)_ttoi(arr2->ElementAt(1));
					m_vmidB3 = (UINT)_ttoi(arr2->ElementAt(2));
					m_vmid3 = (double)_ttof(arr2->ElementAt(3));
				}
				arr2->RemoveAll();
				delete arr2;

				arr2 = CString2LinesBySpace(arr->ElementAt(4));
				if (arr2->GetCount() >= 4) { // only 4 is the correct number here
					m_vmidR4 = (UINT)_ttoi(arr2->ElementAt(0));
					m_vmidG4 = (UINT)_ttoi(arr2->ElementAt(1));
					m_vmidB4 = (UINT)_ttoi(arr2->ElementAt(2));
					m_vmid4 = (double)_ttof(arr2->ElementAt(3));
				}
				arr2->RemoveAll();
				delete arr2;

				arr2 = CString2LinesBySpace(arr->ElementAt(5));
				if (arr2->GetCount() >= 4) { // only 4 is the correct number here
					m_vmidR5 = (UINT)_ttoi(arr2->ElementAt(0));
					m_vmidG5 = (UINT)_ttoi(arr2->ElementAt(1));
					m_vmidB5 = (UINT)_ttoi(arr2->ElementAt(2));
					m_vmid5 = (double)_ttof(arr2->ElementAt(3));
				}
				arr2->RemoveAll();
				delete arr2;

				arr2 = CString2LinesBySpace(arr->ElementAt(6));
				if (arr2->GetCount() >= 4) { // only 4 is the correct number here
					m_vmidR6 = (UINT)_ttoi(arr2->ElementAt(0));
					m_vmidG6 = (UINT)_ttoi(arr2->ElementAt(1));
					m_vmidB6 = (UINT)_ttoi(arr2->ElementAt(2));
					m_vmid6 = (double)_ttof(arr2->ElementAt(3));
				}
				arr2->RemoveAll();
				delete arr2;

				arr2 = CString2LinesBySpace(arr->ElementAt(7));
				if (arr2->GetCount() >= 4) { // only 4 is the correct number here
					m_vmidR7 = (UINT)_ttoi(arr2->ElementAt(0));
					m_vmidG7 = (UINT)_ttoi(arr2->ElementAt(1));
					m_vmidB7 = (UINT)_ttoi(arr2->ElementAt(2));
					m_vmid7 = (double)_ttof(arr2->ElementAt(3));
				}
				arr2->RemoveAll();
				delete arr2;

				arr2 = CString2LinesBySpace(arr->ElementAt(8));
				if (arr2->GetCount() >= 4) { // only 4 is the correct number here
					m_vmidR8 = (UINT)_ttoi(arr2->ElementAt(0));
					m_vmidG8 = (UINT)_ttoi(arr2->ElementAt(1));
					m_vmidB8 = (UINT)_ttoi(arr2->ElementAt(2));
					m_vmid8 = (double)_ttof(arr2->ElementAt(3));
				}
				arr2->RemoveAll();
				delete arr2;


				arr2 = CString2LinesBySpace(arr->ElementAt(9));
				if (arr2->GetCount() >= 4) { // only 4 is the correct number here
					m_vmidR9 = (UINT)_ttoi(arr2->ElementAt(0));
					m_vmidG9 = (UINT)_ttoi(arr2->ElementAt(1));
					m_vmidB9 = (UINT)_ttoi(arr2->ElementAt(2));
					m_vmid9 = (double)_ttof(arr2->ElementAt(3));
				}
				arr2->RemoveAll();
				delete arr2;


				arr2 = CString2LinesBySpace(arr->ElementAt(10));
				if (arr2->GetCount() >= 4) { // only 4 is the correct number here
					m_vmaxR = (UINT)_ttoi(arr2->ElementAt(0));
					m_vmaxG = (UINT)_ttoi(arr2->ElementAt(1));
					m_vmaxB = (UINT)_ttoi(arr2->ElementAt(2));
					m_vmax = (double)_ttof(arr2->ElementAt(3));
				}
				arr2->RemoveAll();
				delete arr2;
			UpdateData(FALSE);
		} // if arr
	}

	if (arr) {
		arr->RemoveAll();
		delete arr;
	}
	Invalidate();

}
