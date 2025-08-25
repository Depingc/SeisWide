// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#ifndef WINVER 
#define WINVER  0x0601
#endif



#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes
#include <afxodlgs.h>       // MFC OLE dialog classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include "TipDlg.h"
// #include <G:\softSomeVersions\seisWide.net\work3\TipDlg.h>

	#define _ATL_APARTMENT_THREADED
#include <atlbase.h>
#include <afxdhtml.h>
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
class CSeisWideModule : public CComModule
{
public:
	LONG Unlock();
	LONG Lock();
	DWORD dwThreadID;
};
extern CSeisWideModule _Module;
//#include <atlcom.h>
//#include <afxcontrolbars.h>

// OpenGL headers
//#include "gl\gl.h"
//#include "gl\glu.h"


#import "MSXML6.dll" rename_namespace(_T("MSXML"))
#include <msxml6.h>
#include <afxcontrolbars.h>
#include <afxext.h>
#include <afxwin.h>
#include <afxcontrolbars.h>
#include <afxwin.h>
#include <afxwin.h>
#include <afxcontrolbars.h>
#include <afxwin.h>



