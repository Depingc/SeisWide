// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SeisWide.h"
//#include "SeisWideDoc.h"
#include "IniReader.h"

#include "MainFrm.h"
#include "Splash.h"
#include "util.h"
#include "dlgHardwareId.h"
#include "dlgRegiDemo.h"
#include "AutoUpdater.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_INITMENU()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_COORD, OnUpdateText)
	// Global help commands
//	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, &CMainFrame::OnUpdateFilePrint)
	ON_COMMAND(ID_HELP_LICENSESTATUS, &CMainFrame::OnHelpLicensestatus)
	ON_UPDATE_COMMAND_UI(ID_FILE_EXPORTBITMAP, &CMainFrame::OnUpdateFileExportbitmap)
	ON_COMMAND(ID_HELP_FINDER, &CMainFrame::OnHelpFinder)
	ON_UPDATE_COMMAND_UI(ID_DIGITIZING_LOADEXISTING, &CMainFrame::OnUpdateDigitizingLoadexisting)
	ON_UPDATE_COMMAND_UI(ID_DIGITIZING_MIGRATELINEDRAWINGS, &CMainFrame::OnUpdateDigitizingMigratelinedrawings)
	ON_UPDATE_COMMAND_UI(ID_PROCESSING_CHANGEREDUCEVELOCITYFORBMP, &CMainFrame::OnUpdateProcessingChangereducevelocityforbmp)
	ON_UPDATE_COMMAND_UI(ID_PROCESSING_COHMIX, &CMainFrame::OnUpdateProcessingCohmix)
	ON_UPDATE_COMMAND_UI(ID_PROCESSING_KDMIGPOST, &CMainFrame::OnUpdateProcessingKdmigpost)
	ON_UPDATE_COMMAND_UI(ID_PROCESSING_KDMIGSIMPLE, &CMainFrame::OnUpdateProcessingKdmigsimple)
	ON_UPDATE_COMMAND_UI(ID_TIME_FORWIDE_NMOVELOCITY, &CMainFrame::OnUpdateTimeForwideNmovelocity)
	ON_UPDATE_COMMAND_UI(ID_TIME_TIMESECTIONWITHPERTURBEDTRAVEL, &CMainFrame::OnUpdateTimeTimesectionwithperturbedtravel)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STACKINGVELOCITYMAP, &CMainFrame::OnUpdateViewStackingvelocitymap)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MAPCONVERTTIME, &CMainFrame::OnUpdateToolsMapconverttime)
	ON_UPDATE_COMMAND_UI(ID_TIME_FORWIDE, &CMainFrame::OnUpdateTimeForwide)
	ON_COMMAND(ID_VIEW_USAGELOG, &CMainFrame::OnViewUsagelog)
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_THRESHOLD, &CMainFrame::OnUpdateIndicatorThreshold)
	
	ON_COMMAND(ID_WINDOW_TILEVERTICAL, &CMainFrame::OnWindowTilevertical)
	ON_WM_HSCROLL()
	ON_UPDATE_COMMAND_UI(ID_IMPORT_BITMAP2SEGY, &CMainFrame::OnUpdateImportBitmap2segy)
	ON_UPDATE_COMMAND_UI(ID_DIGITIZE_ENDCURVESTOSEGY, &CMainFrame::OnUpdateDigitizeEndcurvestosegy)
	ON_COMMAND(ID_FILE_OPENPROJECT, &CMainFrame::OnFileOpenproject)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
