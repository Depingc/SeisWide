// SeisWide.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SeisWide.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "IpFrame.h"
#include "SeisWideDoc.h"
#include "SeisWideView.h"
#include "Splash.h"
#include <initguid.h>
#include "SeisWide_i.c"
#include "util.h"
#include "math.h"
#include "AutoUpdater.h"


//{{AFX_INCLUDES()
//}}AFX_INCLUDES

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSeisWideApp

BEGIN_MESSAGE_MAP(CSeisWideApp, CWinApp)
	ON_COMMAND(CG_IDS_TIPOFTHEDAY, ShowTipOfTheDay)
	//{{AFX_MSG_MAP(CSeisWideApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)

	// ON_COMMAND(ID_FILE_NEW, &CSeisWideApp::OnFileNew)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSeisWideApp construction

CSeisWideApp::CSeisWideApp()
{
	// TODO: add construction code here,

	// Place all significant initialization in InitInstance
	//	_set_se_translator(SeTranslator);
	//AfxOleGetMessageFilter()->EnableBusyDialog(FALSE);  // ???? added oct. 19, 2015, really need it?
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSeisWideApp object
CSeisWideApp theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {208EA380-6429-11D0-8DB1-8DEFF51D4A57}
static const CLSID clsid =
{ 0x208ea380, 0x6429, 0x11d0, { 0x8d, 0xb1, 0x8d, 0xef, 0xf5, 0x1d, 0x4a, 0x57 } };



/////////////////////////////////////////////////////////////////////////////
// CSeisWideApp initialization

BOOL CSeisWideApp::InitInstance()
{

	if (!InitATL())
		return FALSE;

	// CG: The following block was added by the Splash Screen component.
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		//	CSplashWnd::EnableSplashScreen(cmdInfo.m_bShowSplash);
	}
	//	CheckTime(); //check time any more in start up, but do not display messages.
	// note: this will write timing info for use by SeisWideDoc().

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// these are no longer needed, according to compilor's warning 
	//#ifdef _AFXDLL
	////	Enable3dControls();			// Call this when using MFC in a shared DLL
	//#else
	//	Enable3dControlsStatic();	// Call this when linking to MFC statically
	//#endif

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_SEISWITYPE,
		RUNTIME_CLASS(CSeisWideDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CSeisWideView));
	pDocTemplate->SetContainerInfo(IDR_SEISWITYPE_CNTR_IP);
	pDocTemplate->SetServerInfo(
		IDR_SEISWITYPE_SRVR_EMB, IDR_SEISWITYPE_SRVR_IP,
		RUNTIME_CLASS(CInPlaceFrame));
	AddDocTemplate(pDocTemplate);

	// Connect the COleTemplateServer to the document template.
	//  The COleTemplateServer creates new documents on behalf
	//  of requesting OLE containers by using information
	//  specified in the document template.
	m_server.ConnectTemplate(clsid, pDocTemplate, FALSE);

	// Register all OLE server factories as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleTemplateServer::RegisterAll();
	_Module.RegisterClassObjects(CLSCTX_LOCAL_SERVER, 
		REGCLS_MULTIPLEUSE);

	// Note: MDI applications register all server objects without regard
	//  to the /Embedding or /Automation on the command line.

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Check to see if launched as OLE server
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Application was run with /Embedding or /Automation.  Don't show the
		//  main window in this case.
		return TRUE;
	}


	SetRegistryKey(_T("Chian"));

	// When a server application is launched stand-alone, it is a good idea
	//  to update the system registry in case it has been damaged.
	m_server.UpdateRegistry(OAT_INPLACE_SERVER);

	_Module.UpdateRegistryFromResource(IDR_SEISWIDE, TRUE);
	_Module.RegisterServer(TRUE);


	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	//	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->ShowWindow(SW_SHOWNORMAL);

	// CG: This line inserted by 'Tip of the Day' component.
	// ShowTipAtStartup();
	for (int i = this->statWin.winPosCount - 1; i >= 0; i--) {
		register CString f = this->statWin.winPosPathName[i];
		if (!f.IsEmpty() && isFileExistNotDirectory(f)) {
			this->statWin.winPosCurrent = i; // tell ChildFrm.cpp the current index for PreCreateWindow()
			OpenDocumentFile(f);
		}
	}

	pMainFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


	// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:

	afx_msg void OnBnClickedBtnHomepage();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP

	ON_BN_CLICKED(IDC_BTN_HOMEPAGE, &CAboutDlg::OnBnClickedBtnHomepage)
END_MESSAGE_MAP()

// App command to run the dialog
void CSeisWideApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


//########## this makes the OnFileNew() accessible by other classes such as CSeisWideDoc.
// CSeisWideApp* pApp = (CSeisWideApp *)AfxGetApp();
// if (pApp) pApp->OnFileNew();
void CSeisWideApp::OnFileNew()
{
	CWinApp::OnFileNew();
}
//##########


/////////////////////////////////////////////////////////////////////////////
// CSeisWideApp commands

BOOL CSeisWideApp::PreTranslateMessage(MSG* pMsg)
{
	// CG: The following lines were added by the Splash Screen component.
	//	if (CSplashWnd::PreTranslateAppMessage(pMsg))
	//		return TRUE;

	return CWinApp::PreTranslateMessage(pMsg);
}

