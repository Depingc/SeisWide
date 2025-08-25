// dlgSegyColor2.cpp : implementation file
//

#include "stdafx.h"
#include "SeisWide.h"
#include "dlgSegyColor2.h"
//typedef struct _complexStruct { /* complex number */
//	float r,i;
//} complex;
#include "math.h"
#include "util.h"

//CdlgSegyColor2 *color2;

// CdlgSegyColor2 dialog

IMPLEMENT_DYNAMIC(CdlgSegyColor2, CDialog)

CdlgSegyColor2::CdlgSegyColor2(CWnd* pParent /*=NULL*/)
	: CDialog(CdlgSegyColor2::IDD, pParent)
{
	//getAmpColorArray(m_fileName);
	m_isDrag = false;
	m_pointLeft = m_pointXCenter = m_pointRight = 0;
	ampColor11.initAmpColorArray(11);
	ampColor11.fileName = m_colFileName;
}

CdlgSegyColor2::CdlgSegyColor2(CString aname)
{
	m_isDrag = false;
	m_pointLeft = m_pointXCenter = m_pointRight = 0;
	ampColor11.fileName = m_colFileName;
	ampColor11.getAmpColorArray2(2, true);

	GetDlgItem(IDC_BUTTON_COL0)->GetWindowRect(&m_rectButtons[0]); ScreenToClient(m_rectButtons[0]);
	GetDlgItem(IDC_BUTTON_COL1)->GetWindowRect(&m_rectButtons[1]); ScreenToClient(m_rectButtons[1]);
	GetDlgItem(IDC_BUTTON_COL2)->GetWindowRect(&m_rectButtons[2]); ScreenToClient(m_rectButtons[2]);
	GetDlgItem(IDC_BUTTON_COL3)->GetWindowRect(&m_rectButtons[3]); ScreenToClient(m_rectButtons[3]);
	GetDlgItem(IDC_BUTTON_COL4)->GetWindowRect(&m_rectButtons[4]); ScreenToClient(m_rectButtons[4]);
	GetDlgItem(IDC_BUTTON_COL5)->GetWindowRect(&m_rectButtons[5]); ScreenToClient(m_rectButtons[5]);
	GetDlgItem(IDC_BUTTON_COL6)->GetWindowRect(&m_rectButtons[6]); ScreenToClient(m_rectButtons[6]);
	GetDlgItem(IDC_BUTTON_COL7)->GetWindowRect(&m_rectButtons[7]); ScreenToClient(m_rectButtons[7]);
	GetDlgItem(IDC_BUTTON_COL8)->GetWindowRect(&m_rectButtons[8]); ScreenToClient(m_rectButtons[8]);
	GetDlgItem(IDC_BUTTON_COL9)->GetWindowRect(&m_rectButtons[9]); ScreenToClient(m_rectButtons[9]);
	GetDlgItem(IDC_BUTTON_COL10)->GetWindowRect(&m_rectButtons[10]); ScreenToClient(m_rectButtons[10]);
}

CdlgSegyColor2::~CdlgSegyColor2()
{
}

