// dlgNewVelFile.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgNewVelFile.h"
#include "util.h"
#include "segyMeta.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgNewVelFile dialog


CdlgNewVelFile::CdlgNewVelFile(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgNewVelFile::IDD, pParent)
	, m_title(BLANK)
	, m_cruise(BLANK)
	, m_author(BLANK)
	, m_company(BLANK)
	, m_date(BLANK)
	, m_startLat(0)
	, m_startLong(0)
	, m_endLat(0)
	, m_endLong(0)
	, m_deployLat(0)
	, m_deployLong(0)
	, m_secName(BLANK)
	, m_wellLat(0)
	, m_wellLong(0)
	, m_wellDist(0)
	, m_isNoNodeNav(FALSE)
	, m_isViewXml(FALSE)
	, m_maxHorizonValid(0)
	, m_isVelBelowValid(FALSE)
	, m_deployDist(0)
	, m_deployDepth(0)
	, m_startDist(0)
	, m_endDist(0)
	, m_wellTitle(BLANK)
{
	//{{AFX_DATA_INIT(CdlgNewVelFile)
	m_velNewFileName = BLANK;
	//}}AFX_DATA_INIT
}


void CdlgNewVelFile::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgNewVelFile)
	DDX_Text(pDX, IDC_VEL_NEWFILE, m_velNewFileName);
	CString tStr = GetFileExtension(m_velNewFileName);

	//m_velNewFileName.Replace(_T(".bin"), _T(".vxml")); // enforce vxml
    DDX_Radio(pDX, IDC_VEL_FORMAT, m_velFormat);
//	m_velFormat = (tStr.Compare(_T("vxml")) == 0) ? 4 : 0;

	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_IS_VIEWXML, m_isViewXml);
	DDX_Text(pDX, IDC_VEL_MAX_HORIZON_VALID, m_maxHorizonValid);
	DDX_Check(pDX, IDC_CHECK_NO_NODE_NAV3, m_isVelBelowValid);
	DDX_Text(pDX, IDC_VEL_AUTHOR, m_author);
	DDX_Text(pDX, IDC_VEL_COMPANY, m_company);
	DDX_Text(pDX, IDC_VEL_CRUISE, m_cruise);
	DDX_Check(pDX, IDC_CHECK_NO_NODE_NAV, m_isNoNodeNav);
	DDX_Text(pDX, IDC_VEL_SECNAME, m_secName);
	DDX_Text(pDX, IDC_VEL_TITLE, m_title);
	DDX_Text(pDX, IDC_VEL_DEPLOY_DIST, m_deployDist);
	DDX_Text(pDX, IDC_VEL_DEPLOY_DEPTH, m_deployDepth);
	DDX_Text(pDX, IDC_VEL_DEPLOYLONG, m_deployLong);
	DDX_Text(pDX, IDC_VEL_DEPLOYLAT, m_deployLat);
	DDX_Text(pDX, IDC_VEL_STARTDIST, m_startDist);
	DDX_Text(pDX, IDC_VEL_STARTLONG, m_startLong);
	DDX_Text(pDX, IDC_VEL_STARTLAT, m_startLat);
	DDX_Text(pDX, IDC_VEL_ENDDIST, m_endDist);
	DDX_Text(pDX, IDC_VEL_ENDLONG, m_endLong);
	DDX_Text(pDX, IDC_VEL_ENDLAT, m_endLat);
	DDX_Text(pDX, IDC_VEL_TITLE2, m_wellTitle);
	DDX_Text(pDX, IDC_VEL_WELL_DIST, m_wellDist);
	DDX_Text(pDX, IDC_VEL_WELL_LONG, m_wellLong);
	DDX_Text(pDX, IDC_VEL_WELL_LAT, m_wellLat);
	DDX_Text(pDX, IDC_VEL_DATE, m_date);
}


BEGIN_MESSAGE_MAP(CdlgNewVelFile, CDialog)
	//{{AFX_MSG_MAP(CdlgNewVelFile)
	ON_BN_CLICKED(IDC_BUTTON_BROWSEV, OnButtonBrowsev)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_VEL_FORMAT, &CdlgNewVelFile::OnBnClickedVelFormat)
	ON_BN_CLICKED(IDC_RADIO18, &CdlgNewVelFile::OnBnClickedRadio18)
	ON_BN_CLICKED(IDC_RADIO2, &CdlgNewVelFile::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO16, &CdlgNewVelFile::OnBnClickedRadio16)
	ON_BN_CLICKED(IDC_RADIO17, &CdlgNewVelFile::OnBnClickedRadio17)
	ON_BN_CLICKED(IDC_BUTTON_CALC, &CdlgNewVelFile::OnBnClickedButtonCalc)
	ON_EN_CHANGE(IDC_VEL_WELL_DIST, &CdlgNewVelFile::OnEnChangeVelWellDist)
