// OpenGLRenderer.cpp : implementation file
//

#include "stdafx.h"
//#include "OpenGL_MFCDialog.h"
#include "OpenGLRenderer.h"
#include "ealloc_d.h"
#include "dcHelper.h"

/*
From:  https://www.opengl.org/discussion_boards/showthread.php/168995-Rendering-into-BitMap-using-opengl-in-Win32
Rendering into BitMap using opengl in Win32
Yes.. Creating OpenGL context is platform dependent..
http://www.opengl.org/wiki/Creating_an_OpenGL_Context

and most simple example:
http://nehe.gamedev.net/data/lessons....asp?lesson=05

After you create context, you need to create offscreen buffer using FBO:
http://www.songho.ca/opengl/gl_fbo.html

More stuff for reading:
http://www.opengl.org/wiki/GL_EXT_framebuffer_object

and finaly, read pixels to your buffer:
glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, my_fbo);
glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, my_buffer);
*/
// OpenGLRenderer

OpenGLRenderer::OpenGLRenderer()
{
	//m_iWidth = 200;
	//m_iHeight =200;
	//m_iViewPortWidth =200;
	//m_iViewPortHeight=200;
	m_Window = CRect(0, 0, 200, 200);

	// myBmp.m_hMemDC = NULL;
}

OpenGLRenderer::~OpenGLRenderer()
{
	DestroyScene();
}


BEGIN_MESSAGE_MAP(OpenGLRenderer, CWnd)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// OpenGLRenderer message handlers

bool OpenGLRenderer::CreateGLContext(CRect rect, CWnd *parent, int numcurve)
{
	CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_OWNDC, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH), NULL); //default background colour

	CreateEx(0,
		className, 
		_T("OpenGL with MFC/CDialog"), 
		// WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		rect,
		parent, 
		0);

	// SetViewPortSize(rect.Width(), rect.Height());
	m_Window = rect;
	//m_WindowOld = rect;
	//m_WindowOrig = rect;
	if(!InitContext() ) {
		MessageBox(_T("ERROR Creating InitContext"));
		return false;
	};

	myGL.vaoID = (GLuint *)alloc1(numcurve, sizeof(GLuint));
	myGL.vboID = (GLuint *)alloc1(numcurve, sizeof(GLuint));
	myGL.numvo = numcurve;
	return true;
}


bool OpenGLRenderer::CreateGLContextFBO(CRect rect, CWnd *parent, int numcurve) {
	CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_OWNDC, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH), NULL); //default background colour

	CreateEx(0,
		className,
		_T("OpenGL with MFC/CDialog"),
		// WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		rect,
		parent,
		0);

	// SetViewPortSize(rect.Width(), rect.Height());
	m_Window = rect;
	if (!InitContextFBO()) {
		MessageBox(_T("ERROR Creating InitContext"));
		return false;
	};

	myGL.vaoID = (GLuint *)alloc1(numcurve, sizeof(GLuint));
	myGL.vboID = (GLuint *)alloc1(numcurve, sizeof(GLuint));
	myGL.numvo = numcurve;
	return true;
}

bool OpenGLRenderer::InitContext()
{
        PIXELFORMATDESCRIPTOR pfd;
        memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
        pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion   = 1;
		// PFD_DRAW_TO_BITMAP
        pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 32;
        pfd.iLayerType = PFD_MAIN_PLANE;

		CDC *pDC = GetDC();
		 myBmp.m_hMemDC = pDC->m_hDC;

        int nPixelFormat = ChoosePixelFormat(myBmp.m_hMemDC, &pfd);
        if (nPixelFormat == 0) return false;
        BOOL bResult = SetPixelFormat (myBmp.m_hMemDC, nPixelFormat, &pfd);
        if (!bResult) return false; 
 
        HGLRC tempContext = wglCreateContext(myBmp.m_hMemDC);

		// backup/Store old DC and RC
		//myGL.m_hOldDC = ::wglGetCurrentDC();
		//myGL.m_hOldRC = ::wglGetCurrentContext();
		wglMakeCurrent(myBmp.m_hMemDC, tempContext);
 	
		GLenum GlewInitResult;
		glewExperimental = GL_TRUE;
		GlewInitResult = glewInit();
		
        if (GlewInitResult != GLEW_OK)
        {
                AfxMessageBox(_T("GLEW is not initialized!"));
        }
 
        int attribs[] =
        {
                WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
                WGL_CONTEXT_MINOR_VERSION_ARB, 3,
                WGL_CONTEXT_FLAGS_ARB, 0,
                0
        };
 
        if(wglewIsSupported("WGL_ARB_create_context") == 1)
        {
			myBmp.m_hMemRC = wglCreateContextAttribsARB(myBmp.m_hMemDC,0, attribs);
                wglMakeCurrent(NULL,NULL);
                wglDeleteContext(tempContext);
                wglMakeCurrent(myBmp.m_hMemDC, myBmp.m_hMemRC);
        }
        else
        {       //It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
			myBmp.m_hMemRC = tempContext;
        }
 
        if (!myBmp.m_hMemRC)
			return false;

		CString str;
		str.Format(_T("OpenGL version: %s\n"),(CString)glGetString(GL_VERSION));
		TRACE(str);
		return true;
}


bool OpenGLRenderer::InitContextFBO()
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	// PFD_DRAW_TO_BITMAP
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	CDC *pDC = GetDC();
	myBmp.m_hMemDC = pDC->m_hDC;

	int nPixelFormat = ChoosePixelFormat(myBmp.m_hMemDC, &pfd);
	if (nPixelFormat == 0) return false;
	BOOL bResult = SetPixelFormat(myBmp.m_hMemDC, nPixelFormat, &pfd);
	if (!bResult) return false;

	HGLRC tempContext = wglCreateContext(myBmp.m_hMemDC);

	// backup/Store old DC and RC
	//myGL.m_hOldDC = ::wglGetCurrentDC();
	//myGL.m_hOldRC = ::wglGetCurrentContext();
	wglMakeCurrent(myBmp.m_hMemDC, tempContext);

	GLenum GlewInitResult;
	glewExperimental = GL_TRUE;
	GlewInitResult = glewInit();

	if (GlewInitResult != GLEW_OK)
	{
		AfxMessageBox(_T("GLEW is not initialized!"));
	}

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		myBmp.m_hMemRC = wglCreateContextAttribsARB(myBmp.m_hMemDC, 0, attribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(myBmp.m_hMemDC, myBmp.m_hMemRC);
	}
	else
	{       //It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		myBmp.m_hMemRC = tempContext;
	}

	if (!myBmp.m_hMemRC)
		return false;

	CString str;
	str.Format(_T("OpenGL version: %s\n"), (CString)glGetString(GL_VERSION));
	TRACE(str);
	return true;
}

bool OpenGLRenderer::CreateGLContextBmp(CRect rect, CWnd *parent, int numcurve)
{
	//CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_OWNDC, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH), NULL); //default background colour

	//CreateEx(0,
	//	className,
	//	_T("OpenGL with MFC/CDialog"),
	//	// WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
	//	WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
	//	rect,
	//	parent,
	//	0);

	//if (!InitContext()) {
	//	MessageBox(_T("ERROR Creating InitContext"));
	//	return false;
	//};
	// Initialize the bitmap header info.
	memset(&myBmp.m_bmi, 0, sizeof(BITMAPINFO));
	myBmp.m_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	myBmp.m_bmi.bmiHeader.biWidth = rect.Width();
	myBmp.m_bmi.bmiHeader.biHeight = rect.Height();
	myBmp.m_bmi.bmiHeader.biPlanes = 1;
	myBmp.m_bmi.bmiHeader.biBitCount = 24;
	myBmp.m_bmi.bmiHeader.biCompression = BI_RGB;
	myBmp.m_bmi.bmiHeader.biSizeImage = rect.Width() * rect.Height() * 3;
	m_Window = rect;

	// Create DIB
	HDC	hDC = ::GetDC(parent->m_hWnd);
	// HDC	hDC = pDC->m_hDC; // this make the previous window blank

	myBmp.m_hDib = ::CreateDIBSection(hDC, &myBmp.m_bmi, DIB_RGB_COLORS, &myBmp.m_pBitmapBits, NULL, (DWORD)0);
//	::ReleaseDC(parent->m_hWnd, hDC);

	// Create memory DC
	myBmp.m_hMemDC = ::CreateCompatibleDC(NULL);
	if (!myBmp.m_hMemDC) {
		DeleteObject(myBmp.m_hDib);
		myBmp.m_hDib = NULL;
		return false;
	}

	SelectObject(myBmp.m_hMemDC, myBmp.m_hDib);

	// Setup memory DC's pixel format.
	if (!InitContextBmp(myBmp.m_hMemDC, PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_STEREO_DONTCARE)) {
		//SelectObject(myBmp.m_hMemDC, myBmp.m_hOldDib);
		DeleteObject(myBmp.m_hDib);
		myBmp.m_hDib = NULL;
		DeleteDC(myBmp.m_hMemDC);
		myBmp.m_hMemDC = NULL;
		return false;
	}

	// Create memory RC
	myBmp.m_hMemRC = ::wglCreateContext(myBmp.m_hMemDC);
	if (!myBmp.m_hMemRC) {
		//SelectObject(myBmp.m_hMemDC, myBmp.m_hOldDib);
		DeleteObject(myBmp.m_hDib);
		myBmp.m_hDib = NULL;
		DeleteDC(myBmp.m_hMemDC);
		myBmp.m_hMemDC = NULL;
		return false;
	}

	// Store old DC and RC
	//myBmp.m_hOldDC = ::wglGetCurrentDC();
	//myBmp.m_hOldRC = ::wglGetCurrentContext();

	myGL.vaoID = (GLuint *)alloc1(numcurve, sizeof(GLuint));
	myGL.vboID = (GLuint *)alloc1(numcurve, sizeof(GLuint));
	myGL.numvo = numcurve;
	return true;
}