//	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
	ID_INDICATOR_THRESHOLD,
	ID_INDICATOR_COORD,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame():
	m_pWndCurrentChild(NULL)
{
	// TODO: add member initialization code here
	//gIsDisplayFreeze = FALSE; // if TRUE, the display should freeze, useful for printing
	pViewPrev = NULL;
	pViewCurr = NULL;
	CString tStr;
	CString serverAddr = _T("http://seiswide.hopto.org/seiswide/");
	//CString serverAddr2 = _T("http://xiuxian.no-ip.info/seiswide/");

	this->pViewCurr = this->pViewPrev = NULL; // this line avoids program crash, but not effective 
	getSeisWideIni();


// #ifndef _DEBUG
	CAutoUpdater updater;
	CString strResult = _T("The application was updated and will be activated the next time you run it.");
	if (CAutoUpdater::Success == updater.CheckForUpdate(serverAddr)) {
		MessageBox(strResult,
			_T("Updater"),
			MB_ICONINFORMATION | MB_OK);
		PostQuitMessage(0);
	}
	//else {
	//	if (CAutoUpdater::Success == updater.CheckForUpdate(serverAddr2)) {
	//		MessageBox(strResult, 
	//			_T("Updater"), 
	//			MB_ICONINFORMATION|MB_OK);
	//		PostQuitMessage(0);
	//	}
	//}
// #endif _DEBUG


	// if SeisWide is activated, it is ok. 
	CLicenseCheck ls;
	CString hardwareId = ls.getMACaddress();
	if (ls.isNew2SeisWide()) {
		// for a new user to SeisWide, we let 30 days free try automatically
		ls.setDefaultLicenseFile(hardwareId);

		// since the license is changed, we need to detect it again
		ls.getCurrentLicenseStat8();

		// now update the struct info
		this->m_licenseStruct = ls.licenseStruct;
		return;
	}

	ls.getCurrentLicenseStat8();
	this->m_licenseStruct = ls.licenseStruct;
	if (this->m_licenseStruct.strLicenseCode.IsEmpty())
		AfxMessageBox(_T("Invalid license detected."), MB_ICONQUESTION);

	if (this->m_licenseStruct.type == CLicenseCheck::demo) {
		CdlgRegiDemo dlgRegi;
		dlgRegi.m_HardwareId = hardwareId;
		dlgRegi.m_serverRoot = serverAddr;
		while (true) {
			bool isTrue = dlgRegi.grabDataFromServer(hardwareId);
			dlgRegi.saveRegiToLocal(); // will save whatever the user has entered, even if invalid
			if (!isTrue)
				Sleep(3000);
			
				isTrue = dlgRegi.grabDataFromServer(hardwareId); // do it again since
			// sometimes we need to do it twice to succeed

			if (!isTrue) {
				AfxMessageBox(_T("Demo license could not be obtained at this time. You may try it later or email to seiswide@gmail.com for help."), MB_ICONQUESTION);
				break;
			}

			INT_PTR res = dlgRegi.DoModal();
			if (res == IDCANCEL) break;
			if (dlgRegi.m_license.GetLength() > 10)  {
				// first save the license file
				CStringArray *strArr = new CStringArray();
				strArr->Add(dlgRegi.m_license);
				CString f = getSeisWideParaPath() + _T("\\license.txt");
				bool isSuc = saveStringArrayToFile(strArr, f);

				if (isSuc) {
					// since the license is changed, we need to detect it again
					ls.getCurrentLicenseStat8();
					if (ls.licenseStruct.nDaysAllowed > 35) {
						// this indicates that the user has faked the server. So we need to punish it.
						isSuc = saveStringArrayToFile(strArr, f);
						ls.getCurrentLicenseStat8();
					}
					// now update the struct info
					this->m_licenseStruct = ls.licenseStruct;
				}
				else {
					CString f = getSystemTempPathSlash() + _T("license.txt");
					isSuc = saveStringArrayToFile(strArr, f);
					this->m_licenseStruct = ls.licenseStruct;
					tStr.Format(_T("Please manually move %s to SeisWide's folder."), f);
					myUnicodeProcess(_T("explorer.exe"), f, false);
					AfxMessageBox(tStr);
				}
				strArr->RemoveAll();
				delete strArr;
				break;
			}
		}
	}
	else if (this->m_licenseStruct.nDaysLeftover < 3) {
		// now, prompt for hardware ID to email to me
		CdlgHardwareId dlg;
		dlg.m_HardwareId = hardwareId;
		dlg.m_licenseDetectedInfo.Format(_T("Current license status: %s,  %d days left. %s"),
			ls.licenseEnum2Str(ls.licenseStruct.type),
			ls.licenseStruct.nDaysLeftover,
			ls.licenseStruct.strLicenseName
			);

		if (dlg.DoModal() == IDOK) {
		}
	}




}

