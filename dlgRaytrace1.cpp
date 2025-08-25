// dlgRaytrace1.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgRaytrace1.h"
#include "dlgRayAuto.h"
#include "segy.h"
#include "util.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgRaytrace1 dialog


CdlgRaytrace1::CdlgRaytrace1(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgRaytrace1::IDD, pParent)
	, m_kmShift(0)
	, m_isPrompt(FALSE)
{
	//{{AFX_DATA_INIT(CdlgRaytrace1)
	m_rayTracing = BLANK;
	m_velName = BLANK;
	m_floatName = BLANK;
	m_txName = BLANK;
	//}}AFX_DATA_INIT
}


void CdlgRaytrace1::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgRaytrace1)
	DDX_Text(pDX, IDC_DLG_RAYTRACING, m_rayTracing);
	DDX_Text(pDX, IDC_DLG_VELNAME, m_velName);
	DDX_Text(pDX, IDC_FLOATNAME, m_floatName);
	DDX_Text(pDX, IDC_TXNAME, m_txName);
	DDX_Text(pDX, IDC_KM_SHIFT, m_kmShift);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_PROMPT, m_isPrompt);
}


BEGIN_MESSAGE_MAP(CdlgRaytrace1, CDialog)
	//{{AFX_MSG_MAP(CdlgRaytrace1)
	ON_BN_CLICKED(IDC_BUTTON_BROWSERIN, OnButtonBrowserin)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, OnButtonBrowsev)
	ON_BN_CLICKED(IDC_BUTTON_AUTO, OnButtonAuto)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV2, OnButtonBrowseFloat)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_TX, OnButtonBrowseTx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgRaytrace1 message handlers

void CdlgRaytrace1::OnButtonBrowserin() 
{
	CFileDialog dlg( TRUE,_T("in"),_T("*.in"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Zelt's Ray Parameter File (*.in)|*.in|"));

	int pathLen = m_rayTracing.ReverseFind( '\\' );

	CString t = m_rayTracing.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_rayTracing = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DLG_RAYTRACING));
		pEdit->SetWindowText(m_rayTracing);
	}
	
	t.ReleaseBuffer();
}

void CdlgRaytrace1::OnButtonBrowsev() 
{
	//CFileDialog dlg( TRUE,_T("bin"),_T("*.bin"),
	//                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	//                 _T("SeisWide Binary Velocity Format (*.bin)|*.bin|"));

	CFileDialog dlg(TRUE, BLANK, _T("*.vxml;*.bin"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("File name for model (*.vxml)|*.vxml|Older format (*.bin)|*.bin|"));

	int pathLen = m_velName.ReverseFind( '\\' );
	CString t = m_velName.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_velName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_DLG_VELNAME));
		pEdit->SetWindowText(m_velName);
	}
	t.ReleaseBuffer();
	
}

void CdlgRaytrace1::OnButtonBrowseFloat() 
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
	int pathLen = this->m_floatName.ReverseFind( '\\' );
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

