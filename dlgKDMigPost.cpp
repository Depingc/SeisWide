// dlgKDMigPost.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "SeisWideDoc.h"
#include "util.h"

#include "dlgKDMigPost.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgKDMigPost dialog


CdlgKDMigPost::CdlgKDMigPost(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgKDMigPost::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgKDMigPost)
	m_ek = TRUE;
	m_depthSecName = BLANK;
	m_amax = 90.0f;
	m_amin = 0.0f;
	m_angmax = 60.0f;
	m_aperx = 10.f;
	m_da = 2.0f;
	m_dt = 0.008f;
	m_dxo = 0.025f;
	m_dzo = 0.2f;
	m_fa = -60.0f;
	m_fac = 0.01f;
	m_fxo = 0.0f;
	m_fzo = 0.0f;
	m_na = 61;
	m_v0 = 1500.0f;
	m_timeSecName = BLANK;
	m_velSecName = BLANK;
	m_fmax = 0.0f;
	m_dxv = 0.0f;
	m_dzv = 0.0f;
	m_dvz = 0.0f;
	m_xmax = 0.0f;
	m_zmax = 10.0f;
	m_ls = -1;
	m_memMax = 0.0f;
	m_tmaxRay = 0.0f;
	m_dxs = 0.0f;
	m_useExist = FALSE;
	m_dxt = 0.0f;
	m_dzt = 0.0f;
	//}}AFX_DATA_INIT
}


void CdlgKDMigPost::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgKDMigPost)
	DDX_Check(pDX, IDC_CHECK_MIGEK, m_ek);
	DDX_Text(pDX, IDC_DEPTHSECNAME, m_depthSecName);
	DDX_Text(pDX, IDC_MIG_AMAX, m_amax);
	DDX_Text(pDX, IDC_MIG_AMIN, m_amin);
	DDX_Text(pDX, IDC_MIG_ANGMAX, m_angmax);
	DDX_Text(pDX, IDC_MIG_APERX, m_aperx);
	DDX_Text(pDX, IDC_MIG_DA, m_da);
	DDX_Text(pDX, IDC_MIG_DT, m_dt);
	DDX_Text(pDX, IDC_MIG_DXO, m_dxo);
	DDX_Text(pDX, IDC_MIG_DZO, m_dzo);
	DDV_MinMaxFloat(pDX, m_dzo, 1.e-003f, 6.5e-002f);
	DDX_Text(pDX, IDC_MIG_FA, m_fa);
	DDX_Text(pDX, IDC_MIG_FAC, m_fac);
	DDX_Text(pDX, IDC_MIG_FXO, m_fxo);
	DDX_Text(pDX, IDC_MIG_FZO, m_fzo);
	DDX_Text(pDX, IDC_MIG_NA, m_na);
	DDX_Text(pDX, IDC_MIG_V0, m_v0);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_VELSECNAME, m_velSecName);
	DDX_Text(pDX, IDC_MIG_FMAX, m_fmax);
	DDX_Text(pDX, IDC_MIG_DXV, m_dxv);
	DDX_Text(pDX, IDC_MIG_DZV, m_dzv);
	DDX_Text(pDX, IDC_MIG_DVZ, m_dvz);
	DDX_Text(pDX, IDC_MIG_XMAX, m_xmax);
	DDX_Text(pDX, IDC_MIG_ZMAX, m_zmax);
	DDX_Radio(pDX, IDC_RADIO_SPHERICSRC, m_ls);
	DDX_Text(pDX, IDC_MIG_MEMMAX, m_memMax);
	DDV_MinMaxFloat(pDX, m_memMax, 5.f, 99999.f);
	DDX_Text(pDX, IDC_MIG_TMAXRAY, m_tmaxRay);
	DDV_MinMaxFloat(pDX, m_tmaxRay, 1.e-005f, 99999.f);
	DDX_Text(pDX, IDC_MIG_DXS, m_dxs);
	DDX_Check(pDX, IDC_CHECK_USEEXIST, m_useExist);
	DDX_Text(pDX, IDC_MIG_DXT, m_dxt);
	DDV_MinMaxFloat(pDX, m_dxt, 0.f, 999999.f);
	DDX_Text(pDX, IDC_MIG_DZT, m_dzt);
	DDV_MinMaxFloat(pDX, m_dzt, 0.f, 999999.f);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgKDMigPost, CDialog)
	//{{AFX_MSG_MAP(CdlgKDMigPost)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, OnButtonBrowsev)
	ON_BN_CLICKED(IDC_BUTTON_BROWSED, OnButtonBrowsed)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgKDMigPost message handlers

