#pragma once
// SeisWideView.h : interface of the CSeisWideView class

//
/////////////////////////////////////////////////////////////////////////////
// #include "ScrollHelper.h"
//#include "gl\gl.h"    // Include the standard OpenGL headers
//#include "gl\glu.h"   // Add the utility library
// check complete instructions on how to use opengl with MFC:
//    https://www.microsoft.com/msj/archive/S2085.aspx
#include "coords.h"
//#include "OpenGLRenderer.h"

class CSeisWideCntrItem;

//class CSeisWideView : public CScrollView, virtual public CGLObj
class CSeisWideView : public CScrollView
{
protected: // create from serialization only
	CSeisWideView();
	DECLARE_DYNCREATE(CSeisWideView)

// Attributes
public:
	CSeisWideDoc* GetDocument();
	// m_pSelection holds the selection to the current CSeisWideCntrItem.
	// For many applications, such a member variable isn't adequate to
	//  represent a selection, such as a multiple selection or a selection
	//  of objects that are not CSeisWideCntrItem objects.  This selection
	//  mechanism is provided just to help you get started.

	// TODO: replace this selection mechanism with one appropriate to your app.
	CSeisWideCntrItem* m_pSelection;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeisWideView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
//	BOOL InitializeSec();
	virtual ~CSeisWideView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	// CProgressCtrl m_progressBar;

	// if the user starts to drag and drop, the original dragging point is: pDoc->cd.coord.m_point, transient.xOld, transient.yOld
	// if the user move the mouse, and move again, we have the current point accessible at OnMouseMove() function
	// while the point before the second move is stored at  transient.PointOld
	// NOTE: coord.m_point  and  transient.PointOld  are all in pixel! Not using mm_coord()  but use  XY2pixel()
	struct TRANSIENT{
		// bool m_isImmediateAfterDragDrop; // if yes, the next reducing velocity shall be set according to this slope; if no, the drag and drop event already passed as if non-existing
		// double xOld,yOld,xNew,yNew;
		SEI_QUAD aQuad; // used in mouse moving and mouse up events
		bool m_Drag;// checks drag mode. true=L-mouse down; false=L-Mouse up
		int MotionFix;
		CPoint PointOld;// store the previous draw point
		CPoint PointScrollRef; // the scroll position of start for the current screen; if no scroll exist, PointScrollRef === PointOld. May not have been used???
		TRANSIENT(): m_Drag(false),MotionFix(0) {};
	} transient;
	bool m_isModelSwitchMode;
	BOOL gIsNoLableOutsideBackup; // used before and after printing
	BOOL gIsNoLabelInSideBackup; // used before and after printing
	BOOL gisNoBoundaryBackup;
	bool m_isInit;



// Generated message map functions
protected:
	bool RasterSeismic(CString bitmapName, int nrow, int ncol, int numBits, 
		CColorHelper::AMPCOLORDYN *pAmpColorDyn);
	bool drawCurveBetween(CClientDC * clientDC, CPoint p1, CPoint p2, bool isDrawOutside, int ndiv = 2);
	bool drawCurveLabelBetween(CClientDC * clientDC, CPoint p1, CPoint p2, bool isDrawOutside, int ndiv = 2);
	void drawEmpty(CDC* pDC);

	void UpdateViewMenu();
	//{{AFX_MSG(CSeisWideView)
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEditCntr();
	afx_msg void OnCancelEditSrvr();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUpdateToolbarPick(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolbarRemove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToobarMove(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolbarAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTimesectrc(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewTimesection(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewDepthsection(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewVelocitycolormap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewBoth(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewContours(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMenuViewHydro(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditBoundary(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditVelocity(CCmdUI* pCmdUI);

	afx_msg void OnUpdateEditText(CCmdUI* pCmdUI);

	afx_msg void OnUpdateEditNonodes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditTuneupNoboundary(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditTuneupNocolormap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditTuneupObspositions(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditNolables(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDisplaytuneupFillareaonce(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDisplaytuneupBigmodelnodes(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDisplaytuneupExternalDrawings(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDisplaytuneupRemoveoutsideraypoints(CCmdUI* pCmdUI);
	//afx_msg void OnUpdateEditDisplaytuneupSkipcomplainingtracetoodense(CCmdUI* pCmdUI);
	afx_msg void OnFileExportbitmapOld();
	afx_msg void OnFileExportbitmap();
	afx_msg void OnFileExportbitmapOld2();
	//	afx_msg void OnUpdateEditDisplaytuneupLabelastwtt(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnToolbarZoom();
	afx_msg void OnUpdateToolbarZoom(CCmdUI* pCmdUI);
	afx_msg void OnToolbarUnzoom();
	afx_msg void OnToolbarHand();
	afx_msg void OnUpdateToolbarHand(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
    // void DisplayScrollInfo();
    // void DisplayScrollPos();
	void mySchrolUpdate();
public:
	afx_msg void OnUpdateViewNmoVelocity(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewVelocitycolortwtt(CCmdUI *pCmdUI);

	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	CPoint X2CpointDevice(double x, double y);
protected:
//	virtual BOOL OnScrollBy(CSize sizeScroll, BOOL bDoScroll = TRUE);
protected:
	virtual void PostNcDestroy();
	void myUpdateStatusBar();
	// CListBox m_scrollInfoLB;



	// opengl extra testing functions
	//void SaveAsBMP_2(CString szFilename);
	//BOOL SetupPixelFormat(CDC* pDC, HGLRC& hRC, BOOL bDrawToWindow /*= TRUE*/);

public:
	afx_msg void OnUpdateModelToggledetails(CCmdUI *pCmdUI);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	// virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnUpdateEditImagestretchmode(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditLabelInside(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditLablesOutside(CCmdUI *pCmdUI);
	afx_msg void OnOptionsTogglehorizontalgrids();
	afx_msg void OnUpdateOptionsTogglehorizontalgrids(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolbarUnzoom(CCmdUI *pCmdUI);
	// virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
private:
public:
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnUpdateModelingActivateoverlaidmodel(CCmdUI *pCmdUI);
	afx_msg void OnModelingActivateoverlaidmodel();
	afx_msg void OnUpdateOptionsToggleclipchangewithgain(CCmdUI *pCmdUI);
	afx_msg void OnUpdateModelingFlattenhorizon(CCmdUI *pCmdUI);
//	COpenGLControl m_oglWindow2;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnUpdateShow3dAnimation(CCmdUI *pCmdUI);
	afx_msg void OnShow3dAnimation();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnRaycodeDisplay();
	afx_msg void OnUpdateViewGravity(CCmdUI* pCmdUI);
};

#ifndef _DEBUG  // debug version in SeisWideView.cpp
inline CSeisWideDoc* CSeisWideView::GetDocument()
   { return (CSeisWideDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