void CdlgRaytrace1::OnButtonAuto() 
{
	// TODO: Add your control notification handler code here
//	char s[7];
	int i;
	CString line[33], t,t1, newName;

	CdlgRayAuto dlg;
	dlg.m_tmax = tmax;
	dlg.m_tmin = tmin;
	dlg.m_vred = vred;
	dlg.m_xmax = xmax;
	dlg.m_xmin = xmin;

	if (!CCoords::isXShotDefined(xshot)) xshot = 0.f;

	dlg.m_xshot = xshot;  //OBS distance
	dlg.m_zshot = zshot;  //OBS depth.
	dlg.m_zmax = zmax;  //model max depth.
	dlg.m_zmin = zmin;  //model min depth.


	int pathLen = m_velName.ReverseFind( '\\' );
	int pathLen2 = m_velName.ReverseFind( '.' );
	if(pathLen>(-1) && pathLen2>pathLen+1) {
		CString txPath = m_velName.Left(pathLen);
		dlg.m_newName.Format( _T("%s\\r_%s.in"), txPath, m_velName.Mid(pathLen+1, pathLen2-pathLen-1) );
	}
	else {
		dlg.m_newName.Format( _T("r_%s.in"), m_velName );
	}



	dlg.m_timeSecName = timeSecName;
	if(dlg.DoModal() == IDOK) {
		tmax = dlg.m_tmax;
		tmin = dlg.m_tmin;
		vred = dlg.m_vred;
		xmax = dlg.m_xmax;
		xmin = dlg.m_xmin;
		xshot = dlg.m_xshot;
		zshot = dlg.m_zshot;
		zmax = dlg.m_zmax;
		zmin = dlg.m_zmin;
		newName = dlg.m_newName;
		timeSecName = dlg.m_timeSecName;
		if(GetBaseFileName(newName).IsEmpty()) {
			AfxMessageBox(_T("Please specify a file name"), MB_ICONQUESTION);
			return;
		}
	}
	else return;

	FILE *stream;
	if(_tfopen_s(&stream, newName, _T("w") ) != 0) {
		AfxMessageBox( _T("The new velocity file was not opened"), MB_ICONQUESTION);
		return;
	}

	line[0] =  " &pltpar iroute=1,iseg=0,iray=2,ibnd=0,irays=0,ivel=0,";
	line[1] = _T("	  itx=1,itxout=1,idump=0,iszero=0,idash=0,isum=1,iplot=0,");
	line[2] = SPACE;
	for (i=0; i<=2; i++)
		_ftprintf(stream, _T("%s\n"), (LPCTSTR)line[i]);
	_ftprintf(stream, _T("	  idata=0,vred=%6.3f,nrskip=3,nskip=0,velht=0.05,\n"), vred);
	line[4] = _T("          symht=0.1,itxbox=1,isep=0,istep=0,ircol=3,irayps=0,itcol=3,ifcol=6,ibcol=0,");
	line[5] = _T("          mcol=8,14,1,4,14,  colour=1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17");
	line[6] = _T("  /");
	for (i=4; i<=6; i++)
		_ftprintf(stream, _T("%s\n"), (LPCTSTR)line[i]);
	_ftprintf(stream, _T("  &axepar  xmin=%6.1f,xmax=%6.1f,xtmin=%6.1f,xtmax=%6.3f,xmm=200,ntickx=0,ndecix=-1,\n"), xmin,xmax,xmin,xmax);
	_ftprintf(stream, _T("           zmin=%6.1f,zmax=%6.1f,ztmin=%6.1f,ztmax=%6.3f,zmm=85,ntickz=0,ndeciz=-1,\n"),zmin,zmax,zmin,zmax);
	_ftprintf(stream, _T("           tmin=%6.1f,tmax=%6.1f,ttmin=%6.1f,ttmax=%6.1f,tmm=85,ntickt=0,ndecit=-1,\n"),tmin,tmax,tmin,tmax);
	_ftprintf(stream, _T("           xmint=%6.1f,xmaxt=%6.1f,xtmint=%6.1f,xtmaxt=%6.1f,xmm=200,ntickx=0,ndecix=-1,\n"),xmin,xmax,xmin,xmax);
	line[11] = _T("           iaxlab=1,itrev=0, sep=15, orig=15,");
	line[12] = _T("  /");
	line[13] = _T("  &trapar imodf=1, ibsmth=0, nbsmth=0, istop=0, crit=0., hws=1.0, iraysl=1,ifast=1,isrch=1,");
	line[14] = _T("  ishot=   2,");
	for (i=11; i<=14; i++)
		_ftprintf(stream, _T("%s\n"), (LPCTSTR)line[i]);
	_ftprintf(stream, _T("  xshot=%6.3f,\n"), xshot);
	_ftprintf(stream, _T("  zshot=%6.3f,\n"), zshot);
	line[17] = _T("  irayt=1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,");
	line[18] = _T("        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,");
	line[19] = _T("  ray = 1.2, 2.2, 3.1, 3.2, 4.1, 4.2, 5.1, 5.2, 6.1, 6.2, 7.1, 7.2, 8.1, 8.2, 9.1, 9.2,");
    line[20] = _T("  iturn=0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,");
	line[21] = _T("  nray =20,  35,  20,  15,  20,  15,  20,  30,  15,  20,  30,  15,  20,  30,  15,  30,");
	line[22] = _T("  xsmax=0.,nsmax=45,");
	line[23] = _T("  space=1.0, 2.0, 1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0, 2.0,");
	line[24] = _T("  aamin=0.05, aamax=85., step=0.05,\n  i2pt=0,n2pt=100,");
	line[25] = _T("  frbnd=0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,");
	line[26] = _T("  frbnd=0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,");
	line[27] = _T("  pois=0.5,0.25,0.25,0.25,0.25,0.25,0.25, 0.25,0.25,  nhray=30,");
	line[28] = _T("  /");
	line[29] = _T("  &invpar  invr=0,");
	line[30] = _T("  ivray= 2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8,  0,");
	line[31] = _T("           bndunc=0.08,   velunc=0.05,  ximax=2.0,");
	line[32] = _T("  /");
	for (i=17; i<=32; i++)
		_ftprintf(stream, _T("%s\n"), (LPCTSTR)line[i]);

	fclose(stream);

	showFileInNotePad(newName);
	m_rayTracing = newName;
	//end of generating ray parameters.

	UpdateData(FALSE);	//update the dialog box.
}


void CdlgRaytrace1::OnButtonEdit() 
{
	UpdateData(TRUE);
	showFileInNotePad(m_rayTracing);
	
}

void CdlgRaytrace1::OnButtonBrowseTx() 
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("in"),_T("*.in"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Travel-time picks (*.in)|*.in|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Travel-time picks |in |"));
#endif
	int pathLen = m_txName.ReverseFind( '\\' );
	CString t = m_txName.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if(pathLen>(-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if( dlg.DoModal()==IDOK )
	{
		m_txName = dlg.GetPathName();
		CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_TXNAME));
		pEdit->SetWindowText(m_txName);
	}
	t.ReleaseBuffer();
	
	
}
