// SeisWide.h : main header file for the SEISWIDE application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#define NUM_WINPOS 100

#include "resource.h"       // main symbols
#include "SeisWide_i.h"

/////////////////////////////////////////////////////////////////////////////
// CSeisWideApp:
// See SeisWide.cpp for the implementation of this class
//

#define KEY_DOWN 0x8000 
class CSeisWideApp : public CWinApp
{
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CSeisWideApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSeisWideApp)
	public:
	virtual BOOL InitInstance();
		virtual int ExitInstance();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	//}}AFX_VIRTUAL

// Implementation
	COleTemplateServer m_server;
		// Server object for document creation

	//{{AFX_MSG(CSeisWideApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	// BOOL CheckTime();
private:
	void ShowTipAtStartup(void);
private:
	BOOL m_bATLInited;
	BOOL InitATL();
	void ShowTipOfTheDay(void);

public:
	struct STAT_WIN{
		CRect winPosRect[NUM_WINPOS];  // sizing info for the current .SEI
		CString winPosPathName[NUM_WINPOS]; // .SEI file name with complete path
		int winPosCount;  // total number of child windows we have. It is smaller than or equal ro NUM_WINPOS
		int winPosCurrent; // currently processing this index of child window.
		CString winProjectName;
		inline BOOL getControlPressed() {
			int cntlValue = GetKeyState(VK_CONTROL);
			if (cntlValue & KEY_DOWN)
				return TRUE;
			return FALSE; // note: there are other states
		}
		//	BOOL isControlPressed;
		//	BOOL isShiftPressed;
		inline BOOL getShiftPressed() {
			int cntlValue = GetKeyState(VK_SHIFT);
			if (cntlValue & KEY_DOWN)
				return TRUE;
			return FALSE; // note: there are other states
		}

		BOOL isAPressed;
		CRect rectWinCurrent;
		CString nameWinCurrent;

		// for deciding if a traveltime pick (or a raypath) is to keep or not
		// this can be best be done through clipboard instead of files.
		double xPrev = -99999.;
		double xSkipIntervalAbs = 0.;
		inline bool isXKeep(double x) {
			double diff = fabs(this->xPrev - x);
			if (diff > this->xSkipIntervalAbs) {
				this->xPrev = x;
				return true;
			}

			return false;
		}


		STAT_WIN() :
			winPosCount(0)
			, winPosCurrent(0)
		//	, isControlPressed(FALSE)
		//	, isShiftPressed(FALSE)
			, isAPressed(FALSE)
		{
			for (int i = 0; i < NUM_WINPOS; i++)
				winPosPathName[i] = _T("");
		};
	} statWin;

	void OnFileNew();  // this makes the OnFileNew() accessible by other classes such as CSeisWideDoc.

	CDocument* myNewDocument(void);
	CDocument* myNewDocument(CString f);


};


/////////////////////////////////////////////////////////////////////////////
