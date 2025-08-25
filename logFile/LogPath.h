// LogPath.h : main header file for the LOGPATH application
//

#if !defined(AFX_LOGPATH_H__04ABC130_428E_4DB6_9E00_BFC4ABBAC805__INCLUDED_)
#define AFX_LOGPATH_H__04ABC130_428E_4DB6_9E00_BFC4ABBAC805__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLogPathApp:
// See LogPath.cpp for the implementation of this class
//

class CLogPathApp : public CWinApp
{
public:
	CLogPathApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogPathApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLogPathApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGPATH_H__04ABC130_428E_4DB6_9E00_BFC4ABBAC805__INCLUDED_)