bool OpenGLRenderer::InitContextBmp(HDC hDC, DWORD dwFlags)
{
	PIXELFORMATDESCRIPTOR pfd;

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;

	pfd.dwFlags = dwFlags;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 24;
	pfd.cRedBits = 8;
	pfd.cRedShift = 16;
	pfd.cGreenBits = 8;
	pfd.cGreenShift = 8;
	pfd.cBlueBits = 8;
	pfd.cBlueShift = 0;
	pfd.cAlphaBits = 0;
	pfd.cAlphaShift = 0;
	pfd.cAccumBits = 64;
	pfd.cAccumRedBits = 16;
	pfd.cAccumGreenBits = 16;
	pfd.cAccumBlueBits = 16;
	pfd.cAccumAlphaBits = 0;
	pfd.cDepthBits = 32;
	pfd.cStencilBits = 8;
	pfd.cAuxBuffers = 0;
	pfd.iLayerType = PFD_MAIN_PLANE;
	pfd.bReserved = 0;
	pfd.dwLayerMask = 0;
	pfd.dwVisibleMask = 0;
	pfd.dwDamageMask = 0;

	int nPixelFormat = ::ChoosePixelFormat(hDC, &pfd);
	if (nPixelFormat == 0) // Choose default
	{
		nPixelFormat = 1;
		if (::DescribePixelFormat(hDC, nPixelFormat,
			sizeof(PIXELFORMATDESCRIPTOR), &pfd) == 0)
			return false;
	}

	if (!::SetPixelFormat(hDC, nPixelFormat, &pfd))
		return false;

	return true;
}


void OpenGLRenderer::PrepareScene()
{
	wglMakeCurrent(myBmp.m_hMemDC, myBmp.m_hMemRC);
	glClearColor(0.0, 0.0, 1.0, 0.0); //background to clear with.

	//do other preparations here

	wglMakeCurrent(NULL, NULL);
}

void OpenGLRenderer::Reshape(UINT nType, int w, int h)
{
	OnSize(nType, w, h);
}


void OpenGLRenderer::OnSize(UINT nType, int cx, int cy)
{
	if(myBmp.m_hMemDC != NULL)
	{
		wglMakeCurrent(myBmp.m_hMemDC, myBmp.m_hMemRC);
		//---------------------------------
		glViewport(0, 0, (GLsizei)cx, (GLsizei)cy); 
		//---------------------------------
		m_Window.right = cx;
		m_Window.bottom = cy;

		switch (nType)
		{
			case SIZE_RESTORED:
			case SIZE_MAXIMIZED:
				{
				//	MoveWindow(m_WindowOld.left,m_WindowOld.top, m_Window.Width(), m_Window.Height(),TRUE);
					MoveWindow(m_Window, TRUE);
					break;
				}
			default:
				break;
		}
	}
	wglMakeCurrent(NULL, NULL);
}


void OpenGLRenderer::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
		case 1:
		{
			// Clear color and depth buffer bits
			glClear(GL_COLOR_BUFFER_BIT );

			// Draw OpenGL scene
			//myDrawScene();
			// Swap buffers
			SwapBuffers(myBmp.m_hMemDC);
			break;
		}
		default:
			break;
	}
	CWnd::OnTimer(nIDEvent);
}




void OpenGLRenderer::OnPrint1(CDC* pDC, CPrintInfo* pInfo, CView* pView)
{
	CRect rcClient;
	pView->GetClientRect(&rcClient);
	float fClientRatio = float(rcClient.Height()) / rcClient.Width();

	// Get page size
	myBmp.m_szPage.cx = pDC->GetDeviceCaps(HORZRES);
	myBmp.m_szPage.cy = pDC->GetDeviceCaps(VERTRES);

	CSize szDIB;
	if (pInfo && pInfo->m_bPreview) {
		// Use screen resolution for preview.
		szDIB.cx = rcClient.Width();
		szDIB.cy = rcClient.Height();
	}
	else  // Printing
	{
		// Use higher resolution for printing.
		// Adjust size according screen's ratio.
		if (myBmp.m_szPage.cy > fClientRatio*myBmp.m_szPage.cx) {
			// View area is wider than Printer area
			szDIB.cx = myBmp.m_szPage.cx;
			szDIB.cy = long(fClientRatio*myBmp.m_szPage.cx);
		}
		else {
			// View area is narrower than Printer area
			szDIB.cx = long(float(myBmp.m_szPage.cy) / fClientRatio);
			szDIB.cy = myBmp.m_szPage.cy;
		}
		// Reduce the Resolution if the Bitmap size is too big.
		// Ajdust the maximum value, which is depends on printer's memory.
		// I use 20 MB. 
		while (szDIB.cx*szDIB.cy > 20e6) {
			szDIB.cx = szDIB.cx / 2;
			szDIB.cy = szDIB.cy / 2;
		}
	}

	TRACE("Buffer size: %d x %d = %6.2f MB\n", szDIB.cx, szDIB.cy, szDIB.cx*szDIB.cy*0.000001);

	// Initialize the bitmap header info.
	memset(&myBmp.m_bmi, 0, sizeof(BITMAPINFO));
	myBmp.m_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	myBmp.m_bmi.bmiHeader.biWidth = szDIB.cx;
	myBmp.m_bmi.bmiHeader.biHeight = szDIB.cy;
	myBmp.m_bmi.bmiHeader.biPlanes = 1;
	myBmp.m_bmi.bmiHeader.biBitCount = 24;
	myBmp.m_bmi.bmiHeader.biCompression = BI_RGB;
	myBmp.m_bmi.bmiHeader.biSizeImage = szDIB.cx * szDIB.cy * 3;

	// Create DIB
	HDC	hDC = ::GetDC(pView->m_hWnd);
	// HDC	hDC = pDC->m_hDC; // this make the previous window blank

	myBmp.m_hDib = ::CreateDIBSection(hDC, &myBmp.m_bmi, DIB_RGB_COLORS, &myBmp.m_pBitmapBits, NULL, (DWORD)0);
	::ReleaseDC(pView->m_hWnd, hDC);

	// Create memory DC
	myBmp.m_hMemDC = ::CreateCompatibleDC(NULL);
	if (!myBmp.m_hMemDC) {
		DeleteObject(myBmp.m_hDib);
		myBmp.m_hDib = NULL;
		return;
	}

	//myBmp.m_hOldDib = SelectObject(myBmp.m_hMemDC, myBmp.m_hDib);

	// Setup memory DC's pixel format.
	if (!InitContextBmp(myBmp.m_hMemDC, PFD_DRAW_TO_BITMAP | PFD_SUPPORT_OPENGL | PFD_STEREO_DONTCARE)) {
		//SelectObject(myBmp.m_hMemDC, myBmp.m_hOldDib);
		DeleteObject(myBmp.m_hDib);
		myBmp.m_hDib = NULL;
		DeleteDC(myBmp.m_hMemDC);
		myBmp.m_hMemDC = NULL;
		return;
	}

	// Create memory RC
	myBmp.m_hMemRC = ::wglCreateContext(myBmp.m_hMemDC);
	if (!myBmp.m_hMemRC) {
		//SelectObject(myBmp.m_hMemDC, myBmp.m_hOldDib);
		DeleteObject(myBmp.m_hDib);
		myBmp.m_hDib = NULL;
		DeleteDC(myBmp.m_hMemDC);
		myBmp.m_hMemDC = NULL;
		return;
	}

	// Store old DC and RC
	//myBmp.m_hOldDC = ::wglGetCurrentDC();
	//myBmp.m_hOldRC = ::wglGetCurrentContext();

	// Make m_hMemRC the current RC.
	//::wglMakeCurrent(myBmp.m_hMemDC, myBmp.m_hMemRC);

	//// Default mode
	//::glEnable(GL_NORMALIZE);
	//::glEnable(GL_DEPTH_TEST);

	//// Lights, material properties
	//::glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	//::glClearDepth(1.0);
	//::glShadeModel(GL_SMOOTH);

	//// Default polygonmode
	//::glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//::glViewport(0, 0, szDIB.cx, szDIB.cy);
	//
	//::glMatrixMode(GL_PROJECTION);
	//::glLoadIdentity();
	//::gluPerspective(45.0, m_Window.Width() / (double)m_Window.Height(), 1., 15.);
	//::glMatrixMode(GL_MODELVIEW);

	// CreateDisplayList(1);
}

