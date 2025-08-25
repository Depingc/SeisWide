// SeisWideView.cpp : implementation of the CSeisWideView class
//

#include "stdafx.h"
#include "SeisWide.h"

#include "MainFrm.h"
#include "SeisWideDoc.h"
#include "CntrItem.h"
#include "SeisWideView.h"
#include "float.h"
#include "math.h"
#include "dlgPrint.h"
#include "dlgTimeByTrc.h"
#include "dlgExportBitmap.h"
#include "bmp24.h"
#include "util.h"
#include "dlgHydrateDisp.h"
#include "segywrapper.h"
#include "segywrapperEx.h"
#include "segyMeta.h"

#include "dlgImportbitmap.h" 
#include "licenseCheck.h"
#include "VelMod.h"
#include "digitize.h"
#include "ximage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSeisWideView

IMPLEMENT_DYNCREATE(CSeisWideView, CScrollView)

	BEGIN_MESSAGE_MAP(CSeisWideView, CScrollView)
		//{{AFX_MSG_MAP(CSeisWideView)
		ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
		ON_COMMAND(ID_CANCEL_EDIT_CNTR, OnCancelEditCntr)
		ON_COMMAND(ID_CANCEL_EDIT_SRVR, OnCancelEditSrvr)
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_WM_KEYDOWN()
		ON_WM_MOUSEMOVE()
		ON_WM_HSCROLL()
		ON_UPDATE_COMMAND_UI(ID_TOOLBAR_PICK, OnUpdateToolbarPick)
		ON_UPDATE_COMMAND_UI(ID_TOOLBAR__REMOVE, OnUpdateToolbarRemove)
		ON_UPDATE_COMMAND_UI(ID_TOOBAR_MOVE, OnUpdateToobarMove)
		ON_UPDATE_COMMAND_UI(ID_TOOLBAR_ADD, OnUpdateToolbarAdd)
		ON_UPDATE_COMMAND_UI(ID_VIEW_TIMESECTRC, OnUpdateViewTimesectrc)
		ON_UPDATE_COMMAND_UI(ID_VIEW_TIMESECTION, OnUpdateViewTimesection)
		ON_UPDATE_COMMAND_UI(ID_VIEW_DEPTHSECTION, OnUpdateViewDepthsection)
		ON_UPDATE_COMMAND_UI(ID_VIEW_VELOCITYCOLORMAP, OnUpdateViewVelocitycolormap)
		ON_UPDATE_COMMAND_UI(ID_VIEW_BOTH, OnUpdateViewBoth)
		ON_UPDATE_COMMAND_UI(ID_VIEW_CONTOURS, OnUpdateViewContours)
		ON_UPDATE_COMMAND_UI(ID_MENU_VIEW_HYDRO, OnUpdateMenuViewHydro)
		ON_UPDATE_COMMAND_UI(ID_EDIT_BOUNDARY, OnUpdateEditBoundary)
		ON_UPDATE_COMMAND_UI(ID_EDIT_VELOCITY, OnUpdateEditVelocity)
		ON_UPDATE_COMMAND_UI(ID_EDIT_TEXT, OnUpdateEditText)

		ON_UPDATE_COMMAND_UI(ID_EDIT_NONODES, OnUpdateEditNonodes)
		ON_UPDATE_COMMAND_UI(ID_EDIT_TUNEUP_NOBOUNDARY, OnUpdateEditTuneupNoboundary)
		ON_UPDATE_COMMAND_UI(ID_EDIT_TUNEUP_NOCOLORMAP, OnUpdateEditTuneupNocolormap)
		ON_UPDATE_COMMAND_UI(ID_EDIT_TUNEUP_OBSPOSITIONS, OnUpdateEditTuneupObspositions)
		ON_UPDATE_COMMAND_UI(ID_EDIT_DISPLAYTUNEUP_FILLAREAONCE, OnUpdateEditDisplaytuneupFillareaonce)
		ON_UPDATE_COMMAND_UI(ID_EDIT_DISPLAYTUNEUP_BIGMODELNODES, OnUpdateEditDisplaytuneupBigmodelnodes)
		ON_UPDATE_COMMAND_UI(ID_EDIT_DISPLAYTUNEUP_EXTERNAL_DRAWINGS, OnUpdateEditDisplaytuneupExternalDrawings)
		ON_UPDATE_COMMAND_UI(ID_EDIT_DISPLAYTUNEUP_REMOVEOUTSIDERAYPOINTS, OnUpdateEditDisplaytuneupRemoveoutsideraypoints)
		ON_COMMAND(ID_FILE_EXPORTBITMAP, OnFileExportbitmap)
		ON_WM_ERASEBKGND()
		ON_WM_RBUTTONUP()
		ON_COMMAND(ID_TOOLBAR_ZOOM, OnToolbarZoom)
		ON_UPDATE_COMMAND_UI(ID_TOOLBAR_ZOOM, OnUpdateToolbarZoom)
		ON_COMMAND(ID_TOOLBAR_UNZOOM, OnToolbarUnzoom)
		ON_COMMAND(ID_TOOLBAR_HAND, OnToolbarHand)
		ON_UPDATE_COMMAND_UI(ID_TOOLBAR_HAND, OnUpdateToolbarHand)
		//}}AFX_MSG_MAP
		// Standard printing commands
		ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
		ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
		ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
		ON_WM_KEYUP()
		ON_UPDATE_COMMAND_UI(ID_VIEW_NMO_VELOCITY, &CSeisWideView::OnUpdateViewNmoVelocity)
		ON_UPDATE_COMMAND_UI(ID_VIEW_VELOCITYCOLORTWTT, &CSeisWideView::OnUpdateViewVelocitycolortwtt)

		ON_WM_KILLFOCUS()
		ON_WM_LBUTTONDBLCLK()
		ON_UPDATE_COMMAND_UI(ID_MODEL_TOGGLEDETAILS, &CSeisWideView::OnUpdateModelToggledetails)
		ON_WM_VSCROLL()
		ON_WM_SIZE()
		ON_WM_MOUSEACTIVATE()
		ON_UPDATE_COMMAND_UI(ID_EDIT_IMAGESTRETCHMODE, &CSeisWideView::OnUpdateEditImagestretchmode)
		ON_UPDATE_COMMAND_UI(ID_EDIT_LABEL_INSIDE, &CSeisWideView::OnUpdateEditLabelInside)
		ON_UPDATE_COMMAND_UI(ID_EDIT_LABLES_OUTSIDE, &CSeisWideView::OnUpdateEditLablesOutside)
		ON_COMMAND(ID_OPTIONS_TOGGLEHORIZONTALGRIDS, &CSeisWideView::OnOptionsTogglehorizontalgrids)
		ON_UPDATE_COMMAND_UI(ID_OPTIONS_TOGGLEHORIZONTALGRIDS, &CSeisWideView::OnUpdateOptionsTogglehorizontalgrids)
		ON_UPDATE_COMMAND_UI(ID_TOOLBAR_UNZOOM, &CSeisWideView::OnUpdateToolbarUnzoom)
		ON_WM_SETCURSOR()
		ON_UPDATE_COMMAND_UI(ID_MODELING_ACTIVATEOVERLAIDMODEL, &CSeisWideView::OnUpdateModelingActivateoverlaidmodel)
		ON_COMMAND(ID_MODELING_ACTIVATEOVERLAIDMODEL, &CSeisWideView::OnModelingActivateoverlaidmodel)
		ON_UPDATE_COMMAND_UI(ID_OPTIONS_TOGGLECLIPCHANGEWITHGAIN, &CSeisWideView::OnUpdateOptionsToggleclipchangewithgain)
		ON_UPDATE_COMMAND_UI(ID_MODELING_FLATTENHORIZON, &CSeisWideView::OnUpdateModelingFlattenhorizon)
		ON_WM_TIMER()
		ON_WM_MOUSEWHEEL()
		ON_UPDATE_COMMAND_UI(ID_SHOW3D_ANIMATION, &CSeisWideView::OnUpdateShow3dAnimation)
		ON_COMMAND(ID_SHOW3D_ANIMATION, &CSeisWideView::OnShow3dAnimation)
		ON_WM_SETFOCUS()
		ON_COMMAND(32936, &CSeisWideView::OnRaycodeDisplay)
		ON_UPDATE_COMMAND_UI(ID_VIEW_GRAVITYCONT2, &CSeisWideView::OnUpdateViewGravity)
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CSeisWideView construction/destruction

	CSeisWideView::CSeisWideView()
	{
		// TODO: add construction code here
		m_pSelection = NULL; 
		m_isModelSwitchMode = false;
		//bOpenGLWindowsExists = false;
		//m_scrollHelper = new CScrollHelper;
		//m_scrollHelper->AttachWnd(this);
		//m_scrollHelper->SetDisplaySize(701, 375);  // this setting is only temporary.

	}

	CSeisWideView::~CSeisWideView()
	{
		//delete m_scrollHelper;

		// must delete some transient files before closing 
		CSeisWideDoc* pDoc = GetDocument();
		CString aName = getFileNameSubdir(pDoc->draw.timeSecName, _T("rms"), _T("RmsHyperbolas.txt"), false);
		if (!aName.IsEmpty() && pDoc->dig.digName.CompareNoCase(aName)==0 && isFileExistNotDirectory(aName)) {
			if (!DeleteFile(aName)) {
				// pFrame->myCaption(_T("Failed to clean up a temporary file"));
			}
		}


		// clean up
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (!pFrame) return;

		//if (pFrame->pViewCurr && pFrame->pViewPrev && pFrame->pViewCurr->GetDocument() == this) {
		// pFrame->pViewCurr = pFrame->pViewPrev;
		// pFrame->pViewPrev = NULL;
		//}
		if (pFrame->pViewCurr && pFrame->pViewCurr == this) {
			pFrame->pViewCurr = NULL;
		}
		if (pFrame->pViewPrev && pFrame->pViewPrev == this) {
			pFrame->pViewPrev = NULL;
		}


	}

	BOOL CSeisWideView::PreCreateWindow(CREATESTRUCT& cs)
	{
		// TODO: Modify the Window class or styles here by modifying
		//  the CREATESTRUCT cs
		//if (cs.dwExStyle > 0) {
		//	cs.x = 10;
		//}

		// An OpenGL window must be created with the following
		// flags and must not include CS_PARENTDC for the
		// class style.
		cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		//  If you execute the program now and move the window around, 
		// you'll notice that it correctly fails to erase the background of the window.

		CScrollView::PreCreateWindow(cs);
		return TRUE;
	}



	/////////////////////////////////////////////////////////////////////////////
	void CSeisWideView::drawEmpty(CDC* pDC)
	{
	//	COLORREF rgbWhite = RGB(155,155,155);
	//	pDC->SetBkColor(rgbWhite);

		pDC->SetMapMode(MM_TEXT);
		CRect rect;
		GetClientRect(&rect);
		pDC->FillSolidRect(rect, GRAY);
		pDC->SetBkMode(TRANSPARENT); //to avoid texts overlap each other.

		CStringArray strArr;
		strArr.Add(_T(" You may open .sei, .sgy, .segy,"));
		strArr.Add(_T(" .txt, .shp, .bin, .vxml or most image files"));
		strArr.Add(_T("  such as .jpg, .png, .gif, .bmp, .tif, .jp2."));
		strArr.Add(_T("  If .jp2, Seiswide will prompts for Segy conversion."));
		strArr.Add(_T("   "));
		strArr.Add(_T(" Use View menu to adjust display parameters."));


		//CString tStr=_T(" You may open .sei, .sgy, .segy, .txt, .bin, .vxml or most image files by drag/drop to SeisWide.");
		CFont fnt;
		fnt.CreatePointFont(160, _T("Arial"));
		CFont *pOldFont = pDC->SelectObject(&fnt);

		for (int i=0; i<strArr.GetCount(); i++) {
			COLORREF oldColor = pDC->SetTextColor(RGB(200, 200, 200));
			pDC->TextOut(4, 30*i + 4, strArr.ElementAt(i));
			pDC->TextOut(3, 30*i + 3, strArr.ElementAt(i));
			pDC->TextOut(2, 30*i + 2, strArr.ElementAt(i));
			pDC->TextOut(1, 30*i + 1, strArr.ElementAt(i));
			pDC->SetTextColor(RGB(255, 50, 255));
			pDC->TextOut(0, 30*i + 0, strArr.ElementAt(i));
			pDC->SetTextColor(oldColor);
		}

		pDC->SelectObject(pOldFont);

		fnt.DeleteObject();

		strArr.RemoveAll();
	}

#include "dcHelper.h"
// #include "dcsystemclass.h"
	void CSeisWideView::OnDraw(CDC* pDC)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		ASSERT(pFrame);

		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);

		CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();

		CString tStr;

		if(pDoc->cd.coord.drawmode < 0 || pDoc->cd.coord.drawmode>99){
			if (pDoc->cd.coord.drawmode == -1 && pSeiApp->statWin.winPosCount < 11) {
				//pDoc->cd.coord.drawmode = -2; // will not prompt again!
				// if the user moves the CFileDialog box, OnDraw() will be fired again
				drawEmpty(pDC);
				if (!pDoc->draw.isWinShown) {
					CString strTypeSupported = _T("Supported files|*.*|SeisWide|*.SEI||");
					CFileDialog dlg(
						TRUE,             // Open File DialogBox instead of Save
						NULL,			// lpszDefExt; No default extension needed
						NULL,			// lpszFileName; No initial file name needed
						OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
						strTypeSupported
					);
					if (dlg.DoModal() == IDOK) {
						tStr = dlg.GetPathName();

						//CSeisWideDoc* pDoc_to_check = (CSeisWideDoc *)theApp->myNewDocument(tStr);
						if (pDoc->myOpenNewFile(tStr)) {
							this->Invalidate(TRUE);
						}
						else
							pFrame->myCaption(_T("Sorry, failed to recognize the format for SeisWide."));
						return;
					}
					pDoc->draw.isWinShown = TRUE;
				} // if isWinShown
			} // if
			return;
		} 
		if(pDoc->cd.coord.drawmode == 5) return;  // this is historically a disabled mode. Maybe it is nodraw?
		if (pDoc->draw.isPrintingW) return; // stop refreshing screen during printing

		pDoc->draw.isWinShown = TRUE;

		CPoint p,p1;
		CVelMod vm;

		//###################### set the view coordinates for the rectangle of  plot.######################
		/*
		// Given 
		pDC , and coord->xmin, xmax, zmin, zmax (or tmin, tmax if time domain) (input only; this function will not modify them. Please do it yourself)
		// This function calculate and assign
		2. coord->deviceAxes and devicePixelsTotal (will be slightly different fro rect after adjustments)
		1. coord->logicalAxes and logicalTotal (will adjust slightly)
		3. coord->mm_xscale, coord.mm_yscale
		4. coord->xValuePerPixel
		No global variables will be accessed exception a function mm_coord() from pDoc->pCd instance of a class
		*/
