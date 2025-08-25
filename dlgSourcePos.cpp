// dlgSourcePos.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgSourcePos.h"
#include "segy.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgSourcePos dialog


CdlgSourcePos::CdlgSourcePos(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgSourcePos::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgSourcePos)
	m_timeSecName = BLANK;
	m_xShot = 0.0f;
	m_options = 0;
//	m_xyFileName = BLANK;
//	m_recPlus = 0;
	m_isCalcFromUTM = 0;
	m_sx = 0.0f;
	m_sy = 0.0f;
	m_isSxsy = FALSE;
	m_sxOverwrite = FALSE;
	m_sxsyOverwrite = -1;
	m_isNegativeFromLat = FALSE;
	//}}AFX_DATA_INIT
}


void CdlgSourcePos::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgSourcePos)
	DDX_Text(pDX, IDC_REFNAME, m_timeSecName);
	DDX_Radio(pDX, IDC_RADIO_OPTIONS, m_options);
	DDX_Radio(pDX, IDC_RADIO_SXSY_OVERWRITE, m_sxsyOverwrite);
	DDX_Text(pDX, IDC_XSHOT, m_xShot);
	DDX_Text(pDX, IDC_DLG_PROCESSING_SX, m_sx);
	DDX_Text(pDX, IDC_DLG_PROCESSING_SY, m_sy);
	DDX_Check(pDX, IDC_CHECK_ISNEGTIVE_FROMLAT, m_isNegativeFromLat);
//	DDX_Radio(pDX, IDC_RADIO_CALC, m_isCalcFromUTM);
//	DDX_Text(pDX, IDC_DLG_XYFILENAME, m_xyFileName);

//	DDX_Radio(pDX, IDC_RADIO_RECPLUS, m_recPlus);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgSourcePos, CDialog)
	//{{AFX_MSG_MAP(CdlgSourcePos)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, OnButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH, OnButtonRefresh)
	//ON_BN_CLICKED(IDC_BUTTON_BROWSETXT, OnButtonBrowsetxt)
//	ON_BN_CLICKED(IDC_RADIO_CALC, OnRadioCalc)
	ON_BN_CLICKED(IDC_RADIO6, OnRadio6)
	ON_EN_CHANGE(IDC_DLG_PROCESSING_SX, OnChangeDlgProcessingSx)
	ON_EN_CHANGE(IDC_DLG_PROCESSING_SY, OnChangeDlgProcessingSy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgSourcePos message handlers

void CdlgSourcePos::OnButtonBrowset() 
{

	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy Time Section (*.sgy)|*.sgy|"));
	if( dlg.DoModal()==IDOK )
	{
		bhed bh;
		BOOL isUnix = false;
		m_timeSecName = dlg.GetPathName();
		CFile file;
		if (!file.Open(m_timeSecName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
			return;
		}

		
		LONGLONG bytesRead = file.Seek(3200L, CFile::begin);
		if(bytesRead < 3200) 
		{
			file.Close();
			AfxMessageBox(_T("Ascii section header read wrong"), MB_ICONQUESTION);
			return;
		}
		bytesRead = file.Read(&bh, 400);
		if(bytesRead < 400) 
		{
			file.Close();
			AfxMessageBox(_T("Binary section header read wrong"), MB_ICONQUESTION);
			return;
		}
		if (isUnix = IsUnix(bh.format)) {
			file.Close();
			AfxMessageBox(_T("Sorry, please convert to PC format (small-endian) before continue."), MB_ICONQUESTION);
			return;
		}
		
		m_xShot = bh.obsPositionX;
		file.Close();

		//Finish loading segy headers.
		
		
		UpdateData(FALSE);
	}

}

void CdlgSourcePos::OnButtonRefresh() 
{
		//Load segy headers
		bhed bh;
		//segy tr;
		BOOL isUnix = false;
		if(!isFileExistNotDirectory(m_timeSecName)) return;

		CFile file;
		if (!file.Open(m_timeSecName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
			return;
		}
		
		LONGLONG bytesRead = file.Seek(3200L, CFile::begin);
		if(bytesRead < 3200) 
		{
			file.Close();
			AfxMessageBox(_T("Ascii section header read wrong"), MB_ICONQUESTION);
			return;
		}
		bytesRead = file.Read(&bh, 400);
		if(bytesRead < 400) 
		{
			file.Close();
			AfxMessageBox(_T("Binary section header read wrong"), MB_ICONQUESTION);
			return;
		}
		if (isUnix = IsUnix(bh.format)) {
			file.Close();
			AfxMessageBox(_T("Sorry, please convert to PC format (small-endian) before continue."), MB_ICONQUESTION);
			return;
		}
		m_xShot = bh.obsPositionX;
		file.Close();

	
		//Finish loading segy headers.

		m_options = 0;
		
		UpdateData(FALSE);

	
}

//void CdlgSourcePos::OnButtonBrowsetxt() 
//{
//#ifndef _MAC
//	CFileDialog dlg( TRUE,_T("TXT"),_T("*.TXT"),
//	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
//	                 _T("FFID-Distance Numbers (*.TXT)|*.TXT|"));
//#else
//	CFileDialog dlg( TRUE,NULL,NULL,
//	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
//	                 _T("FFID-Distance Numbers |TXT |"));
//#endif
//	if( dlg.DoModal()==IDOK )
//	{
//		m_options = 2;
//		//m_xyFileName = dlg.GetPathName();
//		UpdateData(FALSE);
//	}
//}


void CdlgSourcePos::OnRadioCalc() 
{
	// radio isCalc clicked
	m_options = 1;	
	m_isCalcFromUTM = 0;
	UpdateData(FALSE);
	
}

void CdlgSourcePos::OnRadio6() 
{
	// radio isCalc clicked
	m_options = 1;	
	m_isCalcFromUTM = 1;
	UpdateData(FALSE);
	
}

void CdlgSourcePos::OnChangeDlgProcessingSx() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here


	CButton* pCheck = (CButton*)GetDlgItem(IDC_RADIO3);
	pCheck->SetCheck(1);

	pCheck = (CButton*)GetDlgItem(IDC_RADIO_SXSY_OVERWRITE);
	pCheck->SetCheck(0);
	
}

void CdlgSourcePos::OnChangeDlgProcessingSy() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	CButton* pCheck = (CButton*)GetDlgItem(IDC_RADIO3);
	pCheck->SetCheck(1);

	pCheck = (CButton*)GetDlgItem(IDC_RADIO_SXSY_OVERWRITE);
	pCheck->SetCheck(0);
}
