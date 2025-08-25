// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////
#include "licenseCheck.h"
#include "coords.h"
#include "SegyWrapper.h"
#include "SegyWrapperEx.h"
#include "CMyToolbar.h"
#define RAYCODENUM 31
#define NUM_CACHE 10

// http://forums.codeguru.com/showthread.php?531501-RESOLVED-MDI-quot-More-Windows-quot-dialog&p=2097709#post2097709
//#define AFX_IDM_FIRST_MDICHILD 0xF000  // window list starts here

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
//	CString myXY;
	CMainFrame();

// Attributes
public:
	//Deping's additions for controlling child windows.
   CWnd  m_wndMDIClient;
   CWnd* m_pWndCurrentChild;
//   CWnd* m_pWndCurrentChildOld; // addition Aug 2010: point to the previous window

   // these paras are shared by all DOCs. They cannot be put to CSeisWide.h
	struct SEI_PAR{
		int restoreLevelMax;   // added June 14, 2010
		bool isInitialized;
		bool isLabelLessDecimal; // if true, labels such as 7.50 will be printed as 7.5
			// to be implemented, May 2013
		int anisotropy_layer0;  // when applying anisotropy in velocity mode, do we interpolate from a layer number?   
			// -1 means no interpolation. 1=interpolate from layer 1 downward.

		float freq1s, freq2s, freq3s, freq4s;  // freq filtering ranges
		float ta;  // time varying filter
		float f1a, f2a; // time varying filter at ta
		float tb;  // time varying filter
		float f1b, f2b; // time varying filter at tb

		int num_tfilter; // num of times for a time filter
		bool yesFilterT;
		bool yesFilterF; // true=frequency filter
		float normValue;
		float velWater;
		bool zerophase;
		float sxLong;
		float syLat;
		double regionalModelA;
		double regionalModelB;
		double regionalModelV0;
		double regionalModelVinf;
		double regionalModelAlpha;
		double regionalSedVmin;
		double regionalSedVmax;

		// the following are shared among various CDocument but not saved in .ini file
		// #################################
		// the following 2 lines are for storing a consistent ray color scheme globally across different documents.
		// end#################################


		// for segy caching only
		SegyWrapper *pCacheSegyWraps[NUM_CACHE];
		SegyWrapperEx *pCacheSegyWrapEx[NUM_CACHE];


		CFile *pFile;
		SEI_PAR() :
			regionalSedVmin(1.52), regionalSedVmax(4.3),
			isLabelLessDecimal(false), isInitialized(false), anisotropy_layer0(-1)
		{ 
			for (int i = 0; i < NUM_CACHE; i++)
				pCacheSegyWraps[i] = new SegyWrapper();
			for (int i = 0; i < NUM_CACHE; i++)
				pCacheSegyWrapEx[i] = new SegyWrapperEx(_T(""));
			pFile = NULL;  // will need to initialize before usage
			restoreLevelMax = 10;   // added June 14, 2010
		};

		~SEI_PAR() {
			if (pFile != NULL) {
				pFile->Close();
				delete pFile;
				pFile = NULL;
			}
		}
	};
	SEI_PAR *psei;
	CLicenseCheck::LICENSE_STRUCT m_licenseStruct;

   CMDIChildWnd* GetNextMDIChildWnd();
   int GetCountCMDIChildWnds();
   BOOL HideOtherChild();
   BOOL ShowOtherChild();