void CdlgKDMigPost::OnButtonBrowset() 
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Input Segy Time Section for Migration (*.sgy)|*.sgy|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Input Segy Time Section for Migration |sgy |"));
#endif
	if( dlg.DoModal()==IDOK )
	{
		m_timeSecName = dlg.GetPathName();
		m_timeSecName = dlg.GetPathName();

		//Load segy headers
		// short i2buf[400];
		segy tr;
		bhed bh;
		long len,numSamp;
		unsigned char ascHead[3400];
		//union TWO_FOUR{
		//	short hw[2];
		//	int iv;
		//} ssmic;
		long offset;

		m_timeSecName = dlg.GetPathName();

	CFile file;
	if (!file.Open(m_timeSecName, CFile::modeRead | CFile::typeBinary)) {
		return;
	}

		UINT bytesRead = file.Read(ascHead, 3200L);
		if(bytesRead < 3200L) 
		{
			AfxMessageBox(_T("Ascii section header read wrong"), MB_ICONQUESTION);
			return;
		}
		bytesRead = file.Read(&bh, 400);
		if(bytesRead < 400) 
		{
			AfxMessageBox(_T("Binary section header read wrong"), MB_ICONQUESTION);
			return;
		}
		
		numSamp = bh.nso;
		if(bh.format<0 || bh.format>5)
		{
			AfxMessageBox(_T("Incorrect data format specification is found, try converting to PC format using NSCU in unix!"), MB_ICONQUESTION);
			return;
		}
		if(bh.format!=3)  
			len = numSamp*4 + 240;
		else 
			len = numSamp*2 + 240;
		file.Read(&tr, 240L);
		file.Seek(len-240L, CFile::current);	//point to next trace.
		offset = (long)myRound(tr.getOffset() * 1000.0);

		file.Read(&tr, 240L);
		file.Close();
	//Finish loading segy headers.

		m_dxo = (float)(offset*0.001 - tr.getOffset());
		m_dxv = (float)(2.0*m_dxo);

		UpdateData(FALSE);
	}
}

void CdlgKDMigPost::OnButtonBrowsev() 
{
	CFileDialog dlg(TRUE, BLANK, _T("*.vxml;*.bin"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("File name for model (*.vxml)|*.vxml|Older format (*.bin)|*.bin|"));
	//CFileDialog dlg(TRUE, _T("SGY"), _T("*.BIN"),
	//                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	//                 _T("SeisWide Velocity File (*.bin)|*.bin|"));
	if( dlg.DoModal()==IDOK )
	{
		m_velSecName = dlg.GetPathName();

		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VELSECNAME));
		pEdit->SetWindowText(m_velSecName);
	}
	
}

void CdlgKDMigPost::OnButtonBrowsed() 
{
#ifndef _MAC
	CFileDialog dlg( FALSE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Migrated Output Segy Section (*.sgy)|*.sgy|"));
#else
	CFileDialog dlg( FALSE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Migrated Output Segy Section |sgy |"));
#endif
	if( dlg.DoModal()==IDOK )
	{
		m_depthSecName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DEPTHSECNAME));
		pEdit->SetWindowText(m_depthSecName);
	}
}

void CdlgKDMigPost::OnButton1() 
{

	m_dxv = (m_xmax - m_fxo)*0.01f;
	m_dzv = (m_zmax - m_fzo)*0.01f;
	m_dt = 0.008f;
	m_tmaxRay = 13.f;
	m_fa = -60.f;
	m_da = 2.f;
	m_na = 60;
//	m_angmax = 60.f;
	m_amin = 0.f;
	m_amax = 90.f;
	m_fac = 0.01f;
	m_dxs = m_dxo*4.f;
	UpdateData(FALSE);
}
