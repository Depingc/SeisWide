// dlgLayerNodes.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgLayerNodes.h"
#include "MainFrm.h"
//#include "VelMod.h"
#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CdlgLayerNodes dialog


CdlgLayerNodes::CdlgLayerNodes(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgLayerNodes::IDD, pParent)
	, m_anisotropy(0)
	, m_apply4All(FALSE)
	, m_anisotropy_layer0(0)
	, m_strRegression(_T(""))
	, m_isRegressionChecked(FALSE)
	, m_strRegressionX(_T(""))
{
	//{{AFX_DATA_INIT(CdlgLayerNodes)
	m_layerNum = 0;
	m_layerV2 = (float)0.0;
	m_layerX = (float)0.0;
	//m_layerY = (float)0.0;
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

//	m_waterDepth = 0.f;  // manual addition by Deping
	m_layerV_backup = 0.f;// manual addition by Deping

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	this->m_strRegression.Format(_T("%g     %g     %g"), 
		(float)pFrame->psei->regionalModelV0, 
		(float)pFrame->psei->regionalModelVinf, 
		(float)pFrame->psei->regionalModelAlpha);
//	m_isSedimentLayer = FALSE;
	m_isTimeDomain = true;
}


void CdlgLayerNodes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CdlgLayerNodes)
	DDX_Text(pDX, IDC_LAYERV, m_layerV2);
	DDX_Check(pDX, IDC_CHECK_APPLY4ALL, m_apply4All);
	DDX_Check(pDX, IDC_CHECK_REGRESSION, m_isRegressionChecked);
	DDX_Text(pDX, IDC_STR_REGRESSION, m_strRegression);
	DDX_Text(pDX, IDC_STR_REGRESSION_X, m_strRegressionX);
	DDX_Text(pDX, IDC_LAYER_ANISOTROPY, m_anisotropy);
	DDX_Text(pDX, IDC_LAYER0, m_anisotropy_layer0);
	DDV_MinMaxInt(pDX, m_anisotropy_layer0, -1, 99);

	DDX_Text(pDX, IDC_LAYERNUM, m_layerNum);
	DDV_MinMaxInt(pDX, m_layerNum, 0, 99);
	DDX_Text(pDX, IDC_LAYERX, m_layerX);
	//DDX_Text(pDX, IDC_LAYERY, m_layerY);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CdlgLayerNodes, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_AUTO, &CdlgLayerNodes::OnBnClickedButtonAuto)
	ON_BN_CLICKED(IDC_BUTTON_UNDO, &CdlgLayerNodes::OnBnClickedButtonUndo)
	ON_EN_CHANGE(IDC_LAYERV, &CdlgLayerNodes::OnEnChangeLayerv)
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CdlgLayerNodes message handlers

BOOL CdlgLayerNodes::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// note:  m_layerV  is in meters/s, while m_regionalSedVmin is in km/s
	//CWnd* pwnd;
//	m_isSedimentLayer = m_layerNum == NUM_WATER_LAYER + 1;
	//if (!isSedimentLayer()) {
	//	pwnd = GetDlgItem(IDC_CHECK_REGRESSION); if (pwnd) pwnd->EnableWindow(FALSE);
	//	pwnd = GetDlgItem(IDC_STR_REGRESSION); if (pwnd) pwnd->EnableWindow(FALSE);
	//}

	m_layerX = (float)m_modX->dist;
	double v1 = m_modX->vTop[m_layerNum - 1];
	// double v2 = m_modX->vBot[m_layerNum - 1];
	
	m_isTop = myFloat(v1, 5) == myFloat(m_layerV2*0.001, 5);
	m_layerY = m_isTop ? (float)m_modX->depthMod[m_layerNum - 1] : (float)m_modX->depthMod[m_layerNum];
	//if (m_isTimeDomain) {
	//	m_layerY = m_isTop ? (float)m_modX->timeMod[m_layerNum - 1] : (float)m_modX->timeMod[m_layerNum];
	//}
	//else {
	//	m_layerY = m_isTop ? (float)m_modX->depthMod[m_layerNum - 1] : (float)m_modX->depthMod[m_layerNum];
	//}
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CdlgLayerNodes::OnBnClickedButtonAuto()
{
	UpdateData(TRUE);
	//CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	//float regionalSedVmin = (float)pFrame->psei->regionalSedVmin;
	//float regionalSedVmax = (float)pFrame->psei->regionalSedVmax;

	if (this->m_layerNum <= NUM_WATER_LAYER) return;

		// NOTE: this->m_depthWater is in km, while  this->m_layerV is in m/s!!!
		this->m_layerV_backup = this->m_layerV2;

		// Al-Chalaby's formula
		// this->m_strRegression.Format(_T("%f %f %f"), pFrame->psei->regionalModelV0, pFrame->psei->regionalModelVinf, pFrame->psei->regionalModelAlpha);
		// John's formula for the Artic, Category 3, centrl Canada basin
		float waterDepth = (float)m_modX->depthMod[NUM_WATER_LAYER];
		this->m_layerV2 = (float)(1000. * m_modX->getRegVelAtZ(this->m_layerY - waterDepth));  // displayed in meters
		UpdateData(FALSE);
		OnEnChangeLayerv();
}

