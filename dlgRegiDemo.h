#pragma once
//#include "stdafx.h"
#include <vector>
#include <string>

// CdlgRegiDemo dialog

class CdlgRegiDemo : public CDialogEx
{
	DECLARE_DYNAMIC(CdlgRegiDemo)

public:
	CdlgRegiDemo(CWnd* pParent = NULL);   // standard constructor
	virtual ~CdlgRegiDemo();

// Dialog Data
	enum { IDD = IDD_DLG_REGI_DEMO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_name;
	CString m_email;
	CString m_phone;
	CString m_org;
	CString m_comments;
	CString m_HardwareId;  // this field is hidden to the user, but will be used when submitting to server
	CString m_license;
	CString m_serverRoot;
	//void escape(std::wstr
	void escape(std::string &data);  // not used
	CString escape(CString &data);
	bool grabDataFromServer(CString hid);
	void saveRegiToLocal();

	afx_msg void OnBnClickedOk();
};