// Operations
public:
	void CMainFrame::SetAppName(LPCTSTR Title);
	void BeginProgress();
	void SetProgress(int nProg);
	void EndProgress();

	
	// note: CMainFrame can access SeisWideApp, but not the other way round. This is why some params are put in SeisWideApp
	// but some are put to here.
	struct STAT_MAIN{
		bool m_isXInt;
		bool m_isYInt;
		float m_nX, m_nZ, m_nV;  // calculated in CView OnMouseMove(), for status control
		int m_nN;

		BOOL pick;		//Pick boundary or velocity layer.
		BOOL remove;		//Remove a node point.
		BOOL move;		//Move node points.
		BOOL add;		//Add a node point.
		int bigNodes; // 1,2,3     with 3 being the biggest node.  Should move to CMainFrame

		BOOL isStretchMode;  // for bitmap images ohly
		BOOL isDisplayVsModel; // toggle display S velocity in model;  // if 1=show Vs info in model; otherwise not showing Vs info
		BOOL isShowDetails;  // indicate if details of model is needed, not saved

		BOOL isSwapXActive; // TRUE only if one CDocument is swapped in X axis. 
		// why we need this? It is because when one window is swapped, the other window is not swapped, 
		// and then we want its Y-lables on the right side.
		// To make it simple, if one CDoc is swapped, and it becomes not swapped, 
				//  then statMain->isSwapXActive will not be swapped.  But what about if 2 CDocs are swapped, and I close one CDoc? This would make
				// statMain->isSwapXActive==FALSE !!! This is ok since all other CDoc will be un-aware of the left-over CDoc's swapping, and No problem.
		BOOL isClipChangeWithGain;
		//BOOL isControlPressed;

		CString gAmpThreshold;  // should be kept with 4 chars length
		CString m_strKeep;

		// this will be depreciated at some point, maybe right now?
		float fLabelDistance3;  // set this default to 1
		float thicknessLines;  // default=1; 2=double thickness; 0.5=half thick

		COLORREF gRayColor[RAYCODENUM];
		float gRayCode[RAYCODENUM];
		HCURSOR gCursor[6];

		// this routine converts the 4 states into a variable for easy handling.
		inline int getPtdraw() {
			if (this->pick) return 0;
			else if (this->move) return 1;
			else if (this->add) return 2;
			else if (this->remove) return 3;
			else return -1;
		}

		// this routine converts the single stats into 4 variables.
		inline void setPtdraw(int statNew) {
			if (statNew < 0) {
				this->pick = this->remove = this->move = this->add = FALSE;
			}
			else if (statNew == 0) {
				this->remove = this->move = this->add = FALSE;
				this->pick = TRUE;
			}
			else if (statNew == 1) {
				this->pick = this->remove = this->add = FALSE;
				this->move = TRUE;
			}
			else if (statNew == 2) {
				this->pick = this->remove = this->move = FALSE;
				this->add = TRUE;
			}
			else if (statNew == 3) {
				this->pick = this->add = this->move = FALSE;
				this->remove = TRUE;
			}
			else if (statNew == 4) {
				this->remove = this->move = this->add = FALSE;
				this->pick = TRUE; // when using the right mouse, I do not want to have -1 mode
			}
			else {
				this->pick = this->remove = this->move = this->add = FALSE;
			}
		}

		inline void myCursorsAll(bool isBusy, int drawmode, BOOL isMouseHand, BOOL isShift) {
			if (isBusy) {
				SetCursor(this->gCursor[5]);
			}
			else if (drawmode >= 0) {
				if (isShift || isMouseHand) SetCursor(this->gCursor[4]);
				else if (this->move) SetCursor(this->gCursor[0]);
				else if (this->remove) SetCursor(this->gCursor[1]);
				else if (this->add) SetCursor(this->gCursor[2]);
				else SetCursor(this->gCursor[3]);  // SetCursor(LoadCursor(NULL,IDC_ARROW));
			}
		}

		inline void myCursor(bool isBusy) {
			if (isBusy)
				SetCursor(gCursor[5]);
			else
				SetCursor(gCursor[3]);  // SetCursor(LoadCursor(NULL,IDC_ARROW));
		}

		STAT_MAIN() :
			m_isXInt(false), m_isYInt(false) ,
			m_nX(0.f), m_nZ(0.f), m_nV(-99.f),
			bigNodes(1), move(0), pick(0), add(0), remove(0)
			, isDisplayVsModel(FALSE)
			, fLabelDistance3(1.f)
			, thicknessLines(1)
//			, gIsIBMFloatSegy2(TRUE)
			, isShowDetails(TRUE)
			, isStretchMode(TRUE)
			, isSwapXActive(FALSE)
			, isClipChangeWithGain(FALSE)
			, gAmpThreshold(BLANK)
			, m_strKeep(BLANK)
		{
			this->gCursor[0] = LoadCursor(NULL, IDC_SIZEALL);
			this->gCursor[1] = LoadCursor(NULL, IDC_HELP);
			this->gCursor[2] = LoadCursor(NULL, IDC_CROSS);
			this->gCursor[3] = LoadCursor(NULL, IDC_ARROW);
			this->gCursor[4] = LoadCursor(NULL, IDC_HAND);
			this->gCursor[5] = LoadCursor(NULL, IDC_WAIT);

		};
	} statMain;


	struct RAYSKEPT {
		double xRaysKeptMin[2], xRaysKeptMax[2];
		double xRaysOffsetMax;
		CTime modifiedTime;
		CString xRayFileName;
		int nRaysControl;
		bool isEnabled = false;

		inline void getRaysKept(CString fileFolder) {
			fileFolder = GetPathFromFileSlash(fileFolder) + _T("_raysKept.txt");
			if (!isFileExistNotDirectory(fileFolder)) {
				// this may happen if the _raysKept struct is still valid but the current folder is changed to another place.
				// It may be temporary change so we want to keep the struct but mark it as inactive
				this->isEnabled = false;
				return;
			}

			this->isEnabled = true; // now the _raysKept.txt exist so we know it should be enabled for sure

			CTime timeThis = getFileModifiedTime(fileFolder);
			if (timeThis != this->modifiedTime || xRayFileName.CompareNoCase(fileFolder) != 0) {
				CStringArray* arr = getLinesFromFile(fileFolder, false); // no comments will be read in here
				if (arr) {
					int nRaysControlThis = (int)arr->GetSize();
					this->xRaysOffsetMax = ERRORVAL; // we must reset it first
					this->nRaysControl = 0; // reset it first
					if (arr == NULL || nRaysControlThis < 1) {
						// nothing in this file can be loaded
					}
					else {
						double t1, t2;
						int aInd = 0;
						for (int i = 0; i < nRaysControlThis; i++) {
							_stscanf_s(arr->ElementAt(i).Trim(), _T("%lg%lg"), &t1, &t2);
							if (t2 > ERRORVAL) {
								this->xRaysKeptMin[aInd] = t1;
								this->xRaysKeptMax[aInd] = t2;
								aInd++;
							}
							else {
								this->xRaysOffsetMax = t1;
							}
							if (aInd >= 2) break; // no more than 2 data lines allowed
						}
						this->nRaysControl = aInd;
					} // else
					if (arr) {
						arr->RemoveAll();
						delete arr;
					}
					this->modifiedTime = timeThis;
					this->xRayFileName = fileFolder;
				}
			}
			else {
				// the loaded values are OK to use
			}
		}
		inline bool isXInsideKeepZones(double x) {
			if (!this->isEnabled) return true;

			return this->nRaysControl < 1
				|| isInsideDouble(x, this->xRaysKeptMin[0], this->xRaysKeptMax[0])
				|| isInsideDouble(x, this->xRaysKeptMin[1], this->xRaysKeptMax[1]);
		}
		inline bool isXInsideKeepZones(double x, double xSrc) {
			// 
			if (!this->isEnabled) return true;
			if (this->xRaysOffsetMax < 0.)
				return true; // no control for offsets

			double offset = x - xSrc;
			if (fabs(offset) > this->xRaysOffsetMax)
				return false;

			return this->isXInsideKeepZones(x);
		}
		RAYSKEPT() : nRaysControl(0), modifiedTime(0), xRaysOffsetMax(99999.) {};
	};

	// to maintain which is the current and which is the previous active documents, we need 2 pointers
	CView * pViewPrev;
	CView * pViewCurr;
	//CDocument * pDocCurr;
	//CDocument * pDocPrev;  
	// later, I decided to only store CView instances, 
	// because if you know CDoc, you do not know its CView. But if you know CView, you know its CDoc instance.


	// since it is not easy to obtain the font size, I decide to design a special 
	//   wrapper function to do this. Encouraged to use it more from now on
	CSize getFontNumSize(CDC *pDC, CCoords *pCd);
	//void myCursors(bool isBusy);
	void myCaption(CString text);
	void myCaption(double f, CString prefix);

