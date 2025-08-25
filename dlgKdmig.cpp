// dlgKdmig.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "SeisWideDoc.h"
#include "dlgSourcePos.h"
#include "MainFrm.h"
#include "dlgKdmig.h"
#include "segy.h"
#include "math.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgKdmig dialog


CdlgKdmig::CdlgKdmig(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgKdmig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgKdmig)
	m_ek = TRUE;
	m_depthSecName = BLANK;
	m_amax = 90.0f;
	m_amin = 0.0f;
	m_angmax = 60.0f;
	m_aperx = 10.f;
	m_da = 2.0f;
	m_doff = 99999.0f;
	m_dt = 0.008f;
	m_dxo = 0.025f;
	m_dzo = 0.2f*0.008f;
	m_fa = -60.0f;
	m_fac = 0.01f;
	m_fxo = 0.0f;
	m_fzo = 0.0f;
	m_na = 61;
	m_off0 = 0.0f;
	m_offmax = 3000.0f;
	m_v0 = 1500.0f;
	m_timeSecName = BLANK;
	m_velSecName = BLANK;
	m_fmax = 0.0f;
	m_dxv = 0.0f;
	m_dzv = 0.0f;
	m_dvz = 0.0f;
	m_xmax = 0.0f;
	m_zmax = 10.0f;
	m_memMax = 0.0f;
	m_tmaxRay = 0.0f;
	m_ls = -1;
	m_useExist = FALSE;
	m_dxm = -99999.0f;
	m_dxs = 0.0f;
	m_dzt = 0.0f;
	m_srcDepth = 0.0f;
	m_dxt = 0.0f;
	m_fs = 0.0f;
	//}}AFX_DATA_INIT
}


void CdlgKdmig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgKdmig)
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
	DDX_Text(pDX, IDC_MIG_OFF0, m_off0);
	DDX_Text(pDX, IDC_MIG_OFFMAX, m_offmax);
	DDX_Text(pDX, IDC_MIG_V0, m_v0);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_VELSECNAME, m_velSecName);
	DDX_Text(pDX, IDC_MIG_FMAX, m_fmax);
	DDX_Text(pDX, IDC_MIG_DXV, m_dxv);
	DDX_Text(pDX, IDC_MIG_DZV, m_dzv);
	DDX_Text(pDX, IDC_MIG_DVZ, m_dvz);
	DDX_Text(pDX, IDC_MIG_XMAX, m_xmax);
	DDX_Text(pDX, IDC_MIG_ZMAX, m_zmax);
	DDX_Text(pDX, IDC_MIG_MEMMAX, m_memMax);
	DDV_MinMaxFloat(pDX, m_memMax, 5.f, 99999.f);
	DDX_Text(pDX, IDC_MIG_TMAXRAY, m_tmaxRay);
	DDV_MinMaxFloat(pDX, m_tmaxRay, 1.e-005f, 99999.f);
	DDX_Radio(pDX, IDC_RADIO_SPHERICSRC, m_ls);
	DDX_Check(pDX, IDC_CHECK_USEEXIST, m_useExist);
	DDX_Text(pDX, IDC_MIG_DXM, m_dxm);
	DDV_MinMaxFloat(pDX, m_dxm, 1.e-006f, 1.e+007f);
	DDX_Text(pDX, IDC_MIG_DXS, m_dxs);
	DDX_Text(pDX, IDC_MIG_DZT, m_dzt);
	DDV_MinMaxFloat(pDX, m_dzt, 0.f, 1.e+007f);
	DDX_Text(pDX, IDC_MIG_SRCDEPTH, m_srcDepth);
	DDX_Text(pDX, IDC_MIG_DXT, m_dxt);
	DDV_MinMaxFloat(pDX, m_dxt, 0.f, 999999.f);
	DDX_Text(pDX, IDC_MIG_FS, m_fs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgKdmig, CDialog)
	//{{AFX_MSG_MAP(CdlgKdmig)
	ON_BN_CLICKED(IDC_BUTTON_BROWSED, OnButtonBrowsed)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, OnButtonBrowsev)
	ON_BN_CLICKED(IDC_BUTTON_DXO, OnButtonDxo)
	//ON_BN_CLICKED(IDC_BUTTON_DISTSG, OnButtonDistsg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgKdmig message handlers

void CdlgKdmig::OnButtonBrowsed() 
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