//		pDoc->cd.coord.isTimeDomain = pDoc->isTimeDomain() == TRUE;
//		pDoc->cd.coord.isTraceDomain = pDoc->cd.coord.drawmode == 6;
		int lableOutsideStat = pDoc->draw.isNoLableOutside ? 1 :
			(pDoc->draw.plotTitle.IsEmpty() ? -1 : 0);

		bool isYLabelOnRight = pFrame->statMain.isSwapXActive != pDoc->cd.coord.isSwapX;
		if (pDoc->cd.coord.isSwapYLabel) isYLabelOnRight = !isYLabelOnRight;
		pDoc->cd.setCoordMappingFromWorld(pDC, lableOutsideStat,
			isYLabelOnRight,
			(double)pDoc->draw.printX, (double)pDoc->draw.printY, &pDoc->cd.coord);

		if (pDoc->draw.wiggle != 6 && pDoc->m_GLWin.myGL.isOpenGLWindowsExists) {
			// if not in openGL mode, make sure no openGL window exists to avoid problems
			pDoc->m_GLWin.mySeis3DFBO_End();
			pDoc->m_GLWin.DestroySceneFBO();
			pDoc->m_GLWin.myGL.isOpenGLWindowsExists = false;
		}

		//#####################No more changes for the mapping parameters
		if (pDoc->draw.isNodeChanged && !pDoc->InitializeSec()) {
			tStr.Format(_T("Data initialization failed"));
			pFrame->myCaption(tStr);
		}
		// we want pDoc->tx.xShot to be defined early in this onDraw() since time reductions reply on it
		//if (!CCoords::isXShotDefined(pDoc->tx.xShotRay)) {
		//	CString workingPath = pDoc->getWorkingPathSlash();
		//	pDoc->tx.xShotRay = (float)CRayHelper::getXPosAll(pDoc->draw.timeSecName, workingPath);
		//}
		CRect viewPixelsTotal = pDoc->cd.coord.getPixelsTotal();
		CString fSection = pDoc->cd.coord.isTimeDomain() ? pDoc->draw.timeSecName : pDoc->draw.depthSecName;
		fSection = getPathAbs(fSection, pDoc->getWorkingPathSlash());  // make it absolute path so that color pallete file can be properly obtained
		if (pDoc->draw.wiggle == 6) pDoc->m_GLWin.glMap.loadGLmapping(fSection);
		if (pDC->IsPrinting()) {
			// print just started.
			pDoc->draw.isPrintingW = TRUE;
			CColorHelper::AMPCOLORDYN ampColorDyn2(fSection); // only used in color mode
			ampColorDyn2.getAmpColorArray(pDoc->head.maxAmp, false, pDoc->draw.logScaleThreshold, (double)pDoc->draw.ampPower);
			pDoc->DrawControlsSegy(pDC, &ampColorDyn2, this);
			pDoc->m_GLWin.mySeis3DFBO_Animate(pDC, -1, &pDoc->cd.coord.logicalAxes);
			pDoc->DrawControlsEx(pDC);
			//pDoc->drawFloatReflectors(pDC);  // for some reason this has to be called explicitly here???
			pDoc->Tick(pDC);	//Draw ticks.
			ampColorDyn2.freeAmpColorArray();
			pDoc->draw.isPrintingW = FALSE;
			return;
		}

		mySchrolUpdate();


		pDC->SetBkMode(TRANSPARENT); //to avoid texts overlap each other.
		// COLORREF rgbWhite = RGB(255, 255, 255);
		pDC->SetBkColor(WHITE);
		DWORD  dw = (pDoc->cd.coord.drawmode == 3 || (pDoc->draw.wiggle == 6 && pDoc->draw.isSeisScapeAnimation)) 
			? SRCAND : SRCCOPY;
		
		// all processing for the previous mode will come to below! #####################
		BOOL isTimeOld = isTimeDomainUtil(pDoc->cd.coord.drawmodeOld);
		BOOL isTimeNew = isTimeDomainUtil(pDoc->cd.coord.drawmode);
		//if (pDoc->cd.coord.drawmodeOld < 0) {
		//	// no need to T-Z conversion
		//}
		if (isTimeOld != isTimeNew) {
			static BOOL isRegressionPrompt = TRUE;
			if (isRegressionPrompt
				&& pDoc->velMod.isSedimentRegression()
				&& pDoc->cd.coord.drawmodeOld>=0) {
				isRegressionPrompt = FALSE; // only prompt once
				tStr.Format(_T("Regression is marked, therefore related sedimentary velocities on display will be ignored during time-depth conversion. Continue?"));
				if (AfxMessageBox(tStr, MB_YESNO | MB_ICONINFORMATION | MB_ICONQUESTION) == IDNO) return;
			}

			// this indicates domain changes, so the gDC cannot be used any more
			pDoc->draw.gDCs.gDC->DeleteDC();
			pDoc->draw.gDCs.gDC_isInit = FALSE;
			if (pDoc->cd.coord.isModelingMode() 
				&& pDoc->velMod.isVelModValid()
				&& pDoc->cd.coord.drawmodeOld > 0) {
				if (isTimeOld  &&  !isTimeNew) {
					// the user likely changed the model in tm[] and we now need to re-calc the zm[] array
					vm.myModel_TWTT2Depth(&pDoc->velMod, pDoc->velColor.dz);
				}
				else if (!isTimeOld && isTimeNew) {
					// the user likely changed the model in tm[] and we now need to re-calc the tm[] array
					vm.myModel_Depth2TWTT(&pDoc->velMod, pDoc->velColor.dz);
				}
			}
			dw = SRCCOPY; // we will need to scratch the complete screen display
		}
		pDoc->cd.coord.drawmodeOld = pDoc->cd.coord.drawmode;
		// modeOld processing finished ######################################################

		//if just screen refreshing is desired and no changes are made, just invoke the old memory DC and return--save a lot of time!
		if(pDoc->draw.isSameDataDisplay) {
			// note: only when the scrhol is clicked, aTop is different from pDoc->cd.coord.logicalAxes.top
			int aTop = pDoc->draw.gDCs.gDC_rect.top;
			CDC toDC;
			CDCHelper::myCreateTempDC(pDC, &toDC, viewPixelsTotal, pDoc->cd.coord.viewLogicalTotal);
			toDC.FillSolidRect(pDoc->cd.coord.viewLogicalTotal, WHITE);
			if (pDoc->cd.coord.isModelingMode()) {
				if (pDoc->cd.coord.drawmode==9) {
					if (pDoc->draw.gDCs.gDC != NULL)
						toDC.StretchBlt(pDoc->cd.coord.logicalAxes.left, aTop, pDoc->cd.coord.logicalAxes.Width(), pDoc->cd.coord.logicalAxes.Height(), pDoc->draw.gDCs.gDC,
						pDoc->draw.gDCs.gDC_rect.left, pDoc->draw.gDCs.gDC_rect.top, pDoc->draw.gDCs.gDC_rect.Width(), pDoc->draw.gDCs.gDC_rect.Height(), dw);
					pDoc->DrawVelocityTwtt(pDC);
					// DrawControlsEx() will handle velocity in TWTT
				}
				else if (pDoc->cd.coord.drawmode==3) {
					pDoc->DrawVelocityColormapDepth(&toDC, &pDoc->velMod, pDoc->draw.isNoNodes, 0., pDoc->draw.iLayerFocus);
					// Special case: put this gDC after DrawVel() 
					if (pDoc->draw.gDCs.gDC != NULL)
						toDC.StretchBlt(pDoc->cd.coord.logicalAxes.left, aTop, pDoc->cd.coord.logicalAxes.Width(),
						pDoc->cd.coord.logicalAxes.Height(), pDoc->draw.gDCs.gDC, 
						pDoc->draw.gDCs.gDC_rect.left, pDoc->draw.gDCs.gDC_rect.top, 
						pDoc->draw.gDCs.gDC_rect.Width(), 
						pDoc->draw.gDCs.gDC_rect.Height(), dw);
					pDoc->DrawVelocityBoundaryDepth(&toDC, &pDoc->velMod, pDoc->draw.isNoNodes, 0., NULL, pDoc->draw.iLayerFocus);
					pDoc->DrawVelocityLabelDepth(&toDC, &pDoc->velMod, 0., getGoodColorRand(true));
				}
				else {
					pDoc->DrawVel(&toDC);
				}
				// note: if here I use SRCAND, when the user is resizing the window, the display is blurred. So I have to use SRCCOPY here!!!
				dw = SRCCOPY;
			}
			else {
				if (pDoc->draw.gDCs.gDC != NULL) {
					if (pDoc->cd.coord.drawmode==0 
						&& !isFloatEqual((float)pDoc->draw.red.rvred, 0.0f)
						&& pDoc->draw.red != pDoc->draw.gDCs.gDC_red
						&& (!pDoc->cd.coord.isDistanceDisplay || CCoords::isXShotDefined(pDoc->tx.xShotRay))
						) {
						CDCHelper::transformCDC(pDoc->draw.gDCs.gDC, 
							&pDoc->draw.gDCs.gDC_rect, 
							pDoc->draw.gDCs.gDC_ymin,
							pDoc->draw.gDCs.gDC_red,
							&toDC,
							pDoc->draw.red,
							&pDoc->cd.coord,
							!pDoc->cd.coord.isDistanceDisplay ? 0. : pDoc->tx.xShotRay
							);
					}
					else
						toDC.StretchBlt(pDoc->cd.coord.logicalAxes.left, aTop, pDoc->cd.coord.logicalAxes.Width(), 
							pDoc->cd.coord.logicalAxes.Height(), pDoc->draw.gDCs.gDC,
							pDoc->draw.gDCs.gDC_rect.left, pDoc->draw.gDCs.gDC_rect.top, 
							pDoc->draw.gDCs.gDC_rect.Width(), pDoc->draw.gDCs.gDC_rect.Height(), SRCCOPY);

				}
			}

			if (!pDoc->bmp24.bmpName.IsEmpty())
				pDoc->DrawXImage(&toDC);
			if (pDoc->draw.wiggle == 6) {
				pDoc->m_GLWin.mySeis3DFBO_Animate(&toDC, -2, &pDoc->cd.coord.logicalAxes);
			}
			//pDoc->drawFloatReflectors(&toDC); 
			pDoc->DrawControlsEx(&toDC);  // we want to overlay a colored version
			pDoc->Tick(&toDC);	//Draw ticks.
			if (pDoc->draw.wiggle == 6) {
				pDoc->m_GLWin.mySeis3DFBO_Buttons(&toDC, pDoc->cd.coord.getDefDisplayAdjust(&toDC));
			}
			pDC->BitBlt(0, 0, pDoc->cd.coord.viewLogicalTotal.right, pDoc->cd.coord.viewLogicalTotal.bottom, &toDC, 0, 0, SRCCOPY);
			toDC.DeleteDC();
			if (pDoc->dig.isDigitizing) {
				// not working. pls test more!!!
				pDoc->DrawTempPoint(pDC, pDoc->draw.pt_addedR);
			}

			
			return;
		} // if isSameData

		// since we will re-draw everything for this window, we need wipe any existing displays, 
		//     otherwise it will catch any pixels that are at the same screen display locations (an interesting observation!)
		// m_scrollHelper->SetDisplaySize(pDoc->cd.coord.logicalAxes.Width(), pDoc->cd.coord.logicalAxes.Height());

		pDoc->draw.tmpDxdyDrawn = 0.f;  // this value will indicate that the user's on screen draw is void
		if (pDoc->draw.isSegyKeyXShift) {
			pDoc->draw.isSegyKeyXShift = FALSE;
			if (pDoc->draw.segyKeyXShift != 0.f) {
				tStr.Format(_T("Shift of segy display was %f km. You may manually shift the data accordingly."), pDoc->draw.segyKeyXShift);
				AfxMessageBox(tStr, MB_ICONQUESTION);
			}
		}



		if (pDoc->draw.gDCs.gDC_isInit) {
			pDC->StretchBlt(pDoc->cd.coord.logicalAxes.left, pDoc->cd.coord.logicalAxes.top, pDoc->cd.coord.logicalAxes.Width(), pDoc->cd.coord.logicalAxes.Height(), pDoc->draw.gDCs.gDC, 
				pDoc->draw.gDCs.gDC_rect.left, pDoc->draw.gDCs.gDC_rect.top, pDoc->draw.gDCs.gDC_rect.Width(), pDoc->draw.gDCs.gDC_rect.Height(), SRCCOPY);
			pDoc->draw.gDCs.gDC->DeleteDC(); //if gDC is still there then delete it before we create it.
			pDoc->draw.gDCs.gDC_isInit = FALSE;
		}
		//if (!pDoc->draw.gDCs.gDC_isInit) {
		//	//pDoc->draw.gDCs.gDC = new CDC;
		//	//pDoc->draw.gDCs.gDC_isInit = TRUE;
		//}
		CDCHelper::myCreateTempDC(pDC, pDoc->draw.gDCs.gDC, viewPixelsTotal, pDoc->cd.coord.viewLogicalTotal);

		CColorHelper::AMPCOLORDYN ampColorDyn2(fSection); // only used in color mode
		//double aMaxAmp = pDoc->draw.isAvgEnergy ? pDoc->head.maxAmp*pDoc->head.maxAmp : pDoc->head.maxAmp;
		//double aMaxAmp = pDoc->draw.getDrawAmp(pDoc->head.maxAmp);
		double aMaxAmp = pDoc->head.maxAmp;
		if (pDoc->cd.coord.drawmode == 3) {
			ampColorDyn2.fileName = pDoc->draw.depthSecName;
			ampColorDyn2.getAmpColorArray(aMaxAmp, false, pDoc->draw.logScaleThreshold, (double)pDoc->draw.ampPower);
			pDoc->DrawSectionArr(pDoc->draw.gDCs.gDC,
				pDoc->draw.depthSecName, pDoc->cd.coord.Y1(), pDoc->cd.coord.Y2(), &ampColorDyn2, this);
			pDC->FillSolidRect(pDoc->cd.coord.viewLogicalTotal, WHITE);
			pDoc->DrawVelocityColormapDepth(pDC, &pDoc->velMod, pDoc->draw.isNoNodes, 0., pDoc->draw.iLayerFocus);
		} else {
			ampColorDyn2.getAmpColorArray(aMaxAmp, false, pDoc->draw.logScaleThreshold, (double)pDoc->draw.ampPower);
			pDoc->draw.red.clone(&pDoc->draw.gDCs.gDC_red);
			pDoc->DrawControlsSegy(pDoc->draw.gDCs.gDC, &ampColorDyn2, this);
		}
		// now gDC is filled (which will be useful later for quick screen operation such as resizing), but pDC is blank.

		if (pDoc->draw.wiggle == 6) {
			int isAndOr = pDoc->cd.coord.drawmode == 3 ? -2 : -1;
			pDoc->m_GLWin.mySeis3DFBO_Animate(pDoc->draw.gDCs.gDC, isAndOr, &pDoc->cd.coord.logicalAxes);

			if (pDoc->draw.isSeisScapeAnimation) {
				double aBackup = pDoc->m_GLWin.glMap.camPosition.z;  // if we do not backup, this value will change during the animation
				for (int iv = 50; iv > 6; iv -= 5) {
					Sleep(100);
					pDoc->m_GLWin.mySeis3DFBO_Animate(pDC, iv, &pDoc->cd.coord.logicalAxes);
				}
				for (int iv = 6; iv <= 20; iv += 2) {
					Sleep(100);
					pDoc->m_GLWin.mySeis3DFBO_Animate(pDC, iv, &pDoc->cd.coord.logicalAxes);
				}
				pDoc->m_GLWin.glMap.camPosition.z = aBackup;
				pDoc->draw.isSeisScapeAnimation = FALSE;
			}
		}
		ampColorDyn2.freeAmpColorArray();

		// since we are not resizing window, we can use SRCAND if we want
		if (pDoc->cd.coord.drawmode == 3) dw = SRCAND;
		pDC->StretchBlt(pDoc->cd.coord.viewLogicalTotal.left, pDoc->cd.coord.viewLogicalTotal.top,
				pDoc->cd.coord.viewLogicalTotal.Width(), pDoc->cd.coord.viewLogicalTotal.Height(),
				pDoc->draw.gDCs.gDC,
				pDoc->cd.coord.viewLogicalTotal.left, pDoc->cd.coord.viewLogicalTotal.top,
				pDoc->cd.coord.viewLogicalTotal.Width(), pDoc->cd.coord.viewLogicalTotal.Height(), dw);
		pDoc->Tick(pDC);	//Draw ticks.
							// this routine must not be called from DrawControls() since it takes rectLogicalAxes not rect.

		// this is to carve the aGreyPen TT curves into the gDC so that it is tracible later
		if (pDoc->cd.coord.isTimeDomain() && isFileExistNotDirectory(pDoc->draw.timeSecName)) {
			// when file does not exist, we do not want error bars in gDC!
			// otherwise it is a mess when move arrow keys around
			CPen aGreyPen(PS_SOLID, 1, RGB(10, 10, 10));
			pDoc->myShowTravelTime(pDoc->draw.gDCs.gDC, &aGreyPen);
			aGreyPen.DeleteObject();
		}

		//now finally, draw all the line drawings, including digitized lines.
		pDoc->DrawControlsEx(pDC);  // we want to overlay a colored version

		// backup rect
		pDoc->draw.gDCs.gDC_rect = pDoc->cd.coord.logicalAxes;
		pDoc->draw.gDCs.gDC_ymin = pDoc->cd.coord.Y1();
		pDoc->draw.gDCs.gDC_isInit = TRUE;

		pDoc->draw.isSameDataDisplay = TRUE; //internal control and not saved.
	}

	void CSeisWideView::OnInitialUpdate()
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);

		//m_pSelection = NULL;    // initialize selection

		//CSize sizeTotal;
		// TODO: calculate the total size of this view
		//sizeTotal.cx = (int)(pDoc->cd.coord.logicalAxes.Width()*1.3);
		//sizeTotal.cy = (int)(pDoc->cd.coord.logicalAxes.Height()*1.3);
		//SetScrollSizes(MM_TEXT, sizeTotal);
		//DisplayScrollInfo();

		// ResizeParentToFit(TRUE);
		SetScaleToFitSize(CSize(100, 100));

		//if (pDoc->draw.wiggle == 8) {
		//	CRect rect;
		//	GetWindowRect(rect);
		//	m_oglWindow2.oglCreate(rect, this);
		//	m_oglWindow2.m_unpTimer = m_oglWindow2.SetTimer(1, 1, 0);
		//}


		CScrollView::OnInitialUpdate();

	}


	/////////////////////////////////////////////////////////////////////////////
	// CSeisWideView printing

	BOOL CSeisWideView::OnPreparePrinting(CPrintInfo* pInfo)
	{
		//if (pInfo==NULL || pInfo->m_pPD==NULL || pInfo->m_pPD->m_pd.hDC==NULL) {
		//	// this has happened a lot
		//	AfxMessageBox(_T("Current printer's parameterization cannot be detected. Please select a workable printer and try again."));
		//	return TRUE;
		//}

		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);

		//Re-initialize all sections before printing which has a much higher resolution.
		// otherwise, proper gain will not be re-calculated.
		this->gIsNoLableOutsideBackup = pDoc->draw.isNoLableOutside;
		this->gIsNoLabelInSideBackup = pDoc->draw.isNoLabelInSide;
		this->gisNoBoundaryBackup = pDoc->draw.isNoBoundary;

		CdlgPrint dlg;
		if(pDoc->draw.printX<1.f || pDoc->draw.printX>10.f) pDoc->draw.printX = 1.f;
		if(pDoc->draw.printY<1.f || pDoc->draw.printY>10.f) pDoc->draw.printY = 1.f;

		dlg.m_printX = pDoc->draw.printX;
		dlg.m_printY = pDoc->draw.printY;

		bool isAreaFill = pDoc->draw.wiggle == 1 || pDoc->draw.wiggle == 2 || pDoc->draw.wiggle == 3;
		dlg.m_compactToOnePerTrace = isAreaFill ? pDoc->areaFill.isFillSmall : -1;
			// if it is -1 then those two radio buttons will be disabled
		dlg.m_memoryMax = pDoc->draw.memMaxMB;
		dlg.m_removeLabels = pDoc->draw.isNoLableOutside;
		dlg.m_isNoLabelsInside = pDoc->draw.isNoLabelInSide;
		dlg.m_isNoLines = pDoc->draw.isNoBoundary;
		pDoc->draw.isNodeChanged = FALSE;	//do not refresh screen for now.
		if (dlg.DoModal() == IDOK) {
			pDoc->draw.printX = dlg.m_printX;
			pDoc->draw.printY = dlg.m_printY;
			pDoc->areaFill.isFillSmall = dlg.m_compactToOnePerTrace;
			pDoc->draw.memMaxMB = dlg.m_memoryMax;
			pDoc->draw.isNoLableOutside = dlg.m_removeLabels;
			pDoc->draw.isNoLabelInSide = dlg.m_isNoLabelsInside;
			pDoc->draw.isNoBoundary = dlg.m_isNoLines;
		} else {
			pDoc->draw.isNodeChanged = TRUE;	// refresh screen if wanted.
			return FALSE;
		}

		pDoc->draw.isNodeChanged = FALSE;	//do not refresh screen for now.
		pDoc->draw.isSameDataDisplay = FALSE; //indicate display has been changed and will re-load all data in OnDraw().
		pInfo->SetMaxPage(1);
		return DoPreparePrinting(pInfo);
	}

	void CSeisWideView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		//Re-initialize all sections after printing, 
		// since screen has a much lower resolution than printers.
		// otherwise, proper gain will not be re-calculated.
		//pDoc->draw.isSameDataDisplay = FALSE; //internal control and not saved. 
		SWITCH_BOOL(pDoc->draw.isNoLableOutside, gIsNoLableOutsideBackup);
		SWITCH_BOOL(pDoc->draw.isNoLabelInSide, gIsNoLabelInSideBackup);
		SWITCH_BOOL(pDoc->draw.isNoBoundary, gisNoBoundaryBackup);


		// Invalidate(FALSE);	
		// TODO: add cleanup after printing
		CMainFrame* pFrame = 
			(CMainFrame*)AfxGetMainWnd();
		ASSERT(pFrame);
		ASSERT_KINDOF(CMainFrame, pFrame);
		// pFrame->gIsDisplayFreeze = FALSE;
	}

	/////////////////////////////////////////////////////////////////////////////
	// OLE Client support and commands

	void CSeisWideView::OnInsertObject()
	{
		// Invoke the standard Insert Object dialog box to obtain information
		//  for new CSeisWideCntrItem object.
		COleInsertDialog dlg;
		if (dlg.DoModal() != IDOK)
			return;

		BeginWaitCursor();

		CSeisWideCntrItem* pItem = NULL;
		TRY
		{
			// Create new item connected to this document.
			CSeisWideDoc* pDoc = GetDocument();
			ASSERT_VALID(pDoc);
			pItem = new CSeisWideCntrItem(pDoc);
			ASSERT_VALID(pItem);

			// Initialize the item from the dialog data.
			if (!dlg.CreateItem(pItem))
				AfxThrowMemoryException();  // any exception will do
			ASSERT_VALID(pItem);

			// If item created from class list (not from file) then launch
			//  the server to edit the item.
			if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
				pItem->DoVerb(OLEIVERB_SHOW, this);

			ASSERT_VALID(pItem);

			// As an arbitrary user interface design, this sets the selection
			//  to the last item inserted.

			// TODO: reimplement selection as appropriate for your application

			m_pSelection = pItem;   // set selection to last inserted item
			pDoc->UpdateAllViews(NULL);
		}
		CATCH(CException, e)
		{
			if (pItem != NULL)
			{
				ASSERT_VALID(pItem);
				pItem->Delete();
			}
			AfxMessageBox(IDP_FAILED_TO_CREATE);
		}
		END_CATCH

			EndWaitCursor();
	}

	// The following command handler provides the standard keyboard
	//  user interface to cancel an in-place editing session.  Here,
	//  the container (not the server) causes the deactivation.
	void CSeisWideView::OnCancelEditCntr()
	{
		// Close any in-place active item on this view.
		COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
		if (pActiveItem != NULL)
		{
			pActiveItem->Close();
		}
		ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
	}


	/////////////////////////////////////////////////////////////////////////////
	// OLE Server support

	// The following command handler provides the standard keyboard
	//  user interface to cancel an in-place editing session.  Here,
	//  the server (not the container) causes the deactivation.
	void CSeisWideView::OnCancelEditSrvr()
	{
		GetDocument()->OnDeactivateUI(FALSE);
	}

	/////////////////////////////////////////////////////////////////////////////
	// CSeisWideView diagnostics

#ifdef _DEBUG
	void CSeisWideView::AssertValid() const
	{
		CScrollView::AssertValid();
	}

	void CSeisWideView::Dump(CDumpContext& dc) const
	{
		CScrollView::Dump(dc);
	}

	CSeisWideDoc* CSeisWideView::GetDocument() // non-debug version is inline
	{
		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSeisWideDoc)));
		return (CSeisWideDoc*)m_pDocument;
	}