CMainFrame::~CMainFrame()
{
	if (psei != NULL) {
		for (int i = 0; i < NUM_CACHE; i++) {
			register int nx;
			if (psei->pCacheSegyWraps[i]) {
				nx = psei->pCacheSegyWraps[i]->headRead.getnx();
				psei->pCacheSegyWraps[i]->freeSegyArray(&psei->pCacheSegyWraps[i]->headRead);
				delete psei->pCacheSegyWraps[i];
			}

			if (psei->pCacheSegyWrapEx[i]) {
				// nx = psei->pCacheSegyWrapEx[i]->headRead.nx();
				psei->pCacheSegyWrapEx[i]->freeSegyArray();
				delete psei->pCacheSegyWrapEx[i];
			}
		}
		delete psei;
		psei = NULL;
	}
	m_wndMDIClient.Detach();
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	//deping's addition.
	if (m_wndMDIClient.Attach(m_hWndMDIClient) == 0)    {
        TRACE0("Failed to attach MDIClient.\n");
        return -1;      // fail to create
    }
		
	if (!m_wndToolBar.Create(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

//	SetTimer(1234,1000, NULL); //Added by Deping.
	// CG: The following line was added by the Splash Screen component.
//	CSplashWnd::ShowSplashScreen(this);


	//Deping test on slider control on the toolbar
	RECT rc;
	m_wndToolBar.GetItemRect(m_wndToolBar.GetCount()-7, &rc);
	LONG aWidth = rc.right - rc.left;
	rc.left = rc.right;
	rc.right = rc.left + (int)(aWidth * 5.1);

	m_slider1Threshold.Create(WS_VISIBLE | WS_BORDER | TBS_AUTOTICKS | TBS_HORZ, rc, &m_wndToolBar, 109);
	m_slider1Threshold.SetRange(0, 100, TRUE);
	m_slider1Threshold.SetTicFreq(5);
	m_slider1Threshold.SetLineSize(2);

	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::BeginProgress()
{
  // find the rectangle of the pane
  // where we want the status bar
  RECT rc;
  m_wndStatusBar.GetItemRect (0, &rc);

//  EndProgress();
  m_ctlProgress.DestroyWindow();

  //CProgressCtrl wndProgress;
  VERIFY (m_ctlProgress.Create(WS_CHILD | 
     WS_VISIBLE, rc, &m_wndStatusBar, 1));

  m_ctlProgress.SetRange(0, 100);
  SetCursor(LoadCursor(NULL,IDC_WAIT));
}

void CMainFrame::SetProgress(int nProg)
{
  ASSERT(nProg <= 100);
  ASSERT(m_ctlProgress.m_hWnd != NULL);

  m_ctlProgress.SetPos(nProg);
  if ( (int)(nProg/10)*10 == nProg ) {
	  m_ctlProgress.Invalidate();
	  m_ctlProgress.UpdateWindow();
  }
}

void CMainFrame::EndProgress()
{
  ASSERT(m_ctlProgress.m_hWnd != NULL);

  m_ctlProgress.DestroyWindow();
  SetCursor(LoadCursor(NULL,IDC_ARROW));
}
void CMainFrame::SetAppName(LPCTSTR Title) {
	CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();
	m_strTitle.Format(_T("%s -- %s"), Title, pSeiApp->statWin.winProjectName);
	Invalidate(false); 
}

void CMainFrame::OnUpdateText(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable();

	// now print the text
	if (statMain.m_nV < 0.f) {
		if (statMain.m_isXInt && statMain.m_isYInt)
			strText.Format(_T("XY %d   %d"), (int)(statMain.m_nX+0.5), (int)(statMain.m_nZ+0.5));
		else if (statMain.m_isXInt)
			strText.Format(_T("XY %d   %.5g"), (int)statMain.m_nX, myFloat(statMain.m_nZ, 4));
		else
			strText.Format(_T("XY%.5g   %.5g"), myFloat(statMain.m_nX, 5), myFloat(statMain.m_nZ, 5));
	}
	else {
		// since there are too many chars here, I need to shorten a little
		if (statMain.m_nX < 2. && statMain.m_nX < 2.)
			strText.Format(_T("XYV:%1.4f|%.4g|%.4g"), myFloat(statMain.m_nX, 4), myFloat(statMain.m_nZ, 4), myFloat(statMain.m_nV, 4));
		else if (statMain.m_nX < 10)
			strText.Format(_T("XYV:%.4g|%.4g|%.4g"), myFloat(statMain.m_nX, 4), myFloat(statMain.m_nZ, 4), myFloat(statMain.m_nV, 4));
		else
			strText.Format(_T("XYV:%.4g|%.4g|%.4g"), myFloat(statMain.m_nX, 4), myFloat(statMain.m_nZ, 4), myFloat(statMain.m_nV, 4));
	}
	pCmdUI->SetText(strText);
}



//----------------------------------------------------------------
// This function finds the CMDIChildWnd in the list of windows
// maintained by the application's MDIClient window following the
// one pointed to by the member variable m_pWndCurrentChild. If no
// further CMDIChildWnds are in the list, NULL is returned.
//----------------------------------------------------------------

CMDIChildWnd* CMainFrame::GetNextMDIChildWnd() { 
//	m_pWndCurrentChildOld = m_pWndCurrentChild; // backup the pointer
	
   if (!m_pWndCurrentChild)
     {
      // Get the first child window.
      m_pWndCurrentChild = m_wndMDIClient.GetWindow(GW_CHILD);
     }
   else
     {
      // Get the next child window in the list.
        m_pWndCurrentChild=
           (CMDIChildWnd*)m_pWndCurrentChild->GetWindow(GW_HWNDNEXT);
     }

   if (!m_pWndCurrentChild)
     {
      // No child windows exist in the MDIClient,
      // or you are at the end of the list. This check
      // will terminate any recursion.
      return NULL;
     }

  // Check the kind of window
    if (!m_pWndCurrentChild->GetWindow(GW_OWNER))
      {
        if (m_pWndCurrentChild->
                           IsKindOf(RUNTIME_CLASS(CMDIChildWnd)))
          {
                 // CMDIChildWnd or a derived class.
                 return (CMDIChildWnd*)m_pWndCurrentChild;
          }
        else
          {
                 // Window is foreign to the MFC framework.
                 // Check the next window in the list recursively.
                 return GetNextMDIChildWnd();
          }
      }
    else
      {
          // Title window associated with an iconized child window.
          // Recurse over the window manager's list of windows.
          return GetNextMDIChildWnd();
      }
} 

//-----------------------------------------------------------------
// This function counts the number of CMDIChildWnd objects
// currently maintained by the MDIClient.
//-----------------------------------------------------------------
/*hkfdjf */

int CMainFrame::GetCountCMDIChildWnds()
{ 
 int count = 0;

 CMDIChildWnd* pChild = GetNextMDIChildWnd();
 while (pChild)
  {
    count++;
    pChild = GetNextMDIChildWnd();
  }
 return count;
} 

BOOL CMainFrame::HideOtherChild()
{ 
 int count = 0;

 CMDIChildWnd* pChild = GetNextMDIChildWnd();
//  pChild->ShowWindow(SW_SHOW);

 while (pChild)
  {
    count++;
    pChild = GetNextMDIChildWnd();
	if(pChild) pChild->ShowWindow(SW_MINIMIZE);
  }
 return TRUE;
} 

BOOL CMainFrame::ShowOtherChild()
{ 
 int count = 0;

 CMDIChildWnd* pChild = GetNextMDIChildWnd();
 
 while (pChild)
  {
    count++;
    pChild = GetNextMDIChildWnd();
	pChild->ShowWindow(SW_SHOW);
  }
 return TRUE;
} 


void CMainFrame::OnInitMenu(CMenu* pMenu)
{
   CMDIFrameWnd::OnInitMenu(pMenu);
   int nMenuCount = pMenu->GetMenuItemCount();
   
   //  pMenu->SetDefaultItem()
   /*
	// CG: This block added by 'Tip of the Day' component.
	{
		// TODO: This code adds the "Tip of the Day" menu item
		// on the fly.  It may be removed after adding the menu
		// item to all applicable menu items using the resource
		// editor.

		// Add Tip of the Day menu item on the fly!
		static CMenu* pSubMenu = NULL;

		CString strHelp; strHelp.LoadString(CG_IDS_TIPOFTHEDAYHELP);
		CString strMenu;
		int nMenuCount = pMenu->GetMenuItemCount();

		
		// trying to add More Windows...   to the Windows menu
		CString strSubMenu;
		strSubMenu.Format(_T("&Window"));
		for (int j=0; j < nMenuCount; j++) 
		{
			pMenu->GetMenuString(j, strMenu, MF_BYPOSITION);
			if (strMenu == strSubMenu)
			{ 
				pSubMenu = pMenu->GetSubMenu(j);
				ASSERT(pSubMenu != NULL);
			}
		}
		// end testing
		

		BOOL bFound = FALSE;
		for (int i=0; i < nMenuCount; i++) 
		{
			pMenu->GetMenuString(i, strMenu, MF_BYPOSITION);
			if (strMenu == strHelp)
			{ 
				pSubMenu = pMenu->GetSubMenu(i);
				bFound = TRUE;
				ASSERT(pSubMenu != NULL);
			}
		}

		CString strTipMenu;
		strTipMenu.LoadString(CG_IDS_TIPOFTHEDAYMENU);
		if (!bFound)
		{
			// Help menu is not available. Please add it!
			if (pSubMenu == NULL) 
			{
				// The same pop-up menu is shared between mainfrm and frame 
				// with the doc.
				static CMenu popUpMenu;
				pSubMenu = &popUpMenu;
				pSubMenu->CreatePopupMenu();
				pSubMenu->InsertMenu(0, MF_STRING|MF_BYPOSITION, 
					CG_IDS_TIPOFTHEDAY, strTipMenu);
			} 
			pMenu->AppendMenu(MF_STRING|MF_BYPOSITION|MF_ENABLED|MF_POPUP, 
				(UINT)pSubMenu->m_hMenu, strHelp);

			DrawMenuBar();
		} 
		else
		{      
			// Check to see if the Tip of the Day menu has already been added.
			pSubMenu->GetMenuString(0, strMenu, MF_BYPOSITION);

			if (strMenu != strTipMenu) 
			{
				// Tip of the Day submenu has not been added to the 
				// first position, so add it.
				pSubMenu->InsertMenu(0, MF_BYPOSITION);  // Separator
				pSubMenu->InsertMenu(0, MF_STRING|MF_BYPOSITION, 
					CG_IDS_TIPOFTHEDAY, strTipMenu);
			}
		}
	}
	*/
}


CDocument * CMainFrame::GetActiveDocMy(void)
{
	CDocument *pDoc = this->MDIGetActive()->GetActiveDocument();
	return pDoc;
}
CView * CMainFrame::GetActiveViewMy(void)
{
	CView *pView = this->MDIGetActive()->GetActiveView();
	return pView;
}

void CMainFrame::getSeisWideIni(void)
{
	psei = new SEI_PAR;
	psei->isInitialized = false;

	CString aPath = 	getSeisWideParaPath() + _T("\\Seiswide.ini");
	if (!isFileExistNotDirectory(aPath)) {
		// Seiswide.ini is missing, so we have to set the defaults!
		psei->anisotropy_layer0 = -1;
		psei->regionalModelA = 0.212765957;
		psei->regionalModelB = 0.41223404;
		psei->regionalModelV0 = 1.69;
		psei->regionalModelVinf = 5.33;
		psei->regionalModelAlpha = 0.42745;
		psei->sxLong = 1.44f;
		psei->syLat  = 1.44f;

		// freq filtering ranges
		psei->freq1s = 1.3f;
		psei->freq2s = 1.5f;
		psei->freq3s = 36.f;
		psei->freq4s = 40.f;

		psei->ta =  0.f;
		psei->f1a =  1.5f;
		psei->f2a =  56.f;
		psei->tb =  0.f;
		psei->f1b =  0.f;
		psei->f2b =  0.f;

		psei->num_tfilter = 2;
		psei->yesFilterT = true;
		psei->yesFilterF = false;
		psei->normValue = 1000.f;
		psei->velWater = 1.44f;
		psei->zerophase = true;
		return;
	}

	// CIniReader iniReader(aPath.GetBuffer(_MAX_PATH));
	//aPath.ReleaseBuffer();

	

	CIniReader iniReader(aPath);
	psei->anisotropy_layer0 = iniReader.ReadInteger(_T("Setting"), _T("anisotropy_layer0"), -1);
	psei->regionalModelA = iniReader.ReadFloat(_T("Setting"), _T("regionalModelA"), 0.212765957f);
	psei->regionalModelB = iniReader.ReadFloat(_T("Setting"), _T("regionalModelB"), 0.41223404f);
	psei->regionalModelV0 = iniReader.ReadFloat(_T("Setting"), _T("regionalModelV0"), 1.69f);
	psei->regionalModelVinf = iniReader.ReadFloat(_T("Setting"), _T("regionalModelVinf"), 5.33f);
	psei->regionalModelAlpha = iniReader.ReadFloat(_T("Setting"), _T("regionalModelAlpha"), 0.42745f);

	psei->regionalSedVmin = iniReader.ReadFloat(_T("Setting"), _T("regionalSedVmin"), 1.52f);
	psei->regionalSedVmax = iniReader.ReadFloat(_T("Setting"), _T("regionalSedVmax"), 4.3f);


	psei->sxLong = iniReader.ReadFloat(_T("Setting"), _T("sxLong"), 1.44f);
	psei->syLat  = iniReader.ReadFloat(_T("Setting"),  _T("syLat"), 1.44f);

	// freq filtering ranges
	psei->freq1s = iniReader.ReadFloat(_T("Filters"),  _T("freq1s"), 1.3f);
	psei->freq2s = iniReader.ReadFloat(_T("Filters"),  _T("freq2s"), 1.5f);
	psei->freq3s = iniReader.ReadFloat(_T("Filters"),  _T("freq3s"), 36.f);
	psei->freq4s = iniReader.ReadFloat(_T("Filters"),  _T("freq4s"), 40.f);

	psei->ta =  iniReader.ReadFloat(_T("Filters"),  _T("ta"), 0.f);
	psei->f1a =  iniReader.ReadFloat(_T("Filters"),  _T("f1a"), 1.5f);
	psei->f2a =  iniReader.ReadFloat(_T("Filters"),  _T("f2a"), 56.f);
	psei->tb =  iniReader.ReadFloat(_T("Filters"),  _T("ta"), 0.f);
	psei->f1b =  iniReader.ReadFloat(_T("Filters"),  _T("f1b"), 0.f);
	psei->f2b =  iniReader.ReadFloat(_T("Filters"),  _T("f2b"), 0.f);

	psei->num_tfilter = iniReader.ReadInteger(_T("Filters"),  _T("num_tfilter"), 2);
	psei->yesFilterT = iniReader.ReadBoolean(_T("Filters"),  _T("yesFilterT"), true);
	psei->yesFilterF = iniReader.ReadBoolean(_T("Filters"),  _T("yesFilterF"), false);
	psei->normValue = iniReader.ReadFloat(_T("Filters"),  _T("normValue"), 1000.f);
	psei->velWater = iniReader.ReadFloat(_T("Filters"),  _T("velWater"), 1.44f);
	psei->zerophase = iniReader.ReadBoolean(_T("Filters"),  _T("zerophase"), true);


	// now for initializing all non-ini items
	for (int i=0; i<RAYCODENUM; i++) {
		// do not move this to CSeisWideDoc class!!!
		this->statMain.gRayColor[i] = 0;
		this->statMain.gRayCode[i] = 0.f;
	}
	// end of non-ini items


	psei->isInitialized = true;

}

void CMainFrame::OnHelpLicensestatus()
{
	CLicenseCheck ls;
	ls.getCurrentLicenseStat8(); // this will pupolate the needed parameters for license status
	CdlgHardwareId dlg;
	dlg.m_HardwareId = ls.getMACaddress();
	dlg.m_licenseDetectedInfo.Format(_T("Current license status: %s,  %d days left."), 
		ls.licenseEnum2Str(ls.licenseStruct.type),
		ls.licenseStruct.nDaysLeftover);
	if (dlg.DoModal() == IDOK) {
	}
}

void CMainFrame::OnUpdateFilePrint(CCmdUI *pCmdUI)
{
	if (this->m_licenseStruct.type == CLicenseCheck::demo  ||  this->m_licenseStruct.type == CLicenseCheck::regi) {
		pCmdUI->Enable(FALSE);
	}
}

void CMainFrame::OnUpdateFileExportbitmap(CCmdUI *pCmdUI)
{
	if (this->m_licenseStruct.type == CLicenseCheck::demo  ||  this->m_licenseStruct.type == CLicenseCheck::regi) 
		pCmdUI->Enable(FALSE);
}

void CMainFrame::OnHelpFinder()
{
	myShowHelp(_T("index.html"));

}

// since it is not easy to obtain the font size, I decide to design a special 
//   wrapper function to do this. Encouraged to use it more from now on
CSize CMainFrame::getFontNumSize(CDC *pDC, CCoords *pCd)
{

	LOGFONT logFont; // font for drawing labels such as Distance (km)
	pDC->GetCurrentFont()->GetLogFont(&logFont);
	double xyRatio = (double)logFont.lfWidth / (double)logFont.lfHeight;
	CSize logFontNumSize;
	logFontNumSize.cx = (LONG)myRound(this->statMain.fLabelDistance3 * pCd->coord.device2Logical * pCd->coord.getDefDisplayAdjust(pDC) * 7.);
	logFontNumSize.cy = (LONG)(logFontNumSize.cx / xyRatio);
	return logFontNumSize;

}

CString CMainFrame::getLogFileName()
{
	CString tStr;
	SYSTEMTIME systime;
	GetLocalTime(&systime);
	TCHAR szMyDoc[MAX_PATH];
	int n = GetEnvironmentVariable(_T("USERPROFILE"),
		szMyDoc,
		sizeof(szMyDoc) * sizeof(TCHAR));
	if (n != 0) {
		tStr.Format(_T("%s\\Documents\\Seiswide%i-%02i.log"), szMyDoc, systime.wYear, systime.wMonth);
	}
	return tStr;
}

void CMainFrame::myCaption(double f, CString prefix) {
	CString tStr;
	tStr.Format(_T("%s%g."), prefix, f);
	this->myCaption(tStr);

}
void CMainFrame::myCaption(CString text)
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

	if (statMain.m_strKeep.GetLength() > 0) {
		text.Format(_T("%s ; %s"), statMain.m_strKeep, text.GetString());
		statMain.m_strKeep = BLANK;
	}
	this->SetMessageText(text);
	this->m_wndStatusBar.Invalidate();
	this->m_wndStatusBar.UpdateWindow();


	// write to log file
	static CString newLine = CARRAGE;
	if (psei->pFile == NULL) {
		tStr = getLogFileName();
		if (!tStr.IsEmpty()) {
			// found the myDoc
			HANDLE hFile;
			if (isFileExistNotDirectory(tStr)) {
				hFile = CreateFile(tStr,
					GENERIC_READ | FILE_APPEND_DATA | GENERIC_WRITE,
					FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL);
			}
			else {
				hFile = CreateFile(tStr,
					GENERIC_READ | FILE_APPEND_DATA | GENERIC_WRITE,
					FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
					NULL,
					CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL,
					NULL);
			}
			psei->pFile = new CFile(hFile);
			if (psei->pFile)
				psei->pFile->SeekToEnd();
		}
	}
	if (psei->pFile) {
		// now write to the log
		SYSTEMTIME systime;
		GetLocalTime(&systime);
		tStr.Format(_T("%02i-%02i %02i:%02i:%02i "),
			systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
		psei->pFile->Write(tStr, tStr.GetLength()*sizeof(TCHAR));
		psei->pFile->Write(text, text.GetLength()*sizeof(TCHAR));
		psei->pFile->Write(newLine, 2*sizeof(TCHAR));
	}



}

void CMainFrame::myLogInfo(CString str)
{

	// write to log file
	if (psei->pFile) {
		// now write to the log
		SYSTEMTIME systime;
		GetLocalTime(&systime);
		CString tStr;
		tStr.Format(_T("%02i-%02i %02i:%02i:%02i [INFO]"),
			systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
		psei->pFile->Write(tStr, tStr.GetLength()*sizeof(TCHAR));
		psei->pFile->Write(str, str.GetLength()*sizeof(TCHAR));
		psei->pFile->Write(CARRAGE, 2 * sizeof(TCHAR));
	}

}

void CMainFrame::OnUpdateDigitizingLoadexisting(CCmdUI *pCmdUI)
{
  if (pCmdUI->m_pMenu)
	pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, ID_DIGITIZING_LOADEXISTING);
}


void CMainFrame::OnUpdateDigitizingMigratelinedrawings(CCmdUI *pCmdUI)
{
  if (pCmdUI->m_pMenu)
	pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, ID_DIGITIZING_MIGRATELINEDRAWINGS);
}


void CMainFrame::OnUpdateProcessingChangereducevelocityforbmp(CCmdUI *pCmdUI)
{
  if (pCmdUI->m_pMenu)
	pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, ID_PROCESSING_CHANGEREDUCEVELOCITYFORBMP);
}


void CMainFrame::OnUpdateProcessingCohmix(CCmdUI *pCmdUI)
{
  if (pCmdUI->m_pMenu)
	pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, ID_PROCESSING_COHMIX);
}


