// dlgSonobuoyAutoOffset.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "segy.h"
#include "util.h"
#include "dlgSonobuoyAutoOffset.h"


// CdlgSonobuoyAutoOffset dialog

IMPLEMENT_DYNAMIC(CdlgSonobuoyAutoOffset, CDialog)

CdlgSonobuoyAutoOffset::CdlgSonobuoyAutoOffset(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgSonobuoyAutoOffset::IDD, pParent)
	, m_timeSecName(BLANK)
	, m_long(0)
	, m_lat(0)
	, m_factor(0)
	, m_markupError(FALSE)
{

}

CdlgSonobuoyAutoOffset::~CdlgSonobuoyAutoOffset()
{
}

void CdlgSonobuoyAutoOffset::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DLG_PROCESSING_SX, m_long);
	DDX_Text(pDX, IDC_DLG_PROCESSING_SY, m_lat);
	DDX_Check(pDX, IDC_CHECK_MARKUP_ERROR, m_markupError);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);

	DDX_Text(pDX, IDC_DLG_FACTOR, m_factor);
	//	DDX_Check(pDX, IDC_CHK_FACTORMIL, m_isFactorMIL);
}


BEGIN_MESSAGE_MAP(CdlgSonobuoyAutoOffset, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, &CdlgSonobuoyAutoOffset::OnBnClickedButtonBrowset)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET2, &CdlgSonobuoyAutoOffset::OnBnClickedButtonBrowset2)
	ON_BN_CLICKED(IDHELP, &CdlgSonobuoyAutoOffset::OnBnClickedHelp)
//	ON_BN_CLICKED(IDC_CHK_FACTORMIL, &CdlgSonobuoyAutoOffset::OnBnClickedChkFactormil)
END_MESSAGE_MAP()


// CdlgSonobuoyAutoOffset message handlers

void CdlgSonobuoyAutoOffset::OnBnClickedButtonBrowset()
{
	CFileDialog dlg( TRUE,_T("sgy"),_T("*.sgy"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Segy Time Section (*.sgy)|*.sgy|"));
	
	if( dlg.DoModal()==IDOK )
	{
		m_timeSecName = dlg.GetPathName();
	}
	
}

void CdlgSonobuoyAutoOffset::OnBnClickedButtonBrowset2()
{
	UpdateData(TRUE);
	CFile file;
	if (!file.Open(this->m_timeSecName, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone))
		return;

	segy tr;
	bhed bh;
	BOOL isUnix = false;
	file.Seek(3200L, CFile::current);
	file.Read(&bh, 400L);
	if (isUnix = IsUnix(bh.format)) SwapTapeHeader(&bh);

	if (file.Read(&tr, 240) == (UINT)240)
	{
			if (isUnix) {
				SwapTraceHeader(false, false, false, &tr);
			}

			register double omil = 1.;
			if (m_markupError) {
				if (tr.counit == 2) omil = 1. / (double)MIL;
				else if (tr.counit == 0) omil = 1. / 360000.;
			}
			else {
				if (tr.counit == 2) omil = 1. / 360000.;
				else if (tr.counit == 0) omil = 1. / (double)MIL;
			}

			this->m_long = (float)(tr.gx * omil * m_factor);
			this->m_lat = (float)(tr.gy * omil * m_factor);
	}
	file.Close();
		UpdateData(FALSE);
		Invalidate();
}

void CdlgSonobuoyAutoOffset::OnBnClickedHelp()
{
	myShowHelp("index.html");
}