#endif //_DEBUG

	/////////////////////////////////////////////////////////////////////////////
	// CSeisWideView message handlers


	void CSeisWideView::OnLButtonDown(UINT nFlags, CPoint point) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);

		if (pDoc->draw.wiggle == 6) {
			//transient.tickTimeStart = GetTickCount();
			//if (transient.tickTimeEnd == 0) transient.tickTimeEnd = transient.tickTimeStart;
			CString tStr;
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			tStr = pDoc->m_GLWin.mySeis3DFBO_ButtonClick(pDoc->cd.coord.pixel2logicalScreen(point.x, point.y));
			if (!tStr.IsEmpty()) {
				CRect rect2 = pDoc->cd.coord.getPixelsAxes();
				rect2.left = 0;
				InvalidateRect(rect2, TRUE); // here if I invalidate the whole screen, the print of tStr will be gone.
				//pFrame->myCaption(tStr); // this does not work in release mode, although it works ok in debug mode. Not sure why.

				CClientDC ClientDC(this);  // pixel graphics
					//ClientDC.SetROP2(R2_NOT);
				tStr += +_T("   "); // why adding 3 spaces? It is because when the text is not long, it may not cover all previous  numbers at the end
				ClientDC.TextOut(2, pDoc->cd.coord.getPixelsTotal().bottom-22, tStr);
			}
		} // if

		pDoc->cd.coord.m_point = point;
		this->transient.m_Drag = true;
		transient.PointOld = point;
		transient.PointScrollRef = GetScrollPosition();

	//	CScrollView::OnLButtonDown(nFlags, point);
	}

	void CSeisWideView::OnLButtonUp(UINT nFlags, CPoint point) 
	{

		//Consider: (1) points lie between xmin and xmax,
		//				and between zmin and zmax (done).
		//			(2) no points overlap. points has to be in
		//increasing order.  Low velocity layer not allowed.
		//One layer/boundary may have one point only.
		this->transient.m_Drag = false;
		this->transient.MotionFix=0;

		CSeisWideDoc* pDoc = GetDocument();

		if (pDoc->draw.wiggle == 6 && pDoc->m_GLWin.myGL.m_isGLButtonsShowing) return;
		// this line will make sure when the mouse is inside the GL buttons area, mouse clicking will not affect others such as zooming, digitizing and model editting

		CString tStr;
		SEI_PAIR sp = pDoc->cd.coord.pixel2XY(pDoc->cd.coord.m_point);
		transient.aQuad.x1 = sp.x;
		transient.aQuad.y1 = sp.y;
		sp = pDoc->cd.coord.pixel2XY(point);
		transient.aQuad.x2 = sp.x;
		transient.aQuad.y2 = sp.y;
		double width = pDoc->cd.coord.xWidth();
		double height = pDoc->cd.coord.yHeight();
		bool isClickSingle = fabs(transient.aQuad.width()) < width * 0.01 &&
			fabs(transient.aQuad.height()) < height * 0.01;

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		bool isEditting = pFrame->statMain.add != 0 || pFrame->statMain.pick != 0 ||
			pFrame->statMain.move != 0 || pFrame->statMain.remove != 0;
		CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();
		bool isPanning = pSeiApp->statWin.getShiftPressed() || pDoc->cd.zoom.isMouseHand;

		BOOL isTime = pDoc->cd.coord.isTimeDomain();

		//SEI_QUAD aQuad;
		//aQuad.x1 = transient.xOld;
		//aQuad.y1 = transient.yOld;
		//aQuad.x2 = transient.xNew;
		//aQuad.y2 = transient.yNew;

		if (this->m_isModelSwitchMode) {
			this->m_isModelSwitchMode = false;
			if (pDoc->draw.velSecNamesEx.GetLength() > 0) {
				CVelMod::VELMOD velm;
				CVelMod vm;
				//vm.velModSetArrays(&velm);  // why need this?
				double xposArr[MOD_POINTS_MAX];
				double xShiftArr[MOD_POINTS_MAX];
				CStringArray arrVelNamesEx;
				CVelMod::velNamesExParse(pDoc->draw.velSecNamesEx, xShiftArr, xposArr, &arrVelNamesEx);
				bool isModified = false;
				for (int i = 0; i < arrVelNamesEx.GetCount(); i++) {
					tStr = arrVelNamesEx.ElementAt(i);
					if (vm.loadInitVel(&velm, tStr, pDoc->velColor.dz, tStr)) {
						if (isInsideDouble(transient.aQuad.x2, xShiftArr[i]+velm.getXmin(), xShiftArr[i]+velm.getXmax())) {
							vm.formatCalcVelLimits(&velm);
							register double vxmin = velm.getXmin();
							register double vxmax = velm.getXmax();
							if (transient.aQuad.x2 >= vxmin + xShiftArr[i] && transient.aQuad.x2 <= vxmax + xShiftArr[i]) {
								// chosen model to swap with pDoc->velMod.velSecName
								if (xShiftArr[i] != 0.) {
									// need to shift the model before swap. Testing more please!!!
									vm.saveModelXML_V3(&velm, velm.velSecName,
										pDoc->velColor.dz, FALSE); // old model untouched
									velm.velSecName = getFileNameAppend(velm.velSecName, _T("_Shifted")); // new model is renamed
									vm.myModelShiftX(&velm, (float)xShiftArr[i]);
									vm.saveModelAs(&velm, velm.velSecName, pDoc->velColor.dz, isTime);
									tStr.Format(_T("Newly activated model is shifted by %g km from original in order to become editable."), xShiftArr[i]);
									pFrame->myCaption(tStr);
									xposArr[i] += xShiftArr[i];
									xShiftArr[i] = 0.;
								}

								register double a = pDoc->tx.xShotRay;
								pDoc->tx.xShotRay = (float)xposArr[i];
								xposArr[i] = a;

								tStr = pDoc->velMod.velSecName;
								pDoc->velMod.velSecName = velm.velSecName; // swapped
								if (isFileExistNotDirectory(tStr)) {
									arrVelNamesEx.SetAt(i, tStr);
									pDoc->draw.velSecNamesEx = CVelMod::velNamesExToString(xShiftArr, xposArr, &arrVelNamesEx);
								}
								else {
									// if here, this index will be excluded
									pDoc->draw.velSecNamesEx = CVelMod::velNamesExToString(xShiftArr, xposArr, &arrVelNamesEx, i);
								}
								isModified = true;
								break;
							}
						} // if
					} // if
					else {
						// this model loading failed
						pFrame->myCaption(tStr);
					}
				}
				arrVelNamesEx.RemoveAll();
				if (isModified) {
					Invalidate(TRUE);
				}
			}
		}
		else if( 
			pFrame->statMain.move == 1 &&
			pSeiApp->statWin.isAPressed &&
			(pDoc->cd.coord.drawmode==0 || pDoc->cd.coord.drawmode==7) && 
			isFileExistNotDirectory(pDoc->draw.timeSecName) &&
			!isFloatEqual(transient.aQuad.y2, transient.aQuad.y1) &&
			fabs(transient.aQuad.x2-transient.aQuad.x1) < width*0.01
			) 
		{
			// the xOld and xNew are equal, and yOld and yNew are NOT equal. This means we want to do static shifting for the currently selected trace
			SegyWrapper sw(pDoc->draw.timeSecName, true); 
			if (CSegyMeta::segyHeadInit(pDoc->draw.timeSecName, &pDoc->head) != 1) {
				if (sw.open(&pDoc->head, 0)) {
					int indx = sw.getClosestTrcIndexAtOffset(
						pDoc->cd.coord.getOffsetFromXVal(transient.aQuad.x1, pDoc->tx.xShotRay)
						);
					int itrc = sw.headRead.trArr[indx].tracl;
					float deltat = (float)(transient.aQuad.y2 - transient.aQuad.y1);
					if (sw.staticShift(itrc, deltat)) {
						pDoc->draw.isSameDataDisplay = FALSE;
						Invalidate(TRUE);
					}
				} // if sw.open
			}
		} 
		else if (isClickSingle) {
			if (!isEditting && pDoc->cd.zoom.isZooming) {
				// single click zooming
				double dx = (pDoc->cd.zoom.isZooming < 0) ? width*1.25 : width*0.25;
				if (pDoc->cd.coord.drawmode == 6) {
					pDoc->cd.coord.trace1 = (long)pDoc->cd.coord.xmin;
					pDoc->cd.coord.trace2 = (long)pDoc->cd.coord.xmax;
				}
				else {
					pDoc->cd.coord.xmin = myFloat(transient.aQuad.x2-dx);
					pDoc->cd.coord.xmax = myFloat(pDoc->cd.coord.xmin+2.*dx);
				}

				double dy = (pDoc->cd.zoom.isZooming < 0) ? height*1.25 : height*0.25;
				pDoc->cd.coord.sety(transient.aQuad.y2 - dy, transient.aQuad.y2 + dy);
				pDoc->draw.isNodeChanged = TRUE;
				pDoc->draw.isSameDataDisplay = FALSE;
				Invalidate(FALSE);	
			}
			else if(pDoc->MyMouseClick(&transient.aQuad)) {
				Invalidate(pDoc->draw.isNodeChanged);
			}
		}
		// #####from here down, we handle drag and drops####
		else if(isPanning) {
			// if here, the display is panning, will only move around with hand cursor, not zooming in at all
			double dx = transient.aQuad.width();
			double dy = transient.aQuad.height();
			pDoc->cd.coord.setx(pDoc->cd.coord.X1() - dx, pDoc->cd.coord.X2() - dx);
			pDoc->cd.coord.sety(pDoc->cd.coord.Y1() - dy, pDoc->cd.coord.Y2() - dy);
			pDoc->draw.isNodeChanged = TRUE;
			pDoc->draw.isSameDataDisplay = FALSE;
			Invalidate(FALSE);	
		} 
		else if (pDoc->cd.zoom.isZooming && !isEditting) {
			// zooming, we take drag and drop positions for zooming
			// sometimes the user use mouse to drag from right to left or from down to up.
			// What about if the vertical axis is reversed? Help !!!
			double xx1 = min(transient.aQuad.x1, transient.aQuad.x2);
			double xx2 = max(transient.aQuad.x1, transient.aQuad.x2);
			double yy1 = min(transient.aQuad.y1, transient.aQuad.y2);
			double yy2 = max(transient.aQuad.y1, transient.aQuad.y2);
			pDoc->cd.coord.setx(xx1, xx2);
			pDoc->cd.coord.sety(yy1, yy2);
			pDoc->draw.isNodeChanged = TRUE;
			pDoc->draw.isSameDataDisplay = FALSE;
			Invalidate(FALSE);	
		}
		else {
			if (pDoc->MyMouseClick(&transient.aQuad)) {
				Invalidate(!pDoc->draw.isNodeChanged);
			}
		}

		CScrollView::OnLButtonUp(nFlags, point);
	}

#include "dlgInputValie.h"
	void CSeisWideView::OnRButtonUp(UINT nFlags, CPoint point) 
	{
		// TODO: Add your message handler code here and/or call default
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CString tStr;
		int j;

		//if (pDoc->draw.wiggle == 6) {
		//	transient.tickTimeEnd = 0;
		//}

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (!pFrame) return;

		if (pDoc->cd.coord.drawmode == 8 && pDoc->draw.iLayerFocus>0) {
			// denView mode
			float aF;
			CdlgInputValie dlg;
			int iFocus = pDoc->draw.iLayerFocus;
			dlg.m_value = pDoc->gden.dm[iFocus];
			if (dlg.DoModal() == IDOK) {
				aF = dlg.m_value;
			}
			else return;
			delete dlg;

			if (aF < 1.f || aF > 10.f) {
				AfxMessageBox(_T("Sorry, density value entered incorrect."));
			}
			pDoc->gden.dm[iFocus] = aF;

			tStr.Format(_T("Density updated. Do you want to save it to disk?"));
			if (AfxMessageBox(tStr, MB_YESNO | MB_ICONINFORMATION) == IDYES) {
				// now save
				if (pDoc->gden.saveDen(pDoc->gden.denName))
					pFrame->myCaption(_T("Den model saved sucessfully to ") + pDoc->gden.denName);
			}

			Invalidate(TRUE);
			return;
		}

		if (pDoc->cd.coord.drawmode == 10) {
			// RMS mode
			SEI_PAIR sp = pDoc->cd.coord.pixel2XY(point);
			transient.aQuad.x2 = sp.x;
			transient.aQuad.y2 = sp.y;
			long dPlt = (int)myRound(3 * pFrame->statMain.thicknessLines);
			if (dPlt > 10) dPlt = 10;

			// draw on the current view
			if (pDoc->dig.nPoint != NULL && pDoc->dig.nPoint[0] > 0 && pDoc->cd.coord.xmax > 2.) {
				int np = pDoc->dig.nPoint[0] + 1;
				float *tau = (float *)ealloc1float(np * 2);
				float *vrms = (float *)ealloc1float(np * 2);
				float *vint = (float *)ealloc1float(np * 2);
				// the first point should be at surface
				tau[0] = 0.f;
				vrms[0] = pDoc->dig.x[0][0]; // surface point should have the same velocity as the seafloor
				for (j =1; j<np; j++) {
					tau[j] = pDoc->dig.t[j - 1][0];
					vrms[j] = pDoc->dig.x[j - 1][0];
				} // for j
				CVelMod vm;
				vm.getVelocityIntFromRms(np, &tau[0], &vrms[0], &vint[0]);
				double x,t;

				CStringArray *strArr = new CStringArray();
				tStr.Format(_T("##RmsVelocity(km/s)  TWTT(s)"));
				strArr->Add(tStr);
				for (j=0; j<np; j++) {
					tStr.Format(_T("%.3f  %.3f"), vrms[j], tau[j]);
					strArr->Add(tStr);
				} // for j

				tStr.Format(_T("##RmsVelocity(km/s)  IntVelocity(km/s)  TWTT(s)  Tbsl(s)"));
				strArr->Add(tStr);
				for (j=0; j<np; j++) {
					t = (j>0)?  tau[j]-tau[1]  :  0.f;  // tau[1] is assumed to be seafloor
					tStr.Format(_T("%.3f  %.3f  %.3f  %.3f"), vrms[j], vint[j], tau[j], t);
					strArr->Add(tStr);
				} // for j

				tStr.Format(_T("##IntVelocity(km/s)  TWTT(s) (for charting)"));
				strArr->Add(tStr);
				for (j=0; j<np; j++) {
					tStr.Format(_T("%.3f  %.3f"), vint[j], tau[j]);
					strArr->Add(tStr);
					t = (j<np-1)?  tau[j+1]  :  tau[j]+0.2f;
					tStr.Format(_T("%.3f  %.3f"), vint[j], t);
					strArr->Add(tStr);
				} // for j
				tStr = getFileNameSubdir(pDoc->draw.timeSecName, _T("rms"), _T("calcIntVelocities.txt"), true);
				if (saveStringArrayToFile(strArr, tStr))
					showFileInNotePad(tStr);
				strArr->RemoveAll();
				delete strArr;

				int oldROP = this->GetDC()->SetROP2(R2_NOT);  // (this->GetDC()->GetDeviceCaps(HORZRES) * 0.001
				CPoint point;
				for (j=0; j<np-1; j++) {  /// ignore the first point since is sea surface
					t = (tau[j]+tau[j+1]) * 0.5;
					if (t > pDoc->cd.coord.Y1() && t < pDoc->cd.coord.Y2()) {
						x = (vrms[j]+vrms[j+1]) * 0.5;
						point = pDoc->cd.coord.XY2pixel(x, t);
						tStr.Format(_T("%1.2f"), vint[j]);
						this->GetDC()->TextOut(point.x-dPlt,point.y-dPlt, tStr);
					}
				}

				this->GetDC()->SetROP2(oldROP);
				free1float(tau);
				free1float(vint);
				free1float(vrms);
			} // if digitizing






			if (pFrame->pViewPrev != NULL && pFrame->pViewCurr != pFrame->pViewPrev) {
				CSeisWideView* pViewPrev = (CSeisWideView *)pFrame->pViewPrev;
				CSeisWideDoc* pDocPrev = pViewPrev->GetDocument();
				if (!pDocPrev->cd.coord.isDistanceDisplay && transient.aQuad.x2 > 0.1
					&& (pDocPrev->cd.coord.drawmode == 0 || pDocPrev->cd.coord.drawmode == 7)
					&& pDocPrev->head.numTraces > 2 
					&& pDocPrev->head.offsets != NULL) {
						double oneov2 = 1. / (transient.aQuad.x2 * transient.aQuad.x2);
						double t2 = transient.aQuad.y2*transient.aQuad.y2;
						double x, t;
						CPen *oldPen;

						if (pDocPrev->dig.isDigitizing) {
							CPoint *plt = (CPoint *)alloc1CPoint(pDocPrev->head.numTraces);
							int numPointsPlot = 0;
							for (int i=0; i<pDocPrev->head.numTraces; i++) {
								x = pDocPrev->head.offsets[i];
								t = sqrt(t2 + x*x*oneov2); 

								// remove over-stretched points
								if (transient.aQuad.y2>0.001f &&
									(t-transient.aQuad.y2)/transient.aQuad.y2 > 0.9) continue;

								// apply reduction velocity
								t -= CCoords::getReduceTimeEx2(x, pDocPrev->cd.coord.drawmode, &pDocPrev->draw.red);
								if (pDocPrev->cd.coord.isVisibleOrNot(x, t)) {
									plt[numPointsPlot] = pViewPrev->X2CpointDevice(x, t);
									point = pDoc->cd.coord.XY2pixel(x, t);

									numPointsPlot++;
								}
							} // for i
							CPen redPen(PS_SOLID, 10, RGB(255,22,0));
							oldPen = pViewPrev->GetDC()->SelectObject(&redPen);
							int oldROP = pViewPrev->GetDC()->SetROP2(R2_COPYPEN);
							pViewPrev->GetDC()->Polyline(plt, numPointsPlot);
							pViewPrev->GetDC()->SetROP2(oldROP);
							pViewPrev->GetDC()->SelectObject(oldPen);
							free1CPoint(plt);
						}
						else {
							// if the prev window is not digitizing, we want to show all the hyperbola
							CDigitize pDg;
							pDocPrev->dig.digName = getFileNameSubdir(pDocPrev->draw.timeSecName, _T("rms"), _T("RmsHyperbolas.txt"), true);
							if (isFileExistNotDirectory(pDocPrev->dig.digName)) {
								pDg.myDigitizingLoadTXTUnix2(&pDocPrev->dig, FALSE);
							} else {
								// pDocPrev->initDigitizeArray(pDocPrev->head.numTraces, 20);
								pDocPrev->dig.initDigitizeArray(pDocPrev->head.numTraces, 20);
							}
							int ic = pDocPrev->dig.totCurve;
							pDocPrev->dig.nPoint[ic] = 0;
							for (int i=0; i<pDocPrev->head.numTraces; i++) {
								x = pDocPrev->head.offsets[i];
								t = sqrt(t2 + x*x*oneov2); 
								// apply reduction velocity: with digitizing format, we cannot apply reducing velocity!!!

								// remove over-stretched points
								if (transient.aQuad.y2>0.001f && (t-transient.aQuad.y2)/transient.aQuad.y2 > 0.9) continue;

								if (pDocPrev->dig.nPoint[ic] >= pDocPrev->dig.digPointMax) break;

								pDocPrev->dig.x[pDocPrev->dig.nPoint[ic]][ic] = (float)x;
								pDocPrev->dig.t[pDocPrev->dig.nPoint[ic]][ic] = (float)t;
								pDocPrev->dig.nPoint[ic]++;
							} // for i
							if (pDocPrev->dig.nPoint[ic] > 0) {
								pDocPrev->dig.totCurve++;
								pDocPrev->myDigSaveAs(pDocPrev->dig.digName);
								pViewPrev->Invalidate(TRUE);
							}
						}

						// now draw on the current view
						int oldROP =  this->GetDC()->SetROP2(R2_NOT);  // (this->GetDC()->GetDeviceCaps(HORZRES) * 0.001
						this->GetDC()->Ellipse(point.x-dPlt,point.y-dPlt,point.x+dPlt,point.y+dPlt);
						this->GetDC()->SetROP2(oldROP);
				}
			} // if pFrame
		}




		// NOTE: when the user clicked on a new editting mode, the previous editting mode is stored as a single 
		//       integer as draw.pickOld (0="pick", 1="remove", 2="move", 3="add")
		//       While new editting mode has not been assigned yet abd shall be implemented here! (with several integers: draw.pick, draw.remove, draw.move, draw.add
		int pickOld = pFrame->statMain.getPtdraw();
		pickOld++;
		pFrame->statMain.setPtdraw(pickOld);
		

	//	CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();
	//	pSeiApp->statWin.getControlPressed() = FALSE; // sometimes the program has set this value wrong for a long time. So we fix it here.

		CScrollView::OnRButtonUp(nFlags, point);
	}