void OpenGLRenderer::OnPrint2(CDC* pDC)
{
	//::wglMakeCurrent(NULL, NULL);
	//::wglDeleteContext(myBmp.m_hMemRC);

	// Restore last DC and RC
	//::wglMakeCurrent(myBmp.m_hOldDC, myBmp.m_hOldRC);





	// Size of printing image
	float fBmiRatio = float(myBmp.m_bmi.bmiHeader.biHeight) / myBmp.m_bmi.bmiHeader.biWidth;
	CSize szImageOnPage;
	if (myBmp.m_szPage.cx > myBmp.m_szPage.cy)	 // Landscape page
	{
		if (fBmiRatio<1)	  // Landscape image
		{
			szImageOnPage.cx = myBmp.m_szPage.cx;
			szImageOnPage.cy = long(fBmiRatio * myBmp.m_szPage.cy);
		}
		else			  // Portrait image
		{
			szImageOnPage.cx = long(myBmp.m_szPage.cy / fBmiRatio);
			szImageOnPage.cy = myBmp.m_szPage.cy;
		}
	}
	else		    // Portrait page
	{
		if (fBmiRatio<1)	  // Landscape image
		{
			szImageOnPage.cx = myBmp.m_szPage.cx;
			szImageOnPage.cy = long(fBmiRatio * myBmp.m_szPage.cx);
		}
		else			  // Portrait image
		{
			szImageOnPage.cx = long(myBmp.m_szPage.cx / fBmiRatio);
			szImageOnPage.cy = myBmp.m_szPage.cy;
		}
	}

	CSize szOffset((myBmp.m_szPage.cx - szImageOnPage.cx) / 2, (myBmp.m_szPage.cy - szImageOnPage.cy) / 2);

	// Stretch the Dib to fit the image size.
	int nRet = ::StretchDIBits(pDC->GetSafeHdc(),
		szOffset.cx, szOffset.cy,
		szImageOnPage.cx, szImageOnPage.cy,
		0, 0,
		myBmp.m_bmi.bmiHeader.biWidth,
		myBmp.m_bmi.bmiHeader.biHeight,
		(GLubyte*)myBmp.m_pBitmapBits,
		&myBmp.m_bmi, DIB_RGB_COLORS, SRCCOPY);

	if (nRet == GDI_ERROR)
		TRACE0("Failed in StretchDIBits()");

	//if (myBmp.m_hOldDib)
	//	SelectObject(myBmp.m_hMemDC, myBmp.m_hOldDib);

	DeleteObject(myBmp.m_hDib);
	myBmp.m_hDib = NULL;
	DeleteDC(myBmp.m_hMemDC);
	myBmp.m_hMemDC = NULL;
	//myBmp.m_hOldDC = NULL;


	//CRect rect;
	//// GetClientRect(&rect);
	//GetClientRect(WindowFromDC(pDC->m_hDC), &rect);
	////	pDC->DrawText("OK", &rect, DT_CENTER|DT_BOTTOM|DT_SINGLELINE);
	//pDC->TextOutA(10, rect.bottom * 10, "OKM10");
	//pDC->TextOutA(rect.right / 2, rect.bottom * 2, "OKM2");
	//pDC->TextOutA(10, rect.bottom * 2, "OKM2");
	//pDC->TextOutA(10, rect.bottom / 1, "OK1");
	//pDC->TextOutA(10, rect.bottom / 3, "OK3");
	//pDC->TextOutA(10, rect.bottom / 2, "OK2");
	//pDC->TextOutA(10, rect.bottom / 10, "OK10");

	return;

}



void OpenGLRenderer::myDrawSceneBasic1()
{
	// from prepare
	wglMakeCurrent(myBmp.m_hMemDC, myBmp.m_hMemRC);
	glClearColor(0.0, 0.8, 1.0, 0.0); //background to clear with.
//	wglMakeCurrent(NULL, NULL);
	// end prepare

	// copied from triangle
	//wglMakeCurrent(m_hdc, m_hrc);
	int m_GLIntSize = 4; //number of floats per item. i.e. number of floats per point in space.
	int m_GLSizeCount = 3; //number of items. ie sizeof(TriangleA)/m_GLIntSize

	GLfloat TriangleA[] =
	{
		-0.3f, 0.5f, -1.0f, 1.0f,
		-0.8f, -0.5f, -1.0f, 1.0f,
		0.2f, -0.5f, -1.0f, 1.0f
	};

	GLfloat TriangleB[] =
	{
		-0.2f,  0.5f, -1.0f, 1.0f,
		0.3f,  -0.5f, -1.0f, 1.0f,
		0.8f, 0.5f, -1.0f, 1.0f,
	};

	//VAOs allocation
	glGenVertexArrays(2, &myGL.vaoID[0]);

	// First VAO setup
	glBindVertexArray(myGL.vaoID[0]);
	glGenBuffers(1, myGL.vboID); //VBO allocation
	glBindBuffer(GL_ARRAY_BUFFER, myGL.vboID[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleA), TriangleA, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, m_GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	// Second VAO setup     
	glBindVertexArray(myGL.vaoID[1]);
	glGenBuffers(1, &myGL.vboID[1]); //VBO allocation
	glBindBuffer(GL_ARRAY_BUFFER, myGL.vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleB), TriangleB, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, m_GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);
	
	glEnableVertexAttribArray(0);

	glClear(GL_COLOR_BUFFER_BIT);
	for(int i=0;i<2;i++) {
		glBindVertexArray(myGL.vaoID[i]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, m_GLSizeCount);
	}
	//--------------------------------
//	glFlush();


	SwapBuffers(myBmp.m_hMemDC);
	wglMakeCurrent(NULL, NULL);

	// Deping testing
	//myDrawLine();
}


void OpenGLRenderer::read_shader_src(const char *fname, std::vector<char> &buffer) {
	std::ifstream in;
	in.open(fname, std::ios::binary);

	if (in.is_open()) {
		// Get the number of bytes stored in this file
		in.seekg(0, std::ios::end);
		size_t length = (size_t)in.tellg();

		// Go to start of the file
		in.seekg(0, std::ios::beg);

		// Read the content of the file in a buffer
		buffer.resize(length + 1);
		in.read(&buffer[0], length);
		in.close();
		// Add a valid C - string end
		buffer[length] = '\0';
	}
	else {
		std::cerr << "Unable to open " << fname << " I'm out!" << std::endl;
		exit(-1);
	}
}

// Compile a shader
GLuint OpenGLRenderer::load_and_compile_shader(const char *fname, GLenum shaderType) {
	// Load a shader from an external file
	std::vector<char> buffer;
	read_shader_src(fname, buffer);
	const char *src = &buffer[0];

	// Compile the shader
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);
	// Check the result of the compilation
	GLint test;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &test);
	if (!test) {
		std::cerr << "Shader compilation failed with this message:" << std::endl;
		std::vector<char> compilation_log(512);
		glGetShaderInfoLog(shader, compilation_log.size(), NULL, &compilation_log[0]);
		std::cerr << &compilation_log[0] << std::endl;
		exit(-2);
	}
	return shader;
}

// Create a program from two shaders
GLuint OpenGLRenderer::create_program(const char *path_vert_shader, const char *path_frag_shader) {
	// Load and compile the vertex and fragment shaders
	GLuint vertexShader = load_and_compile_shader(path_vert_shader, GL_VERTEX_SHADER);
	GLuint fragmentShader = load_and_compile_shader(path_frag_shader, GL_FRAGMENT_SHADER);

	// Attach the above shader to a program
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	// Flag the shaders for deletion
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Link and use the program
	glLinkProgram(shaderProgram);
	glUseProgram(shaderProgram);

	return shaderProgram;
}




//copied code from  http://stackoverflow.com/questions/14908743/opengl-render-directly-to-bitmap
// flushes the OpenGL error queue and
// counts the total number of errors
int flushGLErrors(void)
{
	int i = 0;
	while (glGetError() != GL_NO_ERROR) {
		i++;
	}

	return i;
}

// returns a HBITMAP or NULL.
// The HBITMAP must be freed using DeleteObject 
HBITMAP OpenGLRenderer::ReadPixelsToHBITMAP(
	HDC hdc,
	int x,
	int y,
	int width,
	int height)
{
	void *pdata = NULL;

	/* Note that this values only makes sense if we know a target
	* output size like if we put the image to paper. */
	//const int physical_resolution = 2835;
	const int physical_resolution = 72;
	/* 72 DPI */

	BITMAPINFOHEADER bmih = {
		/* .biSize          = */ sizeof(bmih),
		/* .biWidth         = */ width,
		/* .bi.Height       = */ height,
		/* .biPlanes        = */ 1,                   /* mandatory */
		/* .biBitCount      = */ 24,                  /* 8 bits per pixel */
		/* .biCompression   = */ BI_RGB,              /* uncompressed */
		/* .biSizeImage     = */ 0,                   /* implicit */
		/* .biXPelsPerMeter = */ physical_resolution, /* ignored */
		/* .biYPelsPerMeter = */ physical_resolution, /* ignored */
		/* .biClrUsed       = */ 0,                   /* no palette */
		/* .biClrImportant  = */ 0
	};

	HBITMAP hbm = CreateDIBSection(
		hdc, /* may be different than the DC used for OpenGL */
		(PBITMAPINFO)&bmih, /* can do this cast, because no palette is used */
		DIB_RGB_COLORS,
		&pdata,
		NULL,
		0
	);

	if (!hbm) {
		return NULL;
	}

	flushGLErrors();

	glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
	glPixelStorei(GL_PACK_LSB_FIRST, GL_TRUE);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	if (glGetError() != GL_NO_ERROR) {
		DeleteObject(hbm);
		return NULL;
	}

	glReadPixels(x, y, width, height, GL_BGR, GL_UNSIGNED_BYTE, pdata);

	if (glGetError() != GL_NO_ERROR) {
		DeleteObject(hbm);
		return NULL;
	}

	return hbm;
}
//end copied code

