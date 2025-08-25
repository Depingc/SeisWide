// dlgSegyPreview.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgSegyPreview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgSegyPreview dialog


CdlgSegyPreview::CdlgSegyPreview(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgSegyPreview::IDD, pParent)
{
	//{{AFX_DATA_INIT(CdlgSegyPreview)
	m_headInfo = _T("");
	m_timeSecName = _T("");
	m_tmax = 0.0f;
	m_tmin = 0.0f;
	m_tTick = 0.0f;
	m_timeSecTitle = _T("");
	m_faceDown = -1;
	m_clip = 0.0f;
	m_fillOff = 0.0f;
	m_gain = 0.0f;
	m_shot1 = 0;
	m_shot2 = 0;
	m_shotStep = 0;
	m_wiggle = -1;
	//}}AFX_DATA_INIT
}


void CdlgSegyPreview::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgSegyPreview)
	DDX_Text(pDX, IDC_TIMESEC_TITLE, m_timeSecTitle);
	DDX_Text(pDX, IDC_TIMESECNAME, m_timeSecName);
	DDX_Text(pDX, IDC_VIEW_UPDATE_SHOT1, m_shot1);
	DDX_Text(pDX, IDC_VIEW_UPDATE_SHOT2, m_shot2);
	DDX_Text(pDX, IDC_VIEW_UPDATE_SHOTSTEP, m_shotStep);
	DDX_Text(pDX, IDC_VIEW_UPDATE_GAIN, m_gain);
	DDX_Text(pDX, IDC_VIEW_UPDATE_CLIP, m_clip);
	DDX_Text(pDX, IDC_VIEW_UPDATE_FILLOFF, m_fillOff);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMIN, m_tmin);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TMAX, m_tmax);
	DDX_Text(pDX, IDC_VIEW_UPDATE_TTICK3, m_tTick);
	DDX_Radio(pDX, IDC_VIEW_WIGGLE, m_wiggle);
	DDX_Radio(pDX, IDC_VIEW_FACEDOWN, m_faceDown);

	DDX_CBString(pDX, IDC_COMBO_HEADINFO, m_headInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgSegyPreview, CDialog)
	//{{AFX_MSG_MAP(CdlgSegyPreview)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgSegyPreview message handlers
