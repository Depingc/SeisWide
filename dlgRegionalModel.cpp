// dlgRegionalModel.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgRegionalModel.h"


// CdlgRegionalModel dialog

IMPLEMENT_DYNAMIC(CdlgRegionalModel, CDialog)

CdlgRegionalModel::CdlgRegionalModel(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgRegionalModel::IDD, pParent)
	, m_regionalModelA(0)
	, m_regionalModelB(0)
	, m_regionalModelAlpha(0)
	, m_regionalModelBeta(0)
{
	// initialize with category 2
	this->m_regionalModelA = 0.;
	this->m_regionalModelB = 0.;
	this->m_regionalModelAlpha = 0.;
	this->m_regionalModelBeta = 0.;
}

CdlgRegionalModel::~CdlgRegionalModel()
{
}

void CdlgRegionalModel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MODEL_THEORY_A, m_regionalModelA);
	DDX_Text(pDX, IDC_MODEL_THEORY_B, m_regionalModelB);
	DDX_Text(pDX, IDC_MODEL_THEORY_ALPHA, m_regionalModelAlpha);
	DDX_Text(pDX, IDC_MODEL_THEORY_BETA, m_regionalModelBeta);
}


BEGIN_MESSAGE_MAP(CdlgRegionalModel, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, &CdlgRegionalModel::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CdlgRegionalModel::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CdlgRegionalModel::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CdlgRegionalModel::OnBnClickedButton4)
END_MESSAGE_MAP()


// CdlgRegionalModel message handlers

void CdlgRegionalModel::OnBnClickedButton1()
{
	// Category 1
	this->m_regionalModelA = 0.21276957;
	this->m_regionalModelB = 0.41223404;
	this->m_regionalModelAlpha = -0.35562832;
	this->m_regionalModelBeta = -0.12714439;
	UpdateData(FALSE);
	Invalidate();
}

void CdlgRegionalModel::OnBnClickedButton2()
{
	// Category 2
	this->m_regionalModelA = 0.212765957;
	this->m_regionalModelB = 0.41223404;
	this->m_regionalModelAlpha = -0.52242615;
	this->m_regionalModelBeta = -0.07325195;
	UpdateData(FALSE);
	Invalidate();
}

void CdlgRegionalModel::OnBnClickedButton3()
{
	// Category 3
	this->m_regionalModelA = 0.212765957;
	this->m_regionalModelB = 0.41223404;
	this->m_regionalModelAlpha = -0.79105457;
	this->m_regionalModelBeta = 0.08027246;
	UpdateData(FALSE);
	Invalidate();
}

void CdlgRegionalModel::OnBnClickedButton4()
{
	// Category 4  
	// 	
	this->m_regionalModelA = 0.212765957;
	this->m_regionalModelB = 0.412234042;
	this->m_regionalModelAlpha = -1.48496522;
	this->m_regionalModelBeta = 0.18803602;
	UpdateData(FALSE);
	Invalidate();
}