//	ON_EN_CHANGE(IDC_VEL_ENDDIST, &CdlgNewVelFile::OnEnChangeVelEnddist)
//	ON_EN_CHANGE(IDC_VEL_STARTDIST, &CdlgNewVelFile::OnEnChangeVelStartdist)
ON_BN_CLICKED(IDC_BUTTON_CALC2, &CdlgNewVelFile::OnBnClickedButtonCalc2)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgNewVelFile message handlers

void CdlgNewVelFile::OnButtonBrowsev()
{
	CFileDialog dlg(FALSE, BLANK, _T("*.bin; *.vxml"),
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("File name for new model (*.vxml)|*.vxml|Older format (*.bin)|*.bin|"));
	//CFileDialog dlg( FALSE,_T("bin"),_T("*.bin"),
	//                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	//                 _T("SeisWide Binary Velocity Format (*.bin)|*.bin|"));

	int pathLen = m_velNewFileName.ReverseFind('\\');
	CString t = m_velNewFileName.Left(pathLen);
	LPCTSTR lpszFileName = t.GetBufferSetLength(t.GetLength());
	if (pathLen > (-1))
	{
		dlg.m_ofn.lpstrInitialDir = lpszFileName;
	}

	if (dlg.DoModal() == IDOK)
	{
		m_velNewFileName = dlg.GetPathName();
		UpdateData(FALSE);
	}
	t.ReleaseBuffer();
}


void CdlgNewVelFile::OnBnClickedVelFormat()
{
	// .bin format clicked
	UpdateData(TRUE);
	CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VEL_NEWFILE));
	if (isEndWith(m_velNewFileName, _T(".in"))) {
		m_velNewFileName.Replace(_T(".in"), _T(".bin"));
		pEdit->SetWindowText(m_velNewFileName);
	}
	else if (isEndWith(m_velNewFileName, _T(".vxml"))) {
		m_velNewFileName.Replace(_T(".vxml"), _T(".bin"));
		pEdit->SetWindowText(m_velNewFileName);
	}
}


void CdlgNewVelFile::OnBnClickedRadio18()
{
	// .vxml format clicked
	UpdateData(TRUE);
	CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VEL_NEWFILE));
	if (isEndWith(m_velNewFileName, _T(".in"))) {
		m_velNewFileName.Replace(_T(".in"), _T(".vxml"));
		pEdit->SetWindowText(m_velNewFileName);
	}
	else if (isEndWith(m_velNewFileName, _T(".bin"))) {
		m_velNewFileName.Replace(_T(".bin"), _T(".vxml"));
		pEdit->SetWindowText(m_velNewFileName);
	}
}


void CdlgNewVelFile::OnBnClickedRadio2()
{
	// .in format (2 decimal) clicked
	UpdateData(TRUE);
	CEdit *pEdit = static_cast<CEdit*> (GetDlgItem(IDC_VEL_NEWFILE));
	if (isEndWith(m_velNewFileName, _T(".bin"))) {
		m_velNewFileName.Replace(_T(".bin"), _T(".in"));
		pEdit->SetWindowText(m_velNewFileName);
	}
	else if (isEndWith(m_velNewFileName, _T(".vxml"))) {
		m_velNewFileName.Replace(_T(".vxml"), _T(".in"));
		pEdit->SetWindowText(m_velNewFileName);
	}
}


void CdlgNewVelFile::OnBnClickedRadio16()
{
	// .in format (3 decimal) clicked
	OnBnClickedRadio2();
}


void CdlgNewVelFile::OnBnClickedRadio17()
{
	// .in format (4 decimal) clicked
	OnBnClickedRadio2();
}