void CdlgSegyColor2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PALETTENAME, m_colFileName);
	
	DDX_Text(pDX, IDC_AMPLEVEL0, ampColor11.percentAmp[0]);
	DDX_Text(pDX, IDC_AMPLEVEL1, ampColor11.percentAmp[1]);
	DDX_Text(pDX, IDC_AMPLEVEL2, ampColor11.percentAmp[2]);
	DDX_Text(pDX, IDC_AMPLEVEL3, ampColor11.percentAmp[3]);
	DDX_Text(pDX, IDC_AMPLEVEL4, ampColor11.percentAmp[4]);
	DDX_Text(pDX, IDC_AMPLEVEL5, ampColor11.percentAmp[5]);
	DDX_Text(pDX, IDC_AMPLEVEL6, ampColor11.percentAmp[6]);
	DDX_Text(pDX, IDC_AMPLEVEL7, ampColor11.percentAmp[7]);
	DDX_Text(pDX, IDC_AMPLEVEL8, ampColor11.percentAmp[8]);
	DDX_Text(pDX, IDC_AMPLEVEL9, ampColor11.percentAmp[9]);
	DDX_Text(pDX, IDC_AMPLEVEL10, ampColor11.percentAmp[10]);

	GetDlgItem(IDC_BUTTON_COL0)->GetWindowRect(&m_rectButtons[0]); ScreenToClient(m_rectButtons[0]);
	GetDlgItem(IDC_BUTTON_COL1)->GetWindowRect(&m_rectButtons[1]); ScreenToClient(m_rectButtons[1]);
	GetDlgItem(IDC_BUTTON_COL2)->GetWindowRect(&m_rectButtons[2]); ScreenToClient(m_rectButtons[2]);
	GetDlgItem(IDC_BUTTON_COL3)->GetWindowRect(&m_rectButtons[3]); ScreenToClient(m_rectButtons[3]);
	GetDlgItem(IDC_BUTTON_COL4)->GetWindowRect(&m_rectButtons[4]); ScreenToClient(m_rectButtons[4]);
	GetDlgItem(IDC_BUTTON_COL5)->GetWindowRect(&m_rectButtons[5]); ScreenToClient(m_rectButtons[5]);
	GetDlgItem(IDC_BUTTON_COL6)->GetWindowRect(&m_rectButtons[6]); ScreenToClient(m_rectButtons[6]);
	GetDlgItem(IDC_BUTTON_COL7)->GetWindowRect(&m_rectButtons[7]); ScreenToClient(m_rectButtons[7]);
	GetDlgItem(IDC_BUTTON_COL8)->GetWindowRect(&m_rectButtons[8]); ScreenToClient(m_rectButtons[8]);
	GetDlgItem(IDC_BUTTON_COL9)->GetWindowRect(&m_rectButtons[9]); ScreenToClient(m_rectButtons[9]);
	GetDlgItem(IDC_BUTTON_COL10)->GetWindowRect(&m_rectButtons[10]); ScreenToClient(m_rectButtons[10]);

}


