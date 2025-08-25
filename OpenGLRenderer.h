#pragma once

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib") //make sure project settings have: Linker -> Additional Library Directories -> include path for library.
#include <GL/glew.h>
#include <GL/wglew.h>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>

// OpenGLRenderer

class OpenGLRenderer : public CWnd
{
public:
	OpenGLRenderer();
	virtual ~OpenGLRenderer();
	bool CreateGLContext(CRect rect, CWnd *parent, int);
	bool CreateGLContextFBO(CRect rect, CWnd *parent, int);
	bool CreateGLContextBmp(CRect rect, CWnd *parent, int);
	void PrepareScene();				// Scene preparation
	void Reshape(UINT nType, int w, int h); //maps to OnSize()
	void DestroyScene();            // Cleanup
	void DestroySceneFBO();            // Cleanup
									//	CRect &GetWinRect() { return m_WindowOrig; };
	CRect m_Window;
	//	CRect m_WindowOld;
	//	CRect m_WindowOrig;

	struct GLRES {
	//	HGLRC m_hrc;                        // OpenGL Rendering Context 
	//	HDC m_hdc;

		GLuint FramebufferName;
		GLuint renderedTexture;
		GLuint depthrenderbuffer;
		GLuint quad_vertexbuffer;
		int numvo;
		GLuint *vaoID;                      // numvo vertex array objects
		GLuint *vboID;                      // numvo VBOs
		GLRES() :
			numvo(0), vaoID(NULL), vboID(NULL), FramebufferName(0) {
		}
	} myGL;


	struct BMPRES {
		HDC			m_hMemDC;
		HGLRC		m_hMemRC;
		BITMAPINFO	m_bmi;
		LPVOID		m_pBitmapBits;
		HBITMAP		m_hDib;
		CSize		m_szPage;

		BMPRES() {
			m_hDib = NULL;
			m_pBitmapBits = NULL;
			m_hMemDC = NULL;
			m_hMemRC = NULL;
		}

		~BMPRES() {
			if (m_hDib != NULL) {
				DeleteObject(m_hDib);
				m_hDib = NULL;
			}
			if (m_hMemRC != NULL) {
				::wglDeleteContext(m_hMemRC);
				m_hMemRC = NULL;
			}
			if (m_hMemDC != NULL) {
				DeleteDC(m_hMemDC);
				m_hMemDC = NULL;
			}
		}
	} myBmp;

//	UINT_PTR m_unpTimer;


	void OnPrint1(CDC* pDC, CPrintInfo* pInfo, CView* pView);
	void OnPrint2(CDC* pDC);

	// these are what I will need to modify
	void myDrawSceneBasic1();				// Draw scene
	void myDrawSceneLines(float **, float [], float [], int, int, double);				// Draw scene
	void myDrawSceneFills(float **xArr, int nx, int nt);				// Draw scene

	void TrialByFire(float **xArr, int nx, int nt, CWnd *pWnd);
	void FirstFrameBufferImage(float **xArr, int nx, int nt, CWnd *pWnd);
	void myFBOtoBitMapFile();


	HBITMAP ReadPixelsToHBITMAP(
		HDC hdc,
		int x,
		int y,
		int width,
		int height);

	// void mySetData(int iType);  // Creates VAO and VBOs and fill them with data
	//void mySetTriangle();
	// void mySetSquare();
protected:
	bool InitContext();					 // Creates OpenGL Rendering Context
	bool InitContextFBO();					 // Creates OpenGL Rendering Context
	bool InitContextBmp(HDC hDC, DWORD dwFlags);

	afx_msg void OnSize(UINT nType, int cx, int cy);  // Changing viewport
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	static void read_shader_src(const char *fname, std::vector<char> &buffer);
	// Compile a shader
	static GLuint load_and_compile_shader(const char *fname, GLenum shaderType);

	// Create a program from two shaders
	static GLuint create_program(const char *path_vert_shader, const char *path_frag_shader);

	DECLARE_MESSAGE_MAP()
};