void OpenGLRenderer::myDrawSceneLines(float **ampArr, float x[], float y[], int nx, int nt, double dxMin4Plot)
{
		wglMakeCurrent(myBmp.m_hMemDC, myBmp.m_hMemRC);
		glClearColor(0.0, 0.8, 1.0, 0.0); //background to clear with.
		const int GLIntSize = 3; //number of floats per item. i.e. number of floats per point in space.
		const int GLSizeCount = nt; //number of items. ie sizeof(TriangleA)/m_GLIntSize
	
		GLfloat *line = (GLfloat*)alloc1(nt * GLIntSize, sizeof(GLfloat));
		double xPlotted = -99999.;
		glGenVertexArrays(myGL.numvo, myGL.vaoID);
		glClear(GL_COLOR_BUFFER_BIT);
	
		int count = 0;
		for (int ix = 0; ix < nx; ix++) {
			if (fabs(x[ix] - xPlotted) < dxMin4Plot) continue;  // skip too dense traces
			xPlotted = x[ix];
	
			for (int j = 0; j < nt; j++) {
				line[j * GLIntSize] = (GLfloat)ampArr[ix][j];
				line[j * GLIntSize + 1] = (GLfloat)y[j];
				line[j * GLIntSize + 2] = (GLfloat)-1.0;
			}
	
			glBindVertexArray(myGL.vaoID[ix]);
			glGenBuffers(1, &myGL.vboID[ix]); //VBO allocation
			glBindBuffer(GL_ARRAY_BUFFER, myGL.vboID[ix]);
			glBufferData(GL_ARRAY_BUFFER, nt * GLIntSize * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, nt * GLIntSize * sizeof(GLfloat), line);
			glVertexAttribPointer((GLuint)0, GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glDrawArrays(GL_LINE_STRIP, 0, GLSizeCount);
			count++;
		}
		SwapBuffers(myBmp.m_hMemDC);
		wglMakeCurrent(NULL, NULL);

		// restore old DC and RC
		//wglMakeCurrent(myGL.m_hOldDC, myGL.m_hOldRC);

		free1(line);
}

void OpenGLRenderer::myDrawSceneFills(float **xArr, int nx, int nt)
{
	wglMakeCurrent(myBmp.m_hMemDC, myBmp.m_hMemRC);
	glClearColor(0.0, 0.8, 1.0, 0.0); //background to clear with.
	const int GLIntSize = 3; //Number of floats per vertex (x,y,z, etc.)
	const int GLSizeCount = 6; //Number of vertices in the polygon/triangle fan


	GLuint shaderProgram = create_program("E:/project/cpp.net/myOpengl/shaders/vert.shader", "E:/project/cpp.net/myOpengl/shaders/frag.shader");
	GLint position_attribute = glGetAttribLocation(shaderProgram, "position");
	GLint color_attribute = glGetAttribLocation(shaderProgram, "color");
	//Add Shader

	glClear(GL_COLOR_BUFFER_BIT);
	glGenVertexArrays(10, myGL.vaoID); //The 10 will need to be dynamic, aka amount of polygons to be drawn
									   //// First VAO setup


	for (int i = 0; i < 10; i++)
	{

		if (i % 2 == 0) {
			GLfloat colors[] =
			{
				1.0, 0.0, 0.0,
				1.0, 0.0, 0.0,
				1.0, 0.0, 0.0,
				1.0, 0.0, 0.0,
				1.0, 0.0, 0.0,
				1.0, 0.0, 0.0
			};
			//RGB colouring. One colour per vertex. Matches in order with the vertices of the polygon.
			GLfloat TriangleA[] =
			{
				0.0f, 1.0f - i*0.2f, -1.0f,
				0.0f, 0.8f - i*0.2f, -1.0f,
				0.2f, 0.85f - i*0.2f, -1.0f,
				0.3f, 0.9f - i*0.2f, -1.0f,
				0.4f, 0.95f - i*0.2f, -1.0f,
				0.0f, 1.0f - i*0.2f, -1.0f
			};
			//Vertices of the Polygon (number of vertices will be dynamic)
			//Graphics card reads it from the top left corner going counter-clockwise

			glBindVertexArray(myGL.vaoID[1]);  //<- These i's used in vao and vbo will be the i'th polygon being stored
			glGenBuffers(1, &myGL.vaoID[1]);
			glBindBuffer(GL_ARRAY_BUFFER, myGL.vboID[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleA) + sizeof(colors), NULL, GL_STATIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TriangleA), TriangleA);   //This subdata is to hold the vertices polygon
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(TriangleA), sizeof(colors), colors); //This subdata holds the colours of the vertices
			glVertexAttribPointer((GLuint)0, GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);   //This tells the system how many floats per vertex
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(color_attribute, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(TriangleA)); //This tells the system how many floats per colour
			glEnableVertexAttribArray(color_attribute);
			glDrawArrays(GL_TRIANGLE_FAN, 0, GLSizeCount);
		}
		else {
			GLfloat colors[] =
			{
				0.0, 0.0, 1.0,
				0.0, 0.0, 1.0,
				0.0, 0.0, 1.0,
				0.0, 0.0, 1.0,
				0.0, 0.0, 1.0,
				0.0, 0.0, 1.0
			};
			GLfloat TriangleA[] =
			{
				0.0f, 1.0f - i*0.2f, -1.0f,
				-0.2f, 0.95f - i*0.2f, -1.0f,
				-0.3f, 0.9f - i*0.2f, -1.0f,
				-0.2f, 0.85f - i*0.2f, -1.0f,
				0.0f, 0.8f - i*0.2f, -1.0f,
				0.0f, 1.0f - i*0.2f, -1.0f
			};
			glBindVertexArray(myGL.vaoID[1]);
			glGenBuffers(1, &myGL.vaoID[1]);
			glBindBuffer(GL_ARRAY_BUFFER, myGL.vboID[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleA) + sizeof(colors), NULL, GL_STATIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TriangleA), TriangleA);
			glBufferSubData(GL_ARRAY_BUFFER, sizeof(TriangleA), sizeof(colors), colors);
			glVertexAttribPointer((GLuint)0, GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(color_attribute, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(TriangleA));
			glEnableVertexAttribArray(color_attribute);
			glDrawArrays(GL_TRIANGLE_FAN, 0, GLSizeCount);


		}

	}


	//for (int i = 1; i<10; i++) {
	//	glBindVertexArray(myGL.vaoID[i]);   //Load each i'th polygon to be displayed
	//	glDrawArrays(GL_TRIANGLE_FAN, 0, GLSizeCount);   //Draws the shape using triangle fan from vertex 0 to the last vertex
	//}

	SwapBuffers(myBmp.m_hMemDC);
	wglMakeCurrent(NULL, NULL);
}


//
//void OpenGLRenderer::mySetData(int iType)
//{
//	myDrawLine(); return;
//
//	m_iShapeType = iType;
//	switch(iType)
//	{
//		case 1:
//			{
//			// SQUARE
//				mySetSquare();
//			}
//			break;
//		case 3:
//			{
//			// TRIANGLE
//				mySetTriangle();
//			}
//			break;
//		default:
//			mySetTriangle();
//	}
//
//}
//
//
//void OpenGLRenderer::mySetSquare()
//{
//	wglMakeCurrent(m_hdc, m_hrc);
//
//		// First simple object
//		m_GLSizeCount = 4;
//		m_GLIntSize = 4;
//		GLfloat Square[] = 
//		{
//			-0.5f, -0.5f, 0.0f, 1.0f,
//			-0.5f,  0.5f, 0.0f, 1.0f,
//			0.5f,  0.5f, 0.0f, 1.0f,
//			0.5f, -0.5f, 0.0f, 1.0f
//		};
//
//		//VAO allocation
//		glGenVertexArrays(1, &m_vaoID[0]);
// 
//		// First VAO setup
//		glBindVertexArray(m_vaoID[0]);
//		glGenBuffers(1, &m_vboID[0]);
//		glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(Square), Square, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, m_GLIntSize, GL_FLOAT, GL_FALSE, 0, 0); 
//		glEnableVertexAttribArray(0);
//	
//	wglMakeCurrent(NULL, NULL);
//}

// Deping test
//void OpenGLRenderer::myDrawLine()
//{
//	wglMakeCurrent(m_hdc, m_hrc);
//
//	//glLineWidth(2.5);
//	//glColor3f(1.0, 0.0, 0.0);
//	//glBegin(GL_LINES);
//	//glVertex3f(0.0, 0.0, 0.0);
//	//glVertex3f(15, 0, 0);
//	//glEnd();
//	
//	// First simple object
//	m_GLSizeCount = 4;
//	m_GLIntSize = 4;
//	GLfloat myArr[] =
//	{
//		-0.5f, -0.5f, 0.0f, 1.0f,
//		-0.5f,  0.5f, 0.0f, 1.0f,
//		0.5f,  0.5f, 0.0f, 1.0f,
//		 0.5f, -0.5f, 0.0f, 1.0f
//	};
//
//	//VAO allocation
//	glGenVertexArrays(1, &m_vaoID[0]);
//
//	// First VAO setup
//	glBindVertexArray(m_vaoID[0]);
//	glGenBuffers(1, &m_vboID[0]);
//	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);
//	int gter = sizeof(myArr);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(myArr), myArr, GL_STATIC_DRAW);
//	glVertexAttribPointer((GLuint)0, m_GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);
//	glEnableVertexAttribArray(0);
//	wglMakeCurrent(NULL, NULL);
//}


//
//
//void OpenGLRenderer::mySetTriangle()
//{
//	wglMakeCurrent(m_hdc, m_hrc);
//
//		m_GLIntSize = 4; //number of floats per item. i.e. number of floats per point in space.
//		m_GLSizeCount = 3; //number of items. ie sizeof(TriangleA)/m_GLIntSize
//
//		GLfloat TriangleA[] = 
//		{
//			-0.3f, 0.5f, -1.0f, 1.0f,
//			-0.8f, -0.5f, -1.0f, 1.0f,
//			0.2f, -0.5f, -1.0f, 1.0f
//		};
//	
//		GLfloat TriangleB[] = 
//		{
//			-0.2f,  0.5f, -1.0f, 1.0f,
//			0.3f,  -0.5f, -1.0f, 1.0f,
//			0.8f, 0.5f, -1.0f, 1.0f,
//		};
//
//		//VAOs allocation
//		glGenVertexArrays(2, &m_vaoID[0]);
//
//		// First VAO setup
//		glBindVertexArray(m_vaoID[0]);
//		glGenBuffers(1, m_vboID); //VBO allocation
//		glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleA), TriangleA, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, m_GLIntSize, GL_FLOAT, GL_FALSE, 0, 0); 
//		glEnableVertexAttribArray(0);
//
//		// Second VAO setup     
//		glBindVertexArray(m_vaoID[1]);
//		glGenBuffers(1, &m_vboID[1]); //VBO allocation
//		glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleB), TriangleB, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, m_GLIntSize, GL_FLOAT, GL_FALSE, 0, 0); 
//		glEnableVertexAttribArray(0);
//
//	wglMakeCurrent(NULL, NULL);
//}





