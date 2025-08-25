#pragma once

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib") //make sure project settings have: Linker -> Additional Library Directories -> include path for library.
#include <GL/glew.h>
#include <GL/wglew.h>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "colorHelper.h"
// OpenGLRenderer

class OpenGLRendererFBO : public CWnd
{
public:
	OpenGLRendererFBO();
	virtual ~OpenGLRendererFBO();
	bool CreateGLContextFBO(CRect, CRect, CWnd *parent, CDC*, int, bool, bool);
	//void PrepareScene2();				// Scene preparation
	//void Reshape(UINT nType, int w, int h); //maps to OnSize()
	void DestroySceneFBO();            // Cleanup
									//	CRect &GetWinRect() { return m_WindowOrig; };
	struct GLMAP {
		SEI_TRIPLE camPosition;
		SEI_TRIPLE viewAt;
		SEI_TRIPLE headsUp;
		SEI_TRIPLE lightInvDir;
		CString m_f;

		GLMAP() {
			// initialized matrixes are as below (by default)
			camPosition.set(0.1, 0.2, 20); // Camera is at camPosition (0.1, 0.2, 20), in World Space
			viewAt.set(0., 0., 0.); // viewAt=camPosition+direction, so direction=(-0.1, -0.2, -20)
			headsUp.set(0., 1., 0.); // Head is up (set to 0,-1,0 to look upside-down)
			lightInvDir.set(8., 3., 5.);
		}

		// when norm=100., this will transform [-100., 100] to [0., 1.]
		inline double getValueNormed(double a, double norm) {
			if (norm < 0.00001) norm = 0.00001;
			return (a + norm) / (2.*norm);
		}

		// when norm=100., this will transform [0., 1.] back to [-100., 100]
		inline double getValueOrig(double b, double norm) {
			if (norm < 0.00001) norm = 0.00001;
			return (2. * b - 1) * norm;
		}
		inline SEI_TRIPLE calc_barycentric_weights(
			double x, double y, 
			double v1x, double v1y, 
			double v2x, double v2y, 
			double v3x, double v3y) {
			double w_v1 = (x*(v3y - v2y) + v2x * (y - v3y) + v3x * (v2y - y)) / (v1x*(v3y - v2y) + v2x * (v1y - v3y) + v3x * (v2y - v1y));
			double w_v2 = -(x*(v3y - v1y) + v1x * (y - v3y) + v3x * (v1y - y)) / (v1x*(v3y - v2y) + v2x * (v1y - v3y) + v3x * (v2y - v1y));
			double w_v3 = 1.0 - w_v1 - w_v2;
			SEI_TRIPLE out;
			out.set(w_v1, w_v2, w_v3);
			return out;
		}

		inline bool loadGLmapping(CString f) {
			CString colFileName = GetPathFromFileSlash(f) + _T("3dMapping.txt");
			CStringArray *arr = getLinesFromFile(colFileName, false); // no comments will be read in here
			bool isTrue = false;
			if (arr == NULL || arr->GetSize() < 4) {
				// file not existing
			}
			else {
				_stscanf_s(arr->ElementAt(0).Trim(), _T("%lg%lg%lg"), &camPosition.x, &camPosition.y, &camPosition.z);
				_stscanf_s(arr->ElementAt(1).Trim(), _T("%lg%lg%lg"), &viewAt.x, &viewAt.y, &viewAt.z);
				_stscanf_s(arr->ElementAt(2).Trim(), _T("%lg%lg%lg"), &headsUp.x, &headsUp.y, &headsUp.z);
				_stscanf_s(arr->ElementAt(3).Trim(), _T("%lg%lg%lg"), &lightInvDir.x, &lightInvDir.y, &lightInvDir.z);
				isTrue = true;
			} // else
			if (arr) {
				arr->RemoveAll();
				delete arr;
			}
			m_f = f;
			return isTrue;
		}
		inline bool saveGLmapping() {
			CString colFileName = GetPathFromFileSlash(m_f) + _T("3dMapping.txt");

			FILE *fp;
			if (_tfopen_s(&fp, colFileName, _T("w")) != 0) return false; // if the file cannot be written, we stop here right away.

			CString tStr;
			CStringArray *arr = new CStringArray();
			tStr.Format(_T("#Auto generated file for 3d mapping, to be shared to all segy files in the same directory."));
			arr->Add(tStr);
			tStr.Format(_T("#Only 3 space-deliminated float numbers per line for 4 lines, describing camPosition, viewAt, headsUp, and lightInvDir respectively."));
			arr->Add(tStr);
			tStr.Format(_T("#References: https://learnopengl.com/Getting-started/Camera"));
			arr->Add(tStr);
			tStr.Format(_T("#defaults: 0.1  0.2  20\n#0.  0.  0.\n#0.  1.  0.\n#8.  3.  5.\n#(You may manually edit them manually.)"));
			arr->Add(tStr);
			tStr.Format(_T("%lg  %lg  %lg"), camPosition.x, camPosition.y, camPosition.z);
			arr->Add(tStr);
			tStr.Format(_T("%lg  %lg  %lg"), viewAt.x, viewAt.y, viewAt.z);
			arr->Add(tStr);
			tStr.Format(_T("%lg  %lg  %lg"), headsUp.x, headsUp.y, headsUp.z);
			arr->Add(tStr);
			tStr.Format(_T("%lg  %lg  %lg"), lightInvDir.x, lightInvDir.y, lightInvDir.z);
			arr->Add(tStr);
			saveStringArrayToFile(arr, fp);

			// clean up
			fclose(fp);
			arr->RemoveAll();
			delete arr;
			return true;
		}

	} glMap;