void CdlgNewVelFile::OnBnClickedButtonCalc()
{
	UpdateData(TRUE);
	if (this->m_isNoNodeNav) {
		for (int icont = 0; icont < m_velm->ncont; icont++) {
			for (register int j = 0; j < m_velm->nzed[icont]; j++) {
				m_velm->latitude[j][icont] = ERRORVAL;
				m_velm->longitude[j][icont] = ERRORVAL;
			}
		}
	}

	HEAD head;
	CSegyMeta::segyHeadInit(this->m_secName, &head);
	if (head.numTraces < 1) {
		AfxMessageBox(_T("Sorry, cannot open the reflection profile for computing navigation values."), MB_ICONQUESTION);
		return;
	}

	// need to compute startLatLong[] and endLatLong[]
	// note: [xmin, xmax] may be smaller or bigger than the area of [startLatLong, endLatLong]
	// CVelMod::setLatlongs(m_velm, &head);  
	// 1. for xmin and xmax only
	// 2. for all nodes of all boundaries
	CSegyMeta::sortDistance(&head, false);
	float a, b;  // to be overwritten
	float xTrc;
	if (this->m_startLat == ERRORVAL) {
		// note: this->m_startDist and  m_velm->xmin are model distance
		//       but head->dist[] are segy trace distances, and 
		//		 head->dist[]+m_xShift equal model distances
		xTrc = (float)(m_velm->getXmin() - m_xShift);
		this->m_startDist = 
			(float)myFloat(CSegyMeta::getlatLongFromSegyMod(&head, xTrc, a, b) + m_xShift);
		this->m_startLat = (float)a;
		this->m_startLong = (float)b;
	}
	if (this->m_endLat == ERRORVAL) {
		xTrc = (float)(m_velm->getXmax() - m_xShift);
		this->m_endDist = 
			(float)myFloat(CSegyMeta::getlatLongFromSegyMod(&head, xTrc, a, b) + m_xShift);
		this->m_endLat = (float)a;
		this->m_endLong = (float)b;
	}
	if (this->m_wellLat == ERRORVAL) {
		xTrc = (float)(m_wellDist - m_xShift);
		this->m_wellDist =
			(float)myFloat(CSegyMeta::getlatLongFromSegyMod(&head, xTrc, a, b) + m_xShift);
		this->m_wellLat = (float)a;
		this->m_wellLong = (float)b;
	}

	if (!this->m_isNoNodeNav && head.numTraces > 0) {
		for (int icont = 0; icont < m_velm->ncont; icont++) {
			for (register int j = 0; j < m_velm->nzed[icont]; j++) {
				if (m_velm->latitude[j][icont] == ERRORVAL) {
					xTrc = (float)(m_velm->xm[j][icont] - m_xShift);
					CSegyMeta::getlatLongFromSegyMod(&head, xTrc, a, b); 
						// here we cannot overwrite (m_velm->xm[j][icont], otherwise the model may be changed
					m_velm->latitude[j][icont] = (float)a;
					m_velm->longitude[j][icont] = (float)b;
				}
			}
		}
	}

	CSegyMeta::segyHeadFree(&head);
	this->m_velFormat = 4;
	m_velNewFileName.Replace(_T(".bin"), _T(".vxml"));
	UpdateData(FALSE);
}

//
//CEdit *dis = reinterpret_cast<CEdit *>(GetDlgItem(IDC_VEL_STARTLAT));
//CString t;
//dis->GetWindowTextW(t);
//dis->SetWindowText(t + _T("Ok"));





void CdlgNewVelFile::OnEnChangeVelWellDist()
{
	// when m_wellDist is changed, its lat/long values must be re-calculated
	UpdateData(TRUE);
	this->m_wellLat = ERRORVAL;
	this->m_wellLong = ERRORVAL;
	// UpdateData(FALSE);
}


//void CdlgNewVelFile::OnEnChangeVelEnddist()
//{
//	// TODO:  If this is a RICHEDIT control, the control will not
//	// send this notification unless you override the CDialog::OnInitDialog()
//	// function and call CRichEditCtrl().SetEventMask()
//	// with the ENM_CHANGE flag ORed into the mask.
//
//	UpdateData(TRUE);
//	this->m_endLat = ERRORVAL;
//	this->m_endLong = ERRORVAL;
//	UpdateData(FALSE);
//}


//void CdlgNewVelFile::OnEnChangeVelStartdist()
//{
//	// TODO:  If this is a RICHEDIT control, the control will not
//	// send this notification unless you override the CDialog::OnInitDialog()
//	// function and call CRichEditCtrl().SetEventMask()
//	// with the ENM_CHANGE flag ORed into the mask.
//
//	UpdateData(TRUE);
//	this->m_startLat = ERRORVAL;
//	this->m_startLong = ERRORVAL;
//	UpdateData(FALSE);
//}


void CdlgNewVelFile::OnBnClickedButtonCalc2()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	double a;
	a = m_startDist;
	m_startDist = m_endDist;
	m_endDist = (float)a;

	a = m_startLat;
	m_startLat = m_endLat;
	m_endLat = (float)a;

	a = m_startLong;
	m_startLong = m_endLong;
	m_endLong = (float)a;
	UpdateData(FALSE);
}