//	CString m_strKeep;
	inline void myCaptionKeep(CString in) {
		// the same text will be kept twice
		myCaption(in);
		this->statMain.m_strKeep = in;
	}

	void myLogInfo(CString str);
	CString getLogFileName();

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:

	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	// BOOL SetCursorDig(BOOL trueFalse);
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CMyToolbar    m_wndToolBar;
	CSliderCtrl m_slider1Threshold;
//	float m_upDownPower;
	CEdit m_buddyEdit;
	CSpinButtonCtrl m_spinCtrl;
	RAYSKEPT raysKept;

protected:  // control bar embedded members
	CProgressCtrl m_ctlProgress;
	CString strText; //used only in OnUpdateText().

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnInitMenu(CMenu* pMenu);
	//}}AFX_MSG
	afx_msg void OnUpdateText(CCmdUI *pCmdUI);

	DECLARE_MESSAGE_MAP()
public:
	CStatusBar  m_wndStatusBar;
protected:
public:
	//afx_msg void OnOptionsToggleibmfloatingmode();
	//afx_msg void OnUpdateOptionsToggleibmfloatingmode(CCmdUI *pCmdUI);
	CDocument * GetActiveDocMy(void);
	CView * GetActiveViewMy(void);
	CString setProjectStringOLD();
	CString getProjectString(int& count);
	bool parseProjectString(CString strIn);
	void myFileOpenproject(CString fSeiProject);
protected:
	void getSeisWideIni(void);
	int Refresh();

public:
	afx_msg void OnUpdateFilePrint(CCmdUI *pCmdUI);
	afx_msg void OnHelpLicensestatus();
	afx_msg void OnUpdateFileExportbitmap(CCmdUI *pCmdUI);
	afx_msg void OnHelpFinder();
	afx_msg void OnUpdateDigitizingLoadexisting(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDigitizingMigratelinedrawings(CCmdUI *pCmdUI);
	afx_msg void OnUpdateProcessingChangereducevelocityforbmp(CCmdUI *pCmdUI);
	afx_msg void OnUpdateProcessingCohmix(CCmdUI *pCmdUI);
	afx_msg void OnUpdateProcessingKdmigpost(CCmdUI *pCmdUI);
	afx_msg void OnUpdateProcessingKdmigsimple(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTimeForwideNmovelocity(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTimeTimesectionwithperturbedtravel(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewStackingvelocitymap(CCmdUI *pCmdUI);
	afx_msg void OnUpdateToolsMapconverttime(CCmdUI *pCmdUI);
	afx_msg void OnUpdateTimeForwide(CCmdUI *pCmdUI);
	afx_msg void OnViewUsagelog();

	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnUpdateIndicatorThreshold(CCmdUI *pCmdUI);


	afx_msg void OnWindowTilevertical();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnUpdateImportBitmap2segy(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDigitizeEndcurvestosegy(CCmdUI *pCmdUI);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

	afx_msg void OnFileOpenproject();
};

/////////////////////////////////////////////////////////////////////////////