// #include "GL/Window/Window.hpp"
	void CSeisWideView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
	{

		CSeisWideDoc* pDoc = GetDocument();
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		CScrollView::OnPrint(pDC, pInfo);

		//CGLObj::OnPrint(pDC, pInfo, this);

	//	GL::Window window(800, 600, "OpenGL Window", GL::WindowStyle::Close);
	}

	void CSeisWideView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
	{
		CString tStr;
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();

		// https://css-tricks.com/snippets/javascript/javascript-keycodes/
		// this site will tell you all the exact key codes
		if (nChar == 27) {
			pDoc->draw.isTerminate = TRUE;
		}
		else pDoc->draw.isTerminate = FALSE;

		if (nChar == VK_SHIFT) {
			pDoc->cd.zoom.isMouseHand = TRUE;
		//	pFrame->myCaption(_T("Shift key pressed"));
		}
		//else if (nChar == VK_CONTROL) {
		//	pSeiApp->statWin.isControlPressed = TRUE;
		////	pFrame->myCaption(_T("Control key pressed"));
		//}
		//else if (nChar == VK_ALT) {  // VK_ALT cannot be catched. Why???
		//	pSeiApp->statWin.isControlPressed = TRUE;
		//	//	pFrame->myCaption(_T("Control key pressed"));
		//}
		else if (nChar == 65) {
			pSeiApp->statWin.isAPressed = TRUE;
			//pDoc->myCaption(_T("A key pressed");
		}


		// 39=rigth arrow;40=down arrow;  37=left arrow;38=up arrow pressed
		if ( (nChar == 37 || nChar == 39) && pDoc->draw.isSegyKeyXShift) {
			int inc = (nChar == 37)  ?  -10  :  10;
			pDoc->draw.gDCs.gDC_rect.left -= inc;
			pDoc->draw.segyKeyXShift += (float)(inc / pDoc->cd.coord.mm_xscale);
			tStr.Format(_T("Segy display shifted by %f km."), pDoc->draw.segyKeyXShift);
			pFrame->myCaption(tStr);
			this->Invalidate(0);
			return;
		} else  if (pDoc->cd.coord.isModelingMode()) {
			if (pSeiApp->statWin.getControlPressed()) {
				if (nChar == 39) {
					if (pDoc->draw.xWell == ERRORVAL)
						pDoc->draw.xWell = (float)pDoc->velMod.getXWellDef();
					if (pDoc->draw.xWell < pDoc->cd.coord.xmin || pDoc->draw.xWell > pDoc->cd.coord.xmax) {
						// when the currentl xWell is not visible on screen, I put it in the screen center
						pDoc->draw.xWell = (float)(pDoc->cd.coord.xmin + pDoc->cd.coord.xmax) * 0.5f;
					}
					else
						pDoc->draw.xWell += 1.f;
					this->Invalidate(0);
				}
				else if (nChar == 37) {
					if (pDoc->draw.xWell == ERRORVAL)
						pDoc->draw.xWell = (float)pDoc->velMod.getXWellDef();
					if (pDoc->draw.xWell < pDoc->cd.coord.xmin || pDoc->draw.xWell > pDoc->cd.coord.xmax) {
						// when the currentl xWell is not visible on screen, I put it in the screen center
						pDoc->draw.xWell = (float)(pDoc->cd.coord.xmin + pDoc->cd.coord.xmax) * 0.5f;
					}
					else
						pDoc->draw.xWell -= 1.f;
					this->Invalidate(0);
				}
			}
			else {
				// if here, the control key is not pressed
				CRayHelper::RAYPATHS* pRay = pDoc->cd.coord.isTimeDomain() ? &pDoc->raypathsTwtt : &pDoc->raypaths;
				if (nChar == 39) {
					// right arrow key pressed
					if (pRay->m_totCurve > 0 && pDoc->draw.isRayShowing) {
						if (pRay->m_totCurve == 1) {
							pDoc->draw.iLayerFocus++;
						}
						else if (pRay->m_totCurve > 1 && pRay->iShot > pRay->m_totCurve - 1)
							pRay->iShot = -1;
						else pRay->iShot++;
						this->Invalidate(0);
					}
				}
				else if (nChar == 37) {
					// left arrow key pressed
					if (pRay->m_totCurve > 0 && pDoc->draw.isRayShowing) {
						if (pRay->iShot > -1) pRay->iShot--;
						if (pRay->iShot < -1) pRay->iShot = -1;
						this->Invalidate(0);
					}
				}
				else if (nChar == VK_HOME) {
					pRay->iShot = -1;
					pRay->rayCodeShow = -1;
					this->Invalidate(0);
				}
				else if (nChar == VK_END) {
					pRay->iShot = pRay->m_totCurve - 1; // iShot starts from 0.
					this->Invalidate(0);
				}
				else if (nChar == 40) {
					// downward key pressed
					if (pDoc->draw.isRayShowing) {
						pRay->rayCodeShow++;
						if (pRay->rayCodeShow > 20) pRay->rayCodeShow = -1;
						this->Invalidate(0);
					}
					else {
						pDoc->draw.iLayerFocus++;
						if (pDoc->draw.iLayerFocus > pDoc->velMod.ncont)
							pDoc->draw.iLayerFocus = -1;
						this->Invalidate(0);
					}
				}
				else if (nChar == 38) {
					// upward key pressed
					if (pDoc->draw.isRayShowing) {
						if (pRay->rayCodeShow > -1) pRay->rayCodeShow--;
						this->Invalidate(0);
					}
					else {
						pDoc->draw.iLayerFocus--;
						if (pDoc->draw.iLayerFocus < -1)
							pDoc->draw.iLayerFocus = -1;
						this->Invalidate(0);
					}
				}



			} // control pressed
		} // modeling mode
		else
			pDoc->draw.isSegyKeyXShift = FALSE;



		if (pDoc->tx.iShot >= 0) {
			tStr.Format(_T("Ray index %d ;  Source Position: %f km, %f"), pDoc->tx.iShot, pDoc->tx.xzShot[0][pDoc->tx.iShot], pDoc->tx.xShotRay);
			pFrame->myCaption(tStr);
		}


		CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
	}

	void CSeisWideView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		// TODO: Add your message handler code here and/or call default
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();

		if (nChar == VK_SHIFT) {
			pDoc->cd.zoom.isMouseHand = FALSE;
			pFrame->myCaption(_T("Shift key released"));
		}
		else if (nChar == 65) {
			pSeiApp->statWin.isAPressed = FALSE;
			//pDoc->myCaption(_T("A key released");
		}
		else {
			// for all other keys pressed, we need to set the control and shift keys off, otherwise they can be still on
			pSeiApp->statWin.isAPressed = FALSE;
			//pSeiApp->statWin.isControlPressed = FALSE;
		}

		CScrollView::OnKeyUp(nChar, nRepCnt, nFlags);
	}

	void CSeisWideView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
	{
		// The pActivateView and pDeactiveView parameters point to the same view if the application's main 
		// frame window is activated with no change in the active view.

		if(bActivate) {
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if (!pFrame) return;
			UpdateViewMenu();

			if (pFrame->pViewCurr != this) {
				// now the currently active document is changed. we need to update the 2 pointers
				pFrame->pViewPrev = pFrame->pViewCurr;
				pFrame->pViewCurr = this;

				// when a child window is activated, we want to have the correct "current" dir to be the working dir.
				CSeisWideDoc* pDoc = GetDocument();
				CString test = pDoc->getWorkingPathSlash();
				if (test.Find(_T('.')) > 0)
					SetCurrentDirectory(test);

				if (pDoc->cd.coord.isSeismicDisplay()) {
					pFrame->m_wndToolBar.m_pDocActive = pDoc;
					pFrame->m_slider1Threshold.SetPos(pDoc->draw.logScaleThreshold);
					pFrame->m_slider1Threshold.ShowWindow(SW_SHOW);
				}
				else {
					pFrame->m_wndToolBar.m_pDocActive = NULL;
					pFrame->m_slider1Threshold.ShowWindow(SW_HIDE);
				}
			}
		}	//end of bActivate loop.
		CScrollView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	}

	void CSeisWideView::UpdateViewMenu()
	{
		//this routine is written by Deping called only in CView class to update menus. 
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if(pDoc->cd.coord.drawmode < 0 || pDoc->cd.coord.drawmode == 5) return;
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		ASSERT(pFrame);
		ASSERT_KINDOF(CMainFrame, pFrame);
		CMenu* menu = pFrame->GetMenu( );

		menu->CheckMenuItem( ID_VIEW_SEGYINFO, MF_UNCHECKED);
		menu->CheckMenuItem( ID_VIEW_TIMESECTRC, MF_UNCHECKED);
		menu->CheckMenuItem( ID_VIEW_TIMESECTION, MF_UNCHECKED);
		menu->CheckMenuItem( ID_VIEW_DEPTHSECTION, MF_UNCHECKED);
		menu->CheckMenuItem( ID_VIEW_VELOCITYCOLORMAP, MF_UNCHECKED);
		menu->CheckMenuItem( ID_VIEW_BOTH, MF_UNCHECKED);
		menu->CheckMenuItem( ID_VIEW_CONTOURS, MF_UNCHECKED);
		menu->CheckMenuItem( ID_MENU_VIEW_HYDRO, MF_UNCHECKED);
		menu->CheckMenuItem(ID_VIEW_GRAVITYCONT2, MF_UNCHECKED);
		
		switch (pDoc->cd.coord.drawmode) 
		{
		case 0:
			menu->CheckMenuItem( ID_VIEW_TIMESECTION, MF_CHECKED);
			break;
		case 1:
			menu->CheckMenuItem( ID_VIEW_DEPTHSECTION, MF_CHECKED);
			break;
		case 2:
			menu->CheckMenuItem( ID_VIEW_VELOCITYCOLORMAP, MF_CHECKED);
			break;
		case 3:
			menu->CheckMenuItem( ID_VIEW_BOTH, MF_CHECKED);
			break;
		case 4:
			menu->CheckMenuItem( ID_VIEW_CONTOURS, MF_CHECKED);
			break;
		case 6:
			menu->CheckMenuItem( ID_VIEW_TIMESECTRC, MF_CHECKED);
			break;
		case 7:
			menu->CheckMenuItem( ID_MENU_VIEW_HYDRO, MF_CHECKED);
			break;
		case 8:
			menu->CheckMenuItem(ID_VIEW_GRAVITYCONT2, MF_CHECKED);
			break;
		}	//end of switch.

		menu->CheckMenuItem(ID_EDIT_BOUNDARY, MF_UNCHECKED);
		menu->CheckMenuItem(ID_EDIT_VELOCITY, MF_UNCHECKED);
		if (pDoc->draw.nodeMode == 1) menu->CheckMenuItem(ID_EDIT_BOUNDARY, MF_CHECKED);
		else if (pDoc->draw.nodeMode == 2) menu->CheckMenuItem(ID_EDIT_VELOCITY, MF_CHECKED);

		if(pDoc->draw.isNoNodes) menu->CheckMenuItem(ID_EDIT_NONODES, MF_CHECKED);
		else menu->CheckMenuItem(ID_EDIT_NONODES, MF_UNCHECKED);
		if(pDoc->draw.isNoBoundary) menu->CheckMenuItem(ID_EDIT_TUNEUP_NOBOUNDARY, MF_CHECKED);
		else menu->CheckMenuItem(ID_EDIT_TUNEUP_NOBOUNDARY, MF_UNCHECKED);
		if(pDoc->draw.isNoColorMap) menu->CheckMenuItem(ID_EDIT_TUNEUP_NOCOLORMAP, MF_CHECKED);
		else menu->CheckMenuItem(ID_EDIT_TUNEUP_NOCOLORMAP, MF_UNCHECKED);
		if(pDoc->draw.isNoLableOutside) menu->CheckMenuItem(ID_EDIT_LABLES_OUTSIDE, MF_CHECKED);
		else menu->CheckMenuItem(ID_EDIT_LABLES_OUTSIDE, MF_UNCHECKED);
		if(pDoc->draw.isNoOBSPos) menu->CheckMenuItem(ID_EDIT_TUNEUP_OBSPOSITIONS, MF_CHECKED);
		else menu->CheckMenuItem(ID_EDIT_TUNEUP_OBSPOSITIONS, MF_UNCHECKED);
		if(pDoc->draw.isNoOBSPos) menu->CheckMenuItem(ID_EDIT_TUNEUP_OBSPOSITIONS, MF_CHECKED);
		else menu->CheckMenuItem(ID_EDIT_TUNEUP_OBSPOSITIONS, MF_UNCHECKED);
		if (!pDoc->areaFill.isFillSmall) menu->CheckMenuItem(ID_EDIT_DISPLAYTUNEUP_FILLAREAONCE, MF_CHECKED);
		else menu->CheckMenuItem(ID_EDIT_DISPLAYTUNEUP_FILLAREAONCE, MF_UNCHECKED);
	}

	BOOL CSeisWideView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
	{
		// zooming, we take drag and drop positions for zooming
		// pt is in pixel coordinate. Note: pt is counted within the entire screen (or as resource screen layed out), 
		//   whereas OnMouseMove() is pixels within the same child window
		// zDelta>0 means we should zoom in.
		// zDelta<0 means we should zoom out.
		// each step we always have abs(zDelta)=120
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();
		if (!pSeiApp->statWin.getControlPressed()) return FALSE;

		OnToolbarZoom(); // this sets the stage for zooming
			
		CSeisWideDoc* pDoc = GetDocument();
		CRect rectChildWin;
		GetWindowRect(rectChildWin);
		pt.x -= rectChildWin.left;
		pt.y -= rectChildWin.top;

		SEI_PAIR sp = pDoc->cd.coord.pixel2XY(pt);
		double width = pDoc->cd.coord.xWidth();
		double height = pDoc->cd.coord.yHeight();
	
		double factor = zDelta > 0 ? 0.25 : 1.125;
		double xx1 = sp.x - width*factor;
		double xx2 = sp.x + width*factor;
		double yy1 = sp.y - height*factor;
		double yy2 = sp.y + height*factor;
		pDoc->cd.coord.set(xx1, yy1, xx2, yy2);

		pDoc->draw.isNodeChanged = TRUE;
		pDoc->draw.isSameDataDisplay = FALSE;
		Invalidate(FALSE);

		return TRUE;  // if I return FALSE, the screen refresh twice. If return TRUE, it's smooth!!!

	//	return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
		// If here the above line enabled, the screen refresh twice.
	}

	void CSeisWideView::OnMouseMove(UINT nFlags, CPoint point) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (transient.PointOld == point) return;

		//CRect totalP = pDoc->cd.coord.getPixelsTotal();
		//if (pDoc->draw.wiggle == 6 && pDoc->m_GLWin.myGL.m_isGLButtonsShowing &&
		//	(!isInsideLONG(point.x, totalP.left, totalP.right) || !isInsideLONG(point.y, totalP.top, totalP.bottom))
		//	) {
		//	CRect rect2 = pDoc->m_GLWin.myGL.camXYFrame;
		//	rect2.bottom = pDoc->m_GLWin.myGL.lightZFrame.bottom;
		//	rect2.right += 100; // because the dots has a diameter of 200 pixels
		//	InvalidateRect(pDoc->cd.coord.logical2Pixel(&pDoc->m_GLWin.myGL.buttonsFrame), TRUE);
		//	InvalidateRect(pDoc->cd.coord.logical2Pixel(&rect2), TRUE);
		//	pDoc->m_GLWin.myGL.m_isGLButtonsShowing = FALSE;
		//	return;
		//}



		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		ASSERT(pFrame);
		ASSERT_KINDOF(CMainFrame, pFrame);
		if (!pFrame) return;
		CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();

		// when it is not in zooming mode, and the user pressed Shift, the window's coords are changed accordingly, 
		// but then the user cannot get back to the original coords. This can be solved by setting the zooming mode to TRUE
		if (pSeiApp->statWin.getShiftPressed()) pDoc->cd.zoom.isMouseHand = TRUE;
		bool isPanning = pSeiApp->statWin.getShiftPressed() || pDoc->cd.zoom.isMouseHand;

		SEI_PAIR sp = pDoc->cd.coord.pixel2XY(pDoc->cd.coord.m_point);
		transient.aQuad.x1 = sp.x;
		transient.aQuad.y1 = sp.y;
		sp = pDoc->cd.coord.pixel2XY(point);
		transient.aQuad.x2 = sp.x;
		transient.aQuad.y2 = sp.y;
		bool isSamePoint = isFloatEqual(transient.aQuad.x2, transient.aQuad.x1) &&
			isFloatEqual(transient.aQuad.y2, transient.aQuad.y1);
		if (isSamePoint) return;

		bool isNotZoomingActually = 
			!pDoc->cd.zoom.isZooming ||
			pSeiApp->statWin.getShiftPressed() ||
			(pDoc->cd.zoom.isZooming && pFrame->statMain.pick);
		if (pDoc->draw.wiggle == 6 && isNotZoomingActually) {
			CPoint pLogical = pDoc->cd.coord.pixel2logicalScreen(point.x, point.y);
			bool isInFrame = IsPointInside(&pLogical, &pDoc->m_GLWin.myGL.buttonsFrame);
			CRect rect2 = pDoc->m_GLWin.myGL.camXYFrame;
			rect2.bottom = pDoc->m_GLWin.myGL.lightZFrame.bottom;
			rect2.right += 100; // because the dots has a diameter of 200 pixels
			bool isInJoystick = IsPointInside(&pLogical, &rect2);
			if (isInFrame != pDoc->m_GLWin.myGL.m_isGLButtonsShowing) {
				//pDoc->m_GLWin.myGL.m_isGLButtonsShowing = isTrue;
				InvalidateRect(pDoc->cd.coord.logical2Pixel(&pDoc->m_GLWin.myGL.buttonsFrame), TRUE);
				InvalidateRect(pDoc->cd.coord.logical2Pixel(&rect2), TRUE);
			}
			
			if (isInJoystick && transient.m_Drag && !isPanning) {
				// from (xOld, yOld)  to   (xNew, yNew)
				CPoint pLogicalOld = pDoc->cd.coord.pixel2logicalScreen(transient.PointOld.x, transient.PointOld.y);
				if (pDoc->m_GLWin.mySeis3DFBO_ButtonDragDrop(pLogicalOld, pLogical)) {
					InvalidateRect(pDoc->cd.coord.getPixelsAxes(), TRUE); // draw the data inside the axis
					InvalidateRect(pDoc->cd.coord.logical2Pixel(&rect2), TRUE);
				}
			}

			if (isInFrame || isInJoystick || 
				(transient.m_Drag && pDoc->m_GLWin.myGL.m_isGLButtonsShowing)) {
				pDoc->m_GLWin.myGL.m_isGLButtonsShowing = TRUE;
				return;
			}
			else 
				pDoc->m_GLWin.myGL.m_isGLButtonsShowing = FALSE;
		}
		else if (pDoc->draw.wiggle == 6 && !isNotZoomingActually) {
			// zooming mode in 3D display
			pDoc->m_GLWin.myGL.m_isGLButtonsShowing = FALSE;
			CRect rect2 = pDoc->m_GLWin.myGL.camXYFrame;
			rect2.bottom = pDoc->m_GLWin.myGL.lightZFrame.bottom;
			rect2.right += 100; // because the dots has a diameter of 200 pixels
			InvalidateRect(pDoc->cd.coord.logical2Pixel(&pDoc->m_GLWin.myGL.buttonsFrame), TRUE);
			InvalidateRect(pDoc->cd.coord.logical2Pixel(&rect2), TRUE);
		}

		CString tStr;
		bool isEditting = pFrame->statMain.add != 0 || pFrame->statMain.pick != 0 ||
			pFrame->statMain.move != 0 || pFrame->statMain.remove != 0;


		// ####################start status bar control
		myUpdateStatusBar();
		// ####################end status bar control


		if (transient.m_Drag && !isPanning) {
			// we draw measurement line from (xOld, yOld)  to   (xNew, yNew)
				CClientDC ClientDC(this);  // pixel graphics
				ClientDC.SetROP2(R2_NOT);
				// note: point, and yOld, yNew are all un-aware of any reduction velocities/times
				int num = (pDoc->cd.coord.drawmode==0)  ?  2  :  10;
				if (transient.MotionFix && transient.PointOld != point) {
					// clear the previous path
					drawCurveLabelBetween(&ClientDC, transient.PointOld, pDoc->cd.coord.m_point, false, num);
				}
				// now draw a new path
				drawCurveLabelBetween(&ClientDC, pDoc->cd.coord.m_point, point, false, num);
		} // if

		if (!pDoc->draw.isPrintingW && IsPointInside(&transient.PointOld, &pDoc->cd.coord.logicalAxes)) {
				CClientDC ClientDC(this);  // pixel graphics
				ClientDC.SetROP2(R2_NOT);
				// the small marker on left side
				double leftPixel = pDoc->cd.coord.logicalAxes.left / (double)pDoc->cd.coord.device2Logical;
				if (transient.MotionFix) {
					drawCurveBetween(&ClientDC, CPoint(0, transient.PointOld.y), CPoint((int)myRound(leftPixel), transient.PointOld.y), true, 2);
				}
				drawCurveBetween(&ClientDC, CPoint(0, point.y), CPoint((int)myRound(leftPixel), point.y), true, 2);
		}


		transient.PointOld = point;
		transient.MotionFix++;	

		CScrollView::OnMouseMove(nFlags, point);
	}

	void CSeisWideView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
		CSeisWideDoc* pDoc = GetDocument();

		double deltaX = (pDoc->cd.coord.X2() - pDoc->cd.coord.X1()) * 0.1;
		if (deltaX > 1000) deltaX = 1000;
		else if (deltaX > 100) deltaX = 100;
		else if (deltaX > 10) deltaX = 10;
		else if (deltaX > 1) deltaX = 1;
		else if (deltaX > .1) deltaX = .1;
		else if (deltaX > .01) deltaX = .01;
		else deltaX = .001;

		int nDelta = 2;
		int aSign = pDoc->cd.coord.isSwapX  ?  -1 : 1;
		
		switch (nSBCode)
		{
		case SB_LINERIGHT:
			nDelta = aSign * nDelta;
			deltaX *= aSign;
			break;

		case SB_LINELEFT:
			nDelta = -aSign * nDelta;
			deltaX *= -aSign;
			break;

		case SB_PAGERIGHT:
			nDelta = aSign * nDelta * 5;
			deltaX *= aSign * 5.;
			break;

		case SB_PAGELEFT:
			nDelta = -aSign * nDelta * 5;
			deltaX *= -aSign * 5.;
			break;

		case SB_THUMBPOSITION:
			return;

		default:
			return;
		}
		ScrollWindow(nDelta, 0);

		bool isTime = pDoc->cd.coord.isTimeDomain() == TRUE;
		bool isTraceDomain = (pDoc->cd.coord.drawmode == 6) ? true : false;
		if (isTraceDomain) {
			pDoc->cd.coord.trace1 += (int)deltaX;
			pDoc->cd.coord.trace2 += (int)deltaX;
		}
		else {
			pDoc->cd.coord.xmin += deltaX;
			pDoc->cd.coord.xmax += deltaX;
		}

		pDoc->draw.isSameDataDisplay = false;

		Invalidate(FALSE);
	//	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
	}

	void CSeisWideView::OnUpdateToolbarPick(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pCmdUI->SetCheck(pFrame->statMain.pick);

	}

	void CSeisWideView::OnUpdateToolbarRemove(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pCmdUI->SetCheck(pFrame->statMain.remove);

	}

	void CSeisWideView::OnUpdateToobarMove(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pCmdUI->SetCheck(pFrame->statMain.move);

	}

	void CSeisWideView::OnUpdateToolbarAdd(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pCmdUI->SetCheck(pFrame->statMain.add);

	}

	void CSeisWideView::OnUpdateViewTimesectrc(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->cd.coord.drawmode==6);

	}

	void CSeisWideView::OnUpdateViewTimesection(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->cd.coord.drawmode==0);

	}

	void CSeisWideView::OnUpdateViewDepthsection(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->cd.coord.drawmode==1);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame->m_licenseStruct.type == CLicenseCheck::demo  || pFrame->m_licenseStruct.type == CLicenseCheck::regi ||  pFrame->m_licenseStruct.type == CLicenseCheck::refr) {
			pCmdUI->Enable(FALSE);
		}


	}

	void CSeisWideView::OnUpdateViewVelocitycolormap(CCmdUI* pCmdUI) 
	{
		return; // allowed for all licenses. Starting Apr. 19, 2021

		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->cd.coord.drawmode==2);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame->m_licenseStruct.type == CLicenseCheck::demo  || pFrame->m_licenseStruct.type == CLicenseCheck::regi ||  pFrame->m_licenseStruct.type == CLicenseCheck::refl) {
			pCmdUI->Enable(FALSE);
		}

	}

	void CSeisWideView::OnUpdateViewBoth(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->cd.coord.drawmode==3);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame->m_licenseStruct.type != CLicenseCheck::full) {
			pCmdUI->Enable(FALSE);
		}

	}

	void CSeisWideView::OnUpdateViewContours(CCmdUI* pCmdUI) 
	{
		return; // allowed for all licenses. Starting Apr. 19, 2021

		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->cd.coord.drawmode==4);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame->m_licenseStruct.type == CLicenseCheck::demo  || pFrame->m_licenseStruct.type == CLicenseCheck::regi ||  pFrame->m_licenseStruct.type == CLicenseCheck::refl) {
			pCmdUI->Enable(FALSE);
		}

	}

	void CSeisWideView::OnUpdateMenuViewHydro(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->cd.coord.drawmode==7);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame->m_licenseStruct.type != CLicenseCheck::full) {
			pCmdUI->Enable(FALSE);
		}

	}

	void CSeisWideView::OnUpdateEditBoundary(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->draw.nodeMode == 1);

	}

	void CSeisWideView::OnUpdateEditVelocity(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->draw.nodeMode == 2);

	}

	void CSeisWideView::OnUpdateEditText(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->draw.nodeMode == 3);

	}

	void CSeisWideView::OnUpdateEditNonodes(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->draw.isNoNodes);

	}

	void CSeisWideView::OnUpdateEditTuneupNoboundary(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->draw.isNoBoundary);

	}

	void CSeisWideView::OnUpdateEditTuneupNocolormap(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->draw.isNoColorMap);

	}

	void CSeisWideView::OnUpdateEditTuneupObspositions(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->draw.isNoOBSPos);

	}

	void CSeisWideView::OnUpdateEditNolables(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->draw.isNoLableOutside);

	}

	void CSeisWideView::OnUpdateEditDisplaytuneupFillareaonce(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(!pDoc->areaFill.isFillSmall);

	}

	void CSeisWideView::OnUpdateEditDisplaytuneupBigmodelnodes(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		//	pCmdUI->SetCheck(pDoc->draw.bigNodes);  // this check is disabled since we now have 3 states instead of 2

	}

	void CSeisWideView::OnUpdateEditDisplaytuneupExternalDrawings(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->Enable(!pDoc->dig.isDigitizing);

	}

	void CSeisWideView::OnUpdateEditDisplaytuneupRemoveoutsideraypoints(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->draw.isNoOutSidePoints);	
	}

	void CSeisWideView::OnUpdateOptionsTogglehorizontalgrids(CCmdUI *pCmdUI)
	{
		CSeisWideDoc* pDoc = GetDocument();
		pCmdUI->SetCheck(!pDoc->draw.isNoHorzGrid);	
	}

	void CSeisWideView::OnUpdateEditImagestretchmode(CCmdUI *pCmdUI)
	{
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pCmdUI->SetCheck(pFrame->statMain.isStretchMode);
	}


	void CSeisWideView::OnUpdateEditLabelInside(CCmdUI *pCmdUI)
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->draw.isNoLabelInSide);	
	}


	void CSeisWideView::OnUpdateEditLablesOutside(CCmdUI *pCmdUI)
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->draw.isNoLableOutside);	
	}



	void CSeisWideView::OnUpdateModelToggledetails(CCmdUI *pCmdUI)
	{
		// CSeisWideDoc* pDoc = GetDocument();
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pCmdUI->SetCheck(pFrame->statMain.isShowDetails);
	}


	void CSeisWideView::OnUpdateViewNmoVelocity(CCmdUI *pCmdUI)
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->cd.coord.drawmode==10);


		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame->m_licenseStruct.type == CLicenseCheck::demo  || pFrame->m_licenseStruct.type == CLicenseCheck::regi ||  pFrame->m_licenseStruct.type == CLicenseCheck::refr) {
			pCmdUI->Enable(FALSE);
		}

	}

	void CSeisWideView::OnUpdateViewVelocitycolortwtt(CCmdUI *pCmdUI)
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pCmdUI->SetCheck(pDoc->cd.coord.drawmode==9);

		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		if (pFrame->m_licenseStruct.type != CLicenseCheck::full) {
			pCmdUI->Enable(FALSE);
		}
	}


	void CSeisWideView::OnUpdateModelingFlattenhorizon(CCmdUI *pCmdUI)
	{
		// TODO: Add your command update UI handler code here
		CSeisWideDoc* pDoc = GetDocument();
		pCmdUI->SetCheck(pDoc->draw.ncontFlatten > 0 && pDoc->cd.coord.drawmode!=6);
	}


	void CSeisWideView::OnFileExportbitmapOld() 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		static int nrow = 2728;
		static int ncol = 3804;
		CString bmpFile;
		//	pDoc->draw.isSameNodeDisplay = FALSE;	//do not refresh screen for now.
		//static BOOL tempNoLable=FALSE;
		static BOOL isVectorize = FALSE;
		static BOOL t1 = pDoc->draw.isNoLableOutside;
		static BOOL t2 = pDoc->draw.isNoLabelInSide;
		static int numBits = 24;
		// for backup only. do not use backupNoLabel until the last second of exiting this routine!

		CdlgExportBitmap dlg;

		CString segyFile = pDoc->cd.coord.isTimeDomain() ? pDoc->draw.timeSecName : pDoc->draw.depthSecName;
		dlg.m_nameBitmap = getFileNameTemp(segyFile, _T("bmp"));
		dlg.m_biForm = 0;
		dlg.m_xPixels = ncol;
		dlg.m_yPixels = nrow;
		dlg.m_biForm = numBits==24 ? 1  :  0;
		dlg.m_removeLabels = t1;
		dlg.m_isNoLabelsInside = t2;
		dlg.m_isVectorize = isVectorize;
		if (dlg.DoModal() == IDOK) {
			bmpFile = dlg.m_nameBitmap;
			nrow = dlg.m_yPixels;
			ncol = dlg.m_xPixels;
			if (dlg.m_biForm==0) numBits = 1; //1=monochrome.
			else numBits = 24; //24 bit bitmap.
			isVectorize = dlg.m_isVectorize;
			pDoc->draw.isNoLableOutside = dlg.m_removeLabels;
			pDoc->draw.isNoLabelInSide = dlg.m_isNoLabelsInside;
		}
		else return; 

		pDoc->draw.isSameDataDisplay = FALSE; //indicate display has been changed and will re-load all data.

		CString tStr;
		tStr.Format(_T("Preparing for %s ..."), GetBaseFileName(bmpFile));
		pFrame->myCaption(tStr);

		CString fSection = pDoc->cd.coord.isTimeDomain() ? pDoc->draw.timeSecName : pDoc->draw.depthSecName;
		CColorHelper::AMPCOLORDYN ampColorDyn2(fSection); // only used in color mode
		//double aMaxAmp = pDoc->draw.isAvgEnergy ? pDoc->head.maxAmp*pDoc->head.maxAmp : pDoc->head.maxAmp;
		double aMaxAmp = pDoc->head.maxAmp;
		//CColorHelper::getAmpColorArrayFromSegyFile(aMaxAmp,
		//	pDoc->draw.logScaleThreshold,
		//	&ampColorDyn2);
		ampColorDyn2.getAmpColorArray(aMaxAmp, false, pDoc->draw.logScaleThreshold, (double)pDoc->draw.ampPower);
		if (RasterSeismic(bmpFile, nrow, ncol, numBits, &ampColorDyn2)) {
			if (isVectorize) {
				// now try to vectorize and view the pdf
				CString poPathName = getSeisWideParaPath() + _T("\\potrace.exe "); // no back slash
				CString pdfName = getFileNameTemp(segyFile, _T("pdf"));
				CString strCmdAll = poPathName + bmpFile + _T(" -b pdf -o ") + addQuotes(pdfName);

				//WaitForInputIdle
				PROCESS_INFORMATION proc_i = myUnicodeProcess(strCmdAll, BLANK, false);  // info returned from CreateProcess
				if(WaitForSingleObject(proc_i.hProcess, INFINITE) == WAIT_TIMEOUT) {
					AfxMessageBox(_T("Failed to produce vectorized output or just time-out.")) ;
				} else {
					// success
					ShellExecute(NULL, _T("open"), pdfName, NULL, NULL, SW_HIDE);
				} // else	
				CloseHandle(proc_i.hProcess);
				CloseHandle(proc_i.hThread);
			}
			else if (!myProcessSimple(_T("mspaint.exe ") + bmpFile))
				AfxMessageBox(_T("Not able to load with mspaint.exe"));
		}
		ampColorDyn2.freeAmpColorArray();

		SWITCH_BOOL(pDoc->draw.isNoLableOutside, t1);
		SWITCH_BOOL(pDoc->draw.isNoLabelInSide, t2);

		

	}

	// newer version will allow vectorization of segy display to PS format
	void CSeisWideView::OnFileExportbitmap()
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		static int nrow = 2728;
		static int ncol = 3804;
		CString bmpFile;
		//	pDoc->draw.isSameNodeDisplay = FALSE;	//do not refresh screen for now.
		//static BOOL tempNoLable=FALSE;
		static BOOL isVectorize = FALSE;
		BOOL isNonodesBackup = pDoc->draw.isNoNodes;
		gIsNoLableOutsideBackup = pDoc->draw.isNoLableOutside;
		gIsNoLabelInSideBackup = pDoc->draw.isNoLabelInSide;
		gisNoBoundaryBackup = pDoc->draw.isNoBoundary;
		static int numBits = 24;
		// for backup only. do not use backupNoLabel until the last second of exiting this routine!
		static int numColors2Plot = 1;

		CdlgExportBitmap dlg;

		CString segyFile = pDoc->cd.coord.isTimeDomain() ? pDoc->draw.timeSecName : pDoc->draw.depthSecName;
		dlg.m_nameBitmap = getFileNameTemp(segyFile, _T("bmp"));
		dlg.m_biForm = 0;
		dlg.m_xPixels = ncol;
		dlg.m_yPixels = nrow;
		dlg.m_biForm = numBits == 24 ? 1 : 0;
		dlg.m_removeLabels = gIsNoLableOutsideBackup;
		dlg.m_isNoLabelsInside = gIsNoLabelInSideBackup;
		dlg.m_isNoLines = pDoc->draw.isNoBoundary;
		dlg.m_isNonodes = pDoc->draw.isNoNodes;
		dlg.m_isVectorize = isVectorize;

		if (pDoc->draw.wiggle == 3)
			dlg.m_numColors = 1;
		else if (pDoc->draw.wiggle == 1)
			dlg.m_numColors = 1;
		else
			dlg.m_numColors = numColors2Plot;
		dlg.m_wiggle = pDoc->draw.wiggle;

		if (dlg.DoModal() == IDOK) {
			bmpFile = dlg.m_nameBitmap;
			nrow = dlg.m_yPixels;
			ncol = dlg.m_xPixels;
			if (dlg.m_biForm == 0) numBits = 1; //1=monochrome.
			else numBits = 24; //24 bit bitmap.
			isVectorize = dlg.m_isVectorize;
			if (pDoc->draw.wiggle != 1 && pDoc->draw.wiggle != 3) {
				// only if here, area fill and bi-color fills are not present, 
				// and we want to use numColors2Plot 
				numColors2Plot = dlg.m_numColors;
			}
			pDoc->draw.isNoLableOutside = dlg.m_removeLabels;
			pDoc->draw.isNoLabelInSide = dlg.m_isNoLabelsInside;
			pDoc->draw.isNoBoundary = dlg.m_isNoLines;
			pDoc->draw.isNoNodes = dlg.m_isNonodes;
		}
		else return;

		pDoc->draw.isVectoring = isVectorize;

		if (numColors2Plot < 1) numColors2Plot = 1; // fix up invalid number
		pDoc->draw.isSameDataDisplay = FALSE; //indicate display has been changed and will re-load all data.

		CString tStr;
		bmpFile = getFileNameTempNospace(bmpFile, _T("bmp"));
		tStr.Format(_T("Preparing %dX%d px bitmap for %s ..."), ncol, nrow, GetBaseFileName(bmpFile));
		pFrame->myCaption(tStr);

		CString fSection = pDoc->cd.coord.isTimeDomain() ?
			pDoc->draw.timeSecName : pDoc->draw.depthSecName;
		CColorHelper::AMPCOLORDYN ampColorDyn2(fSection); // only used in color mode
		double aMaxAmp = pDoc->head.maxAmp;
		//double aMaxAmp = pDoc->draw.isAvgEnergy ?
		//	pDoc->head.maxAmp*pDoc->head.maxAmp : 
		//	pDoc->head.maxAmp;
		//CColorHelper::getAmpColorArrayFromSegyFile(
		//	aMaxAmp,
		//	pDoc->draw.logScaleThreshold,
		//	&ampColorDyn2);
		ampColorDyn2.getAmpColorArray(aMaxAmp, false, pDoc->draw.logScaleThreshold, (double)pDoc->draw.ampPower);
		CString poPathName = addQuotes(getSeisWideParaPath() + _T("\\potrace.exe")) + SPACE; // no back slash
		if (!isVectorize) {
			if (RasterSeismic(bmpFile, nrow, ncol, numBits, &ampColorDyn2)) {
				if (!myProcessSimple(_T("mspaint.exe ") + bmpFile))
					AfxMessageBox(_T("Not able to load with mspaint.exe"));
			}
		}
		else if (pDoc->draw.wiggle == 1 || numColors2Plot == 1) {
			int wiggleBackup = pDoc->draw.wiggle;
			CString pdfName = getFileNameTempNospace(segyFile, _T("pdf"));
			// note: here the PDF file cannot have space in its path, otherwise potrace cannot work.
			if (RasterSeismic(bmpFile, nrow, ncol, numBits, &ampColorDyn2)) {
					// now try to vectorize and view the pdf
					// CString strCmdAll = poPathName + bmpFile + _T(" -b pdf -o ") + addQuotes(pdfName);
					CString strc = CColorHelper::colorRef2Hex(BLACK);
					CString strCmdAll = poPathName + bmpFile
						+ _T(" -b pdf --color ") + strc
						+ _T(" -o ") + pdfName;

					// showStringInFile(strCmdAll);

					//WaitForInputIdle
					PROCESS_INFORMATION proc_i = myUnicodeProcess(strCmdAll, BLANK, false);  // info returned from CreateProcess
					if (WaitForSingleObject(proc_i.hProcess, INFINITE) == WAIT_TIMEOUT) {
						AfxMessageBox(_T("Failed to produce vectorized output or just time-out."));
					}
					else {
						// success
						AfxMessageBox(_T("Successfully converted output and saved to ") + pdfName);
						ShellExecute(NULL, _T("open"), pdfName, NULL, NULL, SW_HIDE);
					} // else	
					CloseHandle(proc_i.hProcess);
					CloseHandle(proc_i.hThread);
			}
			pDoc->draw.wiggle = wiggleBackup;
		}
		else if (pDoc->draw.wiggle == 3) {
			CStringArray arr;
			CString psName = getFileNameTempNospace(segyFile, _T("ps"));
			pDoc->draw.wiggle = 1;
			pDoc->areaFill.isNegativeFill = true;
			COLORREF colorAvg;
			if (RasterSeismic(bmpFile, nrow, ncol, numBits, &ampColorDyn2)) {
				colorAvg = pDoc->seisBiColor2.getColor(pDoc->areaFill.isNegativeFill==TRUE);
				CString strCmdAll = poPathName + bmpFile
					+ _T(" -p -c --color ") 
					+ CColorHelper::colorRef2Hex(colorAvg)
					+ _T(" -o ") + psName;
				PROCESS_INFORMATION proc_i = myUnicodeProcess(strCmdAll, BLANK, false);  // info returned from CreateProcess
				if (WaitForSingleObject(proc_i.hProcess, INFINITE) == WAIT_TIMEOUT) {
					AfxMessageBox(_T("Failed to produce vectorized output or just time-out."), MB_ICONQUESTION);
				}
				CloseHandle(proc_i.hProcess);
				CloseHandle(proc_i.hThread);
				CString cstr = getTextFromFileUnix(psName);
				int i = cstr.Find(_T("showpage"));
				arr.Add(cstr.Left(i));
			}

			pDoc->areaFill.isNegativeFill = false;
			if (RasterSeismic(bmpFile, nrow, ncol, numBits, &ampColorDyn2)) {
				colorAvg = pDoc->seisBiColor2.getColor(pDoc->areaFill.isNegativeFill==TRUE);
				CString strCmdAll = poPathName + bmpFile
					+ _T(" -p -c --color ") 
					+ CColorHelper::colorRef2Hex(colorAvg)
					+ _T(" -o ") + psName;
				PROCESS_INFORMATION proc_i = myUnicodeProcess(strCmdAll, BLANK, false);  // info returned from CreateProcess
				if (WaitForSingleObject(proc_i.hProcess, INFINITE) == WAIT_TIMEOUT) {
					AfxMessageBox(_T("Failed to produce vectorized output or just time-out."));
				}
				CloseHandle(proc_i.hProcess);
				CloseHandle(proc_i.hThread);
				CString cstr = getTextFromFileUnix(psName);
				arr.Add(cstr);
			}

			
			CString psNameMerge = GetFileNameAppendExt(segyFile, _T("Merge"), _T("ps"));
			static bool isShownAlready = false;
			if (saveStringArrayToFileAsciiUnix(&arr, psNameMerge)) {
				showFileInDefaultProg(psNameMerge);
				if (!isShownAlready) {
					isShownAlready = true;
					tStr.Format(_T("Output saved as postcript file:\n %s. \nTrying to display it."), psNameMerge);
					AfxMessageBox(tStr, MB_ICONQUESTION);
				}
			}
			arr.RemoveAll();
			pDoc->draw.wiggle = 3;
		}
		else {
			CStringArray arr;
			CString cstr;
			double deltaPerc = 2. / (double)(numColors2Plot);
			CString psName = getFileNameTempNospace(segyFile, _T("ps"));
			// bool isLastLoop = false;

			// to generate multi-color PDFs from segy, we generate separate PS file using different 
			// clipped color pallett, and finally assemble them into one PDF.
			// Note: 1. Must start from 0 to 1  and then from 0 to -1, such that higher amp is shown 
			//          on top of lower amp.
			//       2. Clipped lower values should reflect the exact color at the clip point, otherwise a gap can be seen in display
			//			But clipped high values should be WHITE otherwise it masks the overal panel
			register double bPert;
			register double cPert;
			COLORREF colorAvg;
			for (double aPert = .0; aPert < 1.0; aPert += deltaPerc) {
				// do the positive part
				bPert = aPert;
				cPert = bPert + deltaPerc;
				if (cPert > 1.) cPert = 1.;
				if (cPert - bPert < 0.1*deltaPerc) {
					cPert = 1.;
				}
				if (cPert - bPert < 0.001*deltaPerc) continue;

				tStr.Format(_T("%d%%"), (int)((bPert + 1.)*50.));
				pFrame->myCaption(tStr);
				ampColorDyn2.getAmpColorArray(aMaxAmp, false, pDoc->draw.logScaleThreshold, (double)pDoc->draw.ampPower);

				if (cPert == 1.)
					colorAvg = ampColorDyn2.clipAmpColorMin(bPert, aMaxAmp);
				else
					colorAvg = ampColorDyn2.clipAmpColorArray(bPert, cPert, aMaxAmp);

				if (numColors2Plot == 2 && pDoc->draw.wiggle < 4) {
					// overwrite the colorAvg
					colorAvg = pDoc->seisBiColor2.getColor(bPert < 0.);
				}

				if (RasterSeismic(bmpFile, nrow, ncol, numBits, &ampColorDyn2)) {
					CString strCmdAll = poPathName + bmpFile
						+ _T(" -p -c --color ")
						+ CColorHelper::colorRef2Hex(colorAvg)
						+ _T(" -o ") + psName;
					PROCESS_INFORMATION proc_i = myUnicodeProcess(strCmdAll, BLANK, false);  // info returned from CreateProcess
					if (WaitForSingleObject(proc_i.hProcess, INFINITE) == WAIT_TIMEOUT) {
						AfxMessageBox(_T("Failed to produce vectorized output or just time-out."));
					}
					CloseHandle(proc_i.hProcess);
					CloseHandle(proc_i.hThread);
				}
				cstr = getTextFromFileUnix(psName);
				int i = cstr.Find(_T("showpage"));
				arr.Add(cstr.Left(i));


				// do the negative part
				bPert = -aPert - deltaPerc;
				cPert = -aPert;
				if (bPert < -1.) bPert = -1.;
				if (cPert - bPert < 0.1*deltaPerc) {
					bPert = -1.;
					// isLastLoop = true;
				}
				if (cPert - bPert < 0.001*deltaPerc) continue;

				tStr.Format(_T("%d%%"), (int)((bPert + 1.)*50.));
				pFrame->myCaption(tStr);
				ampColorDyn2.getAmpColorArray(aMaxAmp, false, pDoc->draw.logScaleThreshold, (double)pDoc->draw.ampPower);

				if (bPert == -1.)
					colorAvg = ampColorDyn2.clipAmpColorMax(cPert, aMaxAmp);
				else
					colorAvg = ampColorDyn2.clipAmpColorArray(bPert, cPert, aMaxAmp);

				if (numColors2Plot == 2 && pDoc->draw.wiggle < 4) {
					// overwrite the colorAvg
					colorAvg = pDoc->seisBiColor2.getColor(bPert < 0.);
				}

				if (RasterSeismic(bmpFile, nrow, ncol, numBits, &ampColorDyn2)) {
					CString strCmdAll = poPathName + bmpFile
						+ _T(" -p -c --color ")
						+ CColorHelper::colorRef2Hex(colorAvg)
						+ _T(" -o ") + psName;
					PROCESS_INFORMATION proc_i = myUnicodeProcess(strCmdAll, BLANK, false);  // info returned from CreateProcess
					if (WaitForSingleObject(proc_i.hProcess, INFINITE) == WAIT_TIMEOUT) {
						AfxMessageBox(_T("Failed to produce vectorized output or just time-out."), MB_ICONQUESTION);
					}
					CloseHandle(proc_i.hProcess);
					CloseHandle(proc_i.hThread);
				}

				cstr = getTextFromFileUnix(psName);
				if (bPert != -1.) {
					i = cstr.Find(_T("showpage"));
					arr.Add(cstr.Left(i));
				}
				else
					arr.Add(cstr);
			} // for aPerc

			CString psNameMerge = GetFileNameAppendExt(segyFile, _T("Merge"), _T("ps"));
			//CString psNameMerge = getFileNameTemp(GetBaseFileName(segyFile) + _T("Merge"), _T("ps"));
			if (saveStringArrayToFileAsciiUnix(&arr, psNameMerge)) {
				showFileInDefaultProg(psNameMerge);
				tStr.Format(_T("Output saved as postcript file:\n %s. \nTrying to display it."), psNameMerge);
				// pFrame->myCaption(tStr);
				AfxMessageBox(tStr, MB_ICONQUESTION);
			}
			arr.RemoveAll();
		}




		SWITCH_BOOL(pDoc->draw.isNoLableOutside, gIsNoLableOutsideBackup);
		SWITCH_BOOL(pDoc->draw.isNoLabelInSide, gIsNoLabelInSideBackup);
		SWITCH_BOOL(pDoc->draw.isNoBoundary, gisNoBoundaryBackup);