BEGIN_MESSAGE_MAP(CdlgSegyColor2, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSET, &CdlgSegyColor2::OnBnClickedButtonBrowset)
	ON_WM_DRAWITEM()
	ON_WM_SETFOCUS()
	ON_BN_CLICKED(IDC_BUTTON_COL0, &CdlgSegyColor2::OnBnClickedButtonCol0)
	ON_BN_CLICKED(IDC_BUTTON_COL10, &CdlgSegyColor2::OnBnClickedButtonCol10)
	ON_BN_CLICKED(IDC_BUTTON_COL9, &CdlgSegyColor2::OnBnClickedButtonCol9)
	ON_BN_CLICKED(IDC_BUTTON_COL1, &CdlgSegyColor2::OnBnClickedButtonCol1)
	ON_BN_CLICKED(IDC_BUTTON_COL2, &CdlgSegyColor2::OnBnClickedButtonCol2)
	ON_BN_CLICKED(IDC_BUTTON_COL3, &CdlgSegyColor2::OnBnClickedButtonCol3)
	ON_BN_CLICKED(IDC_BUTTON_COL4, &CdlgSegyColor2::OnBnClickedButtonCol4)
	ON_BN_CLICKED(IDC_BUTTON_COL5, &CdlgSegyColor2::OnBnClickedButtonCol5)
	ON_BN_CLICKED(IDC_BUTTON_COL6, &CdlgSegyColor2::OnBnClickedButtonCol6)
	ON_BN_CLICKED(IDC_BUTTON_COL7, &CdlgSegyColor2::OnBnClickedButtonCol7)
	ON_BN_CLICKED(IDC_BUTTON_COL8, &CdlgSegyColor2::OnBnClickedButtonCol8)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CdlgSegyColor2::OnBnClickedButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD, &CdlgSegyColor2::OnBnClickedButtonReload)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD_BW, &CdlgSegyColor2::OnBnClickedButtonReloadBw)
	ON_BN_CLICKED(IDC_BUTTON_LOW, &CdlgSegyColor2::OnBnClickedButtonLow)
	ON_BN_CLICKED(IDC_BUTTON_FULL, &CdlgSegyColor2::OnBnClickedButtonFull)
	ON_BN_CLICKED(IDC_BUTTON_LOWER, &CdlgSegyColor2::OnBnClickedButtonLower)
	ON_BN_CLICKED(IDC_BUTTON_HIGHER, &CdlgSegyColor2::OnBnClickedButtonHigher)
	ON_EN_UPDATE(IDC_AMPLEVEL0, &CdlgSegyColor2::OnEnUpdateAmplevel0)
	ON_EN_UPDATE(IDC_AMPLEVEL1, &CdlgSegyColor2::OnEnUpdateAmplevel1)
	ON_EN_UPDATE(IDC_AMPLEVEL2, &CdlgSegyColor2::OnEnUpdateAmplevel2)
	ON_EN_UPDATE(IDC_AMPLEVEL3, &CdlgSegyColor2::OnEnUpdateAmplevel3)
	ON_EN_UPDATE(IDC_AMPLEVEL4, &CdlgSegyColor2::OnEnUpdateAmplevel4)
	ON_EN_UPDATE(IDC_AMPLEVEL5, &CdlgSegyColor2::OnEnUpdateAmplevel5)
	ON_EN_UPDATE(IDC_AMPLEVEL6, &CdlgSegyColor2::OnEnUpdateAmplevel6)
	ON_EN_UPDATE(IDC_AMPLEVEL7, &CdlgSegyColor2::OnEnUpdateAmplevel7)
	ON_EN_UPDATE(IDC_AMPLEVEL8, &CdlgSegyColor2::OnEnUpdateAmplevel8)
	ON_EN_UPDATE(IDC_AMPLEVEL9, &CdlgSegyColor2::OnEnUpdateAmplevel9)
	ON_EN_UPDATE(IDC_AMPLEVEL10, &CdlgSegyColor2::OnEnUpdateAmplevel10)
	ON_BN_CLICKED(IDC_BUTTON_MORE_COLOR, &CdlgSegyColor2::OnBnClickedButtonMoreColor)
	ON_BN_CLICKED(IDC_BUTTON_BK_RED, &CdlgSegyColor2::OnBnClickedButtonBkRed)
	ON_BN_CLICKED(IDC_BUTTON_SMALLBK, &CdlgSegyColor2::OnBnClickedButtonSmallbk)
	ON_BN_CLICKED(IDC_BUTTON_SMALLBK2, &CdlgSegyColor2::OnBnClickedButtonSmallbk2)
	ON_BN_CLICKED(IDC_BUTTON_MORE_COLOR3, &CdlgSegyColor2::OnBnClickedButtonMoreColor3)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD_RedYELLOW, &CdlgSegyColor2::OnBnClickedButtonReloadRedyellow)
	ON_BN_CLICKED(IDC_BUTTON_RELOAD_RedYELLOW2, &CdlgSegyColor2::OnBnClickedButtonReloadRedyellow2)
END_MESSAGE_MAP()


// CdlgSegyColor2 message handlers
BOOL CdlgSegyColor2::saveAmpColorToFile(CString colFileName)
{
	if (colFileName.GetLength() == 0) return FALSE;
	return CColorHelper::saveAmpColorToFile2(&ampColor11, colFileName);
}



void CdlgSegyColor2::OnBnClickedButtonBrowset()
{
#ifndef _MAC
	CFileDialog dlg( TRUE,_T("txt"),_T("*.txt"),
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("Color Palette File (*.txt)|*.txt|"));
#else
	CFileDialog dlg( TRUE,NULL,NULL,
	                 OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
	                 _T("First Segy File |sgy |"));
#endif
	if( dlg.DoModal()==IDOK )
	{
		m_colFileName = dlg.GetPathName();
		ampColor11.getAmpColorArray2(2, true);

		UpdateData(FALSE);
		Invalidate();
	}
}




	//CString tttt;
	//	tttt.Format(_T("%d", i);

// note: FUNC() must start from 0 and increment only 1
//#define FUNC2(x,y) x##y
//#define FUNC1(x,y) FUNC2(x,y)
//#define FUNC(x) FUNC1(x, __COUNTER__)