const GLchar* sceneVertexSource = R"glsl(
    #version 150 core
    in vec3 position;
    in vec3 color;
    in vec2 texcoord;
    out vec3 Color;
    out vec2 Texcoord;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 proj;
    uniform vec3 overrideColor;
    void main()
    {
        Color = overrideColor * color;
        Texcoord = texcoord;
        gl_Position = proj * view * model * vec4(position, 1.0);
    }
)glsl";
const GLchar* sceneFragmentSource = R"glsl(
    #version 150 core
    in vec3 Color;
    in vec2 Texcoord;
    out vec4 outColor;
    uniform sampler2D texKitten;
    uniform sampler2D texPuppy;
    void main()
    {
        outColor = vec4(Color, 1.0) * mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), 0.5);
    }
)glsl";

const GLchar* screenVertexSource = R"glsl(
    #version 150 core
    in vec2 position;
    in vec2 texcoord;
    out vec2 Texcoord;
    void main()
    {
        Texcoord = texcoord;
        gl_Position = vec4(position, 0.0, 1.0);
    }
)glsl";
const GLchar* screenFragmentSource = R"glsl(
    #version 150 core
    in vec2 Texcoord;
    out vec4 outColor;
    uniform sampler2D texFramebuffer;
    void main()
    {
        outColor = texture(texFramebuffer, Texcoord);
    }
)glsl";


void createShaderProgram(const GLchar* vertSrc, const GLchar* fragSrc, GLuint& vertexShader, GLuint& fragmentShader, GLuint& shaderProgram)
{
	// Create and compile the vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertSrc, NULL);
	glCompileShader(vertexShader);

	// Create and compile the fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragSrc, NULL);
	glCompileShader(fragmentShader);

	// Link the vertex and fragment shader into a shader program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glBindFragDataLocation(shaderProgram, 0, "outColor");
	glLinkProgram(shaderProgram);
}

//
//
//#include "E:/Jeff/fboMsaa/src/teapot.h";
//#include "E:/Jeff/ogl-OpenGL-tutorial_0015_33/external/glm-0.9.7.1/glm/glm.hpp";
//#include <E:/Jeff/ogl-OpenGL-tutorial_0015_33/external/glm-0.9.7.1/glm/gtc/matrix_transform.hpp>;
//#include <E:/Jeff/ogl-OpenGL-tutorial_0015_33/external/glm-0.9.7.1/glm/gtc/quaternion.hpp>;
//#include <E:/Jeff/ogl-OpenGL-tutorial_0015_33/external/glm-0.9.7.1/glm/gtx/quaternion.hpp>;
//#include <E:/Jeff/ogl-OpenGL-tutorial_0015_33/external/glm-0.9.7.1/glm/gtx/euler_angles.hpp>;
//#include <E:/Jeff/ogl-OpenGL-tutorial_0015_33/external/glm-0.9.7.1/glm/gtx/norm.hpp>;
//using namespace glm;
//
//#include <E:/Jeff/ogl-OpenGL-tutorial_0015_33/common/quaternion_utils.hpp>;
//#include <E:/Jeff/ogl-OpenGL-tutorial_0015_33/common/shader.hpp>;

void OpenGLRenderer::FirstFrameBufferImage(float **xArr, int nx, int nt, CWnd *pWnd)
{
	if (myBmp.m_hMemDC)
		wglMakeCurrent(myBmp.m_hMemDC, myBmp.m_hMemRC);
	glGenFramebuffers(1, &myGL.FramebufferName);
	glGenTextures(1, &myGL.renderedTexture);
	glGenRenderbuffers(1, &myGL.depthrenderbuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);

	glBindTexture(GL_TEXTURE_2D, myGL.renderedTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, myGL.renderedTexture, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, myGL.depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, m_Window.Width(), m_Window.Height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, myGL.depthrenderbuffer);
	static float a = 0, b = 0, c = 0;

	glBindTexture(GL_TEXTURE_2D, myGL.renderedTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Window.Width(), m_Window.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, myGL.renderedTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);

	glViewport(0, 0, m_Window.Width(), m_Window.Height());

	glClearColor(1, 1, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	float const light_dir[] = { 1,1,1,0 };
	float const light_color[] = { 1,0.95,0.9,1 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_dir);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_color);

	glTranslatef(0, 0, -5);

	glRotatef(a, 1, 0, 0);
	glRotatef(b, 0, 1, 0);
	glRotatef(c, 0, 0, 1);

	a = fmod(a + 0.1, 360.);
	b = fmod(b + 0.5, 360.);
	c = fmod(c + 0.25, 360.);

	a = 0, b = 0, c = 0;

	const int win_width = m_Window.Width();
	const int win_height = m_Window.Height();
	const float aspect = (float)m_Window.Width() / (float)m_Window.Height();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, m_Window.Width(), m_Window.Height());

	glClearColor(0.5, 0.9, 0.7, 0.);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	static const GLfloat g_vertex_buffer_data[] = {
		-0.5f,-0.5f,0.5f, // triangle 1 : begin
		-0.5f,-0.5f,0.5f,
		-0.5f, 0.5f,0.5f
	};

	glGenBuffers(1, &myGL.vboID[1]);
	glBindBuffer(GL_ARRAY_BUFFER, myGL.vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint sceneVertexShader, sceneFragmentShader, sceneShaderProgram;
	createShaderProgram(sceneVertexSource, sceneFragmentSource, sceneVertexShader, sceneFragmentShader, sceneShaderProgram);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(myGL.vboID[1]);
	glDisable(GL_DEPTH_TEST);
	glUseProgram(sceneShaderProgram);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	myFBOtoBitMapFile();

	/*glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	glGenTextures(1, &myGL.renderedTexture);
	glBindTexture(GL_TEXTURE_2D, myGL.renderedTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, m_Window.Width(), m_Window.Height(), 0, GL_RGB, GL_UNSIGNED_INT, NULL);
	//glGenerateMipmapEXT(GL_TEXTURE_2D);

	glGenFramebuffersEXT(1, &myGL.FramebufferName);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, myGL.FramebufferName);

	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, myGL.renderedTexture, 0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, myGL.FramebufferName);
	glClearColor(0.0, 0.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);

	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, m_Window.Width(), m_Window.Height());

	glColor4f(1.0, 1.0, 0.0, 0.0);
	glBegin(GL_QUADS);
	glVertex2f(0.5, 0.5);
	glVertex2f(0.5, -0.5);
	glVertex2f(-0.5, -0.5);
	glVertex2f(-0.5, 0.5);
	glEnd();

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClearColor(0.0, 0.8, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);

	glPopAttrib();

	glColor4f(1.0, 1.0, 1.0, 0.0);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex2f(1.0, 1.0);
	glTexCoord2f(0.0, 1.0); glVertex2f(1.0, -1.0);
	glTexCoord2f(1.0, 1.0); glVertex2f(-1.0, -1.0);
	glTexCoord2f(1.0, 0.0); glVertex2f(-1.0, 1.0);
	glEnd();

	myFBOtoBitMapFile();*/

	/*glGenVertexArrays(2, myGL.vaoID);
	glBindVertexArray(myGL.vaoID[1]);
	glGenBuffers(1, &myGL.vboID[1]);
	glBindBuffer(GL_ARRAY_BUFFER, myGL.vboID[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glBindFramebufferEXT(myGL.FramebufferName, 0);
	glDrawArrays(GL_TRIANGLES, 0, 3);*/

	/*GLuint texture;
	unsigned char * data;
	FILE * file;

	//The following code will read in our RAW file
	file = fopen("E:/Jeff/OpenGL-101-master/Textures/ex_10/squirrel.jpg", "rb");
	data = (unsigned char *)malloc(800 * 600 * 3);
	fread(data, 800 * 600 * 3, 1, file);
	fclose(file);

	glBindTexture(GL_TEXTURE_2D, myGL.renderedTexture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D,
	0,
	GL_RGBA,
	m_Window.Width(), m_Window.Height(),
	0,
	GL_RGBA,
	GL_UNSIGNED_BYTE,
	data);

	free(data);

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindBuffer(GL_FRAMEBUFFER, myGL.FramebufferName);

	glClearColor(1.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, m_Window.Width(), 0, m_Window.Height());
	glScalef(1, -1, 1);
	glTranslatef(0, -m_Window.Height(), 0);
	glMatrixMode(GL_MODELVIEW);


	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0); glVertex2f(0, 0);
	glTexCoord2d(1.0, 0.0); glVertex2f(1024, 0);
	glTexCoord2d(1.0, 1.0); glVertex2f(1024, 1024);
	glTexCoord2d(0.0, 1.0); glVertex2f(0, 1024);
	glEnd();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);*/

	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, aspect, 1, 10);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -5);

	glRotatef(b, 0, 1, 0);

	b = fmod(b + 0.5, 360.);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, myGL.renderedTexture);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_LIGHTING);

	float cube[][5] =
	{
	{ -0.5, -0.5, -0.5,  0,  0 },
	{ 0.5, -0.5, -0.5,  0.5,  0 },
	{ 0.5,  0.5, -0.5,  0.5,  0.5 },
	{ -0.5,  0.5, -0.5,  0,  0.5 },

	{ -0.5, -0.5,  0.5, -0.5,  0 },
	{ 0.5, -0.5,  0.5,  0,  0 },
	{ 0.5,  0.5,  0.5,  0,  0.5 },
	{ -0.5,  0.5,  0.5, -0.5, 0.5 },
	};
	unsigned int faces[] =
	{
	0, 1, 2, 3,
	1, 5, 6, 2,
	5, 4, 7, 6,
	4, 0, 3, 7,
	3, 2, 6, 7,
	4, 5, 1, 0
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glVertexPointer(3, GL_FLOAT, 5 * sizeof(float), &cube[0][0]);
	glTexCoordPointer(2, GL_FLOAT, 5 * sizeof(float), &cube[0][3]);

	glCullFace(GL_BACK);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, faces);

	glCullFace(GL_FRONT);
	glDrawElements(GL_QUADS, 24, GL_UNSIGNED_INT, faces);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/


	//wglMakeCurrent(NULL, NULL);
	//GLuint shaderProgram = create_program("E:/Jeff/work85vs2015/shaders/StandardShading.vertexshader", "E:/Jeff/work85vs2015/shaders/StandardShading.fragmentshader");

	//GLuint texColorBuffer;
	//glGenTextures(1, &texColorBuffer);
	//glBindTexture(GL_TEXTURE_2D, texColorBuffer);

	//glTexImage2D(
	//	GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
	//);

	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//GLuint rboDepthStencil;
	//glGenRenderbuffers(1, &rboDepthStencil);
	//glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
	//glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600);

	//glFramebufferRenderbuffer(
	//	GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil
	//);

	/*static const GLfloat g_vertex_buffer_data[] = {
	-0.5f,-0.5f,0.5f, // triangle 1 : begin
	-0.5f,-0.5f,0.5f,
	-0.5f, 0.5f,0.5f
	};
	glGenVertexArrays(2, myGL.vaoID);
	glBindVertexArray(myGL.vaoID[1]);
	glGenBuffers(1, &myGL.vaoID[1]); //VBO allocation
	glBindBuffer(GL_ARRAY_BUFFER, myGL.FramebufferName);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);



	//glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);
	//glBindVertexArray(myGL.vboID[1]);
	//glEnable(GL_DEPTH_TEST);
	//glUseProgram(shaderProgram);

	// Clear the screen to white
	//glClearColor(0.5f, 1.0f, 0.8f, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glDrawArrays(GL_TRIANGLES, 0, 1);
	glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glViewport(0, 0, m_Window.Width(), m_Window.Height());*/

	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindVertexArray(myGL.vaoID[1]);
	//glDisable(GL_DEPTH_TEST);
	//glUseProgram(shaderProgram);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texColorBuffer);
	//glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Draw cube scene here

	// Bind default framebuffer and draw contents of our framebuffer
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glBindVertexArray(vaoQuad);
	//glDisable(GL_DEPTH_TEST);
	//glUseProgram(screenShaderProgram);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texColorBuffer);

	//glDrawArrays(GL_TRIANGLES, 0, 6);

	//myFBOtoBitMapFile();
}