//		this->gisNoBoundaryBackup = pDoc->draw.isNoBoundary;
		SWITCH_BOOL(pDoc->draw.isNoNodes, isNonodesBackup);

		ampColorDyn2.freeAmpColorArray();
	}

	void CSeisWideView::OnFileExportbitmapOld2()
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		static int nrow = 2728;
		static int ncol = 3804;
		CString bmpFile;
		//	pDoc->draw.isSameNodeDisplay = FALSE;	//do not refresh screen for now.
		//static BOOL tempNoLable=FALSE;
		static BOOL isVectorize = FALSE;
		gIsNoLableOutsideBackup = pDoc->draw.isNoLableOutside;
		gIsNoLabelInSideBackup = pDoc->draw.isNoLabelInSide;
		gisNoBoundaryBackup = pDoc->draw.isNoBoundary;
		static int numBits = 24;
		// for backup only. do not use backupNoLabel until the last second of exiting this routine!
		static int numColors2Plot = 5;

		CdlgExportBitmap dlg;

		CString segyFile = pDoc->cd.coord.isTimeDomain() ? pDoc->draw.timeSecName : pDoc->draw.depthSecName;
		dlg.m_nameBitmap = getFileNameTemp(segyFile, _T("bmp"));
		dlg.m_biForm = 0;
		dlg.m_xPixels = ncol;
		dlg.m_yPixels = nrow;
		dlg.m_biForm = numBits == 24 ? 1 : 0;
		dlg.m_removeLabels = gIsNoLableOutsideBackup;
		dlg.m_isNoLabelsInside = gIsNoLabelInSideBackup;
		dlg.m_isNoLines = pDoc->draw.isNoBoundary;
		dlg.m_isVectorize = isVectorize;

		if (pDoc->draw.wiggle == 3)
			dlg.m_numColors = 2;
		else if (pDoc->draw.wiggle == 1)
			dlg.m_numColors = 1;
		else
			dlg.m_numColors = numColors2Plot;
		dlg.m_wiggle = pDoc->draw.wiggle;

		if (dlg.DoModal() == IDOK) {
			bmpFile = dlg.m_nameBitmap;
			nrow = dlg.m_yPixels;
			ncol = dlg.m_xPixels;
			if (dlg.m_biForm == 0) numBits = 1; //1=monochrome.
			else numBits = 24; //24 bit bitmap.
			isVectorize = dlg.m_isVectorize;
			if (pDoc->draw.wiggle != 1 && pDoc->draw.wiggle != 3) {
				// only if here, area fill and bi-color fills are not present, 
				// and we want to use numColors2Plot 
				numColors2Plot = dlg.m_numColors;
			}
			pDoc->draw.isNoLableOutside = dlg.m_removeLabels;
			pDoc->draw.isNoLabelInSide = dlg.m_isNoLabelsInside;
			pDoc->draw.isNoBoundary = dlg.m_isNoLines;
		}
		else return;
		if (numColors2Plot < 1) numColors2Plot = 1; // fix up invalid number
		pDoc->draw.isSameDataDisplay = FALSE; //indicate display has been changed and will re-load all data.

		CString tStr;
		tStr.Format(_T("Preparing for %s ..."), GetBaseFileName(bmpFile));
		pFrame->myCaption(tStr);

		CString fSection = pDoc->cd.coord.isTimeDomain() ?
			pDoc->draw.timeSecName : pDoc->draw.depthSecName;
		CColorHelper::AMPCOLORDYN ampColorDyn2(fSection); // only used in color mode
		double aMaxAmp = pDoc->head.maxAmp;
		//double aMaxAmp = pDoc->draw.isAvgEnergy ?
		//	pDoc->head.maxAmp*pDoc->head.maxAmp :
		//	pDoc->head.maxAmp;
		//CColorHelper::getAmpColorArrayFromSegyFile(
		//	aMaxAmp,
		//	pDoc->draw.logScaleThreshold,
		//	&ampColorDyn2);
		ampColorDyn2.getAmpColorArray(aMaxAmp, false, pDoc->draw.logScaleThreshold, (double)pDoc->draw.ampPower);
		CString poPathName = getSeisWideParaPath() + _T("\\potrace.exe "); // no back slash
		if (!isVectorize) {
			if (RasterSeismic(bmpFile, nrow, ncol, numBits, &ampColorDyn2)) {
				if (!myProcessSimple(_T("mspaint.exe ") + bmpFile))
					AfxMessageBox(_T("Not able to load with mspaint.exe"));
			}
		}
		else if (pDoc->draw.wiggle == 1 || numColors2Plot == 1) {
			int wiggleBackup = pDoc->draw.wiggle;
			CString pdfName = getFileNameTemp(segyFile, _T("pdf"));
			if (RasterSeismic(bmpFile, nrow, ncol, numBits, &ampColorDyn2)) {
				// now try to vectorize and view the pdf
				// CString strCmdAll = poPathName + bmpFile + _T(" -b pdf -o ") + addQuotes(pdfName);
				CString strc = CColorHelper::colorRef2Hex(BLACK);
				CString strCmdAll = poPathName + bmpFile
					+ _T(" -b pdf --color ") + strc
					+ _T(" -o ") + addQuotes(pdfName);

				//WaitForInputIdle
				PROCESS_INFORMATION proc_i = myUnicodeProcess(strCmdAll, BLANK, false);  // info returned from CreateProcess
				if (WaitForSingleObject(proc_i.hProcess, INFINITE) == WAIT_TIMEOUT) {
					AfxMessageBox(_T("Failed to produce vectorized output or just time-out."), MB_ICONQUESTION);
				}
				else {
					// success
					ShellExecute(NULL, _T("open"), pdfName, NULL, NULL, SW_HIDE);
				} // else	
				CloseHandle(proc_i.hProcess);
				CloseHandle(proc_i.hThread);
			}
			pDoc->draw.wiggle = wiggleBackup;
		}
		else if (pDoc->draw.wiggle == 3) {
			CStringArray arr;
			CString psName = getFileNameTemp(segyFile, _T("ps"));
			pDoc->draw.wiggle = 1;
			pDoc->areaFill.isNegativeFill = true;
			COLORREF colorAvg;
			if (RasterSeismic(bmpFile, nrow, ncol, numBits, &ampColorDyn2)) {
				colorAvg = pDoc->seisBiColor2.getColor(pDoc->areaFill.isNegativeFill == TRUE);
				CString strCmdAll = poPathName + bmpFile
					+ _T(" -p -c --color ")
					+ CColorHelper::colorRef2Hex(colorAvg)
					+ _T(" -o ") + addQuotes(psName);
				PROCESS_INFORMATION proc_i = myUnicodeProcess(strCmdAll, BLANK, false);  // info returned from CreateProcess
				if (WaitForSingleObject(proc_i.hProcess, INFINITE) == WAIT_TIMEOUT) {
					AfxMessageBox(_T("Failed to produce vectorized output or just time-out."));
				}
				CloseHandle(proc_i.hProcess);
				CloseHandle(proc_i.hThread);
				CString cstr = getTextFromFileUnix(psName);
				int i = cstr.Find(_T("showpage"));
				arr.Add(cstr.Left(i));
			}

			pDoc->areaFill.isNegativeFill = false;
			if (RasterSeismic(bmpFile, nrow, ncol, numBits, &ampColorDyn2)) {
				colorAvg = pDoc->seisBiColor2.getColor(pDoc->areaFill.isNegativeFill == TRUE);
				CString strCmdAll = poPathName + bmpFile
					+ _T(" -p -c --color ")
					+ CColorHelper::colorRef2Hex(colorAvg)
					+ _T(" -o ") + addQuotes(psName);
				PROCESS_INFORMATION proc_i = myUnicodeProcess(strCmdAll, BLANK, false);  // info returned from CreateProcess
				if (WaitForSingleObject(proc_i.hProcess, INFINITE) == WAIT_TIMEOUT) {
					AfxMessageBox(_T("Failed to produce vectorized output or just time-out."));
				}
				CloseHandle(proc_i.hProcess);
				CloseHandle(proc_i.hThread);
				CString cstr = getTextFromFileUnix(psName);
				arr.Add(cstr);
			}


			CString psNameMerge = GetFileNameAppendExt(segyFile, _T("Merge"), _T("ps"));
			if (saveStringArrayToFileAsciiUnix(&arr, psNameMerge)) {
				showFileInDefaultProg(psNameMerge);
				tStr.Format(_T("Output saved as postcript file:\n %s. \nTrying to display it."), psNameMerge);
				AfxMessageBox(tStr);
			}
			arr.RemoveAll();
			pDoc->draw.wiggle = 3;
		}
		else {
			CStringArray arr;
			double deltaPerc = 2. / (double)(numColors2Plot);
			CString psName = getFileNameTemp(segyFile, _T("ps"));
			bool isLastLoop = false;

			// to generate multi-color PDFs from segy, we generate separate PS file using different 
			// clipped color pallett, and finally assemble them into one PDF.
			// Note: 1. Must start from 0 to 1  and then from 0 to -1, such that higher amp is shown 
			//          on top of lower amp.
			//       2. Clipped lower values should reflect the exact color at the clip point, otherwise a gap can be seen in display
			//			But clipped high values should be WHITE otherwise it masks the overal panel
			for (double aPerc = .0; aPerc < 2.0; aPerc += deltaPerc) {
				register double bPerc = aPerc;
				if (bPerc >= 1.) bPerc = 1. - bPerc - deltaPerc;  // it is now negative
				register double cPerc = bPerc + deltaPerc;
				if (cPerc > 1.) cPerc = 1.;

				// if aPerc crosses the center point, we want to make sure the center
				// point is included.
				if (aPerc < 1. && aPerc + deltaPerc>1.) {
					cPerc = 1.;
					aPerc = 1. - deltaPerc;
				}

				if (bPerc + 1. < 0.1*deltaPerc) {
					bPerc = -1.;
					isLastLoop = true;
				}
				if (bPerc < -1.) {
					bPerc = -1.;
					isLastLoop = true;
				}
				if (cPerc - bPerc < 0.001*deltaPerc) continue;
				tStr.Format(_T("%d%%"), (int)((bPerc + 1.)*50.));
				pFrame->myCaption(tStr);
				ampColorDyn2.getAmpColorArray(aMaxAmp, false, pDoc->draw.logScaleThreshold, (double)pDoc->draw.ampPower);
				COLORREF colorAvg;

				if (bPerc == -1.)
					colorAvg = ampColorDyn2.clipAmpColorMax(cPerc, aMaxAmp);
				else if (cPerc == 1.)
					colorAvg = ampColorDyn2.clipAmpColorMin(bPerc, aMaxAmp);
				else
					colorAvg = ampColorDyn2.clipAmpColorArray(bPerc, cPerc, aMaxAmp);


				if (RasterSeismic(bmpFile, nrow, ncol, numBits, &ampColorDyn2)) {
					if (numColors2Plot == 2 && pDoc->draw.wiggle < 4) {
						// overwrite the colorAvg
						colorAvg = pDoc->seisBiColor2.getColor(bPerc < 0.);
					}
					CString strc = CColorHelper::colorRef2Hex(colorAvg);
					CString strCmdAll = poPathName + bmpFile
						+ _T(" -p -c --color ") + strc
						+ _T(" -o ") + addQuotes(psName);
					PROCESS_INFORMATION proc_i = myUnicodeProcess(strCmdAll, BLANK, false);  // info returned from CreateProcess
					if (WaitForSingleObject(proc_i.hProcess, INFINITE) == WAIT_TIMEOUT) {
						AfxMessageBox(_T("Failed to produce vectorized output or just time-out."), MB_ICONQUESTION);
					}
					CloseHandle(proc_i.hProcess);
					CloseHandle(proc_i.hThread);
				}

				CString cstr = getTextFromFileUnix(psName);
				if (!isLastLoop) {
					int i = cstr.Find(_T("showpage"));
					arr.Add(cstr.Left(i));
				}
				else
					arr.Add(cstr);
			} // for aPerc

			CString psNameMerge = GetFileNameAppendExt(segyFile, _T("Merge"), _T("ps"));
			//CString psNameMerge = getFileNameTemp(GetBaseFileName(segyFile) + _T("Merge"), _T("ps"));
			if (saveStringArrayToFileAsciiUnix(&arr, psNameMerge)) {
				showFileInDefaultProg(psNameMerge);
				tStr.Format(_T("Output saved as postcript file:\n %s. \nTrying to display it."), psNameMerge);
				AfxMessageBox(tStr, MB_ICONQUESTION);
			}
			arr.RemoveAll();
		}



		SWITCH_BOOL(pDoc->draw.isNoLableOutside, gIsNoLableOutsideBackup);
		SWITCH_BOOL(pDoc->draw.isNoLabelInSide, gIsNoLabelInSideBackup);
		SWITCH_BOOL(pDoc->draw.isNoBoundary, gisNoBoundaryBackup);
		//		this->gisNoBoundaryBackup = pDoc->draw.isNoBoundary;

		ampColorDyn2.freeAmpColorArray();
	}
	
	bool CSeisWideView::RasterSeismic(CString bitmapName, int nrow, int ncol, int numBits,
		CColorHelper::AMPCOLORDYN *pAmpColorDyn)
