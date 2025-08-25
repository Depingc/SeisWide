// dlgResample.cpp : implementation file
//

#include "stdafx.h"
#include "seiswide.h"
#include "dlgResample.h"
#include "segy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgResample dialog


CdlgResample::CdlgResample(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgResample::IDD, pParent)
	, m_numsampNew(0)
	, m_numsampOld(0)
{
	//{{AFX_DATA_INIT(CdlgResample)
	m_in = _T("");
	m_out = _T("");
	m_tmax = 0.0f;
	m_tmin = 0.0f;
	m_vRed = 0.0f;
	m_rate2 = 0;
	m_rate = 0;
	//}}AFX_DATA_INIT
}


void CdlgResample::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgResample)
	DDX_Text(pDX, IDC_PROCNAME_IN, m_in);
	DDX_Text(pDX, IDC_PROCNAME_OUT, m_out);
	DDX_Text(pDX, IDC_PROC_TMIN, m_tmin);
	DDX_Text(pDX, IDC_PROC_TMAX, m_tmax);
	DDX_Text(pDX, IDC_PROC_VRED, m_vRed);
	DDX_Text(pDX, IDC_PROC_SAMPRATEOLD, m_rate);
	DDX_Text(pDX, IDC_PROC_SAMPRATENEW, m_rate2);
	DDV_MinMaxLong(pDX, m_rate2, 1, 62000);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_PROC_SAMPLENOLD, m_numsampOld);
	DDX_Text(pDX, IDC_PROC_SAMPLENNEW, m_numsampNew);
}


BEGIN_MESSAGE_MAP(CdlgResample, CDialog)
	//{{AFX_MSG_MAP(CdlgResample)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET2, OnButtonBrowset2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgResample message handlers

void CdlgResample::OnButtonBrowset() 
{
	CFileDialog dlg( TRUE,_T("sgy; SEG"),_T("*.sgy; *.SEG"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Input Segy File (*.sgy;*.SEG)|*.sgy; *.SEG|"));
	if( dlg.DoModal()==IDOK )
	{
		m_in = dlg.GetPathName();
		sampRateOld(m_in);
		UpdateData(FALSE);
	}
	
}

void CdlgResample::OnButtonBrowset2() 
{
#ifndef _MAC
	CFileDialog dlg( FALSE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Output Segy File (*.sgy)|*.sgy|"));
#else
	CFileDialog dlg( FALSE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Output Segy File  |sgy |"));
#endif
	if( dlg.DoModal()==IDOK )
	{
		m_out = dlg.GetPathName();
		UpdateData(FALSE);
	}
	
	
}

#include "segyMeta.h"
BOOL CdlgResample::sampRateOld(CString name)
{
	HEAD aHead;
	if (CSegyMeta::segyHeadInit(name, &aHead)) {
		m_rate2 = m_rate = (long)(MIL*aHead.dtFile + 0.5);
		this->m_numsampOld = aHead.numsampFiles;
		this->m_numsampNew = 0; // make it 0 so that the new num of samples is auto calculated. If new rate is changed, the new num of samples need to auto calc
	}
	CSegyMeta::segyHeadFree(&aHead);


	//bhed bh;

	//CFile file;
	//if (!file.Open(name, CFile::modeRead | CFile::typeBinary)) {
	//	return FALSE;
	//}

	//file.Seek(3200, CFile::begin);
	//UINT bytesRead = file.Read(&bh, 400);
	//if(bytesRead < 400) 
	//{
	//	AfxMessageBox(_T("Binary section header read wrong"));
	//	return FALSE;
	//}
	//if (bh.hdt == 0) bh.hdt = bh.dto;
	//m_rate2 = m_rate = (unsigned short)bh.hdt;
	//this->m_numsampOld = (unsigned short)bh.hns;
	//this->m_numsampNew = 0; // make it 0 so that the new num of samples is auto calculated. If new rate is changed, the new num of samples need to auto calc
	//file.Close();

	return TRUE;
}

BOOL CdlgResample::PreCreateWindow(CREATESTRUCT& cs) 
{

	return CDialog::PreCreateWindow(cs);
}

BOOL CdlgResample::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	sampRateOld(m_in);
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