void CMainFrame::OnUpdateProcessingKdmigpost(CCmdUI *pCmdUI)
{
  if (pCmdUI->m_pMenu)
	pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, ID_PROCESSING_KDMIGPOST);
}


void CMainFrame::OnUpdateProcessingKdmigsimple(CCmdUI *pCmdUI)
{
  if (pCmdUI->m_pMenu)
	pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, ID_PROCESSING_KDMIGSIMPLE);
}


void CMainFrame::OnUpdateTimeForwideNmovelocity(CCmdUI *pCmdUI)
{
  if (pCmdUI->m_pMenu)
	pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, ID_TIME_FORWIDE_NMOVELOCITY);
}


void CMainFrame::OnUpdateTimeTimesectionwithperturbedtravel(CCmdUI *pCmdUI)
{
  if (pCmdUI->m_pMenu)
	pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, ID_TIME_TIMESECTIONWITHPERTURBEDTRAVEL);
}


void CMainFrame::OnUpdateViewStackingvelocitymap(CCmdUI *pCmdUI)
{
  if (pCmdUI->m_pMenu)
	pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, ID_VIEW_STACKINGVELOCITYMAP);
}


void CMainFrame::OnUpdateToolsMapconverttime(CCmdUI *pCmdUI)
{
  if (pCmdUI->m_pMenu)
	pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, ID_TOOLS_MAPCONVERTTIME);
}


