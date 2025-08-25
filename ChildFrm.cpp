// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "SeisWide.h"
#include "SeisWideDoc.h"

#include "ChildFrm.h"
// #include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_WM_LBUTTONUP()
	ON_WM_ACTIVATE()
	ON_WM_CHILDACTIVATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	// TODO: add member initialization code here
	//m_isWinSizeChanged = FALSE;
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::OnCreateClient( LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	return m_wndSplitter.Create( this,
		2, 2,                 // TODO: adjust the number of rows, columns
		CSize( 10, 10 ),      // TODO: adjust the minimum pane size
		pContext );
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	if (!CMDIChildWnd::PreCreateWindow(cs)) return FALSE;

	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();
	int i = pSeiApp->statWin.winPosCurrent;
	if (cs.x<0 && i >= 0 && i < pSeiApp->statWin.winPosCount) {
		// note: the adjustment numbers here are all based on trial and error. Not sure if there is any better way to determine them?
		int acx = pSeiApp->statWin.winPosRect[i].Width() + 42;
		int acy = pSeiApp->statWin.winPosRect[i].Height() + 72;
		if (acx > 80 && acy > 60) {
			cs.cx = acx;
			cs.cy = acy;

			cs.x = pSeiApp->statWin.winPosRect[i].left - 22;
			if (cs.x < 0) cs.x = 0;
			int valMax = cs.x + cs.cx;
			if (cs.x >= valMax) cs.x = valMax - 10;

			cs.y = pSeiApp->statWin.winPosRect[i].top - 145;
			if (cs.y < 0) cs.y = 0;
			valMax = cs.y + cs.cy;
			if (cs.y >= valMax) cs.y = valMax - 10;
		}

		pSeiApp->statWin.winPosCurrent = pSeiApp->statWin.winPosCount;
		// this line will ensure that newer windows will not overwrite the settings in this if statement
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers


void CChildFrame::ActivateFrame(int nCmdShow) 
{
	nCmdShow = SW_SHOWNORMAL; 

	CMDIChildWnd::ActivateFrame(nCmdShow);
}

BOOL CChildFrame::HideOtherChild()
{ 
 int count = 0;

 CMDIFrameWnd* pFrame = GetMDIFrame( );
 if(!pFrame) return FALSE;
	 
 CWnd* pChild = pFrame->GetWindow(GW_CHILD);

 while (pChild)
  {
    count++;
    pChild = (CMDIChildWnd*)pFrame->GetWindow(GW_HWNDNEXT);
	if(pChild) pChild->ShowWindow(SW_MINIMIZE);
  }
 return TRUE;
} 
void CChildFrame::myCaption(CString text)
{
	if (text.GetLength() < 1) return; // do not show any since the string is zero length
		// avoiding showing blank texts can keep the existing shown messages longer
	CString tStr;

	// write to caption on the status bar
	this->GetWindowText(tStr);
	CString a = text.Left(50);
	CString b = tStr.Left(50);
	if (a != b) {
		a.Format(_T("%s ... %s"), text.GetString(), tStr);
		text = a.Left(255);
	}

	this->SetMessageText(text);
}

void CChildFrame::OnChildActivate()
{
	CMDIChildWnd::OnChildActivate();


	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);

	CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();
	CString aName;
	this->GetWindowText(aName);
	if (aName != pSeiApp->statWin.nameWinCurrent) {
		if (pSeiApp->statWin.getControlPressed()) {
			wp.rcNormalPosition.bottom = pSeiApp->statWin.rectWinCurrent.bottom;
			wp.rcNormalPosition.top = pSeiApp->statWin.rectWinCurrent.top;

			int left = wp.rcNormalPosition.left;
			int leftOld = pSeiApp->statWin.rectWinCurrent.left;
			if (left < leftOld) {
				// the current child is on the left side of the previous
				int iAdded = (left < leftOld - 20) ? 0 : 20;
				wp.rcNormalPosition.right = pSeiApp->statWin.rectWinCurrent.left + iAdded;
			}
			else {
				// the current child is on the right side of the previous
				int iAdded = (left > leftOld + 20) ? 0 : 20;
				wp.rcNormalPosition.left = pSeiApp->statWin.rectWinCurrent.right + iAdded;
			}

			SetWindowPlacement(&wp);
			//pSeiApp->statWin.getControlPressed() = FALSE; // we do not want to do the same again without further Control key press!
		}

		pSeiApp->statWin.rectWinCurrent = wp.rcNormalPosition;
		this->GetWindowText(pSeiApp->statWin.nameWinCurrent);
	}

	return;


	//CRect aRect;
	//this->GetWindowRect(&aRect); //  relative to the upper-left corner of the screen
	//this->GetWindowText(aName);
	//if (aName != pSeiApp->statWin.nameWinCurrent) {
	//	this->GetWindowText(tStr);
	//	tStr.Format(_T("Activate %s"), tStr);
	//	this->myCaption(tStr);

	//	if (pSeiApp->statWin.isControlPressed) {
	//		aRect.top = pSeiApp->statWin.rectWinCurrent.top;
	//		aRect.bottom = pSeiApp->statWin.rectWinCurrent.bottom;
	//		//if (aRect.left < pSeiApp->statWin.rectWinCurrent.left) {
	//		//	aRect.right = pSeiApp->statWin.rectWinCurrent.left - 2;
	//		//}
	//		//else {
	//		//	aRect.left = pSeiApp->statWin.rectWinCurrent.right + 2;
	//		//}
	//		//if (aRect.right < aRect.left + 20)
	//		//	aRect.right = aRect.left + 20;

	//		CRect r;
	//		GetWindowRect(&r);
	//		aRect.left -= r.left;
	//		aRect.right -= r.left;
	//		aRect.top -= r.top;
	//		aRect.bottom -= r.top;
	//		if (aRect.top < 0) {
	//			aRect.bottom = aRect.Height();
	//			aRect.top = 0;
	//		}

	//		this->MoveWindow(aRect.left, aRect.top, aRect.Width(), aRect.Height(), TRUE);
	//		//  relative to the upper-left corner of the parent window's client area.
	//	}
	//	pSeiApp->statWin.rectWinCurrent = aRect;
	//	this->GetWindowText(pSeiApp->statWin.nameWinCurrent); 
	//	m_bActivated = TRUE;
	//}
	//else {
	//	// if here, the child window is loosing focus, and we want to store windows rect
	//	m_bActivated = FALSE;
	//	this->GetWindowText(tStr);
	//	tStr.Format(_T("DeActivate %s"), tStr);
	//	this->myCaption(tStr);
	//}

}



void CChildFrame::OnSize(UINT nType, int cx, int cy)
{
	CMDIChildWnd::OnSize(nType, cx, cy);

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);
	CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();
	pSeiApp->statWin.rectWinCurrent = wp.rcNormalPosition;
	this->GetWindowText(pSeiApp->statWin.nameWinCurrent);


}