void CdlgSegyColor2::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rect(lpDrawItemStruct->rcItem);
	int delta = 0;
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CBrush *oldBrush;
	CBrush brush;

	if(nIDCtl == IDC_BUTTON_COL0)
	{
		brush.CreateSolidBrush(ampColor11.getColor(0));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left+delta, rect.top+delta,rect.right-delta, rect.bottom-delta);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	}

	if(nIDCtl == IDC_BUTTON_COL1)
	{
		brush.CreateSolidBrush(ampColor11.getColor(1));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left+delta, rect.top+delta,rect.right-delta, rect.bottom-delta);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	}

	if(nIDCtl == IDC_BUTTON_COL2)
	{
		brush.CreateSolidBrush(ampColor11.getColor(2));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left+delta, rect.top+delta,rect.right-delta, rect.bottom-delta);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	}

	if(nIDCtl == IDC_BUTTON_COL3)
	{
		brush.CreateSolidBrush(ampColor11.getColor(3));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left+delta, rect.top+delta,rect.right-delta, rect.bottom-delta);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	}
	if(nIDCtl == IDC_BUTTON_COL4)
	{
		brush.CreateSolidBrush(ampColor11.getColor(4));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left+delta, rect.top+delta,rect.right-delta, rect.bottom-delta);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	}
	if(nIDCtl == IDC_BUTTON_COL5)
	{
		brush.CreateSolidBrush(ampColor11.getColor(5));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left+delta, rect.top+delta,rect.right-delta, rect.bottom-delta);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	}
	if(nIDCtl == IDC_BUTTON_COL6)
	{
		brush.CreateSolidBrush(ampColor11.getColor(6));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left+delta, rect.top+delta,rect.right-delta, rect.bottom-delta);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	}
	if(nIDCtl == IDC_BUTTON_COL7)
	{
		brush.CreateSolidBrush(ampColor11.getColor(7));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left+delta, rect.top+delta,rect.right-delta, rect.bottom-delta);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	}
	if(nIDCtl == IDC_BUTTON_COL8)
	{
		brush.CreateSolidBrush(ampColor11.getColor(8));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left+delta, rect.top+delta,rect.right-delta, rect.bottom-delta);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	}
	if(nIDCtl == IDC_BUTTON_COL9)
	{
		brush.CreateSolidBrush(ampColor11.getColor(9));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left+delta, rect.top+delta,rect.right-delta, rect.bottom-delta);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	}
	if(nIDCtl == IDC_BUTTON_COL10)
	{
		brush.CreateSolidBrush(ampColor11.getColor(10));
		oldBrush = pDC->SelectObject(&brush);
		pDC->Rectangle(rect.left+delta, rect.top+delta,rect.right-delta, rect.bottom-delta);
		pDC->SelectObject(oldBrush);
		InvalidateRect(&rect);
		return;
	}

	if(nIDCtl == IDC_BUTTON_CURVE)
	{
		if (ampColor11.percentAmp[1] == 0.f || ampColor11.percentAmp[9] == 0.f) return; // the user has mistakenly entered wrong numbers that can cause crash
		int x,y;
		m_pointXCenter = (rect.left + rect.right) /2;
		m_pointLeft  = rect.left+5; // we do not want the drawing clipped, so we exclude the border
		m_pointRight = rect.right-5;
		int ycenter = (rect.top + rect.bottom) /2;

		CPen dashPen(PS_DOT, 0, RGB(0,0,0));
		CPen *oldPen = pDC->SelectObject(&dashPen);
		pDC->MoveTo(rect.left, ycenter);
		pDC->LineTo(rect.right, ycenter);
		pDC->MoveTo(m_pointXCenter, rect.top);
		pDC->LineTo(m_pointXCenter, rect.bottom);

		CPen aPen(PS_SOLID, 0, RGB(0,0,0));
		oldPen = pDC->SelectObject(&aPen);
		for (int i=1; i<=5; i++) {
			x = m_pointXCenter + (long)myRound((m_pointLeft - m_pointXCenter)*ampColor11.percentAmp[i] / ampColor11.percentAmp[1]);
			y = ycenter - (long)myRound( (ycenter-rect.top)*(5-i)/5 );
			pDC->Ellipse(x-5, y-5, x+5, y+5);
		}
		for (int i=6; i<=9; i++) {
			x = m_pointXCenter + (long)myRound((m_pointRight - m_pointXCenter)*ampColor11.percentAmp[i] / ampColor11.percentAmp[9]);
			y = rect.bottom - (long)myRound( (rect.bottom-ycenter)*(10-i)/5 );
			pDC->Ellipse(x-5, y-5, x+5, y+5);
		}
		pDC->SelectObject(oldPen);

		InvalidateRect(&rect);
		return;
	}


	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}