void CMainFrame::OnUpdateTimeForwide(CCmdUI *pCmdUI)
{
  if (pCmdUI->m_pMenu) {
	//  pCmdUI->Enable(FALSE);
	pCmdUI->m_pMenu->DeleteMenu(pCmdUI->m_nID, ID_TIME_FORWIDE);
  }
}


int CMainFrame::Refresh()
{
	MSG       msg;
	if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		if ((msg.message == WM_QUIT)
			|| (msg.message == WM_CLOSE)
			|| (msg.message == WM_DESTROY)
			|| (msg.message == WM_NCDESTROY)
			|| (msg.message == WM_HSCROLL)
			|| (msg.message == WM_VSCROLL)
			)
			return(1);
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return(0);
}

void CMainFrame::OnViewUsagelog()
{
	CString tStr = getLogFileName();
	if (isFileExistNotDirectory(tStr)) {
		showFileInDefaultProg(tStr);
	}
}

// From  http://www.codeproject.com/Articles/1154/Saving-a-windows-size-position-and-state-in-MFC

CString CMainFrame::setProjectStringOLD() {
	CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();
	CRect r;
	GetWindowRect(&r);

	//	GetClientRect(&r);

	int count = 0;
	CString tStr;
	CString strOut;
	CMDIChildWnd* pChild = GetNextMDIChildWnd();
	while (pChild) {
		pSeiApp->statWin.winPosPathName[count] = pChild->GetActiveDocument()->GetPathName(); 
		CView* aView = pChild->GetActiveView();
		if (aView) {
			aView->GetWindowRect(&pSeiApp->statWin.winPosRect[count]); // lpRect is now in pixels inside the main frame
																	   //pChild->GetActiveView()->GetClientRect(&r); // lpRect is now in pixels inside the main frame
																	   // pChild->GetActiveView()->GetClientRect(&pSeiApp->statWin.winPosRect[count]); // lpRect is now in pixels inside the main frame
			tStr.Format(_T("%d %d %d %d  %s\r\n"),
				pSeiApp->statWin.winPosRect[count].left - r.left,
				pSeiApp->statWin.winPosRect[count].top - r.top,
				pSeiApp->statWin.winPosRect[count].right - r.left,
				pSeiApp->statWin.winPosRect[count].bottom - r.top,
				pSeiApp->statWin.winPosPathName[count]);
			strOut += tStr;
			count++;
			pChild = GetNextMDIChildWnd();
			if (count >= NUM_WINPOS) break;
		}
		else break;
	}

	tStr.Format(_T("%d %d %d %d  %s\r\n"), 0,0,0,0, pSeiApp->statWin.winProjectName);
	strOut += tStr;

	return strOut;
}

