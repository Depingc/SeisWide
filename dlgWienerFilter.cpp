// dlgWienerFilter.cpp : implementation file
//

#include "stdafx.h"
#include "seiswide.h"
#include "dlgWienerFilter.h"
#include "segy.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgWienerFilter dialog


CdlgWienerFilter::CdlgWienerFilter(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgWienerFilter::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgWienerFilter)
	m_maxlag = 0.0f;
	m_dt = 0.0f;
	m_maxcorr = 0.0f;
	m_mincorr = 0.0f;
	m_minlag = 0.0f;
	m_pnoise = 0.0f;
	m_show_wiener = FALSE;
	m_tmax = 0.0f;
	m_tmin = 0.0f;
	m_nameIn = _T("");
	m_nameOut = _T("");
	//}}AFX_DATA_INIT
}


void CdlgWienerFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgWienerFilter)
	DDX_Text(pDX, IDC_PROCNAME_IN, m_nameIn);
	DDX_Text(pDX, IDC_PROCNAME_OUT, m_nameOut);
	DDX_Text(pDX, IDC_PROC_TMAX, m_tmax);
	DDX_Text(pDX, IDC_PROC_TMIN, m_tmin);
	DDX_Text(pDX, IDC_PROC_DT, m_dt);
	DDX_Text(pDX, IDC_PROC_PNOISE, m_pnoise);
	DDX_Text(pDX, IDC_PROC_MINCORR, m_mincorr);
	DDX_Text(pDX, IDC_PROC_MAXCORR, m_maxcorr);
	DDX_Text(pDX, IDC_PROC_MINLAG, m_minlag);

	DDX_Text(pDX, IDC_FILE_MAXLAG, m_maxlag);
	//	DDX_Check(pDX, IDC_PROC_SHOW_WIENER, m_show_wiener);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgWienerFilter, CDialog)
	//{{AFX_MSG_MAP(CdlgWienerFilter)
	ON_BN_CLICKED(IDC_BUTTON_BROWSER1, OnButtonBrowser1)
	ON_BN_CLICKED(IDC_BUTTON_BROWSER2, OnButtonBrowser2)
	ON_EN_CHANGE(IDC_PROCNAME_IN, OnChangeProcnameIn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgWienerFilter message handlers
void CdlgWienerFilter::OnButtonBrowser1() 
{
	bhed bh;
	unsigned char ascHead[3400];
	CFileDialog dlg( TRUE,_T("sgy; SEG"),_T("*.sgy; *.SEG"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Input Segy File (*.sgy;*.SEG)|*.sgy; *.SEG|"));
	if( dlg.DoModal()==IDOK )
	{
		m_nameIn = dlg.GetPathName();
		m_nameOut = getFileNameAppend(m_nameIn, _T("_decon"));

		CFile file;
		if (!file.Open(m_nameIn, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
			AfxMessageBox(_T("Data file cannot be opened for reading!"), MB_ICONQUESTION);
			return;
		}

	
		file.Seek(0, CFile::begin);

		UINT bytesRead = file.Read(&ascHead, 3200L);
		//if(bytesRead < 3200) 
		//	AfxMessageBox(_T("Ascii section header read wrong");

		bytesRead=file.Read(&bh, 400L);
		//if(bytesRead < 400) 
		//	AfxMessageBox(_T("Binary section header read wrong");

		int nt= bh.hns;
		if (bh.hdt == 0) bh.hdt = bh.dto;
		m_dt=(float)((double)bh.hdt/MIL);
	//	BOOL showwiener=FALSE;
		m_pnoise=0.001f;
		m_minlag=m_dt;
		m_maxlag=(m_tmax-m_tmin)/20.0f;

		if (m_mincorr != 0.f && m_maxcorr != 0.f) {
			CString str;
			CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_PROC_MINCORR));
			pEdit->GetWindowText(str);
			m_mincorr=(float)_tstof(str);

			pEdit = static_cast<CEdit*> (GetDlgItem(IDC_PROC_MAXCORR));
			pEdit->GetWindowText(str);
			m_maxcorr=(float)_tstof(str);
		}
		
		file.Close();

		UpdateData(FALSE);

		if(bh.format != 1 )
		{
			AfxMessageBox(_T("Data file is not in a float format!"), MB_ICONQUESTION);
			m_pnoise=0.0;
			m_minlag=0.0;
			m_maxlag=0.0;
			m_nameIn=_T("");
			

			m_maxcorr=0.0;
			m_mincorr=0.0;
			m_dt=0.0;
			UpdateData(FALSE);
		}
		return;
		
	}
}

void CdlgWienerFilter::OnButtonBrowser2() 
{
	// TODO: Add your control notification handler code here

	CString inFile=_T(""),inFile2=_T("");

	
#ifndef _MAC
	CFileDialog dlg( FALSE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Output Segy File (*.sgy)|*.sgy|"));
#else
	CFileDialog dlg( FALSE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Output Segy File  |sgy |"));
#endif

	//put the path onto file2 if it doesn't have it.
	int pathLen = inFile2.ReverseFind( '\\' );
	

	if( dlg.DoModal()==IDOK )
	{
		m_nameOut = dlg.GetPathName();
		inFile=m_nameIn;
		inFile2=m_nameOut;
		if(GetBaseFileName(inFile).IsEmpty() || GetBaseFileName(inFile2).IsEmpty()) return;

	if(inFile2 == inFile)
	{
		AfxMessageBox(_T("Output file name is the same as the input file name!"), MB_ICONQUESTION);
		return;
	}

	inFile2 = SetFileExtension(inFile2, _T(".sgy"));

	//put the path onto file2 if it doesn't have it.
	int pathLen = inFile2.ReverseFind( _T('\\') );
	if(pathLen==(-1))
	{
		TCHAR curDir[_MAX_PATH];
		GetCurrentDirectory(_MAX_PATH, curDir);
		//inFile2 = (CString)curDir + _T("\\" + inFile2;
		inFile2.Format( _T("%s\\%s"), (CString)curDir, inFile2);
	}

	

	UpdateData(FALSE);


	}
	
}


void CdlgWienerFilter::OnChangeProcnameIn() 
{
	CString inFile;

	bhed bh;
	unsigned char ascHead[3400];


	CEdit *e=(CEdit*)GetDlgItem(IDC_PROCNAME_IN);
	e->GetWindowText(inFile);
	TCHAR Press = inFile.GetAt(inFile.GetLength()-1);
	//This will give you the last charachter that is input
	if(Press==' ')
	{
		inFile.Remove(' ');

		CFile file;
		if (!file.Open(inFile, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
			return;
		}

	
		file.Seek(0, CFile::begin);

		UINT bytesRead = file.Read(&ascHead, 3200L);

		if(bytesRead < 3200) 
			AfxMessageBox(_T("Ascii section header read wrong"), MB_ICONQUESTION);

		bytesRead=file.Read(&bh, 400L);

		if(bytesRead < 400) 
			AfxMessageBox(_T("Binary section header read wrong"), MB_ICONQUESTION);

		int nt= bh.hns;
		if (bh.hdt == 0) bh.hdt = bh.dto;
		float dt=(float)((double)bh.hdt/MIL);
	//	BOOL showwiener=FALSE;
		m_pnoise=0.001f;
		m_minlag=dt;
		m_maxlag=(m_tmax-m_tmin)/20.0f;
		m_nameIn=inFile;
		

		m_maxcorr=m_tmax;
		m_mincorr=m_tmin;
		m_dt=dt;
		
		file.Close();

		UpdateData(FALSE);

		if(bh.format != 1 )
		{
			AfxMessageBox(_T("Input data is not in a float format!"));
			m_pnoise=0.0;
			m_minlag=0.0;
			m_maxlag=0.0;
			m_nameIn=_T("");
			

			m_maxcorr=0.0;
			m_mincorr=0.0;
			m_dt=0.0;
			UpdateData(FALSE);
		}
		return;
	}

}