void OpenGLRenderer::TrialByFire(float **xArr, int nx, int nt, CWnd *pWnd)
{

	//GLuint shaderProgram = create_program("E:/Jeff/work85vs2015/shaders/StandardShading.vertexshader", "E:/Jeff/work85vs2015/shaders/StandardShading.fragmentshader");
	//GLint position_attribute = glGetAttribLocation(shaderProgram, "position");
	//GLint color_attribute = glGetAttribLocation(shaderProgram, "color");

	// from prepare
	//wglMakeCurrent(myGL.m_hdc, myGL.m_hrc);
	//glClearColor(0.0, 0.8, 1.0, 0.0); //background to clear with.
	//	wglMakeCurrent(NULL, NULL);
	// end prepare

	// copied from triangle
	//wglMakeCurrent(m_hdc, m_hrc);
	int m_GLIntSize = 3; //number of floats per item. i.e. number of floats per point in space.
	int m_GLSizeCount = 1; //number of items. ie sizeof(TriangleA)/m_GLIntSize


						   //GLuint MatrixID = glGetUniformLocation(color_attribute, "MVP");

						   //glDepthMask(GL_FALSE); 
						   //glEnable(GL_DEPTH_TEST);
						   //glDepthFunc(GL_ALWAYS);
						   //glDepthFunc(GL_LESS);
						   //glMatrixMode(GL_PROJECTION);
						   //glLoadIdentity();
						   //glOrtho(-0.6, 0.6, -0.2, 0.8, -0.1, 1.0);
						   //gluLookAt(-1.0, -1.0, -0.5, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0);
						   //glMatrixMode(GL_MODELVIEW);

						   //gluPerspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
						   //gluLookAt(4, 3, -3, 0, 0, 0, 0, 1, 0);
						   //glMatrixMode(1.0f);

						   /*static const GLfloat g_vertex_buffer_data[] = {
						   -0.5f,-0.5f,-0.5f, // triangle 1 : begin
						   -0.5f,-0.5f, 0.5f,
						   -0.5f, 0.5f, 0.5f, // triangle 1 : end
						   0.5f, 0.5f,-0.5f, // triangle 2 : begin
						   -0.5f,-0.5f,-0.5f,
						   -0.5f, 0.5f,-0.5f, // triangle 2 : end
						   0.5f,-0.5f, 0.5f,
						   -0.5f,-0.5f,-0.5f,
						   0.5f,-0.5f,-0.5f,
						   0.5f, 0.5f,-0.5f,
						   0.5f,-0.5f,-0.5f,
						   -0.5f,-0.5f,-0.5f,
						   -0.5f,-0.5f,-0.5f,
						   -0.5f, 0.5f, 0.5f,
						   -0.5f, 0.5f,-0.5f,
						   0.5f,-0.5f, 0.5f,
						   -0.5f,-0.5f, 0.5f,
						   -0.5f,-0.5f,-0.5f,
						   -0.5f, 0.5f, 0.5f,
						   -0.5f,-0.5f, 0.5f,
						   0.5f,-0.5f, 0.5f,
						   0.5f, 0.5f, 0.5f,
						   0.5f,-0.5f,-0.5f,
						   0.5f, 0.5f,-0.5f,
						   0.5f,-0.5f,-0.5f,
						   0.5f, 0.5f, 0.5f,
						   0.5f,-0.5f, 0.5f,
						   0.5f, 0.5f, 0.5f,
						   0.5f, 0.5f,-0.5f,
						   -0.5f, 0.5f,-0.5f,
						   0.5f, 0.5f, 0.5f,
						   -0.5f, 0.5f,-0.5f,
						   -0.5f, 0.5f, 0.5f,
						   0.5f, 0.5f, 0.5f,
						   -0.5f, 0.5f, 0.5f,
						   0.5f,-0.5f, 0.5f
						   };

						   static const GLfloat g_color_buffer_data[] = {
						   0.583f,  0.771f,  0.014f,
						   0.609f,  0.115f,  0.436f,
						   0.327f,  0.483f,  0.844f,
						   0.822f,  0.569f,  0.201f,
						   0.435f,  0.602f,  0.223f,
						   0.310f,  0.747f,  0.185f,
						   0.597f,  0.770f,  0.761f,
						   0.559f,  0.436f,  0.730f,
						   0.359f,  0.583f,  0.152f,
						   0.483f,  0.596f,  0.789f,
						   0.559f,  0.861f,  0.639f,
						   0.195f,  0.548f,  0.859f,
						   0.014f,  0.184f,  0.576f,
						   0.771f,  0.328f,  0.970f,
						   0.406f,  0.615f,  0.116f,
						   0.676f,  0.977f,  0.133f,
						   0.971f,  0.572f,  0.833f,
						   0.140f,  0.616f,  0.489f,
						   0.997f,  0.513f,  0.064f,
						   0.945f,  0.719f,  0.592f,
						   0.543f,  0.021f,  0.978f,
						   0.279f,  0.317f,  0.505f,
						   0.167f,  0.620f,  0.077f,
						   0.347f,  0.857f,  0.137f,
						   0.055f,  0.953f,  0.042f,
						   0.714f,  0.505f,  0.345f,
						   0.783f,  0.290f,  0.734f,
						   0.722f,  0.645f,  0.174f,
						   0.302f,  0.455f,  0.848f,
						   0.225f,  0.587f,  0.040f,
						   0.517f,  0.713f,  0.338f,
						   0.053f,  0.959f,  0.120f,
						   0.393f,  0.621f,  0.362f,
						   0.673f,  0.211f,  0.457f,
						   0.820f,  0.883f,  0.371f,
						   0.982f,  0.099f,  0.879f
						   };*/

						   //glMatrixMode(GL_MODELVIEW);
						   //glPushMatrix();
						   //glTranslatef(0.75, 0.75, 0.75);
						   //glRotatef(0.5, 0.5, 0.5, 1.0);
						   //glTranslatef(-0.75, -0.75, -0.75);
						   //glPopMatrix();

	static const GLfloat g_vertex_buffer_data[] = {
		-0.5f,-0.5f,-0.5f, // triangle 1 : begin
		-0.5f,-0.5f, 0.5f,
		-0.5f, 0.5f, 0.5f
	};


	static const GLfloat g_color_buffer_data[] = {
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f
	};

	//glPushMatrix();
	//glRotatef(45.0, 0.0, 0.0, 0.0);
	//glBegin(GL_POLYGON);
	//glVertex3f(0.5, 0.5, 0.5);
	//glVertex3f(-0.5, 0.5, 0.5);
	//glVertex3f(0.5, -0.5, 0.5);
	//glVertex3f(0.5, 0.5, -0.5);
	//glVertex3f(-0.5, -0.5, 0.5);
	//glVertex3f(-0.5, 0.5, -0.5);
	//glVertex3f(0.5, -0.5, -0.5);
	//glVertex3f(-0.5, -0.5, -0.5);
	//glEnd();
	//glPopMatrix();
	//glFlush();



	//glGenVertexArrays(2, myGL.vaoID);
	//glBindVertexArray(myGL.vaoID[1]);
	//glGenBuffers(1, &myGL.vaoID[1]); //VBO allocation
	//glBindBuffer(GL_ARRAY_BUFFER, myGL.vboID[1]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data) + sizeof(g_color_buffer_data), NULL, GL_STATIC_DRAW);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(g_vertex_buffer_data), g_vertex_buffer_data);
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), sizeof(g_color_buffer_data), g_color_buffer_data);
	//glVertexAttribPointer((GLuint)0, m_GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(color_attribute, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(g_vertex_buffer_data));
	//glEnableVertexAttribArray(color_attribute);
	//glDrawArrays(GL_TRIANGLES, 0, m_GLSizeCount);

	//glClearColor(0, 0.8, 1, 0);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glEnable(GL_COLOR_LOGIC_OP); glLogicOp(GL_XOR);
	//glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);

	/*GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(shaderProgram, "V");
	GLuint ModelMatrixID = glGetUniformLocation(shaderProgram, "M");

	vec3 gPosition1(-1.5f, 0.0f, 0.0f);
	vec3 gOrientation1;

	vec3 gPosition2(1.5f, 0.0f, 0.0f);
	quat gOrientation2;*/

	glGenFramebuffers(1, &myGL.FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);

	glGenTextures(1, &myGL.renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, myGL.renderedTexture);
	//glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, myGL.renderedTexture, 0);
	// Give an empty image to OpenGL ( the last "0" )

	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Window.Width(), m_Window.Height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Poor filtering. Needed !
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glGenRenderbuffers(1, &myGL.depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, myGL.depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Window.Width(), m_Window.Height());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, myGL.depthrenderbuffer);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, myGL.renderedTexture, 0);

	// Set the list of draw buffers.
	//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, DrawBuffers);

	glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);
	//glViewport(0, 0, m_Window.Width(), m_Window.Height());

	GLuint quad_VertexArrayID;
	glGenVertexArrays(1, &quad_VertexArrayID);
	glBindVertexArray(quad_VertexArrayID);


	/*glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 ViewMatrix = glm::lookAt(
	glm::vec3(0.3, 0.8, -7), // Camera is here
	glm::vec3(0.7, 0.2, 0), // and looks here
	glm::vec3(0.3, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);

	gOrientation1.y += 3.14159f / 2.0f;

	// Build the model matrix
	glm::mat4 RotationMatrix = eulerAngleYXZ(gOrientation1.y, gOrientation1.x, gOrientation1.z);
	glm::mat4 TranslationMatrix = translate(mat4(), gPosition1); // A bit to the left
	glm::mat4 ScalingMatrix = scale(mat4(), vec3(1.0f, 1.0f, 1.0f));
	glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;

	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);


	// Send our transformation to the currently bound shader,
	// in the "MVP" uniform
	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);*/


	glGenBuffers(1, &myGL.quad_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, myGL.quad_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, m_GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_TEXTURE_2D, 0);
	glDrawArrays(GL_TRIANGLES, 0, m_GLSizeCount);
	//glDrawElements(GL_TRIANGLES, m_GLSizeCount, GL_ARRAY_BUFFER, 0);

	glFramebufferDrawBufferEXT(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//unsigned char *Buff = new unsigned char[512 * 512 * 3];

	myFBOtoBitMapFile();

	/*glShadeModel(GL_SMOOTH);                    // shading mathod: GL_SMOOTH or GL_FLAT
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment

	// enable /disable features
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);   // enable MSAA

	// track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glClearColor(0, 0, 0, 0);                   // background color
	glClearStencil(0);                          // clear stencil buffer
	glClearDepth(1.0f);                         // 0 is near, 1 is far
	glDepthFunc(GL_LEQUAL);

	GLfloat lightKa[] = { .2f, .2f, .2f, 1.0f };  // ambient light
	GLfloat lightKd[] = { .7f, .7f, .7f, 1.0f };  // diffuse light
	GLfloat lightKs[] = { 1, 1, 1, 1 };           // specular light
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);

	// position the light
	float lightPos[4] = { -1, 1, 1, 0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	glEnable(GL_LIGHT0);

	glGenTextures(1, myGL.vaoID);
	glBindTexture(GL_TEXTURE_2D, myGL.vaoID[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap generation included in OpenGL v1.4
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 200, 200, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenFramebuffers(2, myGL.vboID);
	glBindFramebuffer(GL_FRAMEBUFFER, myGL.vboID[1]);

	glGenRenderbuffers(1, &rboColorId);
	glBindRenderbuffer(GL_RENDERBUFFER, rboColorId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGB8, 200, 200);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenRenderbuffers(1, &rboDepthId);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepthId);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, 200, 200);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rboColorId);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthId);

	glGenFramebuffers(2, &myGL.vboID[1]);
	glBindFramebuffer(GL_FRAMEBUFFER, myGL.vboID[1]);

	glGenRenderbuffers(1, &rboId);
	glBindRenderbuffer(GL_RENDERBUFFER, rboId);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 200, 200);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// attach a texture to FBO color attachement point
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, myGL.vaoID[1], 0);

	// attach a rbo to FBO depth attachement point
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, 200, 200);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)(200) / 200, 1.0f, 100.0f);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glTranslatef(0, 0, -3.5f);

	glBindFramebuffer(GL_FRAMEBUFFER, fboMsaaId);

	// clear buffer
	glClearColor(0, 0.8, 1, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glRotatef(45*0.5f, 1, 0, 0);
	glRotatef(45, 0, 1, 0);
	glRotatef(45*0.7f, 0, 0, 1);
	glTranslatef(0, -1.575f, 0);
	drawTeapot();
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, myGL.vaoID[1]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, (GLsizei)200, (GLsizei)200);

	// set perspective viewing frustum
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, (float)(200) / 200, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip

	// switch to modelview matrix in order to set scene
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0, 0, -3.5f);
	glRotatef(45, 1, 0, 0);   // pitch
	glRotatef(45, 0, 1, 0);   // heading

	// clear framebuffer
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glPushMatrix();

	// draw a cube with the dynamic texture
	glBindTexture(GL_TEXTURE_2D, myGL.vaoID[1]);

	glColor4f(0, 1, 0, 0);
	glBegin(GL_TRIANGLES);
	// front faces
	glNormal3f(0, 0, 1);
	// face v0-v1-v2
	glTexCoord2f(1, 1);  glVertex3f(1, 1, 1);
	glTexCoord2f(0, 1);  glVertex3f(-1, 1, 1);
	glTexCoord2f(0, 0);  glVertex3f(-1, -1, 1);
	// face v2-v3-v0
	glTexCoord2f(0, 0);  glVertex3f(-1, -1, 1);
	glTexCoord2f(1, 0);  glVertex3f(1, -1, 1);
	glTexCoord2f(1, 1);  glVertex3f(1, 1, 1);

	// right faces
	glNormal3f(1, 0, 0);
	// face v0-v3-v4
	glTexCoord2f(0, 1);  glVertex3f(1, 1, 1);
	glTexCoord2f(0, 0);  glVertex3f(1, -1, 1);
	glTexCoord2f(1, 0);  glVertex3f(1, -1, -1);
	// face v4-v5-v0
	glTexCoord2f(1, 0);  glVertex3f(1, -1, -1);
	glTexCoord2f(1, 1);  glVertex3f(1, 1, -1);
	glTexCoord2f(0, 1);  glVertex3f(1, 1, 1);

	// top faces
	glNormal3f(0, 1, 0);
	// face v0-v5-v6
	glTexCoord2f(1, 0);  glVertex3f(1, 1, 1);
	glTexCoord2f(1, 1);  glVertex3f(1, 1, -1);
	glTexCoord2f(0, 1);  glVertex3f(-1, 1, -1);
	// face v6-v1-v0
	glTexCoord2f(0, 1);  glVertex3f(-1, 1, -1);
	glTexCoord2f(0, 0);  glVertex3f(-1, 1, 1);
	glTexCoord2f(1, 0);  glVertex3f(1, 1, 1);

	// left faces
	glNormal3f(-1, 0, 0);
	// face  v1-v6-v7
	glTexCoord2f(1, 1);  glVertex3f(-1, 1, 1);
	glTexCoord2f(0, 1);  glVertex3f(-1, 1, -1);
	glTexCoord2f(0, 0);  glVertex3f(-1, -1, -1);
	// face v7-v2-v1
	glTexCoord2f(0, 0);  glVertex3f(-1, -1, -1);
	glTexCoord2f(1, 0);  glVertex3f(-1, -1, 1);
	glTexCoord2f(1, 1);  glVertex3f(-1, 1, 1);

	// bottom faces
	glNormal3f(0, -1, 0);
	// face v7-v4-v3
	glTexCoord2f(0, 0);  glVertex3f(-1, -1, -1);
	glTexCoord2f(1, 0);  glVertex3f(1, -1, -1);
	glTexCoord2f(1, 1);  glVertex3f(1, -1, 1);
	// face v3-v2-v7
	glTexCoord2f(1, 1);  glVertex3f(1, -1, 1);
	glTexCoord2f(0, 1);  glVertex3f(-1, -1, 1);
	glTexCoord2f(0, 0);  glVertex3f(-1, -1, -1);

	// back faces
	glNormal3f(0, 0, -1);
	// face v4-v7-v6
	glTexCoord2f(0, 0);  glVertex3f(1, -1, -1);
	glTexCoord2f(1, 0);  glVertex3f(-1, -1, -1);
	glTexCoord2f(1, 1);  glVertex3f(-1, 1, -1);
	// face v6-v5-v4
	glTexCoord2f(1, 1);  glVertex3f(-1, 1, -1);
	glTexCoord2f(0, 1);  glVertex3f(1, 1, -1);
	glTexCoord2f(0, 0);  glVertex3f(1, -1, -1);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);

	glPopMatrix();*/

	/*glDrawBuffer(GL_BACK);
	glReadBuffer(GL_BACK);

	static float	*buffer;
	buffer = new float[1920000];

	glReadPixels(0, 0, 800, 600, GL_COLOR_INDEX, GL_FLOAT, buffer);

	HBITMAP screenImage = CreateBitmap(800, 600, 1, 32, buffer);

	CRect WinRect;
	GetClientRect(&WinRect);

	CDC* pMemDC;
	pMemDC = new CDC();
	pMemDC->CreateCompatibleDC(pWnd);
	CBitmap* pNewBitmap = new CBitmap();
	pNewBitmap->CreateCompatibleBitmap(pWnd, WinRect.Width(), WinRect.Height());
	pMemDC->SelectObject(pNewBitmap);

	CRect BMPRect;
	GetBitmapDrawSize(&BMPRect);
	if (BMPRect != NULL)
	{
	if (!(BMPRect.Width() >= WinRect.Width() && BMPRect.Height() >= WinRect.Height()))
	{
	//The bitmap is smaller than the window, so fill the window with the background color.
	CBrush BackBrush(BackGndColor);
	CPen BackPen(PS_SOLID, 1, BackGndColor);
	pMemDC->SelectObject(&BackBrush);
	pMemDC->SelectObject(&BackPen);
	pMemDC->Rectangle(&WinRect);
	BackBrush.DeleteObject();
	BackPen.DeleteObject();
	}
	}

	OverrideAndDrawInHere(pMemDC, resize);
	pDrawDC->BitBlt(0, 0, WinRect.right, WinRect.bottom, pMemDC, 0, 0, SRCCOPY);*/

	//SwapBuffers(myGL.m_hdc);
	//wglMakeCurrent(NULL, NULL);
}
void OpenGLRenderer::myFBOtoBitMapFile()
{
	byte* Buff = new byte[2512 * 2512 * 3];
	if (!Buff)
		return;


	glReadBuffer(GL_BACK);
	glReadPixels(0, 0, m_Window.Width(), m_Window.Height(), GL_BGR_EXT, GL_UNSIGNED_BYTE, Buff);

	FILE *Out = fopen("E:/Jeff/abc.bmp", "wb");
	if (!Out)
		return;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;

	bitmapFileHeader.bfType = 0x4D42;
	bitmapFileHeader.bfSize = m_Window.Width()*m_Window.Height() * 3;
	bitmapFileHeader.bfReserved1 = 0;
	bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits =
		sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = m_Window.Width() - 1;
	bitmapInfoHeader.biHeight = m_Window.Height() - 1;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 8; //24
	bitmapInfoHeader.biCompression = 0; //BI_RGB
	bitmapInfoHeader.biSizeImage = 0;
	bitmapInfoHeader.biXPelsPerMeter = 0; // ?
	bitmapInfoHeader.biYPelsPerMeter = 0; // ?
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;

	fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, Out);
	fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, Out);
	fwrite(Buff, m_Window.Width()*m_Window.Height() * 3, 1, Out);
	fclose(Out);

	delete[] Buff;
}
//
//void OpenGLRenderer::mySetData(int iType)
//{
//	myDrawLine(); return;
//
//	m_iShapeType = iType;
//	switch(iType)
//	{
//		case 1:
//			{
//			// SQUARE
//				mySetSquare();
//			}
//			break;
//		case 3:
//			{
//			// TRIANGLE
//				mySetTriangle();
//			}
//			break;
//		default:
//			mySetTriangle();
//	}
//
//}
//
//
//void OpenGLRenderer::mySetSquare()
//{
//	wglMakeCurrent(m_hdc, m_hrc);
//
//		// First simple object
//		m_GLSizeCount = 4;
//		m_GLIntSize = 4;
//		GLfloat Square[] = 
//		{
//			-0.5f, -0.5f, 0.0f, 1.0f,
//			-0.5f,  0.5f, 0.0f, 1.0f,
//			0.5f,  0.5f, 0.0f, 1.0f,
//			0.5f, -0.5f, 0.0f, 1.0f
//		};
//
//		//VAO allocation
//		glGenVertexArrays(1, &m_vaoID[0]);
// 
//		// First VAO setup
//		glBindVertexArray(m_vaoID[0]);
//		glGenBuffers(1, &m_vboID[0]);
//		glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(Square), Square, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, m_GLIntSize, GL_FLOAT, GL_FALSE, 0, 0); 
//		glEnableVertexAttribArray(0);
//	
//	wglMakeCurrent(NULL, NULL);
//}