CString CMainFrame::getProjectString(int &count) {
	CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();
	CRect r;
	GetWindowRect(&r);
	//WINDOWPLACEMENT wp;
	//GetWindowPlacement(&wp);
	//r = wp.rcNormalPosition;

	count = 0;
	CString tStr;
	CString strOut;
	CMDIChildWnd* pChild = GetNextMDIChildWnd();
	while (pChild) {
		pSeiApp->statWin.winPosPathName[count] = pChild->GetActiveDocument()->GetPathName();
		pChild->ShowWindow(SW_RESTORE); // if a child window is minimized, we need to restore it before getting its sizing info
		CView* aView = pChild->GetActiveView();
		if (aView) {
			
			aView->GetWindowRect(&pSeiApp->statWin.winPosRect[count]); // lpRect is now in pixels inside the main frame
																	   //pChild->GetActiveView()->GetClientRect(&r); // lpRect is now in pixels inside the main frame
																	   // pChild->GetActiveView()->GetClientRect(&pSeiApp->statWin.winPosRect[count]); // lpRect is now in pixels inside the main frame
		//	aView->GetWindowPlacement(&wp);
			tStr.Format(_T("%d %d %d %d  %s\r\n"), 
				pSeiApp->statWin.winPosRect[count].left - r.left,
				pSeiApp->statWin.winPosRect[count].top - r.top,
				pSeiApp->statWin.winPosRect[count].right - r.left,
				pSeiApp->statWin.winPosRect[count].bottom - r.top,
				pSeiApp->statWin.winPosPathName[count]);
			strOut += tStr;
			count++;
			pChild = GetNextMDIChildWnd();
			if (count >= NUM_WINPOS) break;
		}
		else break;
	}

	tStr.Format(_T("%d %d %d %d  %s\r\n"), 0, 0, 0, 0, pSeiApp->statWin.winProjectName);
	strOut += tStr;

	// we only write the project str to registry when there are MDI child windows on display

	return strOut;
}