//COLORREF &colorAvg, double percAmpColorMin, double percAmpColorMax)
	{
		//this routine converts seismic data into pixels and write them to bmp24.
		//uses: para.trace1, para.traceStep, para.trace2, ...
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		CPoint p, p1;

		//these codes generate a memory DC.
		CClientDC dc(this);
		CDC aDC;
		aDC.CreateCompatibleDC(&dc);

		CBitmap* m_bmp = new CBitmap();
		// if (pDoc->bmp24.biForm == 24) {
		if (numBits > 1  ||  numBits <= 32) {
			//		m_bmp.CreateBitmap(ncol, nrow, 1, pDoc->bmp24.biForm, NULL); //only for monochrome.
			m_bmp->CreateCompatibleBitmap(&dc, ncol, nrow);
		}
		else if(numBits==1) {
			m_bmp->CreateBitmap(ncol, nrow, 1, 1, NULL); //creates a monochrome bitmap.
		}
		else {
			m_bmp->DeleteObject();
			return false;
		}


		ASSERT(m_bmp->m_hObject != NULL);
		CBitmap* pOldBitmap = aDC.SelectObject(m_bmp);
		CSize pixelTotal = CSize(ncol, nrow);
		CSize logicalTotal = CSize((int)(ncol * pDoc->cd.coord.device2Logical), (int)(nrow * pDoc->cd.coord.device2Logical));
		int lableOutsideStat = pDoc->draw.isNoLableOutside ? 1 :
			(pDoc->draw.plotTitle.IsEmpty() ? -1 : 0);
		bool isYLabelOnRight = pFrame->statMain.isSwapXActive != pDoc->cd.coord.isSwapX;
		if (pDoc->cd.coord.isSwapYLabel) isYLabelOnRight = !isYLabelOnRight;
		pDoc->cd.setCoordMappingFromWorld(&aDC, lableOutsideStat,
			isYLabelOnRight,
			pixelTotal, logicalTotal,
			(double)pDoc->draw.printX, (double)pDoc->draw.printY, &pDoc->cd.coord);

		aDC.SetBkMode(TRANSPARENT); //to avoid texts overlap each other.

		CBrush brBackground(::GetSysColor(COLOR_WINDOW));
		aDC.FillRect(pDoc->cd.coord.viewLogicalTotal, &brBackground);
		pDoc->DrawControlsSegy(&aDC, pAmpColorDyn, this);
		if (pDoc->draw.wiggle == 6) {
			pDoc->m_GLWin.glMap.loadGLmapping(pAmpColorDyn->fileName);
			pDoc->m_GLWin.mySeis3DFBO_Animate(&aDC, -1, &pDoc->cd.coord.logicalAxes);
			pDoc->m_GLWin.mySeis3DFBO_End();
			pDoc->m_GLWin.DestroySceneFBO();
		}
		pDoc->DrawControlsEx(&aDC);
		pDoc->Tick(&aDC);	//Draw ticks last



		CxImage aXimage;
		aXimage.CreateFromHBITMAP((HBITMAP)*m_bmp);  // get the dimension of bitmap and put to pDib array
		aXimage.Stretch(aDC.GetSafeHdc(), pDoc->cd.coord.viewLogicalTotal);
		aXimage.Save(bitmapName, 
			aXimage.GetTypeIdFromName(GetFileExtension(bitmapName))
			);
		

		////initialize bitmap file.
		//int row, col;
		//int linesize;
		//if (pDoc->bmp24.biForm == 24) {
		//	linesize = 3*ncol + ncol%4; //presumed line size.
		//}
		//else {
		//	linesize = ((ncol + 31) & ~31) >> 3;   //this is the scanline width for 1-bit BMPs, where ncol is number of pixels in x.
		//}
		//COLORREF color;
		////int  red, green, blue;
		//unsigned char* bmpline;
		//FILE* fou;
		//if (0==(fou = BMP24CreateWriteStream(bitmapName, ncol, nrow, pDoc->bmp24.biForm))) {
		//	return false;
		//}

		//if (0== (bmpline = BMP24CreateLineBuffer(ncol, pDoc->bmp24.biForm)) )
		//{
		//	AfxMessageBox(_T("Unable to allocate memory for holding bitmap data"));
		//	aDC.SelectObject(pOldBitmap);
		//	return false;
		//}

		////Begin progress control.
		//CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		//ASSERT(pFrame);
		//ASSERT_KINDOF(CMainFrame, pFrame);
		//pFrame->BeginProgress();
		//for (row=0; row<nrow; row++) {
		//	int iPos = (int)((float)row/(float)nrow*100.0);
		//	if(iPos<0)iPos=0;
		//	if(iPos>100)iPos=100;
		//	pFrame->SetProgress(iPos);

		//	for(col=0; col<ncol; col++) {
		//		p = pDoc->cd.pixel2logical(col, row);
		//		color = aDC.GetPixel(p); //this func uses logical coord. 
		//		BMP24SetRow(bmpline, col, color, pDoc->bmp24.biForm);
		//	}
		//	BMP24WriteRow(bmpline, ncol, fou, pDoc->bmp24.biForm);
		//}
		//fclose(fou);
		//free(bmpline);
		//pFrame->EndProgress();



				//red   = (int)GetRValue(color);
				//green   = (int)GetGValue(color);
				//blue   = (int)GetBValue(color);


		aDC.SelectObject(pOldBitmap);
		m_bmp->DeleteObject();
		delete m_bmp;
		aDC.DeleteDC();

		return true;
	}

	BOOL CSeisWideView::OnEraseBkgnd(CDC* pDC) 
	{
		// TODO: Add your message handler code here and/or call default

		// since I'll handle background erasing in separate places, I disable the following since it makes screen flickers often.

		// Set brush to desired background color 
		//CBrush backBrush(CColorHelper::WHITE); 
		//CBrush* pOldBrush = pDC->SelectObject(&backBrush); 
		//CRect rect; 
		//pDC->GetClipBox(&rect); 
		//pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY); 
		//pDC->SelectObject(pOldBrush); 

		return TRUE;  // returning TRUE, telling Windows not to erase the background
		//	return CScrollView::OnEraseBkgnd(pDC);
	}

	void CSeisWideView::OnToolbarZoom() 
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

		pFrame->statMain.add = pFrame->statMain.pick = pFrame->statMain.move = pFrame->statMain.remove = 0;
		if (pDoc->cd.zoom.isZooming == 0) {
			pDoc->cd.zoom.isZooming = 1;
			pDoc->cd.zoom.xminOld = pDoc->cd.coord.X1();
			pDoc->cd.zoom.xmaxOld = pDoc->cd.coord.X2();
			pDoc->cd.zoom.xTickOld = pDoc->cd.coord.xTick;
			pDoc->cd.zoom.yminOld = pDoc->cd.coord.Y1();
			pDoc->cd.zoom.ymaxOld = pDoc->cd.coord.Y2();
			pDoc->cd.zoom.yTickOld = pDoc->cd.coord.getYTick();
		}
		pDoc->cd.zoom.isMouseHand = FALSE;
		pFrame->myCaption(_T("When zooming, press Shift key and drag/drop to shift the display area in any direction."));
	}


	void CSeisWideView::OnUpdateToolbarZoom(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		pCmdUI->SetCheck(pDoc->cd.zoom.isZooming > 0);
	}

	void CSeisWideView::OnToolbarUnzoom() 
	{
		CSeisWideDoc* pDoc = GetDocument();
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->statMain.add = pFrame->statMain.pick = pFrame->statMain.move = pFrame->statMain.remove = 0;
		if(pDoc->cd.zoom.isZooming==0) {
			// if not zooming, and you click UnZoom, it actually activates zooming
			pDoc->cd.zoom.isZooming = -1;
			pDoc->cd.zoom.xminOld = pDoc->cd.coord.X1();
			pDoc->cd.zoom.xmaxOld = pDoc->cd.coord.X2();
			pDoc->cd.zoom.xTickOld = pDoc->cd.coord.xTick;
			if (pDoc->cd.coord.isTimeDomain()) {
				pDoc->cd.zoom.yTickOld = pDoc->cd.coord.tTick;
			} else {
				pDoc->cd.zoom.yTickOld = pDoc->cd.coord.zTick;
			}
			pDoc->cd.zoom.yminOld = pDoc->cd.coord.Y1();
			pDoc->cd.zoom.ymaxOld = pDoc->cd.coord.Y2();
		}
		else {
			pDoc->cd.coord.xTick = pDoc->cd.zoom.xTickOld;
			if (pDoc->cd.coord.drawmode == 6) {
				pDoc->cd.coord.trace1 = (long)pDoc->cd.zoom.xminOld;
				pDoc->cd.coord.trace2 = (long)pDoc->cd.zoom.xmaxOld;
			}
			else {
				pDoc->cd.coord.xmin = pDoc->cd.zoom.xminOld;
				pDoc->cd.coord.xmax = pDoc->cd.zoom.xmaxOld;
			}

			if (pDoc->cd.coord.isTimeDomain()) {
				pDoc->cd.coord.tTick = pDoc->cd.zoom.yTickOld;
			} else {
				pDoc->cd.coord.zTick = pDoc->cd.zoom.yTickOld;
			}
			pDoc->cd.coord.setY1(pDoc->cd.zoom.yminOld);
			pDoc->cd.coord.setY2(pDoc->cd.zoom.ymaxOld);
			pDoc->cd.zoom.isZooming = 0;
			pDoc->cd.zoom.isMouseHand = FALSE;

			// pDoc->draw.add = pDoc->draw.pick = pDoc->draw.move = pDoc->draw.remove = 0;
			pDoc->draw.isNodeChanged = TRUE;
			pDoc->draw.isSameDataDisplay = FALSE;
			pDoc->clearGDC(pDoc->draw.gDCs.gDC);
		}
		Invalidate(FALSE);	
	}

	void CSeisWideView::OnUpdateToolbarHand(CCmdUI* pCmdUI) 
	{
		CSeisWideDoc* pDoc = GetDocument();
		pCmdUI->SetCheck(pDoc->cd.zoom.isMouseHand);
	}

	void CSeisWideView::OnToolbarHand() 
	{
		CSeisWideDoc* pDoc = GetDocument();
		pDoc->cd.zoom.isMouseHand = !pDoc->cd.zoom.isMouseHand;

		// it would be inconvenient if this operation nulls the mouse clicking activities:
		//	pDoc->draw.add=0;
		//	pDoc->draw.pick=0;
		//	pDoc->draw.move=0;
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pFrame->statMain.remove = 0;
	}

	void CSeisWideView::OnKillFocus(CWnd* pNewWnd)
	{
		CScrollView::OnKillFocus(pNewWnd);

		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
	}

	void CSeisWideView::OnLButtonDblClk(UINT nFlags, CPoint point)
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();

		SEI_PAIR sp = pDoc->cd.coord.pixel2XY(point);
		transient.aQuad.x1 = sp.x;
		transient.aQuad.y1 = sp.y;
		if (pDoc->cd.coord.isVisibleOrNot(transient.aQuad.x1, transient.aQuad.y1) &&
			(pDoc->cd.coord.drawmode==0 || pDoc->cd.coord.drawmode==6 || pDoc->cd.coord.drawmode==7) && 
			pSeiApp->statWin.isAPressed &&
			isFileExistNotDirectory(pDoc->draw.timeSecName) ) {
				// the xOld and xNew are equal, and yOld and yNew are NOT equal. 
				// This means we want to kill or do static shifting for the currently selected trace
			CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
			if (pFrame->statMain.remove == 1 || pFrame->statMain.move == 1) {
					SegyWrapper sw(pDoc->draw.timeSecName, true); 

					if (CSegyMeta::segyHeadInit(pDoc->draw.timeSecName, &pDoc->head) != 1) {
						if (sw.open(&pDoc->head, 0)) {
							int indx = -1;
							if (pDoc->cd.coord.drawmode == 0 || pDoc->cd.coord.drawmode == 7) {
								sw.sortDistance(true);  // sorting is needed here
								indx = sw.getClosestTrcIndexAtOffset(
									pDoc->cd.coord.getOffsetFromXVal(transient.aQuad.x1, pDoc->tx.xShotRay)
									);
							}
							else if (pDoc->cd.coord.drawmode == 6)
								indx = sw.getClosestTrcIndexAtTraceNum((float)transient.aQuad.x1);
							if (indx >= 0) {
								int itrc = sw.headRead.trArr[indx].tracl;
								if (pFrame->statMain.remove == 1)
									sw.deleteTrc(itrc);
								else
									sw.staticShift(itrc, 0.5f);

								pDoc->draw.isSameDataDisplay = FALSE;
								this->Invalidate(TRUE);
							}
						} // if sw.open
					}
				} // if draw.move
		} // if

		CScrollView::OnLButtonDblClk(nFlags, point);
	}


	// Please retain this function, since we may need to call the function of another view which has different parameters
	CPoint CSeisWideView::X2CpointDevice(double x, double y) {
		// output is device coord
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		return pDoc->cd.coord.XY2pixel(x, y);
	}


	bool CSeisWideView::drawCurveBetween(CClientDC * clientDC, CPoint p1, CPoint p2, bool isDrawOutside, int ndiv)
	{
		if (p1.x == p2.x) return false;
		if (ndiv == 0) ndiv = 2;
		CString tStr; 


		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		SEI_PAIR sp = pDoc->cd.coord.pixel2XY(p1);
		double x1 = sp.x;
		double y1 = sp.y;
		sp = pDoc->cd.coord.pixel2XY(p2);
		double x2 = sp.x;
		double y2 = sp.y;
		if (!isDrawOutside && (!pDoc->cd.coord.isVisibleOrNot(x1, y1) || !pDoc->cd.coord.isVisibleOrNot(x2, y2))) return false;
		double y1Abs = y1 + CCoords::getReduceTimeEx2(
			pDoc->cd.coord.getOffsetFromXVal(x1, pDoc->tx.xShotRay),
			pDoc->cd.coord.drawmode, &pDoc->draw.red);
		double y2Abs = y2 + CCoords::getReduceTimeEx2(
			pDoc->cd.coord.getOffsetFromXVal(x2, pDoc->tx.xShotRay),
			pDoc->cd.coord.drawmode, &pDoc->draw.red);
		double ovred = (y2Abs - y1Abs) / (x2 - x1);  // this is slowness. it can be minus or zero
		double dxdiv = (x2 - x1) / (ndiv-1);
		clientDC->MoveTo(p1);
		for (int i=0; i<ndiv; i++) {
			register double x = x1 + i * dxdiv;
			register double y = y1Abs +  i * dxdiv * ovred - 
				CCoords::getReduceTimeEx2(
				pDoc->cd.coord.getOffsetFromXVal(x, pDoc->tx.xShotRay),
				pDoc->cd.coord.drawmode, &pDoc->draw.red);
			register CPoint p = pDoc->cd.coord.XY2pixel(x, y);
			clientDC->LineTo(p);
			//tStr.Format(_T("p1, p, p2: (%d %d) (%d %d) (%d %d)"), p1.x, p1.y, p.x, p.y, p2.x, p2.y);
			//pDoc->myCaption(tStr);
		}

		return true;
	}


	bool CSeisWideView::drawCurveLabelBetween(CClientDC * clientDC, CPoint p1, CPoint p2, bool isDrawOutside, int ndiv)
	{
		if (p1.x == p2.x) return false;
		if (ndiv == 0) ndiv = 2;

		CSeisWideDoc* pDoc = GetDocument();
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

		SEI_PAIR sp = pDoc->cd.coord.pixel2XY(p1);
		double x1 = sp.x;
		double y1 = sp.y;
		sp = pDoc->cd.coord.pixel2XY(p2);
		double x2 = sp.x;
		double y2 = sp.y;
	//	if (!isDrawOutside && (!pDoc->cd.coord.isVisibleOrNot(x1, y1) || !pDoc->cd.coord.isVisibleOrNot(x2, y2))) return false;
		//CSegyMeta::getXShotInSegy
		CSegyMeta::segyHeadInit(pDoc->draw.timeSecName, &pDoc->head);
		double xshotUse = isFloatEqual3((float)pDoc->head.xshotInSegy, ERRORVAL) ? pDoc->tx.xShotRay : pDoc->head.xshotInSegy;
		if (!SAME_SIGN(x1 - xshotUse, x2 - xshotUse)) {
			// now we need to disallow crossing over xshot
			return false;
		}


		double y1Abs = y1 + CCoords::getReduceTimeEx2(
			pDoc->cd.coord.getOffsetFromXVal(x1, xshotUse),
			pDoc->cd.coord.drawmode, &pDoc->draw.red);
		double y2Abs = y2 + CCoords::getReduceTimeEx2(
			pDoc->cd.coord.getOffsetFromXVal(x2, xshotUse),
			pDoc->cd.coord.drawmode, &pDoc->draw.red);
		double dx = x2 - x1;
		if (dx == 0.) return false;
		double ovred = (y2Abs - y1Abs) / dx;  // this is slowness. it can be minus or zero
		double dxdiv = (ndiv == 1) ? dx : dx / (double)(ndiv-1.);

		bool isEditting = pFrame->statMain.add != 0 || pFrame->statMain.pick != 0 ||
			pFrame->statMain.move != 0 || pFrame->statMain.remove != 0;

		if (pDoc->cd.zoom.isZooming && !isEditting) {
			clientDC->MoveTo(p1);
			clientDC->LineTo(p2.x, p1.y);
			clientDC->LineTo(p2);
			clientDC->LineTo(p1.x, p2.y);
			clientDC->LineTo(p1);
		}
		else {
			clientDC->MoveTo(p1);
			if (pDoc->dig.isDigitizing) {
				// in digitizing we do not want to show a curved mouse dragging line
				clientDC->LineTo(p2);
			}
			else {
				double x, y, tt;
				for (int i = 0; i<ndiv; i++) {
					tt = i * dxdiv;
					x = x1 + tt;
					y = y1Abs + tt * ovred -
						CCoords::getReduceTimeEx2(
						pDoc->cd.coord.getOffsetFromXVal(x, xshotUse),
						pDoc->cd.coord.drawmode, &pDoc->draw.red);
					register CPoint p = pDoc->cd.coord.XY2pixel(x, y);
					clientDC->LineTo(p);
					//tStr.Format(_T("p1, p, p2: (%d %d) (%d %d) (%d %d)"), p1.x, p1.y, p.x, p.y, p2.x, p2.y);
					//pDoc->myCaption(tStr);
				}
			}
		}

		CString tStr;
		if ( (pDoc->cd.coord.drawmode==0 || pDoc->cd.coord.drawmode==7) && fabs(ovred)>MINAMP ) {
			pDoc->draw.tmpDxdyDrawn = (float)fabs(1. / ovred);
			// note: we cannot have Vred>15km/s
			if (fabs(pDoc->draw.tmpDxdyDrawn) > 0.1 && fabs(pDoc->draw.tmpDxdyDrawn)<15.)
				tStr.Format(_T("Velocity(km/s): %f"), pDoc->draw.tmpDxdyDrawn);
			else
				tStr.Format(_T("dx/dy: %f"), pDoc->draw.tmpDxdyDrawn);
		} 
		else
			tStr.Format(_T("dy/dx: %f"), ovred );
		pFrame->myCaption(tStr);


		return true;
	}




	//
	//BOOL CSeisWideView::OnScrollBy(CSize sizeScroll, BOOL bDoScroll)
	//{
	//	CSeisWideDoc* pDoc = GetDocument();
	//	ASSERT_VALID(pDoc);
	//	if (sizeScroll.cx > sizeScroll.cy) {
	//		// if here, we only scroll horizontally
	//		double deltaX = (pDoc->cd.coord.xmax-pDoc->cd.coord.xmin)*0.5;
	//		pDoc->cd.coord.xmin += deltaX;
	//		pDoc->cd.coord.xmax += deltaX;
	//	}
	//	else {
	//		if (pDoc->isTimeDomain()) {
	//			double deltaY = (pDoc->cd.coord.tmax - pDoc->cd.coord.tmin)*0.5;
	//			pDoc->cd.coord.tmin += deltaY;
	//			pDoc->cd.coord.tmax += deltaY;
	//		}
	//		else {
	//			double deltaZ = (pDoc->cd.coord.zmax - pDoc->cd.coord.zmin)*0.5;
	//			pDoc->cd.coord.zmin += deltaZ;
	//			pDoc->cd.coord.zmax += deltaZ;
	//		}
	//	} // else
	//
	//	return CScrollView::OnScrollBy(sizeScroll, bDoScroll);
	//}


	void CSeisWideView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
		CSeisWideDoc* pDoc = GetDocument();
		bool isTime = pDoc->cd.coord.isTimeDomain() == TRUE;
		double deltaY = (pDoc->cd.coord.Y2() - pDoc->cd.coord.Y1() > 1.) ? 1. : 0.1;
		int nDelta = (int)(deltaY * (double)pDoc->cd.coord.mm_yscale);
		int aSign = (pDoc->cd.coord.isFaceUp) ? -1 : 1;

		switch (nSBCode)
		{
		case SB_LINEDOWN:
			break;

		case SB_LINEUP:
			nDelta = -nDelta;
			aSign = -aSign;
			break;

		case SB_PAGEDOWN:
			nDelta = nDelta * 4;
			break;

		case SB_PAGEUP:
			nDelta = -nDelta * 4;
			aSign = -aSign;
			break;

		case SB_THUMBPOSITION:
			return;

		default:
			return;
		}

		ScrollWindow(0, aSign * nDelta);

		if (isTime) {
			pDoc->cd.coord.m_tmin += aSign * deltaY;
			pDoc->cd.coord.m_tmax += aSign * deltaY;
		}
		else {
			pDoc->cd.coord.m_zmin += aSign * deltaY;
			pDoc->cd.coord.m_zmax += aSign * deltaY;
		}
		pDoc->draw.isSameDataDisplay = false;

		Invalidate(FALSE);
	//	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
	}

	void CSeisWideView::PostNcDestroy()
	{
		// TODO: Add your specialized code here and/or call the base class
		//m_scrollHelper->DetachWnd();
		CScrollView::PostNcDestroy();
	}


	void CSeisWideView::OnSize(UINT nType, int cx, int cy)
	{
		// mySchrolUpdate();
		CScrollView::OnSize(nType, cx, cy);

		if (0 >= cx || 0 >= cy || nType == SIZE_MINIMIZED)
			return;


		CSeisWideDoc* pDoc = GetDocument();
		switch (nType)
		{
		case SIZE_RESTORED:
		case SIZE_MAXIMIZED:
		{
			//if (pDoc->draw.wiggle>5 && pDoc->m_GLWin.myGL.isOpenGLWindowsExists) {
			//	CRect rectPixel = pDoc->cd.coord.getPixelsAxes();
			//	rectPixel.right = conditionPowerTwo(rectPixel.Width());
			//	rectPixel.bottom = conditionPowerTwo(rectPixel.Height());
			//	rectPixel.left = rectPixel.top = 0;
			//	pDoc->m_GLWin.myGL.m_rectPixel = rectPixel;

			//	pDoc->m_GLWin.myGL.m_rectDraw.right = pDoc->cd.coord.logicalAxes.Width();
			//	pDoc->m_GLWin.myGL.m_rectDraw.bottom = pDoc->cd.coord.logicalAxes.Height();
			//	pDoc->m_GLWin.myGL.m_rectDraw.left = pDoc->m_GLWin.myGL.m_rectDraw.top = 0;
			//}
			break;
		}

		default:
			break;
		}

	}





	void CSeisWideView::OnOptionsTogglehorizontalgrids()
	{
		CSeisWideDoc* pDoc = GetDocument();
		pDoc->draw.isNoHorzGrid = !pDoc->draw.isNoHorzGrid;
		Invalidate(TRUE);
	}




	void CSeisWideView::OnUpdateToolbarUnzoom(CCmdUI *pCmdUI)
	{
		CSeisWideDoc* pDoc = GetDocument();
		pCmdUI->SetCheck(pDoc->cd.zoom.isZooming < 0);
	}


	void CSeisWideView::mySchrolUpdate()
	{
		// cx and cy are the current display sizes in client coord
		// and they correspond to axisX1, axisX1, axisY1 and axisY2
		//CSeisWideDoc* pDoc = GetDocument();
		//bool isTime = pDoc->isTimeDomain() == TRUE;
		//int cYminFile = pDoc->cd.coord.mm_coordy(pDoc->head.tminFile, isTime);
		//int cYmaxFile = pDoc->cd.coord.mm_coordy(pDoc->head.tmaxFile, isTime);
		//int heightFile = cYmaxFile - cYminFile;

		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL; // SIF_ALL = SIF_PAGE | SIF_RANGE | SIF_POS;
		si.nMin = 0;
		si.nMax = 100;
		si.nPage = 10;
		si.nPos = 0;
		// this si.nPos is very important. Currently I have to set it to 0,
		// otherwise, the coordinate system would be wrong. If nPos > 0, 
		// SetScrollInfo will think that nPos=0 is the actual top of the current window size
		// and what to do with pDoc->cd.coord.logicalAxis? GetClientRect(rect) seems like pointing
		// to the entire drawing size going beyond the displayable area. 
		// So I am very confused here; it causes problems. 

		// And since OnDraw() will call this function at the end of full refreshing,
		// setting nPos=0 will put the scrholbar to the top. Maybe we do not need to worry about this?

		// Another complication: if the user zooms up, what to do?

		SetScrollInfo(SB_VERT, &si, TRUE); 


		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL; // SIF_ALL = SIF_PAGE | SIF_RANGE | SIF_POS;
		si.nMin = 0;
		si.nMax = 100;
		si.nPage = 10;
		si.nPos = 0;
		SetScrollInfo(SB_HORZ, &si, TRUE);

	}

	//void CSeisWideView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
	//{
	//	// TODO: Add your specialized code here and/or call the base class
	//	CSeisWideDoc* pDoc = GetDocument();
	//	//int cyOld = pDoc->cd.mm_coordy(pDoc->cd.coord.Y2() - pDoc->cd.coord.Y1(), 
	//	//	pDoc->isTimeDomain() == TRUE);

	//	bool isTime = pDoc->isTimeDomain() == TRUE;
	//	int cy = pDoc->cd.mm_coordy(pDoc->cd.coord.Y2(), isTime) - 
	//		pDoc->cd.mm_coordy(pDoc->cd.coord.Y1(), isTime);

	//	mySchrolUpdate(0, cy);
	//}






	BOOL CSeisWideView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
	{
		// TODO: Add your message handler code here and/or call default
		CSeisWideDoc* pDoc = GetDocument();
		pDoc->myCursorsAll();

		// return CScrollView::OnSetCursor(pWnd, nHitTest, message);
		// Note: if I allow the default cursor function CScrollView::OnSetCursor(), the actual cursor displayed
		//       will flash frequently, and I think it is confused about what cursor to use. So I disable it
		//       completely here, by simply return TRUE
		return TRUE;
	}



	void CSeisWideView::OnUpdateModelingActivateoverlaidmodel(CCmdUI *pCmdUI)
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		// pCmdUI->Enable(!pDoc->draw.velSecNamesEx.IsEmpty() && !pDoc->velMod.velSecName.IsEmpty());
		pCmdUI->Enable(!pDoc->draw.velSecNamesEx.IsEmpty());
		pCmdUI->SetCheck(pDoc->draw.velSecNamesEx.IsEmpty());
	}


	void CSeisWideView::OnModelingActivateoverlaidmodel()
	{
		// CString tStr;
		CSeisWideDoc* pDoc = GetDocument();
		if (pDoc->draw.velSecNamesEx.GetLength() > 0) {
			this->m_isModelSwitchMode = true;
		}
	}




	void CSeisWideView::OnUpdateOptionsToggleclipchangewithgain(CCmdUI *pCmdUI)
	{
		// TODO: Add your command update UI handler code here
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		pCmdUI->SetCheck(pFrame->statMain.isClipChangeWithGain);
	}




	void CSeisWideView::myUpdateStatusBar()
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
		ASSERT(pFrame);

		pFrame->statMain.m_nX = (float)transient.aQuad.x2;
		pFrame->statMain.m_nZ = (float)transient.aQuad.y2;
		if (pDoc->cd.coord.drawmode == 6) {
			pFrame->statMain.m_isXInt = true;
			pFrame->statMain.m_isYInt = false;
		}
		else {
			pFrame->statMain.m_isXInt = pFrame->statMain.m_isYInt = false;
		}

		if (
			pDoc->cd.coord.isModelingMode() &&
			pDoc->cd.coord.drawmode>0 &&
			pDoc->cd.coord.drawmode<5 &&
			transient.aQuad.y2 > pDoc->cd.coord.Y1() &&
			transient.aQuad.y2 < pDoc->cd.coord.Y2()
			) {
			int icont, j0;
			j0 = (int)((transient.aQuad.x2 - pDoc->cd.coord.xmin) / pDoc->velColor.dxVDesired + 0.5);
			if (j0 < 0) j0 = 0;
			if (j0 > 499) j0 = 499;
			// Note: 500 is used for internal velocity interpolation on the screen, which is not in v.in !

			for (icont = 0; icont<pDoc->velMod.ncont - 1; icont++) {
				if (transient.aQuad.y2 >= pDoc->velMod.grdDepth[icont][j0] &&
					transient.aQuad.y2 <= pDoc->velMod.grdDepth[icont + 1][j0]) {
					register double t = (pDoc->velMod.grdDepth[icont + 1][j0] - pDoc->velMod.grdDepth[icont][j0]);
					if (t>0.000001)
						pFrame->statMain.m_nV = (float)(pDoc->velMod.vTop[icont][j0] +
						(pDoc->velMod.vBot[icont][j0] - pDoc->velMod.vTop[icont][j0])*
						(transient.aQuad.y2 - pDoc->velMod.grdDepth[icont][j0]) / t);
					else
						pFrame->statMain.m_nV = (float)pDoc->velMod.vTop[icont][j0];
					pFrame->statMain.m_nN = icont + 1;
					break;
				}
			} // for icont
		} // if
		else
			pFrame->statMain.m_nV = -99.f;

		pDoc->myUpdateStatusBar();  // this line only update for  pFrame->statMain.gAmpThreshold
	}



