// dlgRegiDemo.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgRegiDemo.h"
//#include "afxdialogex.h"
#include "AutoUpdater.h"
#include "IniReader.h"
#include "util.h"

// CdlgRegiDemo dialog

IMPLEMENT_DYNAMIC(CdlgRegiDemo, CDialogEx)

CdlgRegiDemo::CdlgRegiDemo(CWnd* pParent /*=NULL*/)
	: CDialogEx(CdlgRegiDemo::IDD, pParent)
	, m_name(_T(""))
	, m_email(_T(""))
	, m_phone(_T(""))
	, m_org(_T(""))
	, m_comments(_T(""))
{

}

CdlgRegiDemo::~CdlgRegiDemo()
{
}

void CdlgRegiDemo::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_NAME, m_name);
	DDV_MaxChars(pDX, m_name, 50);
	DDX_Text(pDX, IDC_EDIT_EMAIL, m_email);
	DDV_MaxChars(pDX, m_email, 100);
	DDX_Text(pDX, IDC_EDIT_PHONE, m_phone);
	DDV_MaxChars(pDX, m_phone, 30);
	DDX_Text(pDX, IDC_EDIT_ORG, m_org);
	DDV_MaxChars(pDX, m_org, 200);
	DDX_Text(pDX, IDC_EDIT_COMMENTS, m_comments);
	DDV_MaxChars(pDX, m_comments, 1000);
}


BEGIN_MESSAGE_MAP(CdlgRegiDemo, CDialogEx)
	ON_BN_CLICKED(IDOK, &CdlgRegiDemo::OnBnClickedOk)
END_MESSAGE_MAP()


// CdlgRegiDemo message handlers
void CdlgRegiDemo::escape(std::string &data)
{
    std::string buffer;
    buffer.reserve((unsigned int)(data.size()*1.1));
    for(size_t pos = 0; pos != data.size(); ++pos) {
        switch(data[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&apos;");      break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[pos], 1); break;
        }
    }
    data.swap(buffer);
}

CString CdlgRegiDemo::escape(CString &data)
{
	CString buf = _T("");
	for(int pos = 0; pos != data.GetLength(); ++pos) {
        switch(data[pos]) {
            case '&':  buf += _T("&amp;");       break;
            case '\"': buf += _T("&quot;");      break;
            case '\'': buf += _T("&apos;");      break;
            case '<':  buf += _T("&lt;");        break;
            case '>':  buf += _T("&gt;");        break;
            default:   buf += data[pos]; break;
        }
    }
	return buf;
}


bool CdlgRegiDemo::grabDataFromServer(CString hid)
{
	// After clicking OK, this function will go to the server to grab information about this hardID. 
	// and then enable the associated boxes, and then populate this dialog box.

	// need to escape each one of the variables here before constructing the string and then submit
	// m_serverRoot = http://seiswide.hopto.org/seiswide/
	CString url = m_serverRoot + _T("lic.php?hardid=");
	url += escape(m_HardwareId); 
	url += _T("&name="); url += escape(m_name);
	url += _T("&email="); url += escape(m_email);
	url += _T("&phone="); url += escape(m_phone);
	url += _T("&org="); url += escape(m_org);
	url += _T("&comments="); url += escape(m_comments);


	CAutoUpdater updater;
	CString aFields = updater.GetUrlContents(url);
	// http://globalrescue.hopto.org/seiswide/lic.php?hardid=0050-56C0-0008&name=Deping&org=Deping%20at%20Home&email=deping.chian@gmail.com&phone=902-449-8907
	//int dd = aFields.Find(_T("[Setting]"));


	if (aFields.Find(_T("[Setting]")) < 0) {
		// the returned string does have license info cotnained
		m_license = aFields.Trim();

		// only if the license string contains space, it is invalid
		int iii = m_license.Find(SPACE);
		int nnn = m_license.GetLength();
		//if (m_license.Find(SPACE) > 0)
		//	return false;
	}
	else {
		// now need to parse for the 6 pieces of strings
		// Note: there are 2 versions of info: 1 version from the server, and the other version is from the local file
		if (aFields.Find(_T("name=\norg")) < 0 && aFields.Find(_T("email=\nphone")) < 0) {
			saveStringToRegiFile(aFields);
		}

		// now pull the info from the local file
		CString tStr = 	regiTempFileName();
		CIniReader iniReader(tStr);
		m_email = iniReader.ReadString(_T("Setting"), _T("email"));
		m_name = iniReader.ReadString(_T("Setting"), _T("name"));
		m_phone = iniReader.ReadString(_T("Setting"), _T("phone"));
		m_org = iniReader.ReadString(_T("Setting"), _T("org"));
		m_comments = iniReader.ReadString(_T("Setting"), _T("comments"));
		m_license = _T("");  // no license issued yet 
	}
	return true;
}

void CdlgRegiDemo::saveRegiToLocal() {
	// now save the regi info to local file for later usage
	CStringArray *strArr = new CStringArray();
	strArr->Add(_T("[Setting]"));
	strArr->Add(BLANK);
	strArr->Add(_T("hardid=") + m_HardwareId);
	strArr->Add(_T("name=") + m_name);
	strArr->Add(_T("org=") + m_org);
	strArr->Add(_T("email=") + m_email);
	strArr->Add(_T("phone=") + m_phone);
	strArr->Add(_T("comments=") + m_comments);
	CString tStr = regiTempFileName();
	saveStringArrayToFile(strArr, tStr);
	strArr->RemoveAll();
	delete strArr;
}

void CdlgRegiDemo::OnBnClickedOk()
{
	grabDataFromServer(m_HardwareId);
	CDialogEx::OnOK();
}