void CdlgLayerNodes::OnBnClickedButtonUndo()
{
	if (this->m_layerV_backup != 0.f) {
		UpdateData(TRUE);
		this->m_layerV2 = this->m_layerV_backup;
		UpdateData(FALSE);
		OnEnChangeLayerv();
	}
}

double CdlgLayerNodes::getLayerV()
{
	return (double)this->m_layerV2 * 0.001;
}

void CdlgLayerNodes::setLayerV(double v)
{
	m_layerV2 =  (float)myFloat(v * 1000., 5);
}

void CdlgLayerNodes::OnEnChangeLayerv()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	UpdateData(TRUE);

	// m_modX is changed.
	if (m_isTop)
		m_modX->vTop[m_layerNum - 1] = m_layerV2 * 0.001;
	else
		m_modX->vBot[m_layerNum - 1] = m_layerV2 * 0.001;


	// this->calcRegression();
	Invalidate();  // this will call OnDrawItem()
}


void CdlgLayerNodes::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect(lpDrawItemStruct->rcItem);
	int delta = 0;
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

//	CBrush *oldBrush;
	CBrush brush;
	if (nIDCtl == IDC_BUTTON_CURVE) {

		if (m_modX->ncont < 2) return;
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		float regionalSedVmin = (float)pFrame->psei->regionalSedVmin;
		float regionalSedVmax = (float)pFrame->psei->regionalSedVmax;

		double zt[MOD_LAYER_MAX], v[MOD_LAYER_MAX];
		double ztEditting;
		double ztEditting2;  // the layer in editting would be from ztEditting to ztEditting2. ztEditting2 can either be bigger or smaller
		double vEditting2;
		int count = 0;
		CPen *oldPen;

		for (int icont = 0; icont < m_modX->ncont - 1; icont++) {
			zt[count] = m_isTimeDomain ? m_modX->timeMod[icont] : m_modX->depthMod[icont];
			v[count] = m_modX->vTop[icont];
			count++;
			zt[count] = m_isTimeDomain ? m_modX->timeMod[icont + 1] : m_modX->depthMod[icont + 1];
			v[count] = m_modX->vBot[icont];

			if (icont == m_layerNum-1) {
				// this is the currently editting layer number
				ztEditting = m_isTop ? zt[count - 1] : zt[count];
				ztEditting2 = !m_isTop ? zt[count - 1] : zt[count];
				vEditting2 = !m_isTop ? v[count - 1] : v[count];
			}

			count++;
			if (count >= MOD_LAYER_MAX) break;
		}
		if (count < 2) return;

		// I will need to transform my curve to pixels
		double vmax = m_modX->vBot[m_modX->ncont - 2];
		double vmin = m_modX->vTop[0];
		double ztmax = m_isTimeDomain ? m_modX->timeMod[m_modX->ncont - 1] : m_modX->depthMod[m_modX->ncont - 1];
		if (m_layerV2*0.001 < regionalSedVmax) {
			for (int i = 0; i < count; i++) {
				if (v[i] > regionalSedVmax) {
					vmax = v[i];
					ztmax = zt[i];
					break;
				}
			}
		}
		if (vmax - vmin < 0.001 || ztmax < 0.001) return;

		// note|: the drawing of rect is pixel based only here, since there is no transformation for plotting
		int x;
		int y;



		// draw vertical grids first
		CPen gridPen(PS_DOT, 0, RGB(160, 160, 160));
		oldPen = pDC->SelectObject(&gridPen);
		x = (int)((2.0 - v[0]) * rect.right / (vmax - vmin) + 0.5);
		pDC->MoveTo(x, 0);
		pDC->LineTo(x, rect.bottom);
		pDC->TextOut(x - 3, rect.bottom - 12, _T("2"));
		x = (int)((3.0 - v[0]) * rect.right / (vmax - vmin) + 0.5);
		pDC->MoveTo(x, 0);
		pDC->LineTo(x, rect.bottom);
		pDC->TextOut(x - 3, rect.bottom - 12, _T("3"));
		if (vmax >= 4.) {
			x = (int)((4.0 - v[0]) * rect.right / (vmax - vmin) + 0.5);
			pDC->MoveTo(x, 0);
			pDC->LineTo(x, rect.bottom);
			pDC->TextOut(x - 3, rect.bottom - 12, _T("4"));
		}
		if (vmax >= 5.) {
			x = (int)((5.0 - v[0]) * rect.right / (vmax - vmin) + 0.5);
			pDC->MoveTo(x, 0);
			pDC->LineTo(x, rect.bottom);
			pDC->TextOut(x - 3, rect.bottom - 12, _T("5"));
		}
		if (vmax >= 6.) {
			x = (int)((6.0 - v[0]) * rect.right / (vmax - vmin) + 0.5);
			pDC->MoveTo(x, 0);
			pDC->LineTo(x, rect.bottom);
			pDC->TextOut(x - 3, rect.bottom - 12, _T("6"));
		}
		if (vmax >= 7.) {
			x = (int)((7.0 - v[0]) * rect.right / (vmax - vmin) + 0.5);
			pDC->MoveTo(x, 0);
			pDC->LineTo(x, rect.bottom);
			pDC->TextOut(x - 3, rect.bottom - 12, _T("7"));
		}
		if (vmax >= 8.) {
			x = (int)((8.0 - v[0]) * rect.right / (vmax - vmin) + 0.5);
			pDC->MoveTo(x, 0);
			pDC->LineTo(x, rect.bottom);
			pDC->TextOut(x - 3, rect.bottom - 12, _T("8"));
		}



		// draw seafloor horizontal line
		int xcenter = (rect.left + rect.right) / 2;
		CPen dashPen(PS_DOT, 0, BLACK);
		pDC->SelectObject(&dashPen);

		float waterDepth = (float)m_modX->depthMod[NUM_WATER_LAYER];
		y = (int)(waterDepth * rect.bottom / ztmax + 0.5);
		pDC->MoveTo(rect.left, y);
		pDC->LineTo(rect.right, y);
		pDC->TextOut(xcenter, y-6, _T("Seafloor"));


		CPen bluePen(PS_SOLID, 0, BLUE);
		pDC->SelectObject(&bluePen);
		x = (int)((m_layerV2*0.001 - v[0]) * rect.right / (vmax - vmin) + 0.5);
		y = (int)(ztEditting * rect.bottom / ztmax + 0.5);
		pDC->Ellipse(x - 8, y - 8, x + 8, y + 8);

		CPen greenPen(PS_SOLID, 4, GREEN);
		pDC->SelectObject(&greenPen);
		x = (int)((vEditting2 - v[0]) * rect.right / (vmax - vmin) + 0.5);
		y = (int)(ztEditting2 * rect.bottom / ztmax + 0.5);
		pDC->MoveTo(x, y);
		x = (int)((m_layerV2*0.001 - v[0]) * rect.right / (vmax - vmin) + 0.5);
		y = (int)(ztEditting * rect.bottom / ztmax + 0.5);
		pDC->LineTo(x, y);

		CPen linePen(PS_SOLID, 0, RED);
		pDC->SelectObject(&linePen);
		pDC->MoveTo(0, 0);
		for (int i = 0; i < count; i++) {
			// each point to transform to pixel domain before plotting
			x = (int)( (v[i] - v[0]) * rect.right / (vmax - vmin) + 0.5);
			y = (int)( zt[i] * rect.bottom / ztmax + 0.5);
			pDC->LineTo(x, y);
		}

		pDC->SelectObject(oldPen);
		InvalidateRect(&rect);
		return;
	}

	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}
