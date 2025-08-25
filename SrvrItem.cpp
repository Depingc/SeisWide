// SrvrItem.cpp : implementation of the CSeisWideSrvrItem class
//

#include "stdafx.h"
#include "SeisWide.h"
#include "MainFrm.h"

#include "SeisWideDoc.h"
#include "SrvrItem.h"
#include "CntrItem.h"
#include "util.h"
#include "coords.h"

//#include "dlgLayerNodes.h"
//#include "dlgNewVelFile.h"
//#include "math.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//#define MINAMP 0.0000001
//#define MIL 1000000.

/////////////////////////////////////////////////////////////////////////////
// CSeisWideSrvrItem implementation

IMPLEMENT_DYNAMIC(CSeisWideSrvrItem, COleServerItem)

	CSeisWideSrvrItem::CSeisWideSrvrItem(CSeisWideDoc* pContainerDoc)
	: COleServerItem(pContainerDoc, TRUE)
{
	// TODO: add one-time construction code here
	//  (eg, adding additional clipboard formats to the item's data source)
}

CSeisWideSrvrItem::~CSeisWideSrvrItem()
{
	// TODO: add cleanup code here
}

void CSeisWideSrvrItem::Serialize(CArchive& ar)
{
	// CSeisWideSrvrItem::Serialize will be called by the framework if
	//  the item is copied to the clipboard.  This can happen automatically
	//  through the OLE callback OnGetClipboardData.  A good default for
	//  the embedded item is simply to delegate to the document's Serialize
	//  function.  If you support links, then you will want to serialize
	//  just a portion of the document.

	if (!IsLinkedItem())
	{
		CSeisWideDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->Serialize(ar);
	}
}

BOOL CSeisWideSrvrItem::OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize)
{
	// Most applications, like this one, only handle drawing the content
	//  aspect of the item.  If you wish to support other aspects, such
	//  as DVASPECT_THUMBNAIL (by overriding OnDrawEx), then this
	//  implementation of OnGetExtent should be modified to handle the
	//  additional aspect(s).

	if (dwDrawAspect != DVASPECT_CONTENT)
		return COleServerItem::OnGetExtent(dwDrawAspect, rSize);

	// CSeisWideSrvrItem::OnGetExtent is called to get the extent in
	//  HIMETRIC units of the entire item.  The default implementation
	//  here simply returns a hard-coded number of units.

	CSeisWideDoc* pDoc = GetDocument(); 
	ASSERT_VALID(pDoc);

	// TODO: replace this arbitrary size

	rSize.cx = pDoc->draw.clipBoardSizeX*100;   // HIMETRIC units
	rSize.cy = pDoc->draw.clipBoardSizeY*100;   // 3000 x 3000 HIMETRIC units
	//CClientDC pDC;
	//rSize = pDC.GetWindowExt();
	/*CClientDC dc(NULL);
	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetViewportExt(dc.GetDeviceCaps(LOGPIXELSX), dc.GetDeviceCaps(LOGPIXELSY));
	dc.SetWindowExt(100, -100);
	dc.LPtoHIMETRIC(&rSize);
	*/ 
	return TRUE;
}



BOOL CSeisWideSrvrItem::OnDraw(CDC* pDC, CSize& rSize)
{

	CSeisWideDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// CPoint p,p1,tick[2];
	if(pDoc->cd.coord.drawmode == 5) return TRUE;
	if(pDoc->cd.coord.drawmode<0) return FALSE;

	//###################### set the view coordinates for the rectangle of  plot.######################
	// defines device coords for the plotting rectangle
	pDC->SetMapMode(MM_ANISOTROPIC);
	this->OnGetExtent(DVASPECT_CONTENT, rSize);
//	pDoc->cd.coord.isTimeDomain = pDoc->isTimeDomain() == TRUE;
//	pDoc->cd.coord.isTraceDomain = pDoc->cd.coord.drawmode == 6;


	// Note: be very careful here. It can take a long time to get the clipboard right since I use ANISOTROPIC transformation
	//  the logical total printing area is about 4 times the pixel size, assuming coord->device2Logical=35.
	//  Not sure why it has to be this way.
	double factor = (int)myRound(pDoc->cd.coord.device2Logical * 0.05);  // about 2
		// if factor=5, some plots not showing correct.
	//  if factor<4, the texts show bigger in clipboard. factor=2 makes texts too big
	// NOTE: the final size rSize.cx*factor  or  rSize.cy*factor  cannot exceed 32000,
	// otherwise it cannot display well.
	double amax = max(rSize.cx*factor, rSize.cy*factor);
	if (amax > 32000) {
		factor *= 32000.0 / (double)amax;
	}


	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();
	bool isYLabelOnRight = pFrame->statMain.isSwapXActive != pDoc->cd.coord.isSwapX;
	if (pDoc->cd.coord.isSwapYLabel) isYLabelOnRight = !isYLabelOnRight;
	pDoc->cd.setCoordMappingFromWorld(pDC, pDoc->draw.isNoLableOutside,
		isYLabelOnRight,
		rSize, CSize((int)(rSize.cx*factor), (int)(rSize.cy*factor)), 
		1, 1, &pDoc->cd.coord);
	//#####################No more changes for the mapping parameters

	if (!pDoc->InitializeSec()) return FALSE;

	// In clipboard, we do not really need the segy data, since it will be very slow anyway
	CString fSection = pDoc->cd.coord.isTimeDomain() ? pDoc->draw.timeSecName : pDoc->draw.depthSecName;
	CColorHelper::AMPCOLORDYN ampColorDyn2(fSection); // only used in color mode
	ampColorDyn2.getAmpColorArray(pDoc->head.maxAmp, false, pDoc->draw.logScaleThreshold, (double)pDoc->draw.ampPower);
	pDoc->draw.isSameDataDisplay = FALSE;
	pDoc->DrawControlsSegy(pDC, &ampColorDyn2, NULL);
	pDoc->m_GLWin.mySeis3DFBO_Animate(pDC, -1, &pDoc->cd.coord.logicalAxes);

	if (pDoc->isModelVisible(&pDoc->velMod, 0.))
		pDoc->DrawVel(pDC);
	pDoc->DrawControlsEx(pDC);
	pDoc->Tick(pDC);	//Draw ticks.
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSeisWideSrvrItem diagnostics

#ifdef _DEBUG
void CSeisWideSrvrItem::AssertValid() const
{
	COleServerItem::AssertValid();
}

void CSeisWideSrvrItem::Dump(CDumpContext& dc) const
{
	COleServerItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
