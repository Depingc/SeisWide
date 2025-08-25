// ShotTable.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "ShotTable.h"


// CShotTable dialog

IMPLEMENT_DYNAMIC(CShotTable, CDialog)

CShotTable::CShotTable(CWnd* pParent /*=NULL*/)
	: CDialog(CShotTable::IDD, pParent)
	, m_long(0)
	, m_lat(0)
{

}

CShotTable::~CShotTable()
{
}

void CShotTable::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DLG_PROCESSING_SX, m_long);
	DDX_Text(pDX, IDC_DLG_PROCESSING_SY, m_lat);
}


BEGIN_MESSAGE_MAP(CShotTable, CDialog)
END_MESSAGE_MAP()


// CShotTable message handlers
