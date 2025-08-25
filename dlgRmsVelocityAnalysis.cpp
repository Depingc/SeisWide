// dlgRmsVelocityAnalysis.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgRmsVelocityAnalysis.h"
//#include "afxdialogex.h"


// dlgRmsVelocityAnalysis dialog

IMPLEMENT_DYNAMIC(dlgRmsVelocityAnalysis, CDialog)

dlgRmsVelocityAnalysis::dlgRmsVelocityAnalysis(CWnd* pParent /*=NULL*/)
	: CDialog(dlgRmsVelocityAnalysis::IDD, pParent)
	, m_timeSecName(_T(""))
	, m_outfile(_T(""))
	, m_velSecName(_T(""))
	, m_tmin(0)
	, m_tmax(0)
	, m_dtRatio(5)
	, m_vmin(0)
	, m_vmax(0)
	, m_dv(0)
	, m_secsmooth(0)
	, m_percMute(0)
	, m_isNmoPanels(FALSE)
	, m_xmax(0)
	, m_pwr(0)
	, m_isNorm(FALSE)
{

}

dlgRmsVelocityAnalysis::~dlgRmsVelocityAnalysis()
{
}

void dlgRmsVelocityAnalysis::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_TIMESECNAME2, m_outfile);
	DDX_Text(pDX, IDC_VELSECNAME, m_velSecName);
	DDX_Text(pDX, IDC_VIEW_TMIN, m_tmin);
	DDX_Text(pDX, IDC_VIEW_TMAX, m_tmax);
	DDX_Text(pDX, IDC_VIEW_DTRATIO, m_dtRatio);
	DDX_Text(pDX, IDC_CONVERSION_VMIN, m_vmin);
	DDX_Text(pDX, IDC_CONVERSION_VMAX, m_vmax);
	DDX_Text(pDX, IDC_CONVERSION_DV, m_dv);
	DDX_Text(pDX, IDC_PROC_TWINDOWSEC, m_secsmooth);
	DDX_Text(pDX, IDC_PROC_XPERCENT, m_percMute);
	DDX_Check(pDX, IDC_CHK_NMOPANELS, m_isNmoPanels);
	DDX_Text(pDX, IDC_PROC_XMAX, m_xmax);
	DDX_Text(pDX, IDC_PROC_XPERCENT2, m_pwr);
	DDX_Check(pDX, IDC_CHK_NORM, m_isNorm);
}


BEGIN_MESSAGE_MAP(dlgRmsVelocityAnalysis, CDialog)
END_MESSAGE_MAP()


// dlgRmsVelocityAnalysis message handlers