// Deping test
//void OpenGLRenderer::myDrawLine()
//{
//	wglMakeCurrent(m_hdc, m_hrc);
//
//	//glLineWidth(2.5);
//	//glColor3f(1.0, 0.0, 0.0);
//	//glBegin(GL_LINES);
//	//glVertex3f(0.0, 0.0, 0.0);
//	//glVertex3f(15, 0, 0);
//	//glEnd();
//	
//	// First simple object
//	m_GLSizeCount = 4;
//	m_GLIntSize = 4;
//	GLfloat myArr[] =
//	{
//		-0.5f, -0.5f, 0.0f, 1.0f,
//		-0.5f,  0.5f, 0.0f, 1.0f,
//		0.5f,  0.5f, 0.0f, 1.0f,
//		 0.5f, -0.5f, 0.0f, 1.0f
//	};
//
//	//VAO allocation
//	glGenVertexArrays(1, &m_vaoID[0]);
//
//	// First VAO setup
//	glBindVertexArray(m_vaoID[0]);
//	glGenBuffers(1, &m_vboID[0]);
//	glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);
//	int gter = sizeof(myArr);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(myArr), myArr, GL_STATIC_DRAW);
//	glVertexAttribPointer((GLuint)0, m_GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);
//	glEnableVertexAttribArray(0);
//	wglMakeCurrent(NULL, NULL);
//}