void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	// CWinApp* pApp = AfxGetApp();
	CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();

	WINDOWPLACEMENT wp;
	BOOL isTrue = GetWindowPlacement(&wp);
	if (isTrue && 
		pSeiApp->WriteProfileBinary(_T("Chian"), _T("WP"), (LPBYTE)&wp, sizeof(wp))
		) {
	}

	int count = 0;
	CString strOut = this->getProjectString(count);
	pSeiApp->WriteProfileString(_T("Chian"), _T("WPChild"), strOut);

	CMDIFrameWnd::OnClose();
}

// After parsing the proj strings, it will populate to:
// pSeiApp->statWin.winProjectName
// pSeiApp->statWin.winPosCount
// pSeiApp->statWin.winPosRect[]
// pSeiApp->statWin.winPosPathName[]
bool CMainFrame::parseProjectString(CString strIn) {
	if (!strIn.IsEmpty()) {
		// now need to parse strIn
		CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();
		CStringArray arr;
		SplitStringArr(strIn, CARRAGE, &arr, true);
		// int n = arr.GetCount();

		pSeiApp->statWin.winPosCount = 0;
		for (int i = 0; i < arr.GetCount(); i++) {
			CString tStr = arr.ElementAt(i);
			if (tStr.IsEmpty()) break;
			int j = tStr.Find(_T("  "));
			CStringArray *arr2 = CString2LinesBySpace(tStr.Left(j));
			if (arr2->GetCount() == 4) {
				// only 4 is the correct number here
				pSeiApp->statWin.winPosRect[i].left = (LONG)_ttoi(arr2->ElementAt(0));
				pSeiApp->statWin.winPosRect[i].top = (LONG)_ttoi(arr2->ElementAt(1));
				pSeiApp->statWin.winPosRect[i].right = (LONG)_ttoi(arr2->ElementAt(2));
				pSeiApp->statWin.winPosRect[i].bottom = (LONG)_ttoi(arr2->ElementAt(3));
			}
			arr2->RemoveAll();
			delete arr2;

			if (pSeiApp->statWin.winPosRect[i] != CRect(0, 0, 0, 0)) {
				pSeiApp->statWin.winPosPathName[i] = tStr.Mid(j).Trim();
				pSeiApp->statWin.winPosCount++;
			}
			else {
				// the last string to parse
				tStr = arr.ElementAt(arr.GetCount()-1);
				int j = tStr.ReverseFind('\r');
				if (j > 0)
					tStr = tStr.Mid(j + 1).Trim();

				// need to strip all 
				CString strStrip = _T("0 0 0 0");
				while ( (j = tStr.Find(strStrip)) == 0) {
					tStr = tStr.Mid(strStrip.GetLength()).Trim();
				}

				if (tStr.GetLength() < 1)
					tStr = _T("Untitled.seiProj");
				if (!isFileExistNotDirectory(tStr)) {
					// since the project file does not exist, its folder might have changed, 
					// so we can just remove its path and leave the name untouched
					tStr = getFileNameOnly(tStr);
				}
				pSeiApp->statWin.winProjectName = tStr; // this line is needed when Seiswide start up

				break;
			}
			if (pSeiApp->statWin.winPosCount >= NUM_WINPOS) break;
		}
		arr.RemoveAll();

		// remove some invalid names. This part is wrong
		//for (int i = pSeiApp->statWin.winPosCount - 1; i >= 0; i--) {
		//	if (!isFileExistNotDirectory(pSeiApp->statWin.winPosPathName[i]))
		//		pSeiApp->statWin.winPosCount--;
		//}
	}
	return true;
}