void CSeisWideApp::ShowTipAtStartup(void)
{
	// CG: This function added by 'Tip of the Day' component.

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if (cmdInfo.m_bShowSplash)
	{
		// we disable the showSplash option
		//CTipDlg dlg;
		//if (dlg.m_bStartup)
		//	dlg.DoModal();
	}

}

void CSeisWideApp::ShowTipOfTheDay(void)
{
	// CG: This function added by 'Tip of the Day' component.

	CTipDlg dlg;
	dlg.DoModal();

}


CSeisWideModule _Module;


LONG CSeisWideModule::Unlock()
{
	AfxOleUnlockApp();
	return 0;
}

LONG CSeisWideModule::Lock()
{
	AfxOleLockApp();
	return 1;
}


int CSeisWideApp::ExitInstance()
{
	if (m_bATLInited)
	{
		_Module.RevokeClassObjects();
		_Module.Term();
	}

	return CWinApp::ExitInstance();

}

BOOL CSeisWideApp::InitATL()
{
	m_bATLInited = TRUE;

	//_Module.Init(ObjectMap, AfxGetInstanceHandle());
	_Module.dwThreadID = GetCurrentThreadId();

	return TRUE;

}


CDocument* CSeisWideApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (lpszFileName == NULL) return NULL;

	CString fNameSei(lpszFileName);
	if (!isFileExistNotDirectory(fNameSei)) return NULL;

	CMainFrame* pFrame = (CMainFrame*)AfxGetMainWnd();

	CString t = GetFileExtension(fNameSei).MakeLower();
	if (t.CompareNoCase(_T("sei")) != 0) {
		// a new sgy/txt/in/jpg file is to be opened. We need to open a new .sei file for it




		CSeisWideDoc* pDoc_to_check = (CSeisWideDoc *)myNewDocument();



		//this->OnFileNew();
		//POSITION template_pos = this->GetFirstDocTemplatePosition();
		//while(NULL!=template_pos) {
		//	CDocTemplate* pDocTemplate = this->GetNextDocTemplate(template_pos);
		//	if(NULL!=pDocTemplate) {
		//		POSITION document_pos = pDocTemplate->GetFirstDocPosition();
		//		while(NULL!=document_pos) {
		//			pDoc_to_check = (CSeisWideDoc*)pDocTemplate->GetNextDoc(document_pos);
		//		}
		//	}		
		//}

		if (pDoc_to_check != NULL) {
			if (!pDoc_to_check->myOpenNewFile(fNameSei)) {
				pFrame->myCaption(_T("Sorry, failed to recognize the format for SeisWide."));
				// pDoc_to_check->close_unused_documents(pDoc_to_check);
			}
		}
		return NULL;
		// do not do the normal file-opening procesure, since this file is already handled 
		// in the OnFileNew() and its display parameters are already properly propogated
	}

	// if here, the dropped in file has .SEI extension. We need to open it directly
	return CWinApp::OpenDocumentFile(lpszFileName);
}



CDocument* CSeisWideApp::myNewDocument(void)
{
	this->OnFileNew();

		CDocument* pDoc_to_check;
		POSITION template_pos = this->GetFirstDocTemplatePosition();
		while(NULL!=template_pos) {
			CDocTemplate* pDocTemplate = this->GetNextDocTemplate(template_pos);
			if(NULL!=pDocTemplate) {
				POSITION document_pos = pDocTemplate->GetFirstDocPosition();
				while(NULL!=document_pos) {
					pDoc_to_check = (CDocument*)pDocTemplate->GetNextDoc(document_pos);
				}
			}		
		}

		return pDoc_to_check;
}

CDocument* CSeisWideApp::myNewDocument(CString f)
{
	CSeisWideDoc* pDoc_to_check = (CSeisWideDoc *)this->myNewDocument();
	if (pDoc_to_check != NULL) {
		pDoc_to_check->myOpenNewFile(f);
		// pDoc_to_check->close_unused_documents(pDoc_to_check);
	}
	return pDoc_to_check;
}


void CAboutDlg::OnBnClickedBtnHomepage()
{
	// TODO: Add your control notification handler code here
	CAutoUpdater updater;
	CString serverAddr = _T("http://globalrescue.hopto.org/seiswide/");
//	CString serverAddr2 = _T("http://xiuxian.no-ip.info/seiswide/");
	CString strResult = _T("The application was updated and will be activated the next time you run it.");

	CAutoUpdater::ErrorType  t = updater.CheckForUpdate(serverAddr);

	if (CAutoUpdater::Success == t) {
		MessageBox(strResult, 
			_T("Updater"), 
			MB_ICONINFORMATION|MB_OK);
	}
	//else if (CAutoUpdater::FileDownloadFailure == t) {
	//	t = updater.CheckForUpdate(serverAddr2);
	//	if (CAutoUpdater::Success == t) {
	//		MessageBox(strResult, 
	//			_T("Updater"), 
	//			MB_ICONINFORMATION|MB_OK);
	//	}
	//}

	if (CAutoUpdater::UpdateNotRequired == t)
		AfxMessageBox(_T("You already have the newest version of SeisWide."), MB_ICONQUESTION);
}