void CdlgKdmig::OnButtonBrowset() 
{
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Input Segy Time Section for Migration (*.sgy)|*.sgy|"));
	if( dlg.DoModal()==IDOK )
	{
		m_timeSecName = dlg.GetPathName();

		//Load segy headers
	//	short i2buf[400];
		long len,numSamp;
		unsigned char ascHead[3400];
	segy tr;
	short *i2buf;
	i2buf = (short *) &tr;

		m_timeSecName = dlg.GetPathName();
		CFile file;
		if (!file.Open(m_timeSecName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
			return;
		}

		UINT bytesRead = file.Read(ascHead, 3200L);
		if(bytesRead < 3200L) 
		{
			AfxMessageBox(_T("Ascii section header read wrong"), MB_ICONQUESTION);
			return;
		}
		bytesRead = file.Read(i2buf, 400);
		if(bytesRead < 400) 
		{
			AfxMessageBox(_T("Binary section header read wrong"), MB_ICONQUESTION);
			return;
		}
		numSamp = (unsigned short)i2buf[10];
		if(i2buf[12]<0 || i2buf[12]>5)
		{
			AfxMessageBox(_T("Incorrect data format specification is found, try 'Segy Conversion: UNIX to PC'!"), MB_ICONQUESTION);
			return;
		}
		if(i2buf[12]!=3)  
			len = numSamp*4 + 240;
		else 
			len = numSamp*2 + 240;
		while(file.Read(&tr, 240L) == 240)
		{
			file.Seek(len-240L, CFile::current);	//point to next trace.

			if(tr.trid ==2) //dead traces.
				continue;
			m_off0 = tr.getOffset();
			file.Read(&tr, 240L);
			m_doff = (float)fabs(m_off0 - tr.getOffset());
			if(m_doff < 0.000001f) m_doff = 0.05f;
			m_off0 = (float)fabs(m_off0);	//m_off0 can be negative.
			m_dxm = m_dxo =(float)(m_doff*0.5);
			m_dxv = (float)(2.0*m_dxo);
			break;
		}

	file.Seek(3600, CFile::begin);
	int cdpt0 = 0;
	float offmax0 = 0;
	while(file.Read(&tr, 240L) == 240)
	{
		file.Seek(len-240L, CFile::current);	//point to next trace.
		if(tr.trid ==2) //dead traces.
			continue;

		if(tr.cdpt < cdpt0)
		{
			m_offmax = offmax0;
			break;
		}
		else
		{
			cdpt0 = tr.cdpt;
			offmax0 = tr.getOffset();
		}
	}
	m_offmax = offmax0; //this is required when only one gather is there.

	file.Close();
	//Finish loading segy headers.

		UpdateData(FALSE);
	}
	
}

void CdlgKdmig::OnButtonBrowsev() 
{
	CFileDialog dlg(TRUE, BLANK, _T("*.vxml;*.bin"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("File name for model (*.vxml)|*.vxml|Older format (*.bin)|*.bin|"));

	//CFileDialog dlg( TRUE,_T("SGY"),_T("*.bin"),
	//                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	//                 _T("SeisWide Velocity File (*.bin)|*.bin|"));
	if( dlg.DoModal()==IDOK )
	{
		m_velSecName = dlg.GetPathName();

		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VELSECNAME));
		pEdit->SetWindowText(m_velSecName);
	}
	
}

void CdlgKdmig::OnButtonDxo() 
{
	long len,numSamp;
	unsigned char ascHead[3400];
	segy tr;
	short *i2buf;
	i2buf = (short *) &tr;
	float *trcdat;
	trcdat = (float *) &i2buf[120];

	CFile file;
	if (!file.Open(m_timeSecName, CFile::modeRead | CFile::typeBinary)) {
		return;
	}

	UINT bytesRead = file.Read(ascHead, 3200L);
	if(bytesRead < 3200L) {
		AfxMessageBox(_T("Ascii section header read wrong"), MB_ICONQUESTION);
		return;
	}
	bytesRead = file.Read(i2buf, 400);
	if(bytesRead < 400) {
		AfxMessageBox(_T("Binary section header read wrong"), MB_ICONQUESTION);
		return;
	}

	int tpp = (unsigned short)i2buf[8];
	m_fmax=0.25f*1000000.f/tpp;

	numSamp = (unsigned short)i2buf[10];
	if(i2buf[12]<0 || i2buf[12]>5){
		AfxMessageBox(_T("Incorrect data format specification is found, try 'Segy Conversion: UNIX to PC'!"), MB_ICONQUESTION);
		return;
	}
	if(i2buf[12]!=3)  
		len = numSamp*4 + 240;
	else 
		len = numSamp*2 + 240;
	while(file.Read(&tr, 240L) == 240) {
		file.Seek(len-240L, CFile::current);	//point to next trace.
		if(tr.trid ==2) //dead traces.
			continue;
		m_off0 = tr.getOffset();
		file.Read(&tr, 240L);
		m_doff = (float)fabs(m_off0 - tr.getOffset());
		if(m_doff < 0.000001f) m_doff = 0.05f;
		m_off0 = (float)fabs(m_off0);	//m_off0 can be negative.
		m_dxm = m_dxo =(float)(m_doff*0.5);
		m_dxs = 2.f*m_dxo;
		m_dxv = (float)(2.0*m_dxo);
		break;
	}

	file.Seek(3600, CFile::begin);
	int cdpt0 = 0;
	float offmax0 = 0;
	while(file.Read(&tr, 240L) == 240) {
		file.Seek(len-240L, CFile::current);	//point to next trace.
		if(tr.trid ==2) //dead traces.
			continue;

		if(tr.cdpt < cdpt0) {
			m_offmax = offmax0;
			break;
		}
		else {
			cdpt0 = tr.cdpt;
			offmax0 = tr.getOffset();
		}
	}

	m_offmax = offmax0; //this is required when only one gather is there.

	file.Close();
	//Finish loading segy headers.

	UpdateData(FALSE);
}