/*
	// ----------------------------------------------------------
	// COglView::SaveAsBMP_2()
	//
	void CSeisWideView::SaveAsBMP_2(CString szFilename)
	{
		// -------------------------------------------
		// Declare local constants.
		//
		const int   k_nInchesPerFoot = 12;
		const float k_fFeetPerMeter = 3.280829895f;

		// -------------------------------------------
		// Declare local variables.
		//
		BITMAPINFO     ViewBitmapInfo;
		int            nXRes = 0;
		int            nYRes = 0;
		HDC            hViewDC = NULL;
		HBITMAP        hViewBitmap = NULL;
		LPVOID         pViewBitmapBits = NULL;
		HGLRC          hViewRC = NULL;
		CGLMemoryDC    GLMemoryDC;
		CRect          ImageRect(0, 0, 0, 0);
		CFile          File;
		CFileException FileException;

		// -------------------------------------------
		// Create a compatible.
		//
		hViewDC = CreateCompatibleDC(GetDC()->GetSafeHdc());

		// -------------------------------------------
		// Determine the number of pixels per logical
		// inch along the DC's width and height.
		//
		nXRes = GetDeviceCaps(hViewDC, LOGPIXELSX);
		nYRes = GetDeviceCaps(hViewDC, LOGPIXELSX);

		// -------------------------------------------
		// Initialize the bitmap header.
		//
		memset(&ViewBitmapInfo, 0, sizeof(BITMAPINFO));

		ViewBitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		ViewBitmapInfo.bmiHeader.biWidth = 800;
		ViewBitmapInfo.bmiHeader.biHeight = 600;
		ViewBitmapInfo.bmiHeader.biPlanes = 1;
		ViewBitmapInfo.bmiHeader.biBitCount = 24;
		ViewBitmapInfo.bmiHeader.biCompression = BI_RGB;
		ViewBitmapInfo.bmiHeader.biSizeImage = ViewBitmapInfo.bmiHeader.biWidth
			* ViewBitmapInfo.bmiHeader.biHeight * (ViewBitmapInfo.bmiHeader.biBitCount / 8);
		ViewBitmapInfo.bmiHeader.biXPelsPerMeter = long(nXRes * (k_nInchesPerFoot * k_fFeetPerMeter)); // was int(2840.0 / 72.0 * nXRes * fac);
		ViewBitmapInfo.bmiHeader.biYPelsPerMeter = long(nYRes * (k_nInchesPerFoot * k_fFeetPerMeter)); // was int(2840.0 / 72.0 * nYRes * fac);

																									   // -------------------------------------------
																									   // Create the DIB section.
																									   //
		hViewBitmap = CreateDIBSection(GetDC()->GetSafeHdc(), &ViewBitmapInfo, DIB_RGB_COLORS,
			&pViewBitmapBits, NULL, DWORD(0));
		SelectObject(hViewDC, hViewBitmap);

		//    // -------------------------------------------
		//    // Create a compatible bitmap and select the
		//    // bitmap into the DC.
		//    //
		//    hViewBitmap = CreateCompatibleBitmap (hViewDC,
		//        ViewBitmapInfo.bmiHeader.biWidth, ViewBitmapInfo.bmiHeader.biHeight);
		//    SelectObject (hViewDC, hViewBitmap);



		// -------------------------------------------
		// Set up the ViewImage's DC's pixel format.
		//
		SetupPixelFormat(CDC::FromHandle(hViewDC), hViewRC, FALSE);

		// -------------------------------------------
		// Prepare the memory DC.
		//
		GLMemoryDC.SetMemorySize(ViewBitmapInfo.bmiHeader.biWidth,
			ViewBitmapInfo.bmiHeader.biHeight);

		// -------------------------------------------
		// Call OpenGL drawing code.
		//
		wglMakeCurrent(hViewDC, hViewRC);
		DoRenderScene();
		//SwapBuffers (hViewDC); // Commented out because offscreen bitmap is single-buffered

		// -------------------------------------------
		// Copy the current OpenGL drawn image to
		// the memory DC.
		//
		ImageRect.SetRect(0, ViewBitmapInfo.bmiHeader.biWidth,
			0, ViewBitmapInfo.bmiHeader.biHeight);
		GLMemoryDC.CopyDataFromDC(CDC::FromHandle(hViewDC), ImageRect);

		// -------------------------------------------
		// Open, write, and close BMP image file.
		//
		File.Open(szFilename, CFile::modeCreate | CFile::modeWrite, &FileException);

		GLMemoryDC.WriteDataToDIBfile(&File);

		File.Close();

		// -------------------------------------------
		// Reset current rendering context.
		//
		wglMakeCurrent(hViewDC, NULL);

		// -------------------------------------------
		// Delete the ViewImage OpenGL rendering
		// context.
		//
		wglDeleteContext(hViewRC);

	} // end COglView::SaveAsBMP_2()


//	Here is COglView::SetupPixelFormat() :

		// ----------------------------------------------------------
		// COglView::SetupPixelFormat()
		//
		BOOL CSeisWideView::SetupPixelFormat(CDC* pDC, HGLRC& hRC, BOOL bDrawToWindow)
	{
		// -------------------------------------------
		// Declare local constants.
		//
		const char* k_szMessageFormatString_ss
			= "%s() failed in COglView::SetupPixelFormat().\n\n%s";

		// -------------------------------------------
		// Declare local variables.
		//
		CDC*        pLocalDC = NULL;
		HDC         hLocalDC = NULL;
		DWORD       dwPFDFlags = 0;
		BYTE        ucColorBufferSize = 24;
		BYTE        ucDepthBufferSize = 32;
		BOOL        bStatus = TRUE;
		int         nPixelFormat = 0;
		DWORD       dwError = 0;
		BOOL        bLocalStatus = FALSE;
		CString     szMessageString = "";
		LPTSTR      pErrorBuffer = NULL;

		// -------------------------------------------
		// If the passed pDC is NULL, set pLocalDC
		// to this class's CDC*, else set pLocalDC
		// to pDC.
		//
		if (pDC == NULL)
			pLocalDC = GetDC();
		else
			pLocalDC = pDC;

		hLocalDC = pLocalDC->GetSafeHdc();

		// -------------------------------------------
		// Only proceed if hLocalDC is non-NULL;
		//
		if (hLocalDC != NULL)
		{
			// -------------------------------------------
			// Determine the proper pixel format
			// descriptor flags.
			//
			if (bDrawToWindow != FALSE)
				dwPFDFlags
				= PFD_DRAW_TO_WINDOW   // dwFlags: draw to window (not bitmap)
				| PFD_SUPPORT_OPENGL   // dwFlags: support OpenGL
				| PFD_DOUBLEBUFFER;    // dwFlags: double-buffered mode
			else
				dwPFDFlags
				= PFD_DRAW_TO_BITMAP   // dwFlags: draw to bitmap (not window)
				| PFD_SUPPORT_OPENGL   // dwFlags: support OpenGL
				| PFD_SUPPORT_GDI;

			// -------------------------------------------
			// Determine the proper color buffer and
			// depth buffer sizes.
			//
			if (bDrawToWindow == FALSE)
			{
				CBitmap* pLocalBitmap = NULL;
				BITMAP   LocalBmInfo;

				pLocalBitmap = CDC::FromHandle(hLocalDC)->GetCurrentBitmap();
				pLocalBitmap->GetObject(sizeof(BITMAP), &LocalBmInfo);

				ucColorBufferSize = BYTE(LocalBmInfo.bmBitsPixel);
				ucDepthBufferSize = 32;
			}
			else
			{
				ucColorBufferSize = 24;
				ucDepthBufferSize = 32;

			} // end if (bDrawToWindow == FALSE) else

			  // -------------------------------------------
			  // Define the pixel format descriptor.
			  //
			static PIXELFORMATDESCRIPTOR pfd =
			{
				sizeof(PIXELFORMATDESCRIPTOR),  // structure size
				1,                              // structure version
				dwPFDFlags,                     // dwFlags:
												//   choices: PFD_DOUBLEBUFFER,
												//            PFD_STEREO
												//            PFD_DRAW_TO_WINDOW
												//            PFD_DRAW_TO_BITMAP
												//            PFD_SUPPORT_GDI
												//            PFD_SUPPORT_OPENGL
												//            PFD_GENERIC_FORMAT
												//            PFD_NEED_PALETTE
												//            PFD_NEED_SYSTEM_PALETTE
												//            PFD_SWAP_EXCHANGE
												//            PFD_SWAP_COPY
												//            PFD_SWAP_LAYER_BUFFERS
												//            PFD_GENERIC_ACCELERATED
												//            PFD_DEPTH_DONTCARE
												//            PFD_DOUBLEBUFFER_DONTCARE
												//            PFD_STEREO_DONTCARE
												PFD_TYPE_RGBA,                  // RGBA color mode; was PFD_TYPE_RGBA
																				//   choices: PFD_TYPE_RGBA,
																				//            PFD_TYPE_COLORINDEX
																				ucColorBufferSize,              // size of color buffer; 24 or 16; was 16
																				0,0,                            // not used (red bits, red shift)
																				0,0,                            // not used (green bits, red shift)
																				0,0,                            // not used (blue bits, red shift)
																				0,0,                            // not used (alpha bits, alpha shift)
																				0,                              // not used (accum bits)
																				0,                              // not used (accum red bits)
																				0,                              // not used (accum green bits)
																				0,                              // not used (accum blue bits)
																				0,                              // not used (accum alpha bits)
																				ucDepthBufferSize,              // size of depth buffer; 32 or 16; was 32
																				0,                              // not used (size of stencil buffer)
																				0,                              // not used (aux buffers)
																				PFD_MAIN_PLANE,                 // layer: draw in main plane; was PFD_MAIN_PLANE
																												//   choices: PFD_MAIN_PLANE,
																												//            PFD_OVERLAY_PLANE,
																												//            PFD_UNDERLAY_PLANE
																												0,                              // not used (reserved byte)
																												0,                              // not used (layer mask)
																												0,                              // not used (visible mask)
																												0                               // not used (damage mask)
			};

			// -------------------------------------------
			// Initialize bStatus to TRUE.
			//
			bStatus = TRUE;

			// -------------------------------------------
			// Choose the pixel format that best matches
			// that described in pfd.
			//
			nPixelFormat = ChoosePixelFormat(pLocalDC->m_hDC, &pfd);
			if (nPixelFormat == 0)
			{
				dwError = GetLastError();
				bLocalStatus = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
					NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
					(LPTSTR)&pErrorBuffer, 0, NULL);
				if (bLocalStatus != FALSE)
				{
					szMessageString.Format(k_szMessageFormatString_ss, "ChoosePixelFormat",
						pErrorBuffer);
					LocalFree(pErrorBuffer);
					pErrorBuffer = NULL;
				}
				else
					szMessageString.Format(k_szMessageFormatString_ss, "ChoosePixelFormat",
						"<<UNKNOWN ERROR>>");

				// -------------------------------------------
				// ERROR: SetupPixelFormat() failed.
				//
				MessageBox(szMessageString, "Fatal Error", MB_ICONERROR);

				bStatus = FALSE;

			} // end if (nPixelFormat == 0)

			  // -------------------------------------------
			  // Set the pixel format for the device context.
			  //
			if (bStatus != FALSE)
			{
				bLocalStatus = SetPixelFormat(pLocalDC->m_hDC, nPixelFormat, &pfd);
				if (bLocalStatus == FALSE)
				{
					dwError = GetLastError();
					bLocalStatus = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
						NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
						(LPTSTR)&pErrorBuffer, 0, NULL);
					if (bLocalStatus != FALSE)
					{
						szMessageString.Format(k_szMessageFormatString_ss, "SetPixelFormat",
							pErrorBuffer);
						LocalFree(pErrorBuffer);
						pErrorBuffer = NULL;
					}
					else
						szMessageString.Format(k_szMessageFormatString_ss, "SetPixelFormat",
							"<<UNKNOWN ERROR>>");

					// -------------------------------------------
					// ERROR: SetupPixelFormat() failed.
					//
					MessageBox(szMessageString, "Fatal Error", MB_ICONERROR);

					bStatus = FALSE;

				} // end if (bLocalStatus == FALSE)

			} // end if (bStatus != FALSE)

			  // -------------------------------------------
			  // Create the OpenGL rendering context.
			  //
			if (bStatus != FALSE)
			{
				hRC = wglCreateContext(pLocalDC->m_hDC);
				if (hRC == NULL)
				{
					dwError = GetLastError();
					bLocalStatus = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
						NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
						(LPTSTR)&pErrorBuffer, 0, NULL);
					if (bLocalStatus != FALSE)
					{
						szMessageString.Format(k_szMessageFormatString_ss, "wglCreateContext",
							pErrorBuffer);
						LocalFree(pErrorBuffer);
						pErrorBuffer = NULL;
					}
					else
						szMessageString.Format(k_szMessageFormatString_ss, "wglCreateContext",
							"<<UNKNOWN ERROR>>");

					// -------------------------------------------
					// ERROR: SetupPixelFormat() failed.
					//
					MessageBox(szMessageString, "Fatal Error", MB_ICONERROR);

					bStatus = FALSE;

				} // end if (hRC == NULL)

			} // end if (bStatus != FALSE)

			  // -------------------------------------------
			  // Make the OpenGL rendering context current.
			  //
			if (bStatus != FALSE)
			{
				bLocalStatus = wglMakeCurrent(pLocalDC->m_hDC, hRC);
				if (bLocalStatus == FALSE)
				{
					dwError = GetLastError();
					bLocalStatus = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
						NULL, dwError, MAKELANGID(LANG_NEUTRAL, SUBLANG_SYS_DEFAULT),
						(LPTSTR)&pErrorBuffer, 0, NULL);
					if (bLocalStatus != FALSE)
					{
						szMessageString.Format(k_szMessageFormatString_ss, "wglMakeCurrent",
							pErrorBuffer);
						LocalFree(pErrorBuffer);
						pErrorBuffer = NULL;
					}
					else
						szMessageString.Format(k_szMessageFormatString_ss, "wglMakeCurrent",
							"<<UNKNOWN ERROR>>");

					// -------------------------------------------
					// ERROR: SetupPixelFormat() failed.
					//
					MessageBox(szMessageString, "Fatal Error", MB_ICONERROR);

					bStatus = FALSE;

				} // end if (bLocalStatus == FALSE)

			} // end if (bStatus != FALSE)

			  // -------------------------------------------
			  // If SetupPixelFormat() didn't fail,
			  // initialize and then deselect the renedering
			  // context.
			  //
			if (bStatus != FALSE)
			{
				// -------------------------------------------
				// Initialize the rendering context.
				//
				Initialize();

				// -------------------------------------------
				// Deselect the rendering context.
				//
				wglMakeCurrent(NULL, NULL);

			} // end if (bStatus != FALSE)

			  // -------------------------------------------
			  // Release the pLocalDC if it was retrieved
			  // locally via GetDC().
			  //
			if (pDC == NULL)
				ReleaseDC(pLocalDC);
		}
		else
		{
			bStatus = FALSE;

		} // end if (pLocalDC != NULL) else

		  // -------------------------------------------
		  // Return bStatus.
		  //
		return bStatus;

	} // end COglView::SetupPixelFormat()

	*/