void CdlgSegyColor2::OnSetFocus(CWnd* pOldWnd)
{
	// will remove this overwrides.

	CDialog::OnSetFocus(pOldWnd);


}

INT_PTR CdlgSegyColor2::DoModal()
{
	// TODO: Add your specialized code here and/or call the base class
	//if ( !isFileExistNotDirectory(m_colFileName)) {
	//	// now we try 
	//	m_colFileName = _T("colPalette.txt");
	//}

	//if ( !isFileExistNotDirectory(m_colFileName)) {
	//	// now we try the current directory
	//	TCHAR curDir[_MAX_PATH];
	//	GetCurrentDirectory(_MAX_PATH, curDir);
	//	m_colFileName = CString(curDir) +  _T("\\colPalette.txt");
	//}
	//if (isFileExistNotDirectory(m_colFileName))
	ampColor11.fileName = GetPathFromFileSlash(m_colFileName) + _T("colPalette.txt");
	ampColor11.getAmpColorArray2(2, true);

	return CDialog::DoModal();
}




void CdlgSegyColor2::OnBnClickedButtonCol0()
{
	CColorDialog dlgColor(ampColor11.getColor(0));
	if (dlgColor.DoModal() == IDOK)
	{
		ampColor11.setColor(dlgColor.GetColor(), 0);
		//COLORREF color =
		//	RGB(
		//	(int)myRound((GetRValue(ampColor11.rgb[0]) + GetRValue(ampColor11.rgb[1]))*0.5),
		//	(int)myRound((GetGValue(ampColor11.rgb[0]) + GetGValue(ampColor11.rgb[1]))*0.5),
		//	(int)myRound((GetBValue(ampColor11.rgb[0]) + GetBValue(ampColor11.rgb[1]))*0.5)
		//	);
		COLORREF color = CColorHelper::interpolateHSV(ampColor11.colorRef[0], ampColor11.colorRef[1], 0.5f);

		ampColor11.setColor(color, 1);

		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonCol1()
{
	CColorDialog dlgColor(ampColor11.getColor(1));
//	dlgColor.SetCurrentColor(RGB(m_maxAmpR, m_maxAmpG, m_maxAmpB));
	if (dlgColor.DoModal() == IDOK)
	{
		ampColor11.setColor(dlgColor.GetColor(), 1);
		COLORREF color = CColorHelper::interpolateHSV(ampColor11.colorRef[0], ampColor11.colorRef[1], 0.5f);
		COLORREF colorb = CColorHelper::interpolateHSV(ampColor11.colorRef[2], ampColor11.colorRef[1], 0.5f);
		ampColor11.setColor(color, 0);
		ampColor11.setColor(colorb, 2);
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonCol2()
{
	CColorDialog dlgColor(ampColor11.getColor(2));
	if (dlgColor.DoModal() == IDOK)
	{
		ampColor11.setColor(dlgColor.GetColor(), 2);
		COLORREF color = CColorHelper::interpolateHSV(ampColor11.colorRef[2], ampColor11.colorRef[1], 0.5f);
		COLORREF colorb = CColorHelper::interpolateHSV(ampColor11.colorRef[2], ampColor11.colorRef[3], 0.5f);
		ampColor11.setColor(color, 1);
		ampColor11.setColor(colorb, 3);
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonCol3()
{
	CColorDialog dlgColor(ampColor11.getColor(3));
	if (dlgColor.DoModal() == IDOK)
	{
		ampColor11.setColor(dlgColor.GetColor(), 3);
		COLORREF color = CColorHelper::interpolateHSV(ampColor11.colorRef[2], ampColor11.colorRef[3], 0.5f);
		COLORREF colorb = CColorHelper::interpolateHSV(ampColor11.colorRef[4], ampColor11.colorRef[3], 0.5f);
		ampColor11.setColor(color, 2);
		ampColor11.setColor(colorb, 4);
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonCol4()
{
	CColorDialog dlgColor(ampColor11.getColor(4));
	if (dlgColor.DoModal() == IDOK)
	{
		ampColor11.setColor(dlgColor.GetColor(), 4);
		COLORREF color = CColorHelper::interpolateHSV(ampColor11.colorRef[4], ampColor11.colorRef[3], 0.5f);
		COLORREF colorb = CColorHelper::interpolateHSV(ampColor11.colorRef[4], ampColor11.colorRef[5], 0.5f);
		ampColor11.setColor(color, 3);
		ampColor11.setColor(colorb, 5);
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonCol5()
{
	CColorDialog dlgColor(ampColor11.getColor(5));
	if (dlgColor.DoModal() == IDOK)
	{
		ampColor11.setColor(dlgColor.GetColor(), 5);
		COLORREF color = CColorHelper::interpolateHSV(ampColor11.colorRef[4], ampColor11.colorRef[5], 0.5f);
		COLORREF colorb = CColorHelper::interpolateHSV(ampColor11.colorRef[6], ampColor11.colorRef[5], 0.5f);
		ampColor11.setColor(color, 4);
		ampColor11.setColor(colorb, 6);
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonCol6()
{
	CColorDialog dlgColor(ampColor11.getColor(6));
	if (dlgColor.DoModal() == IDOK)
	{
		ampColor11.setColor(dlgColor.GetColor(), 6);
		COLORREF color = CColorHelper::interpolateHSV(ampColor11.colorRef[6], ampColor11.colorRef[5], 0.5f);
		COLORREF colorb = CColorHelper::interpolateHSV(ampColor11.colorRef[6], ampColor11.colorRef[7], 0.5f);
		ampColor11.setColor(color, 5);
		ampColor11.setColor(colorb, 7);
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonCol7()
{
	CColorDialog dlgColor(ampColor11.getColor(7));
	if (dlgColor.DoModal() == IDOK)
	{
		ampColor11.setColor(dlgColor.GetColor(), 7);
		COLORREF color = CColorHelper::interpolateHSV(ampColor11.colorRef[6], ampColor11.colorRef[7], 0.5f);
		COLORREF colorb = CColorHelper::interpolateHSV(ampColor11.colorRef[8], ampColor11.colorRef[7], 0.5f);
		ampColor11.setColor(color, 6);
		ampColor11.setColor(colorb, 8);
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonCol8()
{
	CColorDialog dlgColor(ampColor11.getColor(8));
	if (dlgColor.DoModal() == IDOK)
	{
		ampColor11.setColor(dlgColor.GetColor(), 8);
		COLORREF color = CColorHelper::interpolateHSV(ampColor11.colorRef[8], ampColor11.colorRef[7], 0.5f);
		COLORREF colorb = CColorHelper::interpolateHSV(ampColor11.colorRef[8], ampColor11.colorRef[9], 0.5f);
		ampColor11.setColor(color, 7);
		ampColor11.setColor(colorb, 9);
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonCol9()
{
	CColorDialog dlgColor(ampColor11.getColor(9));
	if (dlgColor.DoModal() == IDOK)
	{
		ampColor11.setColor(dlgColor.GetColor(), 9);
		COLORREF color = CColorHelper::interpolateHSV(ampColor11.colorRef[8], ampColor11.colorRef[9], 0.5f);
		COLORREF colorb = CColorHelper::interpolateHSV(ampColor11.colorRef[10], ampColor11.colorRef[9], 0.5f);
		ampColor11.setColor(color, 8);
		ampColor11.setColor(colorb, 10);
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonCol10()
{
	CColorDialog dlgColor(ampColor11.getColor(10));
	if (dlgColor.DoModal() == IDOK)
	{
		ampColor11.setColor(dlgColor.GetColor(), 10);
		COLORREF color = CColorHelper::interpolateHSV(ampColor11.colorRef[10], ampColor11.colorRef[9], 0.5f);
		ampColor11.setColor(color, 8);
		UpdateData(FALSE);
		Invalidate();
	}
}


void CdlgSegyColor2::OnLButtonDown(UINT nFlags, CPoint point)
{
	for (int i=0; i<11; i++) {
		if (isInside((int)point.y, (int)m_rectButtons[i].top, (int)m_rectButtons[i].bottom)) {
			m_iBtnDrag = i;
			this->m_isDrag = true;
			break;
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}


void CdlgSegyColor2::OnLButtonUp(UINT nFlags, CPoint point)
{
	UpdateData(TRUE);
	if (this->m_isDrag) {
		for (int i=0; i<11; i++) {
			if (isInside((int)point.y, (int)m_rectButtons[i].top, (int)m_rectButtons[i].bottom)) {
				if (point.x <= m_rectButtons[i].right) {
					if (m_iBtnDrag != i)
						ampColor11.colorInterpolate(m_iBtnDrag, i);
				}
				else if (i>0 && i<10 && 
					point.x>=m_pointLeft && point.x<=m_pointRight &&
					m_pointLeft != m_pointXCenter) {
					// percentInterpolate(m_iBtnDrag, i);  // this vertical adjustment function is disabled since it is not quite useful and can cause problems
					if (point.x <= m_pointXCenter)
						ampColor11.percentAmp[i] = ampColor11.percentAmp[1] * (float)(point.x - m_pointXCenter) / (float)(m_pointLeft - m_pointXCenter);
					else
						ampColor11.percentAmp[i] = ampColor11.percentAmp[9] * (float)(point.x - m_pointXCenter) / (float)(m_pointRight - m_pointXCenter);
				}

				UpdateData(FALSE);
				Invalidate();
				break;
			} // if
		} // for i

		//AfxMessageBox(_T("Drag completed!");
	}

	
	this->m_isDrag = false;
	CDialog::OnLButtonUp(nFlags, point);
}

void CdlgSegyColor2::OnBnClickedButtonUpdate()
{
	// Apply to synchronize to file
	UpdateData(TRUE);
	CString str;
	if (CColorHelper::saveAmpColorToFile2(&ampColor11, m_colFileName)) {
		str.Format(_T("Successfully synchronized to file: %s ."), m_colFileName);
		showFileInNotePad(m_colFileName);
	}
	else {
		str.Format(_T("Failed to synchronize to file."));
		AfxMessageBox(str);
	}
}

void CdlgSegyColor2::OnBnClickedButtonReload()
{
	// TODO: Add your control notification handler code here
	DeleteFile(m_colFileName + _T(".bak"));
	MoveFile(m_colFileName, m_colFileName + _T(".bak"));
	if (ampColor11.getAmpColorArray2(1, false)) {
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonMoreColor()
{
	DeleteFile(m_colFileName + _T(".bak"));
	MoveFile(m_colFileName, m_colFileName + _T(".bak"));
	if (ampColor11.getAmpColorArray2(2, false)) {
		UpdateData(FALSE);
		Invalidate();
	}
}


void CdlgSegyColor2::OnBnClickedButtonReloadBw()
{
	DeleteFile(m_colFileName + _T(".bak"));
	MoveFile(m_colFileName, m_colFileName + _T(".bak"));
	if (ampColor11.getAmpColorArray2(0, false)) {
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonBkRed()
{
	DeleteFile(m_colFileName + _T(".bak"));
	MoveFile(m_colFileName, m_colFileName + _T(".bak"));
	if (ampColor11.getAmpColorArray2(3, false)) {
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonSmallbk()
{
	DeleteFile(m_colFileName + _T(".bak"));
	MoveFile(m_colFileName, m_colFileName + _T(".bak"));
	if (ampColor11.getAmpColorArray2(4, false)) {
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonSmallbk2()
{
	DeleteFile(m_colFileName + _T(".bak"));
	MoveFile(m_colFileName, m_colFileName + _T(".bak"));
	if (ampColor11.getAmpColorArray2(5, false)) {
		UpdateData(FALSE);
		Invalidate();
	}
}


void CdlgSegyColor2::OnBnClickedButtonLow()
{
	//CColorHelper::AMPCOLORDYN ampColorDynLow;
	//ampColorDynLow.

	static const float col_p[NUMCOLORS_WORKING] = { -1.f, -0.35f, -0.3f, -0.2f, -0.1f, 0.f,
		0.1f, 0.2f, 0.3f, 0.35f, 1.f};

	for (int i = 0; i<NUMCOLORS_WORKING; i++) {
		ampColor11.percentAmp[i] = col_p[i];
	}

	UpdateData(FALSE);
	Invalidate();
}

void CdlgSegyColor2::OnBnClickedButtonFull()
{
	static const float col_p[NUMCOLORS_WORKING] = { -1.f, -0.9f, -0.7f, -0.4f, -0.1f, 0.f,
		0.1f, 0.4f, 0.7f, 0.9f, 1.f};

	// note:  log(1) = 0;  log(e)=1;
	for (int i = 0; i<NUMCOLORS_WORKING; i++) {
		ampColor11.percentAmp[i] = col_p[i];
	}

	UpdateData(FALSE);
	Invalidate();
}


void CdlgSegyColor2::OnBnClickedButtonLower()
{
	if (ampColor11.ampColorLower()) {
		UpdateData(FALSE);
		Invalidate();
	}
}

void CdlgSegyColor2::OnBnClickedButtonHigher()
{
	if (ampColor11.ampColorHigher()) {
		UpdateData(FALSE);
		Invalidate();
	}
}


void CdlgSegyColor2::OnEnUpdateAmplevel0()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

		UpdateData(TRUE);
		Invalidate();
}

void CdlgSegyColor2::OnEnUpdateAmplevel1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

		UpdateData(TRUE);
		Invalidate();
}

void CdlgSegyColor2::OnEnUpdateAmplevel2()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

		UpdateData(TRUE);
		Invalidate();
}

void CdlgSegyColor2::OnEnUpdateAmplevel3()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

		UpdateData(TRUE);
		Invalidate();
}

void CdlgSegyColor2::OnEnUpdateAmplevel4()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

		UpdateData(TRUE);
		Invalidate();
}

void CdlgSegyColor2::OnEnUpdateAmplevel5()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

		UpdateData(TRUE);
		Invalidate();
}

void CdlgSegyColor2::OnEnUpdateAmplevel6()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

		UpdateData(TRUE);
		Invalidate();
}

void CdlgSegyColor2::OnEnUpdateAmplevel7()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

		UpdateData(TRUE);
		Invalidate();
}

void CdlgSegyColor2::OnEnUpdateAmplevel8()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

		UpdateData(TRUE);
		Invalidate();
}

void CdlgSegyColor2::OnEnUpdateAmplevel9()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

		UpdateData(TRUE);
		Invalidate();
}

void CdlgSegyColor2::OnEnUpdateAmplevel10()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function to send the EM_SETEVENTMASK message to the control
	// with the ENM_UPDATE flag ORed into the lParam mask.

		UpdateData(TRUE);
		Invalidate();
}








void CdlgSegyColor2::OnBnClickedButtonMoreColor3()
{
	DeleteFile(m_colFileName + _T(".bak"));
	MoveFile(m_colFileName, m_colFileName + _T(".bak"));
	if (ampColor11.getAmpColorArray2(6, false)) {
		UpdateData(FALSE);
		Invalidate();
	}
}




void CdlgSegyColor2::OnBnClickedButtonReloadRedyellow()
{
	DeleteFile(m_colFileName + _T(".bak"));
	MoveFile(m_colFileName, m_colFileName + _T(".bak"));
	//ampColor11.fileName = m_colFileName;
	if (ampColor11.getAmpColorArray2(7, false)) {
		UpdateData(FALSE);
		Invalidate();
	}
}


void CdlgSegyColor2::OnBnClickedButtonReloadRedyellow2()
{
	// industry standard: : blue-white-red-yellow
	DeleteFile(m_colFileName + _T(".bak"));
	MoveFile(m_colFileName, m_colFileName + _T(".bak"));
	//ampColor11.fileName = m_colFileName;
	if (ampColor11.getAmpColorArray2(8, false)) {
		UpdateData(FALSE);
		Invalidate();
	}
}
