// dlgLatLong.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgLatLong.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgLatLong dialog


CdlgLatLong::CdlgLatLong(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgLatLong::IDD, pParent)
	, m_isDistanceInline(TRUE)
{
	//{{AFX_DATA_INIT(CdlgLatLong)
	m_latLongMulti = _T("");
	m_isLonglat = FALSE;
	//}}AFX_DATA_INIT
}


void CdlgLatLong::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgLatLong)
	DDX_Text(pDX, IDC_EDIT_LATLONG_MULTI, m_latLongMulti);
	DDX_Check(pDX, IDC_CHECK1, m_isLonglat);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_CHECK_INLINE, m_isDistanceInline);
}


BEGIN_MESSAGE_MAP(CdlgLatLong, CDialog)
	//{{AFX_MSG_MAP(CdlgLatLong)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgLatLong message handlers