	struct GLRES {
		// use these 3 variables to test opengl data states
		bool isOpenGLWindowsExists;
		GLuint gpuProgramID; // data already loaded if !=0
		unsigned int nx; // for backup and later usage
		unsigned int nt; // for backup
		unsigned int numElem; // actual num of drawing elements in GMU memory, should be >0 if any data existing

		GLuint vbo1;
		GLuint vbo2;
		GLuint vbo3;
		GLuint vbo0;
		GLuint elementbuffer1;

		// window controls
		CRect m_rectPixel;
		CRect m_rectDraw; // should be in client coordinates, i.e. 35*m_rectPixel
		HGLRC		m_hMemRC;
		HDC m_hDC;

		// shaders et al.
		CString shaderFolderSlash; // stores the shader folder in absolute path format with a ending back slash
		GLuint FramebufferName;
		bool m_isFaceUp;
		bool m_isSwapX;
		CRect buttons[12];
		CRect buttonsFrame;
		//CRect joystick;

		CRect camXYFrame;
		CRect camZFrame;
		CRect lightXYFrame;
		CRect lightZFrame;

		// now for coloring
		//CRect hueFrame;
		//CRect satFrame;
		//CRect valueFrame;
		//byte hue;
		//byte saturation;
		//byte value;

		bool m_isGLButtonsShowing; // will be decided based on variable buttonsFrame in OnMouseMove()
		GLRES() :
			isOpenGLWindowsExists(false)
			, gpuProgramID(0)
			, numElem(0)
			, m_isFaceUp(false)
			, m_isSwapX(false)
			, m_isGLButtonsShowing(false)
			, FramebufferName(0) 
		{
			if (m_hMemRC != NULL) {
				::wglDeleteContext(m_hMemRC);
				m_hMemRC = NULL;
			}

			buttonsFrame.SetRect(20, 350, 4300, 6150);

			// 6 left buttons for marking -
			buttons[0].SetRect(20, 400, 820, 1200);
			buttons[1].SetRect(20, 1300, 820, 2100);
			buttons[2].SetRect(20, 2200, 820, 3000);
			buttons[3].SetRect(20, 3100, 820, 3900);
			buttons[4].SetRect(20, 4000, 820, 4800);
			buttons[5].SetRect(20, 4900, 820, 5700);

			// 6 right buttons for marking +
			buttons[6].SetRect(920, 400, 1720, 1200);
			buttons[7].SetRect(920, 1300, 1720, 2100);
			buttons[8].SetRect(920, 2200, 1720, 3000);
			buttons[9].SetRect(920, 3100, 1720, 3900);
			buttons[10].SetRect(920, 4000, 1720, 4800);
			buttons[11].SetRect(920, 4900, 1720, 5700);

			//joystick.SetRect(20, 6200, 4300, 10000);
			camXYFrame.SetRect(20, 6200, 4300, 9400);
			camZFrame.SetRect(20, 9400, 4300, 10000);
			lightXYFrame.SetRect(20, 10000, 4300, 13200);
			lightZFrame.SetRect(20, 13200, 4300, 13800);

			// color options
			//hueFrame.SetRect(20, 13800, 4300, 14400);
			//hue = 100; // default 100, [0,100]
			//satFrame.SetRect(20, 14400, 4300, 15000);
			//saturation = 100; // default 100, [0,100]
			//valueFrame.SetRect(20, 15000, 4300, 15600);
			//value = 100; // default 100, [0,100]
		}
	} myGL;