void CSeisWideView::OnTimer(UINT_PTR nIDEvent)
{
	// not working
	//for (int i = 0; i < 100; i++) {
	//	if (!this->transient.m_Drag) break;
	//	Sleep(500);
	//	LPARAM lParam = MAKELPARAM(transient.PointOld.x, transient.PointOld.y);
	//	SendMessage(WM_LBUTTONDOWN, MK_LBUTTON);
	//}

	CScrollView::OnTimer(nIDEvent);
}




void CSeisWideView::OnUpdateShow3dAnimation(CCmdUI *pCmdUI)
{
	CSeisWideDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
	pCmdUI->SetCheck(pDoc->draw.isSeisScapeAnimation);
	//pCmdUI->Enable(FALSE);

}


void CSeisWideView::OnShow3dAnimation()
{
	CSeisWideDoc* pDoc = GetDocument();
	pDoc->draw.isSeisScapeAnimation = pDoc->draw.isSeisScapeAnimation != TRUE;
	Invalidate(TRUE);
}


void CSeisWideView::OnSetFocus(CWnd* pOldWnd)
{
	CScrollView::OnSetFocus(pOldWnd);

	// TODO: Add your message handler code here
	// when a MDI window gained focus, we must assume control and shift keys are not pressed to begin with
		// I constantly have problem of control pressed too often. Hopefully these lines here can help in this
	//CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();
	//pSeiApp->statWin.isShiftPressed = FALSE;
	//pSeiApp->statWin.isAPressed = FALSE;
	//pSeiApp->statWin.isControlPressed = FALSE;

}


void CSeisWideView::OnRaycodeDisplay()
{
	CSeisWideDoc* pDoc = GetDocument();
	pDoc->dig.isDigitizing = !pDoc->dig.isDigitizing;
	Invalidate(TRUE);
}




void CSeisWideView::OnUpdateViewGravity(CCmdUI* pCmdUI)
{

	CSeisWideDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	pCmdUI->SetCheck(pDoc->cd.coord.drawmode == 8);
}