void CMainFrame::myFileOpenproject(CString fSeiProject) {
	CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();

	CString fProjStr = getTextFromFileUnix(fSeiProject);

	if (this->parseProjectString(fProjStr))
		pSeiApp->statWin.winProjectName = fSeiProject; // the project name might have changed


	for (int i = pSeiApp->statWin.winPosCount - 1; i >= 0; i--) {
		register CString f = pSeiApp->statWin.winPosPathName[i];
		myMakeSameDrive(f, fSeiProject);

		if (!f.IsEmpty() && !isFileExistNotDirectory(f)) {
			// since the .SEI file does not exist, we try to see if its folder is changed, and try to adjust
			f = getSecNameRevised(f, fSeiProject);
		}

		if (!f.IsEmpty() && isFileExistNotDirectory(f)) {
			pSeiApp->statWin.winPosCurrent = i; // tell ChildFrm.cpp the current index for PreCreateWindow()
			pSeiApp->OpenDocumentFile(f);
		}
	}
	this->SetTitle(pSeiApp->statWin.winProjectName);
}


void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CMDIFrameWnd::OnShowWindow(bShow, nStatus);

	CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();
	static bool bOnce = true;
	
	if(bShow && !IsWindowVisible() && bOnce) {
        bOnce = false;
	
        WINDOWPLACEMENT *lwp;
        UINT nl;
		if (pSeiApp->GetProfileBinary(_T("Chian"), _T("WP"), (LPBYTE*)&lwp, &nl)) {
            SetWindowPlacement(lwp);
            delete [] lwp;
        }

		pSeiApp->statWin.winPosCount = 0;
		CString strIn = pSeiApp->GetProfileString(_T("Chian"), _T("WPChild"));
		parseProjectString(strIn);
    }
}


void CMainFrame::OnUpdateIndicatorThreshold(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
	CString strPage;
	strPage.Format(_T("%4s"), statMain.gAmpThreshold);
	pCmdUI->SetText(strPage);
}





// I added an extra menu for tiling vertically
void CMainFrame::OnWindowTilevertical()
{
	PostMessage(WM_COMMAND, ID_WINDOW_TILE_VERT);
}





void CMainFrame::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CMDIFrameWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CMainFrame::OnUpdateImportBitmap2segy(CCmdUI *pCmdUI)
{
	if (this->m_licenseStruct.type == CLicenseCheck::demo || this->m_licenseStruct.type == CLicenseCheck::regi) {
		pCmdUI->Enable(FALSE);
	}
}


void CMainFrame::OnUpdateDigitizeEndcurvestosegy(CCmdUI *pCmdUI)
{
	if (this->m_licenseStruct.type == CLicenseCheck::demo || this->m_licenseStruct.type == CLicenseCheck::regi) {
		pCmdUI->Enable(FALSE);
	}

}


void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	// TODO: Add your specialized code here and/or call the base class
	if (bAddToTitle) {
		CSeisWideApp* pSeiApp = (CSeisWideApp*)AfxGetApp();
		if (m_strTitle.Find(pSeiApp->statWin.winProjectName) < 0) {
			// before appending .seiProj filename, we need to strip off any existing .seiProj inside it
			int i = m_strTitle.Find(_T(" -- "));
			if (i >= 0)
				m_strTitle.Format(_T("%s"), m_strTitle.Mid(0, i));
			m_strTitle.Format(_T("%s -- %s"), m_strTitle, pSeiApp->statWin.winProjectName);
		}
	}

	CMDIFrameWnd::OnUpdateFrameTitle(bAddToTitle);
}




void CMainFrame::OnFileOpenproject()
{
	CString fSeiProject;
	CFileDialog dlg(TRUE, NULL, _T("*.seiProj"), OFN_EXPLORER, _T("Seiswide Project File name|*.seiProj"));
	//	dlg.m_ofn.lpstrTitle = _T("Each shot number in one line please";
	if (dlg.DoModal() == IDOK)
		fSeiProject = dlg.GetPathName();
	else
		return;
	this->myFileOpenproject(fSeiProject);
}
