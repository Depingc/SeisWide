// dlgAttenuationCorr.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgAttenuationCorr.h"


// CdlgAttenuationCorr dialog

IMPLEMENT_DYNAMIC(CdlgAttenuationCorr, CDialog)

CdlgAttenuationCorr::CdlgAttenuationCorr(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgAttenuationCorr::IDD, pParent)
	, m_inFile(_T(""))
	, m_outFile(_T(""))
	, m_attenuationCorr(0)
	, m_sphericOrder(0)
{

}

CdlgAttenuationCorr::~CdlgAttenuationCorr()
{
}

void CdlgAttenuationCorr::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INFILE, m_inFile);
	DDX_Text(pDX, IDC_OUTFILE, m_outFile);
	DDX_Text(pDX, IDC_PROC_SPHERORDER, m_sphericOrder);
	DDX_Text(pDX, IDC_PROC_ATTENUATIONCORR, m_attenuationCorr);
	DDX_Text(pDX, IDC_PROC_ATTENUATIONSEAFLOOR, m_attenuationSeafloor);
}


BEGIN_MESSAGE_MAP(CdlgAttenuationCorr, CDialog)
END_MESSAGE_MAP()


// CdlgAttenuationCorr message handlers
