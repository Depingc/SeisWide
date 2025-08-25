// dlgAgc.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgAgc.h"
//#include "afxdialogex.h"


// dlgAgc dialog

IMPLEMENT_DYNAMIC(dlgAgc, CDialog)

dlgAgc::dlgAgc(CWnd* pParent /*=NULL*/)
	: CDialog(dlgAgc::IDD, pParent)
	, m_inFile(_T(""))
	, m_outFile(_T(""))
	, m_window_sliding(0)
{

}

dlgAgc::~dlgAgc()
{
}

void dlgAgc::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INFILE, m_inFile);
	DDX_Text(pDX, IDC_OUTFILE, m_outFile);
	DDX_Text(pDX, IDC_PROC_WINDOW_SLIDING, m_window_sliding);
}


BEGIN_MESSAGE_MAP(dlgAgc, CDialog)
END_MESSAGE_MAP()


// dlgAgc message handlers