	void createShaderProgram_Notused(const GLchar* vertSrc, const GLchar* fragSrc,
		GLuint& vertexShader, GLuint& fragmentShader, GLuint& shaderProgram);
	GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);
	void myFBOtoBitMapFile(CString);
	bool myFBOtoDC(CDC*, CRect* rectDraw, DWORD);
	void mySeis3DFBO_Init(float **ampArr, CColorHelper::AMPCOLORDYN *ampColor, float x[], float y[], int nx, int nt);
	void mySeis3DFBO_Animate(CDC*, int, CRect* rectDraw);
	void mySeis3DFBO_Buttons(CDC* pDC, double);
	void mySeis3DFBO_joystick(CDC* pDC);

	bool mySeis3DFBO_ButtonDragDrop(CPoint p1, CPoint p2); // if return true, the screen needs to refresh
	CString mySeis3DFBO_ButtonClick(CPoint p); // if return true, the screen needs to refresh
	void mySeis3DFBO_End();


	//void myDrawSceneBasic1();				// Draw scene
	//void myDrawSceneLines(float **, float [], float [], int, int, double);				// Draw scene
	//void myDrawSceneLinesFBO(float **, float [], float [], int, int, double);				// Draw scene
	//void myDrawSceneFills(float **xArr, int nx, int nt);				// Draw scene
	//void FirstFrameBufferImage(float **xArr, int nx, int nt);
	//void myTestFBO(float **ampArr, CColorHelper::AMPCOLORDYN *ampColor, float x[], float y[], int nx, int nt, double dxMin4Plot, double dyMin4Plot);
	//void mySeis3DFBO(float **ampArr, CColorHelper::AMPCOLORDYN *ampColor, float x[], float y[], int nx, int nt);
	// void myTest3FBO(float **ampArr, CColorHelper::AMPCOLORDYN *ampColor, float x[], float y[], int nx, int nt, double dxMin4Plot, double dyMin4Plot);
	// void depSeisFBO(float **ampArr, CColorHelper::AMPCOLORDYN *ampColor, float x[], float y[], int nx, int nt, float, float);
	void depSeisFBO3(CColorHelper::rgb **colorsArr, CDC*, float x[], float y[], int nx, int nt, float, float);
	//void depSeisFBOBackup(float **ampArr, CColorHelper::AMPCOLORDYN *ampColor, float x[], float y[], int nx, int nt, double dxMin4Plot);

	HBITMAP ReadPixelsToHBITMAP_NotUsed(
		HDC hdc,
		int x,
		int y,
		int width,
		int height);

	// void mySetData(int iType);  // Creates VAO and VBOs and fill them with data
	//void mySetTriangle();
	// void mySetSquare();
protected:
	bool InitContextFBO();					 // Creates OpenGL Rendering Context

	//afx_msg void OnSize(UINT nType, int cx, int cy);  // Changing viewport

	static void read_shader_src(const char *fname, std::vector<char> &buffer);
	// Compile a shader
	static GLuint load_and_compile_shader(const char *fname, GLenum shaderType);

	const bool isDownDip(float trcA[], float trcB[], int ns, int iFocus);

	void CalculateVectorNormal_Notused(GLfloat vertex1x, GLfloat vertex1y,
		GLfloat vertex1z, GLfloat vertex2x, GLfloat vertex2y,
		GLfloat vertex2z, GLfloat vertex3x, GLfloat vertex3y,
		GLfloat vertex3z, GLfloat vertex4x, GLfloat vertex4y,
		GLfloat vertex4z, GLfloat vertex5x, GLfloat vertex5y,
		GLfloat vertex5z, GLfloat *fNormalX,
		GLfloat *fNormalY, GLfloat *fNormalZ);

	void CalculateVectorNormal3_Notused(GLfloat fVert1[], GLfloat fVert2[],
		GLfloat fVert3[], GLfloat *fNormalX,
		GLfloat *fNormalY, GLfloat *fNormalZ);

	//void CalculateVectorNormal3(
	//	GLfloat fVert0X, GLfloat fVert0Y, GLfloat fVert0Z,
	//	GLfloat fVert1X, GLfloat fVert1Y, GLfloat fVert1Z,
	//	GLfloat fVert2X, GLfloat fVert2Y, GLfloat fVert2Z,
	//	GLfloat dx, GLfloat dy,
	//	GLfloat *fNormalX, GLfloat *fNormalY, GLfloat *fNormalZ);

	void CalculateVectorNormal9(
		GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat,
		GLfloat dx, GLfloat dx2, GLfloat dy, bool isDownHill,
		GLfloat *fNormalX, GLfloat *fNormalY, GLfloat *fNormalZ);


	// Create a program from two shaders
	static GLuint create_program(const char *path_vert_shader, const char *path_frag_shader);

	DECLARE_MESSAGE_MAP()
};
