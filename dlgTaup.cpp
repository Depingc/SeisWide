// dlgTaup.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgTaup.h"
#include "afxdialogex.h"


// CdlgTaup dialog

IMPLEMENT_DYNAMIC(CdlgTaup, CDialog)

CdlgTaup::CdlgTaup(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgTaup::IDD, pParent)
	, m_timeSecName(_T(""))
	, m_taupName(_T(""))
	, m_FiltTaupName(_T(""))
	, m_ppMin(0)
	, m_ppMax(0)
	, m_pp1(0)
	, m_pp2(0)
	, m_pp3(0)
	, m_pp4(0)
	, m_dp(0)
	, m_npoints(0)
	, m_tmin(0)
	, m_tmax(0)
	, m_isForwardOnly(FALSE)
	, m_isInBetween(FALSE)
	, m_dx(0)
	, m_fMax(0)
{

}

CdlgTaup::~CdlgTaup()
{
}

void CdlgTaup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_2NDSECNAME, m_taupName);
	DDX_Text(pDX, IDC_2NDSECNAME2, m_FiltTaupName);
	DDX_Text(pDX, IDC_EDIT_PMIN, m_ppMin);
	DDV_MinMaxFloat(pDX, m_ppMin, -100, 100);
	DDX_Text(pDX, IDC_EDIT_PMAX, m_ppMax);
	DDX_Text(pDX, IDC_EDIT_P1, m_pp1);
	DDX_Text(pDX, IDC_EDIT_P2, m_pp2);
	DDX_Text(pDX, IDC_EDIT_P3, m_pp3);
	DDX_Text(pDX, IDC_EDIT_P4, m_pp4);
	DDX_Text(pDX, IDC_EDIT_NP, m_npoints);
	DDV_MinMaxInt(pDX, m_npoints, 10, 1000);
	DDX_Check(pDX, IDC_IS_INBETWEEN, m_isInBetween);
	DDX_Text(pDX, IDC_EDIT_DP, m_dp);
	DDV_MinMaxFloat(pDX, m_dp, 0.00000001f, 10);
	DDX_Text(pDX, IDC_EDIT_DXAVG, m_dx);
	DDX_Text(pDX, IDC_EDIT_FREQMAX, m_fMax);
	DDX_Check(pDX, IDC_IS_FORWARD, m_isForwardOnly);
	DDX_Text(pDX, IDC_EDIT_TMIN, m_tmin);
	DDX_Text(pDX, IDC_EDIT_TMAX, m_tmax);
}


BEGIN_MESSAGE_MAP(CdlgTaup, CDialog)
	ON_EN_CHANGE(IDC_EDIT_FREQMAX, &CdlgTaup::OnEnChangeEditFreqmax)
END_MESSAGE_MAP()


// CdlgTaup message handlers


void CdlgTaup::OnEnChangeEditFreqmax()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	UpdateData(TRUE);
	// pmax-pmin <= 1/(dx*fMax
	float a = m_dx * m_fMax;
	if (fabs(a) > 0.00000001)
		this->m_ppMax = m_ppMin + 1.f / a;
	UpdateData(FALSE);
	Invalidate();

}