BOOL CdlgKdmig::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
//	OnButtonDxo();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//void CdlgKdmig::OnButtonDistsg() 
//{
//	float distTrc;
//	CString xyFileName;
//	long FFID[20000];
//	float dist[20000];
//	int i=1, num, options, recPlus;
//	CString timeSecName;
//
//	bhed bh;
//	segy tr;
//	//short *i2buf;
//	//i2buf = (short *) &tr;
//
//	CdlgSourcePos dlg;
//	dlg.m_options = 2;
//	if (dlg.DoModal() == IDOK) {
//		timeSecName = dlg.m_timeSecName;
//		options = dlg.m_options;
//		//xyFileName = dlg.m_xyFileName;
//		recPlus = dlg.m_sxsyOverwrite;
//	}
//	else  {
//		return;
//	}
//	delete dlg;
//
//	CFile file;
//	if (!file.Open(timeSecName, CFile::modeReadWrite | CFile::typeBinary)) {
//		return;
//	}
//
//	//For progress control, need to know the last bytes no.
//	 LONGLONG curBytesRead;
//	 LONGLONG totBytesRead = (long)file.GetLength();
//	 file.Seek(0, CFile::begin);
//	//Begin progress control.
//     CMainFrame* pFrame = 
//        (CMainFrame*)AfxGetMainWnd();
//     ASSERT(pFrame);
//     ASSERT_KINDOF(CMainFrame, pFrame);
//     if (pFrame) pFrame->BeginProgress();
//
//	LONGLONG bytesRead = file.Seek(3200L, CFile::begin);
//	if(bytesRead < 3200) {
//		AfxMessageBox(_T("Ascii section header read wrong"), MB_ICONQUESTION);
//		return;
//	}
//	bytesRead = file.Read(&bh, 400);
//	if(bytesRead < 400) {
//		AfxMessageBox(_T("Binary section header read wrong"), MB_ICONQUESTION);
//		return;
//	}
//	int numsamp = bh.hns;
//	int format = bh.format;
//	int len;
//	if(format!=3)  
//		len = numsamp*4;
//	else
//		len = numsamp*2;
//	int tpp = bh.hdt;
//
//	if(options==2) {
//		if(xyFileName==BLANK) {
//			AfxMessageBox(_T("please specify a FFID-Distance file name in ascii format"), MB_ICONQUESTION);
//			return;
//		}
//		FILE *stream;
//		if (0 != _tfopen_s(&stream, xyFileName, _T("r"))) return;
//		_ftscanf_s(stream, _T("%ld %f"), &FFID[0], &dist[0]);
//
//
//		while (_ftscanf_s(stream, _T("%ld %f"),&FFID[i],&dist[i]) !=EOF) {
//			if(i==20000) {
//				AfxMessageBox(_T("No more than 20,000 lines are allowed! Program continues though."), MB_ICONQUESTION);
//				break;
//			}
//			i++;
//		}
//		fclose(stream);
//		num = i;
//		for (i=0; i<num-1; i++) {
//			if(FFID[i+1]<FFID[i]) {
//				AfxMessageBox(_T("FFID table not in sequential order"), MB_ICONQUESTION);
//				return;
//			}
//		}
//	}
//
//	while (file.Read(&tr, 240) == 240) {
//		curBytesRead = (long)file.Seek(0, CFile::current);
//		int iPos = (int)((double)curBytesRead/(double)totBytesRead*100.);
//		if(iPos<0)iPos=0;
//		if(iPos>100)iPos=100;
//		if (pFrame) pFrame->SetProgress(iPos);
//
//		if(options==0) {
//			return;
//		}
//		else if(options==1) {
//			return;
//		}
//		else {
//			if(tr.fldr<FFID[0])
//				distTrc = dist[0];
//			else if(tr.fldr>FFID[num-1])
//				distTrc = dist[num-1];
//			else {
//				for (i=0; i<num-1; i++) {
//					if(tr.fldr>=FFID[i] && tr.fldr<=FFID[i+1]) {
//						if(abs(tr.fldr-FFID[i])<1) {
//							distTrc = dist[i];
//							break;
//						}
//						distTrc = dist[i]+(dist[i+1]-dist[i])/
//						(FFID[i+1]-FFID[i])*(tr.fldr-FFID[i]);
//						break;
//					}
//				}
//			}
//			tr.sx = (long)(distTrc*1000.);
//
//			if(recPlus == 0)
//				tr.gx = tr.sx + tr.offset; //receiver pos.  note: offset info will ignore anything less than 1 meter. So we only use tr.offset, not the float tr.trcOffset
//			else
//				tr.gx = tr.sx - tr.offset; //receiver pos.
//
//		}
//		tr.counit = 1;  //meters used.
//		file.Seek(-240, CFile::current);
//		file.Write(&tr, 240);
//		file.Seek((long)len, CFile::current);
//	}
//
//	file.Close();
//
//	if (pFrame) pFrame->EndProgress();
//}
