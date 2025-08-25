// dlgHardwareId.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgHardwareId.h"


// CdlgHardwareId dialog

IMPLEMENT_DYNCREATE(CdlgHardwareId, CDHtmlDialog)

CdlgHardwareId::CdlgHardwareId(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(CdlgHardwareId::IDD, CdlgHardwareId::IDH, pParent)
	, m_HardwareId(_T(""))
	, m_licenseDetectedInfo(_T(""))
{

}

CdlgHardwareId::~CdlgHardwareId()
{
}

void CdlgHardwareId::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HARDID_MULTI, m_HardwareId);
	DDX_Text(pDX, IDC_EDIT_EMAIL, m_licenseDetectedInfo);
}

BOOL CdlgHardwareId::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(CdlgHardwareId, CDHtmlDialog)
END_MESSAGE_MAP()

BEGIN_DHTML_EVENT_MAP(CdlgHardwareId)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()



// CdlgHardwareId message handlers

HRESULT CdlgHardwareId::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;
}

HRESULT CdlgHardwareId::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;
}