//
//
//void OpenGLRenderer::mySetTriangle()
//{
//	wglMakeCurrent(m_hdc, m_hrc);
//
//		m_GLIntSize = 4; //number of floats per item. i.e. number of floats per point in space.
//		m_GLSizeCount = 3; //number of items. ie sizeof(TriangleA)/m_GLIntSize
//
//		GLfloat TriangleA[] = 
//		{
//			-0.3f, 0.5f, -1.0f, 1.0f,
//			-0.8f, -0.5f, -1.0f, 1.0f,
//			0.2f, -0.5f, -1.0f, 1.0f
//		};
//	
//		GLfloat TriangleB[] = 
//		{
//			-0.2f,  0.5f, -1.0f, 1.0f,
//			0.3f,  -0.5f, -1.0f, 1.0f,
//			0.8f, 0.5f, -1.0f, 1.0f,
//		};
//
//		//VAOs allocation
//		glGenVertexArrays(2, &m_vaoID[0]);
//
//		// First VAO setup
//		glBindVertexArray(m_vaoID[0]);
//		glGenBuffers(1, m_vboID); //VBO allocation
//		glBindBuffer(GL_ARRAY_BUFFER, m_vboID[0]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleA), TriangleA, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, m_GLIntSize, GL_FLOAT, GL_FALSE, 0, 0); 
//		glEnableVertexAttribArray(0);
//
//		// Second VAO setup     
//		glBindVertexArray(m_vaoID[1]);
//		glGenBuffers(1, &m_vboID[1]); //VBO allocation
//		glBindBuffer(GL_ARRAY_BUFFER, m_vboID[1]);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleB), TriangleB, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, m_GLIntSize, GL_FLOAT, GL_FALSE, 0, 0); 
//		glEnableVertexAttribArray(0);
//
//	wglMakeCurrent(NULL, NULL);
//}


void OpenGLRenderer::DestroyScene()
{
	if (myGL.numvo == 0) return;
	DestroyWindow();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(myGL.numvo, myGL.vboID);

	glBindVertexArray(0);
	glDeleteVertexArrays(myGL.numvo, myGL.vaoID);
	if (myGL.vaoID) free1(myGL.vaoID); myGL.vaoID = NULL;
	if (myGL.vboID) free1(myGL.vboID); myGL.vboID = NULL;
	myGL.numvo = 0;
	wglMakeCurrent(NULL, NULL);
	glDeleteFramebuffers(myGL.FramebufferName, 0);

	if (myBmp.m_hMemRC)
	{
		wglDeleteContext(myBmp.m_hMemRC);
		myBmp.m_hMemRC = NULL;
	}

	//if (myGL.numvo == 0) return;
	//DestroyWindow();

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
 //   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
 //   glDeleteBuffers(myGL.numvo, myGL.vboID);

 //   glBindVertexArray(0);
 //   glDeleteVertexArrays(myGL.numvo, myGL.vaoID);
	//if (myGL.vaoID) free1(myGL.vaoID); myGL.vaoID = NULL;
	//if (myGL.vboID) free1(myGL.vboID); myGL.vboID = NULL;
	//myGL.numvo = 0;

	//glDeleteFramebuffers(myGL.FramebufferName, 0);

	//wglMakeCurrent(NULL, NULL);
	//if(myBmp.m_hMemRC)
	//{
	//	wglDeleteContext(myBmp.m_hMemRC);
	//	myBmp.m_hMemRC = NULL;
	//}
}


void OpenGLRenderer::DestroySceneFBO()
{
	if (myGL.numvo == 0) return;
	DestroyWindow();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(myGL.numvo, myGL.vboID);

	glBindVertexArray(0);
	glDeleteVertexArrays(myGL.numvo, myGL.vaoID);
	if (myGL.vaoID) free1(myGL.vaoID); myGL.vaoID = NULL;
	if (myGL.vboID) free1(myGL.vboID); myGL.vboID = NULL;
	myGL.numvo = 0;
	wglMakeCurrent(NULL, NULL);
	glDeleteFramebuffers(myGL.FramebufferName, 0);

	if (myBmp.m_hMemRC)
	{
		wglDeleteContext(myBmp.m_hMemRC);
		myBmp.m_hMemRC = NULL;
	}
}

