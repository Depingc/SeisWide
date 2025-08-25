// OpenGLRenderer.cpp : implementation file
//

#include "stdafx.h"
//#include "OpenGL_MFCDialog.h"
#include "OpenGLRendererFBO.h"
#include "ealloc_d.h"
#include "dcHelper.h"
#include "util.h"
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

OpenGLRendererFBO::OpenGLRendererFBO()
{
	myGL.m_rectPixel = CRect(0, 0, 200, 200);
}

OpenGLRendererFBO::~OpenGLRendererFBO()
{
	mySeis3DFBO_End();
	DestroySceneFBO();
}


BEGIN_MESSAGE_MAP(OpenGLRendererFBO, CWnd)
//	ON_WM_SIZE()
//	ON_WM_TIMER()
END_MESSAGE_MAP()


#include "AutoUpdater.h"
bool OpenGLRendererFBO::CreateGLContextFBO(CRect rectPixel, CRect rectDraw, CWnd *parent, CDC* parentDC, 
	int numcurve, bool isFaceUp, bool isSwapX) {

	// make sure the video card's memory can handle this display task with rectPixel pixels
	// No. This does not work.

	if (myGL.isOpenGLWindowsExists) {
		mySeis3DFBO_End();
		DestroySceneFBO();
	}

	// make sure the shader files are properly placed, otherwise download them.
	myGL.shaderFolderSlash = getSeisWideParaPath() + _T("\\shaders\\");
	if (!isDirectoryExist(myGL.shaderFolderSlash)) {
		CreateDirectory(myGL.shaderFolderSlash, NULL);
	}
	if (!isDirectoryExist(myGL.shaderFolderSlash)) {
		AfxMessageBox(_T("3D display of segy data is working. But it requires extra files that cannot be auto updated. Please download full Seiswide from http://seiswide.hopto.org ."), MB_ICONQUESTION);
		return false;
	}
	CString s1 = myGL.shaderFolderSlash + "ShadowMapping.vertexshader";
	if (!isFileExistNotDirectory(s1)) {
		CAutoUpdater updater;
		CString serverAddr = _T("http://seiswide.hopto.org/seiswide/shaders/");
		updater.JustUpdate(serverAddr, s1);
		CString s2 = myGL.shaderFolderSlash + "ShadowMapping.fragmentshader";
		updater.JustUpdate(serverAddr, s2);
	}




	// now start window creation
	CString className = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_OWNDC, NULL, (HBRUSH)GetStockObject(WHITE_BRUSH), NULL); //default background colour

	CreateEx(0,
		className,
		_T("OpenGL with MFC/CDialog"),
		// WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
		 WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		rectPixel,
		parent,
		0);

	// rectPixel is for axes, and we should not include area outside its borders
	myGL.m_rectPixel.right = conditionPowerTwo(rectPixel.Width());
	myGL.m_rectPixel.bottom = conditionPowerTwo(rectPixel.Height());
	myGL.m_rectPixel.left = myGL.m_rectPixel.top = 0;

	myGL.m_rectDraw.right = rectDraw.Width();
	myGL.m_rectDraw.bottom = rectDraw.Height();
	myGL.m_rectDraw.left = myGL.m_rectDraw.top = 0;


	myGL.m_isFaceUp = isFaceUp;
	myGL.m_isSwapX = isSwapX;
	if (!InitContextFBO()) {
		MessageBox(_T("ERROR Creating InitContext"));
		return false;
	};

	myGL.isOpenGLWindowsExists = true; 
	return true;
}


bool OpenGLRendererFBO::InitContextFBO()
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

	//CDC *glDC = GetDC();
	myGL.m_hDC = GetDC()->m_hDC;

	int nPixelFormat = ChoosePixelFormat(myGL.m_hDC, &pfd);
	if (nPixelFormat == 0) return false;
	BOOL bResult = SetPixelFormat(myGL.m_hDC, nPixelFormat, &pfd);
	if (!bResult) return false;

	HGLRC tempContext = wglCreateContext(myGL.m_hDC);
	wglMakeCurrent(myGL.m_hDC, tempContext);

	GLenum GlewInitResult;
	glewExperimental = GL_TRUE;
	GlewInitResult = glewInit();

	if (GlewInitResult != GLEW_OK) {
		AfxMessageBox(_T("GLEW is not initialized!"));
	}

	int attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 2,
		WGL_CONTEXT_MINOR_VERSION_ARB, 1,
		WGL_CONTEXT_FLAGS_ARB, 0,
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1) {
		myGL.m_hMemRC = wglCreateContextAttribsARB(myGL.m_hDC, 0, attribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(myGL.m_hDC, myGL.m_hMemRC);
	}
	else {       //It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		myGL.m_hMemRC = tempContext;
	}

	if (!myGL.m_hMemRC)
		return false;

	CString str;
	str.Format(_T("OpenGL version: %s\n"), (CString)glGetString(GL_VERSION));
	TRACE(str);
	return true;
}




void OpenGLRendererFBO::read_shader_src(const char *fname, std::vector<char> &buffer) {
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
GLuint OpenGLRendererFBO::load_and_compile_shader(const char *fname, GLenum shaderType) {
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
GLuint OpenGLRendererFBO::create_program(const char *path_vert_shader, const char *path_frag_shader) {
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
HBITMAP OpenGLRendererFBO::ReadPixelsToHBITMAP_NotUsed(
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



#include <string>
GLuint OpenGLRendererFBO::LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {
	// Create the shaders

//	GLuint ProgramID2 = glCreateProgram();
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		getchar();
		return 0;
	}


	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;


	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}



	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();

	// check if this programID is already attached with 4 attributes
	//GLint x[4];
	//glGetIntegerv(GL_VIEWPORT, &x[0]);
	//if (x[2] > 0) {
	//	int gfer = 1;
	//}
	//glGetProgramiv(ProgramID, GL_ACTIVE_ATTRIBUTES, &Result);
	//if (Result > 0) {
	//	int sdfsd = 1;
	//}



	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}


	//glDetachShader(ProgramID, VertexShaderID);
	//glDetachShader(ProgramID, FragmentShaderID);
	//glDeleteShader(VertexShaderID);
	//glDeleteShader(FragmentShaderID);

	return ProgramID;
}


void OpenGLRendererFBO::createShaderProgram_Notused(const GLchar* vertSrc, const GLchar* fragSrc,
	GLuint& vertexShader, GLuint& fragmentShader, GLuint& shaderProgram)
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


#include "DIB.h"
bool OpenGLRendererFBO::myFBOtoDC(CDC* pDC, CRect* rectDraw, DWORD srcMode)
{
	CDib dib;

	// note: after this call, nrow or ncol can change
	//int ncolDC = rectPixel->Width();
	//int nrowDC = rectPixel->Height();
	int ncolGL = myGL.m_rectPixel.Width();
	int nrowGL = myGL.m_rectPixel.Height();
	float reduceRate = dib.Create24b(ncolGL, nrowGL);
	if (reduceRate == 0.f) {
		return false;
	}
	dib.SetPalette(pDC);

	byte *buff = (byte *)dib.getBits();
	//	glReadBuffer(GL_BACK);

	flushGLErrors();
	glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
	glPixelStorei(GL_PACK_LSB_FIRST, GL_TRUE);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	//glReadPixels(0, 0, ncol, nrow, GL_BGR_EXT, GL_UNSIGNED_BYTE, buff);
	glReadPixels(0, 0, ncolGL, nrowGL, GL_BGR, GL_UNSIGNED_BYTE, buff);



	// try to modify the *buff
	//int ncol = this->myGL.m_rectPixel.Width(); //
	//int nrow = this->myGL.m_rectPixel.Height();
	//myGL.illumination = 1;
	//double fHue = myGL.hue * 0.01; // / (1. + myGL.hue);
	//double fSat = myGL.saturation * 0.01; // / (1. + myGL.saturation);
	//double fValue = myGL.value * 0.01; // / (1. + myGL.value);
	//WORD b;
	//COLORREF col;
	//CColorHelper::HsvColor ahsv;
	//if (fHue < 0.98 || fSat < 0.98 || fValue < 0.98) {
	//	for (int i = 0; i < nrowGL; i++) {
	//		//break;
	//		register int nn = i * ncolGL * 3;
	//		for (int j = 0; j < ncolGL; j++) {
	//			register int k = nn + j * 3;
	//			col = RGB(buff[k + 2], buff[k + 1], buff[k]);
	//			ahsv = CColorHelper::RgbToHsvi(col);
	//			b = (WORD)(ahsv.h*fHue + 0.5); if (b > 255) b = 255;
	//			ahsv.h = (byte)b;
	//			b = (WORD)(ahsv.s*fSat + 0.5); if (b > 255) b = 255;
	//			ahsv.s = (byte)b;
	//			b = (WORD)(ahsv.v*fValue + 0.5); if (b > 255) b = 255;
	//			ahsv.v = (byte)b;
	//			col = CColorHelper::HsvToRgbi(ahsv);
	//			buff[k] = GetBValue(col);
	//			buff[k + 1] = GetGValue(col);
	//			buff[k + 2] = GetRValue(col);
	//		}
	//	}
	//}
	// end of modify *buff



	CDC aDC;
	CDCHelper::myCreateTempDC(pDC, &aDC, myGL.m_rectPixel, myGL.m_rectDraw);
	if (!dib.Draw(&aDC, 0, 0,
		myGL.m_rectDraw.Width(), myGL.m_rectDraw.Height(),
		!myGL.m_isFaceUp, myGL.m_isSwapX)) {
		throw;
	}
	pDC->StretchBlt(rectDraw->left, rectDraw->top,
		rectDraw->Width(), rectDraw->Height(),
		&aDC, 0, 0, myGL.m_rectDraw.Width(), myGL.m_rectDraw.Height(), srcMode);

	return true;
}

void OpenGLRendererFBO::myFBOtoBitMapFile(CString f)
{
	int ncol = this->myGL.m_rectPixel.Width();
	int nrow = this->myGL.m_rectPixel.Height();

	//	glReadBuffer(GL_BACK);
	flushGLErrors();
	glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
	glPixelStorei(GL_PACK_LSB_FIRST, GL_TRUE);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	// byte* Buff = new byte[2048 * 2048 * 3];
	byte* Buff = (byte*)alloc1(ncol*nrow * 3, 1);
	if (!Buff) return;
	glReadPixels(0, 0, ncol, nrow, GL_BGR, GL_UNSIGNED_BYTE, Buff);

	FILE *Out;
	if (_tfopen_s(&Out, f, _T("wb")) != 0) {
		free1(Buff);
		return;
	}
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;

	bitmapFileHeader.bfType = 0x4D42;
	bitmapFileHeader.bfSize = ncol*nrow * 3;
	bitmapFileHeader.bfReserved1 = 0;
	bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits =
		sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = ncol;
	bitmapInfoHeader.biHeight = nrow;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24; //24
	bitmapInfoHeader.biCompression = 0; //BI_RGB
	bitmapInfoHeader.biSizeImage = 0;
	bitmapInfoHeader.biXPelsPerMeter = 0; // ?
	bitmapInfoHeader.biYPelsPerMeter = 0; // ?
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;

	fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, Out);
	fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, Out);
	fwrite(Buff, ncol*nrow * 3, 1, Out);
	fclose(Out);
	free1(Buff);
}


void OpenGLRendererFBO::CalculateVectorNormal3_Notused(
	GLfloat fVertX[], GLfloat fVertY[], GLfloat fVertZ[],
	GLfloat *fNormalX, GLfloat *fNormalY, GLfloat *fNormalZ)
{
	GLfloat Qx, Qy, Qz, Px, Py, Pz;

	Qx = fVertY[0] - fVertX[0];
	Qy = fVertY[1] - fVertX[1];
	Qz = fVertY[2] - fVertX[2];
	Px = fVertZ[0] - fVertX[0];
	Py = fVertZ[1] - fVertX[1];
	Pz = fVertZ[2] - fVertX[2];

	*fNormalX = Py*Qz - Pz*Qy;
	*fNormalY = Pz*Qx - Px*Qz;
	*fNormalZ = Px*Qy - Py*Qx;

}
//
//void OpenGLRendererFBO::CalculateVectorNormal3(
//	GLfloat fVert0X, GLfloat fVert0Y, GLfloat fVert0Z,
//	GLfloat fVert1X, GLfloat fVert1Y, GLfloat fVert1Z,
//	GLfloat fVert2X, GLfloat fVert2Y, GLfloat fVert2Z,
//	GLfloat *fNormalX, GLfloat *fNormalY, GLfloat *fNormalZ)
//{
//	GLfloat Qx, Qy, Qz, Px, Py, Pz;
//
//	Qx = fVert0Y - fVert0X;
//	Qy = fVert1Y - fVert1X;
//	Qz = fVert2Y - fVert2X;
//	Px = fVert0Z - fVert0X;
//	Py = fVert1Z - fVert1X;
//	Pz = fVert2Z - fVert2X;
//
//	*fNormalX = Py*Qz - Pz*Qy;
//	*fNormalY = Pz*Qx - Px*Qz;
//	*fNormalZ = Px*Qy - Py*Qx;
//
//}

/*
9-Point average for normal vector at Z4
**********************************
**********************************
***********Z0****Z3*****Z6********
**********************************
**********************************
***********Z1****Z4*****Z7********
**********************************
**********************************
***********Z2****Z5*****Z8********
**********************************
Target: Z4
dx1 and dx2 are trace spacing before and after the current trace
dy is sample interval, fixed
*/
void OpenGLRendererFBO::CalculateVectorNormal9(
	GLfloat z0, GLfloat z1, GLfloat z2, GLfloat z3, GLfloat z4, GLfloat z5, GLfloat z6, GLfloat z7, GLfloat z8,
	GLfloat dx1, GLfloat dx2, GLfloat dy, bool isDownHill,
	GLfloat *fNormalX, GLfloat *fNormalY, GLfloat *fNormalZ)
{
	if (isDownHill) {
		*fNormalX = dx1 * (-z4 - z4 + z1 + z1 + z0 - z3)
			+ dx2 * (-z7 - z7 + z4 + z4 + z5 - z8);
		*fNormalY = dy * (z0 - z1 + z3 + z3 - z5 - z5 + z7 - z8);
		*fNormalZ = 3.f * dy * (dx1 + dx2);
	}
	else {
		*fNormalX = dx1 * (-z1 - z1 + z4 + z4 - z2 + z5)
			+ dx2 * (-z3 - z4 - z4 + z6 + z7 + z7);
		*fNormalY = dy * (z1 - z2 + z3 + z3 - z5 - z5 + z6 - z7);
		*fNormalZ = -3.f * dy * (dx1 + dx2);
	}
}
const bool OpenGLRendererFBO::isDownDip(float trcA[], float trcB[], int ns, int iFocus) {
//	return false;

	// if the sampling rate is 4ms, nsumHalf should be 5. But if the samling rate is 2ms, we should use 10 samples
	const int nsumHalf = 5;
	const int shiftMax = 5;
	int tauSkip = (int)ceil(nsumHalf / (double)shiftMax); // if 1, then no skip 
	
	float xcorr[2*shiftMax+1]; 
	register double a;
	int i, tau;

	memset(xcorr, 0, (2 * shiftMax + 1) * sizeof(float));
	int countTau = 0;
	for (tau = -shiftMax; tau < shiftMax; tau+=tauSkip) {
		register double denom = 0.;
		register double nom = 0.;
		for (i = iFocus - nsumHalf; i <= iFocus + nsumHalf; i++) {
			if (i < 0 || i >= ns) continue;
			if (i + tau < 0 || i + tau >= ns) continue;
			a = trcA[i] + trcB[i + tau];
			denom += a*a;
			nom += trcA[i] * trcA[i] + trcB[i+tau] * trcB[i + tau];
		}
		if (nom > 0.) {
			xcorr[countTau] = (float)(denom / nom);
			countTau++;
		}
	}
	int out = getMaxIndex(xcorr, countTau);
	return out>countTau /2;
}
void OpenGLRendererFBO::CalculateVectorNormal_Notused(GLfloat vertex1x, GLfloat vertex1y,
	GLfloat vertex1z, GLfloat vertex2x, GLfloat vertex2y,
	GLfloat vertex2z, GLfloat vertex3x, GLfloat vertex3y,
	GLfloat vertex3z, GLfloat vertex4x, GLfloat vertex4y,
	GLfloat vertex4z, GLfloat vertex5x, GLfloat vertex5y,
	GLfloat vertex5z, GLfloat *fNormalX,
	GLfloat *fNormalY, GLfloat *fNormalZ)
{
	GLfloat Qx, Qy, Qz, Px, Py, Pz;
	double xA, yA, zA, xB, yB, zB;

	Qx = vertex3x - vertex1x;
	Qy = vertex3y - vertex1y;
	Qz = vertex3z - vertex1z;
	Px = vertex4x - vertex1x;
	Py = vertex4y - vertex1y;
	Pz = vertex4z - vertex1z;

	xA = Qy*Pz - Qz*Py;
	yA = Qz*Px - Qx*Pz;
	zA = Qx*Py - Qy*Px;


	Qx = vertex1x - vertex1x;
	Qy = vertex1y - vertex1y;
	Qz = vertex1z - vertex1z;
	Px = vertex5x - vertex1x;
	Py = vertex5y - vertex1y;
	Pz = vertex5z - vertex1z;

	xB = Qy*Pz - Qz*Py;
	yB = Qz*Px - Qx*Pz;
	zB = Qx*Py - Qy*Px;

	xA = (xA + xB) / 2;
	yA = (yA + yB) / 2;
	zA = (zA + zB) / 2;

	double vectorLength = std::sqrt(std::pow(xA, 2) + std::pow(yA, 2) + std::pow(zA, 2));
	double oLength = 1.0 / vectorLength;

	*fNormalX = (GLfloat)(xA * oLength);
	*fNormalY = (GLfloat)(yA * oLength);
	*fNormalZ = (GLfloat)(zA * oLength);

}


#include "opengl/glm-0.9.7.1/glm/glm.hpp"
#include "opengl/glm-0.9.7.1/glm/gtc/matrix_transform.hpp"
#include "opengl/common/texture.h"
#include "opengl/common/objloader.h"
#include "opengl/common/vboindexer.h"
void OpenGLRendererFBO::mySeis3DFBO_Init(float **ampArr, CColorHelper::AMPCOLORDYN *ampColor, float x[], float y[], int nx, int nt)
{
	// when this starts, I want to reset the color options
//	myGL.hue = myGL.saturation = myGL.value = 100;


	//Set up framebuffer
	glGenFramebuffers(1, &myGL.FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);
	int ncol = this->myGL.m_rectPixel.Width();
	int nrow = this->myGL.m_rectPixel.Height();
	glViewport(0, 0, ncol, nrow);

	//Create texture buffer
	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);
	glBindTexture(GL_TEXTURE_2D, renderedTexture); // "Bind" the newly created texture : all future texture functions will modify this texture

												   //Generate background colour and backgrou
	glClearColor(1.0f, 1.0f, 1.f, 0.f); //background to clear with.
//	glClearColor(0.9f, 0.9f, 1.f, 0.f); //background to clear with. // If I use this line, the background would be a little faint blue.

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_CULL_FACE);
	//glDepthFunc(GL_CULL_FACE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		ncol, nrow,
		0, GL_RGB, GL_UNSIGNED_BYTE, 0);// Give an empty image to OpenGL ( the last "0" )

										// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// The depth buffer
	//glDepthFunc(GL_ALWAYS); // deping add
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ncol, nrow);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);


	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, ncol, nrow, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);


	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	// Set the list of draw buffers.
	//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	//glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;


	//##################################
	// calculate the maximum trace spacing: if there is a big data gap, we will show the gap
	float dxMaxPlot = getAvgInterval(x, nx) * 5.f;

	//	each data point will need 2 triangles to plot
	int ix, j;
	int xx, jj, xyz, x4;
	float o2 = 0.5f;

	GLfloat *colour = (GLfloat*)alloc1(nt * 3 * nx, sizeof(GLfloat));
	CColorHelper::rgb *colors1D = (CColorHelper::rgb *)alloc1(nt, sizeof(CColorHelper::rgb));
	for (ix = 0; ix < nx; ix++) {
		ampColor->getColorRef4Ampf(ampArr[ix], colors1D, nt);
		xx = ix*nt * 3;
		for (j = 0; j < nt; j++) {
			jj = j * 3;
			colour[xx + jj] = (GLfloat)colors1D[j].r;
			colour[xx + jj + 1] = (GLfloat)colors1D[j].g;
			colour[xx + jj + 2] = (GLfloat)colors1D[j].b;
		}
	} // for ix
	free1((void *)colors1D); // colors1D* completed its mission
	myGL.nx = nx; // backup for later usage
	myGL.nt = nt;
							 
	//float ampMaxNeg = ampColor->percentAmp[0];
	//float ampMaxPos = ampColor->percentAmp[ampColor->num - 1];
	//float oMax = 1.f / (fabs(ampMaxNeg) < fabs(ampMaxPos) ? fabs(ampMaxPos) : fabs(ampMaxNeg));

	float oMax = ampColor->m_maxAmp>MINAMP ? (float)(1. / ampColor->getDrawAmp(ampColor->m_maxAmp)) : 1.f;
	//float oMax = ampColor->m_maxAmp>MINAMP ? 1.f / ampColor->m_maxAmp : 1.f;
	for (int i = 0; i < nx; i++)
		for (int j = 0; j < nt; j++) {
			ampArr[i][j] = (float)ampColor->getDrawAmp(ampArr[i][j]);
			ampArr[i][j] = ampArr[i][j] * oMax;
			if (ampArr[i][j] < -2.f) ampArr[i][j] = -2.f;
			else if (ampArr[i][j] > 2.f) ampArr[i][j] = 2.f;
		}

	// compute down hill array
	bool *isDownHillArr = (bool*)alloc1(nt * nx, sizeof(bool));
	for (ix = 1; ix < nx - 1; ix++) {
		xx = ix*nt;
		for (j = 1; j < nt - 1; j++) {
			isDownHillArr[xx + j] = isDownDip(ampArr[ix], ampArr[ix + 1], nt, j);
		}
	} // for ix


	  // compute vertex and color
	GLfloat *line = (GLfloat*)alloc1(nt * 3 * nx, sizeof(GLfloat)); // a triangle has 3 points
	for (ix = 0; ix < nx; ix++) {
		xx = ix*nt * 3;
		for (j = 0; j < nt; j++) {
			jj = j * 3;
			line[xx + jj] = (GLfloat)x[ix];
			line[xx + jj + 1] = (GLfloat)y[j];
			line[xx + jj + 2] = (GLfloat)ampArr[ix][j];

			// test
			//if (isDownHillArr[ix*nt + j]) {
			//	colour[xx + jj] = (GLfloat)1.;
			//	colour[xx + jj + 1] = (GLfloat)0.;
			//	colour[xx + jj + 2] = (GLfloat)0.;
			//}
			//else {
			//	colour[xx + jj] = (GLfloat)0.;
			//	colour[xx + jj + 1] = (GLfloat)0.;
			//	colour[xx + jj + 2] = (GLfloat)1.;
			//}
		}
	} // for ix


	  // compute uv array
	GLfloat *uvs1 = (GLfloat*)alloc1(nt * 2 * nx, sizeof(GLfloat));
	uvs1[0] = 1.0f;
	for (ix = 0; ix < nx; ix++) {
		x4 = ix*nt * 2;
		for (j = 0; j < nt; j++) {
			jj = j * 2;
			uvs1[x4 + jj] = (x[ix] + 1.0f) * o2;
			uvs1[x4 + jj + 1] = (y[j] + 1.0f) * o2;
		}
	} // for ix


	  // compute normals
	GLfloat *normals1 = (GLfloat*)alloc1(nt * 3 * nx, sizeof(GLfloat));
	for (ix = 1; ix < nx - 1; ix++) {
		xx = ix*nt * 3;
		for (j = 1; j < nt - 1; j++) {
			jj = j * 3;
			CalculateVectorNormal9(
				ampArr[ix - 1][j - 1], ampArr[ix - 1][j], ampArr[ix - 1][j + 1],
				ampArr[ix][j - 1], ampArr[ix][j], ampArr[ix][j + 1],
				ampArr[ix + 1][j - 1], ampArr[ix + 1][j], ampArr[ix + 1][j + 1],
				x[ix] - x[ix - 1], x[ix + 1] - x[ix], y[j] - y[j - 1], true,
				&normals1[xx + jj], &normals1[xx + jj + 1], &normals1[xx + jj + 2]
			);
		}
	} // for ix

	  // now set the border values to the same as middle ones
	  // first trace
	for (j = 1; j < nt - 1; j++) {
		jj = j * 3;
		normals1[jj] = normals1[nt * 3 + jj];
		normals1[jj + 1] = normals1[nt * 3 + jj + 1];
		normals1[jj + 1] = normals1[nt * 3 + jj] + 1;
		isDownHillArr[j] = isDownHillArr[nt + j];
	}

	// last trace
	xx = (nx - 1)*nt * 3;
	for (j = 1; j < nt - 1; j++) {
		jj = j * 3;
		normals1[xx + jj] = normals1[xx - nt * 3 + jj];
		normals1[xx + jj + 1] = normals1[xx - nt * 3 + jj + 1];
		normals1[xx + jj + 2] = normals1[xx - nt * 3 + jj + 2];
		isDownHillArr[(nx - 1)*nt + j] = isDownHillArr[(nx - 2)*nt + j];
	}

	// first sample for all traces
	for (ix = 0; ix < nx; ix++) {
		xx = ix*nt * 3;
		normals1[xx] = normals1[xx + 3];
		normals1[xx + 1] = normals1[xx + 4];
		normals1[xx + 2] = normals1[xx + 5];
		isDownHillArr[ix*nt] = isDownHillArr[ix*nt + 1];
	}

	// last sample for all traces
	for (ix = 0; ix < nx; ix++) {
		xx = ix*nt * 3 + (nt - 1) * 3;
		normals1[xx] = normals1[xx - 3];
		normals1[xx + 1] = normals1[xx - 2];
		normals1[xx + 2] = normals1[xx - 1];
		isDownHillArr[ix*nt + nt - 1] = isDownHillArr[ix*nt + nt - 2];
	}
	// #####now all samples of nx*nt are covered#######

	//Use shader to load in matrix and set viewing direction
 
	//	glDrawArrays(GL_TRIANGLES, 0, 6 * (nt-1));
	// now prepare for the other triangles using indexes
//	GLfloat x1, y1, z1, x2, y2, z2, x3, y3, z3;
	int offset = 0;

	unsigned int numElem = (nx) * 6 * (nt - 1);
	unsigned int *elem1 = (unsigned int*)alloc1(numElem, sizeof(unsigned int));
	int countTrc = 0;
	for (ix = 0; ix < nx - 1; ix++) {
		xx = ix*nt;
		if (fabs(x[ix] - x[ix + 1]) > dxMaxPlot) continue; // we do not plot data gap
		xyz = countTrc * (nt - 1) * 6;
		for (j = 0; j < nt - 1; j++) {
			///Set for Left-Right alternating triangle drawings
			//jj = j * 6;
			offset = xyz + j * 6;

			register bool isDownHill = isDownHillArr[xx + j];
			if (isDownHill) {
				//DOWNHILL
				// first set of triangles
				elem1[offset] = (unsigned int)(xx + j);
				elem1[offset + 1] = (unsigned int)(xx + nt + j + 1);
				elem1[offset + 2] = (unsigned int)(xx + nt + j);

				// second set of triangles
				elem1[offset + 3] = (unsigned int)(xx + j);
				elem1[offset + 4] = (unsigned int)(xx + j + 1);
				elem1[offset + 5] = (unsigned int)(xx + nt + j + 1);
			}
			else {
				//UPHILL
				// first set of triangles
				elem1[offset] = (unsigned int)(xx + j);
				elem1[offset + 1] = (unsigned int)(xx + j + 1);
				elem1[offset + 2] = (unsigned int)(xx + nt + j);

				// second set of triangles
				elem1[offset + 3] = (unsigned int)(xx + j + 1);
				elem1[offset + 4] = (unsigned int)(xx + nt + j + 1);
				elem1[offset + 5] = (unsigned int)(xx + nt + j);
			}
		}
		countTrc++;
	} // for ix

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//GLuint vbo[4];
	//glGenBuffers(4, vbo);
	glGenBuffers(1, &myGL.vbo1);
	glGenBuffers(1, &myGL.vbo2);
	glGenBuffers(1, &myGL.vbo3);
	glGenBuffers(1, &myGL.vbo0);
	//GLuint elementbuffer1;
	glGenBuffers(1, &myGL.elementbuffer1);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myGL.elementbuffer1);

	//GLint color_attribute = glGetAttribLocation(programID, "color");
	// Get a handle for our "MVP" uniform


	//Place data in an array to be drawn to the screen
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, myGL.vbo0);
	GLuint sizeBytes = (3 * nt*nx) * sizeof(GLfloat);
	glBufferData(GL_ARRAY_BUFFER, sizeBytes, line, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_TRUE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, myGL.vbo1);
	glBufferData(GL_ARRAY_BUFFER, sizeBytes, colour, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_TRUE, 0, 0);

	glEnable(GL_BLEND);
	glBindBuffer(GL_ARRAY_BUFFER, myGL.vbo2);
	sizeBytes = (2 * nt*nx) * sizeof(GLfloat);
	glBufferData(GL_ARRAY_BUFFER, sizeBytes, &uvs1[0], GL_STATIC_DRAW);
	glVertexAttribPointer(
		1,                                // attribute
		2,                                // size
		GL_FLOAT,                         // type
		GL_TRUE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glBindBuffer(GL_ARRAY_BUFFER, myGL.vbo3);
	sizeBytes = (3 * nt*nx) * sizeof(GLfloat);
	glBufferData(GL_ARRAY_BUFFER, sizeBytes, &normals1[0], GL_STATIC_DRAW);

	// 3rd attribute buffer : normals
	glVertexAttribPointer(
		2,                                // attribute
		3,                                // size
		GL_FLOAT,                         // type
		GL_TRUE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myGL.elementbuffer1);
	myGL.numElem = countTrc * 6 * (nt - 1);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, myGL.numElem * sizeof(unsigned int), &elem1[0], GL_STATIC_DRAW);

	free1(line);
	free1(colour);
	free1(elem1);
	free1(normals1);
	free1(isDownHillArr);
	free1(uvs1);


	// #############Rendering to the texture
	// Rendering to the texture is straightforward. Simply bind your framebuffer, and draw your scene as usual. Easy !
	// Render to our framebuffer
	CString s1 = myGL.shaderFolderSlash + "ShadowMapping.vertexshader";
	CString s2 = myGL.shaderFolderSlash + "ShadowMapping.fragmentshader";
	char * ascS1 = getCharFromCString(s1, 100);
	char * ascS2 = getCharFromCString(s2, 100);
	myGL.gpuProgramID = LoadShaders(ascS1, ascS2);
	free1(ascS1);
	free1(ascS2);
	glUseProgram(myGL.gpuProgramID);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

}

// if iv>=-1, I will use  	myFBOtoDC(pDC, rectDraw, SRCCOPY);  // this is doing animation
// else   I will use   myFBOtoDC(pDC, rectDraw, SRCAND); // this will keep previus colors on screen
// meanwhile, if iv>=0, glMap.camPosition.z = iv  for animation purposes
void OpenGLRendererFBO::mySeis3DFBO_Animate(CDC* pDC, int iv, CRect* rectDraw)
{
	if (myGL.gpuProgramID == 0 || myGL.numElem == 0) return;

	// we want to check if the current program id is already active, if so, we should not replicate the same ID 
	// the glCreateProgram() will always return the same ID if the same call
	//GLint Result;
	//glGetProgramiv(myGL.gpuProgramID, GL_DELETE_STATUS, &Result);
	//if (Result) {
	////	return;  // this prevents crashing when another window already deletes the same program ID,
	//	// but this window does not know that program ID is already deleted.
	//}

	wglMakeCurrent(myGL.m_hDC, myGL.m_hMemRC); // always make current before doing the real rendering!!!

	// Camera matrix
	// Projection matrix : 45�� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	//glm::mat4 Projection = glm::perspective(15.0f, 4.0/3.0, 0.1f, 100.0f);
	// Or, for an ortho camera :
	glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -100.0f, 100.0f); // In world coordinates

	if (iv>=0)
		glMap.camPosition.z = iv;

	//GLuint programID = (GLuint)aProgramID;
	GLuint MatrixID = glGetUniformLocation(myGL.gpuProgramID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(myGL.gpuProgramID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(myGL.gpuProgramID, "M");
	GLuint DepthBiasID = glGetUniformLocation(myGL.gpuProgramID, "DepthBiasMVP");
	GLuint ShadowMapID = glGetUniformLocation(myGL.gpuProgramID, "shadowMap");

	// Get a handle for our "LightPosition" uniform
	GLuint lightInvDirID = glGetUniformLocation(myGL.gpuProgramID, "LightInvDirection_worldspace");

	glm::mat4 View = glm::lookAt(
		glm::vec3(glMap.camPosition.x, glMap.camPosition.y, glMap.camPosition.z),
		glm::vec3(glMap.viewAt.x, glMap.viewAt.y, glMap.viewAt.z),
		glm::vec3(glMap.headsUp.x, glMap.headsUp.y, glMap.headsUp.z)
	);


	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);

	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model;

	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);

	glm::vec3 lightInvDir = glm::vec3(glMap.lightInvDir.x, glMap.lightInvDir.y, glMap.lightInvDir.z);
	glUniform3f(lightInvDirID, lightInvDir.x, lightInvDir.y, lightInvDir.z);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_TRIANGLES, myGL.numElem, GL_UNSIGNED_INT, (void*)0);

	//myFBOtoBitMapFile(fOut);
	//if (iv >= -1)
		myFBOtoDC(pDC, rectDraw, SRCCOPY);  // this is doing animation
	//else
	//	myFBOtoDC(pDC, rectDraw, SRCCOPY); // we want to show the background color map as well. This causes previous colors to stay ons screen


	wglMakeCurrent(NULL, NULL);  // if I do not release the RC and DC, the 2 child windows of MDI would show the same rendering.
}


void OpenGLRendererFBO::mySeis3DFBO_joystick(CDC* pDC)
{
	// need to draw the joystick frame
	if (!myGL.m_isGLButtonsShowing) return;

	//CPen bluePen(255, 0, 0);
	CPen bluePen(PS_SOLID, 5, RGB(0, 0, 255));
	CPen *pOldPen = pDC->SelectObject(&bluePen);
	CBrush b1(RGB(255, 127, 0));
	CBrush *pOldBrush = pDC->SelectObject(&b1);
	pDC->Rectangle(myGL.camXYFrame);
	pDC->Rectangle(myGL.camZFrame);
	pDC->Rectangle(myGL.lightXYFrame);
	pDC->Rectangle(myGL.lightZFrame);
	//pDC->Rectangle(myGL.hueFrame);
	//pDC->Rectangle(myGL.satFrame);
	//pDC->Rectangle(myGL.valueFrame);
	pDC->SetBkMode(TRANSPARENT);
	COLORREF cOld = pDC->SetTextColor(GRAY2);
	pDC->TextOut(myGL.camXYFrame.left + 10, (myGL.camXYFrame.top + myGL.camXYFrame.bottom) / 2, _T("X"));
	pDC->TextOut((myGL.camXYFrame.left + myGL.camXYFrame.right) / 2, myGL.camXYFrame.top + 10, _T("Y"));
	pDC->TextOut(myGL.camXYFrame.left + 1000, (myGL.camXYFrame.top + myGL.camXYFrame.bottom) / 2, _T("Cam"));
	pDC->TextOut(myGL.camZFrame.left + 10, myGL.camZFrame.top, _T("Z"));

	pDC->TextOut(myGL.lightXYFrame.left + 10, (myGL.lightXYFrame.top + myGL.lightXYFrame.bottom) / 2, _T("X"));
	pDC->TextOut((myGL.lightXYFrame.left + myGL.lightXYFrame.right) / 2, myGL.lightXYFrame.top + 10, _T("Y"));
	pDC->TextOut(myGL.lightXYFrame.left + 1050, (myGL.lightXYFrame.top + myGL.lightXYFrame.bottom) / 2, _T("Light"));
	pDC->TextOut(myGL.lightXYFrame.left + 10, myGL.lightZFrame.top, _T("Z"));
	//pDC->TextOut(myGL.hueFrame.left + 50, myGL.hueFrame.top, _T("Hue"));
	//pDC->TextOut(myGL.satFrame.left + 50, myGL.satFrame.top, _T("Saturation"));
	//pDC->TextOut(myGL.valueFrame.left + 50, myGL.valueFrame.top, _T("Value"));
	pDC->SetTextColor(cOld);
	CBrush b2(RGB(255, 0, 0));
	pDC->SelectObject(&b2);


	// put a star at the proper place
//	SEI_TRIPLE sp = glMap.calc_barycentric_weights()
	//double wv1 = glMap.getValueNormed(glMap.camPosition.x, 1.);
	//double wv2 = glMap.getValueNormed(glMap.camPosition.y, 1.);
	//double wv3 = glMap.getValueNormed(glMap.camPosition.z, 100.);
	//// wv1-3 must be from 0-100% only. But x and y are [-1, 1], and z is inside [-100, 100]
	//double xv1 = myGL.joystick.left;
	//double xv2 = myGL.joystick.right;
	//double xv3 = myGL.joystick.top;
	//int px = (int)(wv1 * xv1 + wv2 * xv2 + wv3 * xv3 + 0.5);
	//double yv1 = myGL.joystick.left;
	//double yv2 = myGL.joystick.right;
	//double yv3 = myGL.joystick.top;
	//int py = (int)(wv1 * yv1 + wv2 * yv2 + wv3 * yv3 + 0.5);


	double v1 = myGL.camXYFrame.left;
	double v2 = myGL.camXYFrame.right;
	int px = (int)(0.5 + v1 + (glMap.camPosition.x + 1.) * 0.5 * (v2 - v1));
	v1 = myGL.camXYFrame.top;
	v2 = myGL.camXYFrame.bottom;
	int py = (int)(0.5 + v1 + (glMap.camPosition.y + 1.) * 0.5 * (v2 - v1));

	int dPlt = 100;
	register int ix1 = px - dPlt;
	register int iy1 = py - dPlt;
	register int ix2 = px + dPlt;
	register int iy2 = py + dPlt;
	pDC->Ellipse(ix1, iy1, ix2, iy2);

	v1 = myGL.camZFrame.left;
	v2 = myGL.camZFrame.right;
	px = (int)(0.5 + v1 + (glMap.camPosition.z + 100.) * 0.005 * (v2 - v1));
	ix1 = px - dPlt;
	iy1 = (myGL.camZFrame.top + myGL.camZFrame.bottom) / 2 - dPlt;
	ix2 = px + dPlt;
	iy2 = iy1 + 2*dPlt;
	pDC->Ellipse(ix1, iy1, ix2, iy2);




	v1 = myGL.lightXYFrame.left;
	v2 = myGL.lightXYFrame.right;
	px = (int)(0.5 + v1 + (glMap.lightInvDir.x + 100.) * 0.005 * (v2 - v1));
	v1 = myGL.lightXYFrame.top;
	v2 = myGL.lightXYFrame.bottom;
	py = (int)(0.5 + v1 + (glMap.lightInvDir.y + 100.) * 0.005 * (v2 - v1));

	ix1 = px - dPlt;
	iy1 = py - dPlt;
	ix2 = px + dPlt;
	iy2 = py + dPlt;
	pDC->Ellipse(ix1, iy1, ix2, iy2);

	v1 = myGL.lightZFrame.left;
	v2 = myGL.lightZFrame.right;
	px = (int)(0.5 + v1 + (glMap.lightInvDir.z + 100.) * 0.005 * (v2 - v1));
	py = (myGL.lightZFrame.top + myGL.lightZFrame.bottom) / 2;

	ix1 = px - dPlt;
	iy1 = py - dPlt;
	ix2 = px + dPlt;
	iy2 = py + dPlt;
	pDC->Ellipse(ix1, iy1, ix2, iy2);



	// start color manipulations in HSV space
	//v1 = myGL.hueFrame.left;
	//v2 = myGL.hueFrame.right;
	//px = (int)(0.5 + v1 + (myGL.hue) * 0.01 * (v2 - v1));
	//py = (myGL.hueFrame.top + myGL.hueFrame.bottom) / 2;
	//ix1 = px - dPlt;
	//iy1 = py - dPlt;
	//ix2 = px + dPlt;
	//iy2 = py + dPlt;
	//pDC->Ellipse(ix1, iy1, ix2, iy2);

	//v1 = myGL.satFrame.left;
	//v2 = myGL.satFrame.right;
	//px = (int)(0.5 + v1 + (myGL.saturation) * 0.01 * (v2 - v1));
	//py = (myGL.satFrame.top + myGL.satFrame.bottom) / 2;
	//ix1 = px - dPlt;
	//iy1 = py - dPlt;
	//ix2 = px + dPlt;
	//iy2 = py + dPlt;
	//pDC->Ellipse(ix1, iy1, ix2, iy2);

	//v1 = myGL.valueFrame.left;
	//v2 = myGL.valueFrame.right;
	//px = (int)(0.5 + v1 + (myGL.value) * 0.01 * (v2 - v1));
	//py = (myGL.valueFrame.top + myGL.valueFrame.bottom) / 2;
	//ix1 = px - dPlt;
	//iy1 = py - dPlt;
	//ix2 = px + dPlt;
	//iy2 = py + dPlt;
	//pDC->Ellipse(ix1, iy1, ix2, iy2);

	// cleanup
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldPen);
}

void OpenGLRendererFBO::mySeis3DFBO_Buttons(CDC* pDC, double resAmplify)
{
	// need to define 12 CRect for 12 buttons
	if (!myGL.m_isGLButtonsShowing) return;

	 
	COLORREF oldC = pDC->SetTextColor(RGB(0, 0, 255));
	pDC->SetBkMode(OPAQUE);
	pDC->TextOut(myGL.buttons[6].right + 10, myGL.buttons[0].top - 120, _T("X"));
	pDC->TextOut(myGL.buttons[7].right + 10, myGL.buttons[1].top - 120, _T("Y Light"));
	pDC->TextOut(myGL.buttons[8].right + 10, myGL.buttons[2].top - 120, _T("Z"));
	pDC->TextOut(myGL.buttons[9].right + 10, myGL.buttons[3].top - 120, _T("X"));
	pDC->TextOut(myGL.buttons[10].right + 10, myGL.buttons[4].top - 120, _T("Y Camera"));
	pDC->TextOut(myGL.buttons[11].right + 10, myGL.buttons[5].top - 120, _T("Z"));

	pDC->TextOut(myGL.buttons[0].left, myGL.buttons[5].bottom, _T(" -     +"));
	pDC->SetTextColor(oldC);

	CBrush b(WHITE), b1(RGB(255, 127, 0));;
	CBrush *pOldBrush = pDC->SelectObject(&b1);

	char i;
	for (i = 0; i<12; i++) {
		pDC->Rectangle(myGL.buttons[i]);
	}
	pDC->SelectObject(pOldBrush);

	mySeis3DFBO_joystick(pDC);
}

bool OpenGLRendererFBO::mySeis3DFBO_ButtonDragDrop(CPoint p1, CPoint p2)
{
	if (!myGL.isOpenGLWindowsExists || myGL.gpuProgramID == 0) return false;
	if (!myGL.m_isGLButtonsShowing) return false;
	if (IsPointInside(&p1, &myGL.camXYFrame)) {
		// change the values for glMap.camPosition.x   and glMap.camPosition.y
		glMap.camPosition.x = -1. + (double)(p2.x - myGL.camXYFrame.left) / (double)myGL.camXYFrame.Width() * 2.;
		glMap.camPosition.y = -1. + (double)(p2.y - myGL.camXYFrame.top) / (double)myGL.camXYFrame.Height() * 2.;
	}
	else if (IsPointInside(&p1, &myGL.camZFrame)) {
		glMap.camPosition.z = -100. + (double)(p2.x - myGL.camZFrame.left) / (double)myGL.camZFrame.Width() * 200.;
	}
	else if (IsPointInside(&p1, &myGL.lightXYFrame)) {
		// change the values for glMap.lightInvDir.x   and glMap.lightInvDir.y
		glMap.lightInvDir.x = -100. + (double)(p2.x - myGL.lightXYFrame.left) / (double)myGL.lightXYFrame.Width() * 200.;
		glMap.lightInvDir.y = -100. + (double)(p2.y - myGL.lightXYFrame.top) / (double)myGL.lightXYFrame.Height() * 200.;
	}
	else if (IsPointInside(&p1, &myGL.lightZFrame)) {
		glMap.lightInvDir.z = -100. + (double)(p2.x - myGL.lightZFrame.left) / (double)myGL.lightZFrame.Width() * 200.;
	}
	//else if (IsPointInside(&p1, &myGL.hueFrame)) {
	//	myGL.hue = (byte)((double)(p2.x - myGL.hueFrame.left) / (double)myGL.hueFrame.Width() * 100.);
	//	return true; // this will not change the GL mapping, so exit earlier
	//}
	//else if (IsPointInside(&p1, &myGL.satFrame)) {
	//	myGL.saturation = (byte)((double)(p2.x - myGL.satFrame.left) / (double)myGL.satFrame.Width() * 100.);
	//	return true; // this will not change the GL mapping, so exit earlier
	//}
	//else if (IsPointInside(&p1, &myGL.valueFrame)) {
	//	myGL.value = (byte)((double)(p2.x - myGL.valueFrame.left) / (double)myGL.valueFrame.Width() * 100.);
	//	return true; // this will not change the GL mapping, so exit earlier
	//}

	limitValueAbs(glMap.camPosition.x, 1.);
	limitValueAbs(glMap.camPosition.y, 1.);
	limitValueAbs(glMap.camPosition.z, 100.);
	limitValueAbs(glMap.lightInvDir.x, 100.);
	limitValueAbs(glMap.lightInvDir.y, 100.);
	limitValueAbs(glMap.lightInvDir.z, 100.);
	glMap.saveGLmapping();



	return true;
}

// if the user clicked one of the 12 buttons, its corresponding parameters will need to change
// if return true, the screen needs to refresh
CString OpenGLRendererFBO::mySeis3DFBO_ButtonClick(CPoint p)
{
	if (!myGL.isOpenGLWindowsExists || myGL.gpuProgramID == 0) return BLANK;
	if (!myGL.m_isGLButtonsShowing) return BLANK;
	/*
	// initialized matrixes are as below (by default)
	position.set(0.1, 0.2, 20); // Camera is at position (0.1, 0.2, 20), in World Space
	viewAt.set(0., 0., 0.); // viewAt=position+direction, so direction=(-0.1, -0.2, -20)
	headsUp.set(0., 1., 0.); // Head is up (set to 0,-1,0 to look upside-down)
	lightInvDir.set(8., 3., 5.);
	*/

	CString tStr = BLANK;
	double dx = this->myGL.m_isSwapX ? -1. : 1.;
	double dy = this->myGL.m_isFaceUp ? -1. : 1.;
	int dz;
	if (IsPointInside(&p, &myGL.buttons[0])) {
		glMap.lightInvDir.x -= dx;
		tStr.Format(_T("lightVir.x=%g"), glMap.lightInvDir.x);
	}
	else if (IsPointInside(&p, &myGL.buttons[1])) {//
		glMap.lightInvDir.y -= dy;
		tStr.Format(_T("lightVir.y=%g"), glMap.lightInvDir.y);
	}
	else if (IsPointInside(&p, &myGL.buttons[2])) {
		glMap.lightInvDir.z -= 2.;
		tStr.Format(_T("lightVir.z=%g"), glMap.lightInvDir.z);
	}
	else if (IsPointInside(&p, &myGL.buttons[3])) {//
		dx = fabs(glMap.camPosition.x) < 0.1 ? 0.1 : glMap.camPosition.x * 0.2;
		glMap.camPosition.x -= fabs(dx);
		tStr.Format(_T("camPosition.x=%g"), glMap.camPosition.x);
	}
	else if (IsPointInside(&p, &myGL.buttons[4])) {//
		dy = fabs(glMap.camPosition.y) < 0.1 ? 0.1 : glMap.camPosition.y * 0.2;
		glMap.camPosition.y -= fabs(dy);
		tStr.Format(_T("camPosition.y=%g"), glMap.camPosition.y);
	}
	else if (IsPointInside(&p, &myGL.buttons[5])) {//
		double zBackup = glMap.camPosition.z;
		dz = (int)fabs(glMap.camPosition.z * 0.2); if (dz == 0) dz = 1;
		glMap.camPosition.z -= dz;
		if (glMap.camPosition.z < 0.) glMap.camPosition.z = 0.1;
		if (zBackup != glMap.camPosition.z)
			tStr.Format(_T("camPosition.z=%g"), glMap.camPosition.z);
	}
	else if (IsPointInside(&p, &myGL.buttons[6])) {//
		glMap.lightInvDir.x += dx;
		tStr.Format(_T("lightVir.x=%g"), glMap.lightInvDir.x);
	}
	else if (IsPointInside(&p, &myGL.buttons[7])) {//
		glMap.lightInvDir.y += dy;
		tStr.Format(_T("lightVir.y=%g"), glMap.lightInvDir.y);
	}
	else if (IsPointInside(&p, &myGL.buttons[8])) {
		glMap.lightInvDir.z += 2.;
		tStr.Format(_T("lightVir.z=%g"), glMap.lightInvDir.z);
	}
	else if (IsPointInside(&p, &myGL.buttons[9])) {//
		dx = fabs(glMap.camPosition.x) < 0.1 ? 0.1 : glMap.camPosition.x * 0.2;
		glMap.camPosition.x += fabs(dx);
		tStr.Format(_T("camPosition.x=%g"), glMap.camPosition.x);
	}
	else if (IsPointInside(&p, &myGL.buttons[10])) {//
		dy = fabs(glMap.camPosition.y) < 0.1 ? 0.1 : glMap.camPosition.y * 0.2;
		glMap.camPosition.y += fabs(dy);
		tStr.Format(_T("camPosition.y=%g"), glMap.camPosition.y);
	}
	else if (IsPointInside(&p, &myGL.buttons[11])) {//
		dz = (int)fabs(glMap.camPosition.z * 0.2); if (dz == 0) dz = 1;
		glMap.camPosition.z += dz;
		if (glMap.camPosition.z > 100) glMap.camPosition.z = 100;
		tStr.Format(_T("camPosition.z=%g"), glMap.camPosition.z);
	}
	if (!tStr.IsEmpty()) {
		// when the values are changed, we just save them to file
		glMap.saveGLmapping();
	}
	return tStr;
}

void OpenGLRendererFBO::mySeis3DFBO_End()
{
	if (myGL.gpuProgramID != 0) {
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(3);

		glDeleteBuffers(1, &myGL.vbo0);
		glDeleteBuffers(1, &myGL.vbo1);
		glDeleteBuffers(1, &myGL.vbo2);
		glDeleteBuffers(1, &myGL.vbo3);
		glDeleteBuffers(1, &myGL.elementbuffer1);

		glUseProgram(0);
		glDeleteProgram(myGL.gpuProgramID);
		myGL.gpuProgramID = 0;
	}
}

void OpenGLRendererFBO::DestroySceneFBO()
{
	if (!myGL.isOpenGLWindowsExists) return;  // if I do not have this line, the program sometimes may freeze

	DestroyWindow();
	wglMakeCurrent(NULL, NULL);
	if (myGL.m_hMemRC) {
		wglDeleteContext(myGL.m_hMemRC);
		myGL.m_hMemRC = NULL;
	}
	myGL.isOpenGLWindowsExists = false;

	// frameBuffer specific
	//glDeleteFramebuffers(1, &myGL.FramebufferName);
}



void OpenGLRendererFBO::depSeisFBO3(CColorHelper::rgb **colorsArr, CDC* pDC, float x[], float y[], int nx, int nt,
	float dxMinPlot, float dxMaxPlot)
{

	//Set up framebuffer
	glGenFramebuffers(1, &myGL.FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);
	int ncol = this->myGL.m_rectPixel.Width();
	int nrow = this->myGL.m_rectPixel.Height();
	glViewport(0, 0, ncol, nrow);

	//Create texture buffer
	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);
	glBindTexture(GL_TEXTURE_2D, renderedTexture); // "Bind" the newly created texture : all future texture functions will modify this texture

												   //Generate background colour and backgrou
												   // Enable depth test
												   // glEnable(GL_DEPTH_TEST);
												   //glDepthFunc(GL_LESS); // Accept fragment if it closer to the camera than the former one
												   //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 1.0, 1.0, 0.0); //background to clear with.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		ncol, nrow,
		0, GL_RGB, GL_UNSIGNED_BYTE, 0);// Give an empty image to OpenGL ( the last "0" )

										// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// The depth buffer
	//glDepthFunc(GL_ALWAYS); // deping add
	GLuint depthrenderbuffer;
	glGenRenderbuffers(1, &depthrenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ncol, nrow);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

								   // Always check that our framebuffer is ok
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return;
	// #############Rendering to the texture
	// Rendering to the texture is straightforward. Simply bind your framebuffer, and draw your scene as usual. Easy !
	// Render to our framebuffer

	GLuint programID = LoadShaders("E:/Jeff/work85vs2015/shaders/TransformVertexShader.vertexshader", "E:/Jeff/work85vs2015/shaders/ColorFragmentShader.fragmentshader");
	//GLint color_attribute = glGetAttribLocation(programID, "color");
	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/#homogeneous-coordinates
	// Projection matrix : 45�� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	//glm::mat4 Projection = glm::perspective(
	//	glm::radians(45.0f), 
	//	(float)ncol / (float)nrow, 
	//	0.1f, 100.0f);
	// Or, for an ortho camera :
	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
	glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1000.0f); // In world coordinates

																				// Camera matrix
	glm::mat4 View = glm::lookAt(
		glm::vec3(0, 0, 1000), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
	);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP = Projection * View * Model;

	//##################################
	int ix, j;

	if (true) {
		//	each data point will need 2 triangles to plot
		GLfloat *line = (GLfloat*)alloc1(nt * 3 * nx, sizeof(GLfloat)); // a triangle has 3 points
		GLfloat *colour = (GLfloat*)alloc1(nt * 3 * nx, sizeof(GLfloat));
		unsigned int *elem1 = (unsigned int*)alloc1(nt * 3, sizeof(unsigned int));
		unsigned int *elem2 = (unsigned int*)alloc1(nt * 3, sizeof(unsigned int));

		int count = 0;
		//glGenVertexArrays(myGL.numvo, myGL.vaoID); // not used at all

		GLuint vao;
		GLuint vbo[2];
		glGenVertexArrays(1, &vao);
		glClear(GL_COLOR_BUFFER_BIT);
		glGenBuffers(2, vbo);
		GLuint elementbuffer1;
		GLuint elementbuffer2;
		glGenBuffers(1, &elementbuffer1);
		glGenBuffers(1, &elementbuffer2);

		// bool isUpHill = true; // this will need to be calculated later
		//Set up x and z value for position and red and blue values for colour
		//Alternating patterns to mesh triangles
		int xx, jj;
		for (ix = 0; ix < nx; ix++) {
			xx = ix*nt * 3;
			for (j = 0; j < nt; j++) {
				jj = j * 3;
				line[xx + jj] = (GLfloat)x[ix];
				line[xx + jj + 1] = (GLfloat)y[j];
				line[xx + jj + 2] = (GLfloat).2f;
				colour[xx + jj] = (GLfloat)colorsArr[ix][j].r;
				colour[xx + jj + 1] = (GLfloat)colorsArr[ix][j].g;
				colour[xx + jj + 2] = (GLfloat)colorsArr[ix][j].b;
			}
		}

		//Use shader to load in matrix and set viewing direction
		glUseProgram(programID);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		//Place data in an array to be drawn to the screen
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		GLuint sizeBytes = (3 * nt*nx) * sizeof(GLfloat);
		glBufferData(GL_ARRAY_BUFFER, sizeBytes, line, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_TRUE, 0, 0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeBytes, colour, GL_STATIC_DRAW);
		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_TRUE, 0, 0);
		glEnableVertexAttribArray(1);

		//	glDrawArrays(GL_TRIANGLES, 0, 6 * (nt-1));
		// now prepare for the other triangles using indexes

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer1);
		for (ix = 0; ix < nx - 1; ix++) {
			xx = ix*nt;
			if (fabs(x[ix] - x[ix + 1]) > dxMaxPlot) continue; // we do not plot data gap
															   // each trace have 3*(nt-1) indexes (or vertex)
			for (j = 0; j < nt - 1; j++) {
				jj = j * 3;

				// first set of triangles
				elem1[jj] = (unsigned int)(xx + j);
				elem1[jj + 1] = (unsigned int)(xx + nt + j);
				elem1[jj + 2] = (unsigned int)(xx + j + 1);

				// second set of triangles
				elem2[jj] = (unsigned int)(xx + j + 1);
				elem2[jj + 1] = (unsigned int)(xx + nt + j);
				elem2[jj + 2] = (unsigned int)(xx + nt + j + 1);
			}
			register int numElem = 3 * (nt - 1);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElem * sizeof(unsigned int), &elem1[0], GL_STATIC_DRAW);
			glDrawElements(GL_TRIANGLES, numElem, GL_UNSIGNED_INT, (void*)0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer2);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElem * sizeof(unsigned int), &elem2[0], GL_STATIC_DRAW);
			glDrawElements(GL_TRIANGLES, numElem, GL_UNSIGNED_INT, (void*)0);
		}
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		free1(line);
		free1(colour);
		free1(elem1);
		free1(elem2);
	}

	myFBOtoDC(pDC, &this->myGL.m_rectDraw, SRCCOPY);
//	myFBOtoBitMapFile();
}



//
//void OpenGLRendererFBO::depSeisFBO(float **ampArr, CColorHelper::AMPCOLORDYN *ampColor, float x[], float y[], int nx, int nt,
//	float dxMinPlot, float dxMaxPlot)
//{
//
//	//Set up framebuffer
//	glGenFramebuffers(1, &myGL.FramebufferName);
//	glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);
//	int ncol = this->myGL.m_rectPixel.Width();
//	int nrow = this->myGL.m_rectPixel.Height();
//	glViewport(0, 0, ncol, nrow);
//
//	//Create texture buffer
//	GLuint renderedTexture;
//	glGenTextures(1, &renderedTexture);
//	glBindTexture(GL_TEXTURE_2D, renderedTexture); // "Bind" the newly created texture : all future texture functions will modify this texture
//
//												   //Generate background colour and backgrou
//												   // Enable depth test
//												   // glEnable(GL_DEPTH_TEST);
//												   //glDepthFunc(GL_LESS); // Accept fragment if it closer to the camera than the former one
//												   //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glClearColor(1.0, 1.0, 1.0, 0.0); //background to clear with.
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
//		ncol, nrow,
//		0, GL_RGB, GL_UNSIGNED_BYTE, 0);// Give an empty image to OpenGL ( the last "0" )
//
//										// Poor filtering. Needed !
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	// The depth buffer
//	//glDepthFunc(GL_ALWAYS); // deping add
//	GLuint depthrenderbuffer;
//	glGenRenderbuffers(1, &depthrenderbuffer);
//	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ncol, nrow);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
//
//	// Set "renderedTexture" as our colour attachement #0
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
//
//	// Set the list of draw buffers.
//	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
//	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
//
//								   // Always check that our framebuffer is ok
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		return;
//	// #############Rendering to the texture
//	// Rendering to the texture is straightforward. Simply bind your framebuffer, and draw your scene as usual. Easy !
//	// Render to our framebuffer
//
//	GLuint programID = LoadShaders("E:/Jeff/work85vs2015/shaders/TransformVertexShader.vertexshader", "E:/Jeff/work85vs2015/shaders/ColorFragmentShader.fragmentshader");
//	//GLint color_attribute = glGetAttribLocation(programID, "color");
//	// Get a handle for our "MVP" uniform
//	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
//
//	// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/#homogeneous-coordinates
//	// Projection matrix : 45�� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
//	//glm::mat4 Projection = glm::perspective(
//	//	glm::radians(45.0f), 
//	//	(float)ncol / (float)nrow, 
//	//	0.1f, 100.0f);
//	// Or, for an ortho camera :
//	//glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates
//	glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1000.0f); // In world coordinates
//
//																				// Camera matrix
//	glm::mat4 View = glm::lookAt(
//		glm::vec3(0, 0, 1000), // Camera is at (4,3,3), in World Space
//		glm::vec3(0, 0, 0), // and looks at the origin
//		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
//	);
//	// Model matrix : an identity matrix (model will be at the origin)
//	glm::mat4 Model = glm::mat4(1.0f);
//	// Our ModelViewProjection : multiplication of our 3 matrices
//	glm::mat4 MVP = Projection * View * Model;
//
//	//##################################
//	int ix, j;
//
//	if (true) {
//		CColorHelper::rgb **colorsArr = (CColorHelper::rgb **)alloc2(nt, nx, sizeof(CColorHelper::rgb));
//		for (ix = 0; ix < nx; ix++) {
//			ampColor->getColorRef4Ampf(ampArr[ix], colorsArr[ix], nt);
//		}
//
//		//	each data point will need 2 triangles to plot
//		GLfloat *line = (GLfloat*)alloc1(nt * 3 * nx, sizeof(GLfloat)); // a triangle has 3 points
//		GLfloat *colour = (GLfloat*)alloc1(nt * 3 * nx, sizeof(GLfloat));
//		unsigned int *elem1 = (unsigned int*)alloc1(nt * 3, sizeof(unsigned int));
//		unsigned int *elem2 = (unsigned int*)alloc1(nt * 3, sizeof(unsigned int));
//
//		int count = 0;
////		glGenVertexArrays(myGL.numvo, myGL.vaoID); // not used at all
//
//		GLuint vao;
//		GLuint vbo[4];
//		glGenVertexArrays(1, &vao);
//		glClear(GL_COLOR_BUFFER_BIT);
//		glGenBuffers(2, vbo);
//		GLuint elementbuffer1;
//		GLuint elementbuffer2;
//		glGenBuffers(1, &elementbuffer1);
//		glGenBuffers(1, &elementbuffer2);
//
//		// bool isUpHill = true; // this will need to be calculated later
//		//Set up x and z value for position and red and blue values for colour
//		//Alternating patterns to mesh triangles
//		int xx, jj;
//		for (ix = 0; ix < nx; ix++) {
//			xx = ix*nt * 3;
//			for (j = 0; j < nt; j++) {
//				jj = j * 3;
//				line[xx + jj] = (GLfloat)x[ix];
//				line[xx + jj + 1] = (GLfloat)y[j];
//				line[xx + jj + 2] = (GLfloat).2f;
//				colour[xx + jj] = (GLfloat)colorsArr[ix][j].r;
//				colour[xx + jj + 1] = (GLfloat)colorsArr[ix][j].g;
//				colour[xx + jj + 2] = (GLfloat)colorsArr[ix][j].b;
//			}
//		}
//
//		//Use shader to load in matrix and set viewing direction
//		glUseProgram(programID);
//		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
//
//		//Place data in an array to be drawn to the screen
//		glBindVertexArray(vao);
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
//		GLuint sizeBytes = (3 * nt*nx) * sizeof(GLfloat);
//		glBufferData(GL_ARRAY_BUFFER, sizeBytes, line, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_TRUE, 0, 0);
//		glEnableVertexAttribArray(0);
//
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
//		glBufferData(GL_ARRAY_BUFFER, sizeBytes, colour, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_TRUE, 0, 0);
//		glEnableVertexAttribArray(1);
//
//		//	glDrawArrays(GL_TRIANGLES, 0, 6 * (nt-1));
//		// now prepare for the other triangles using indexes
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer1);
//		for (ix = 0; ix < nx - 1; ix++) {
//			xx = ix*nt;
//			if (fabs(x[ix] - x[ix + 1]) > dxMaxPlot) continue; // we do not plot data gap
//															   // each trace have 3*(nt-1) indexes (or vertex)
//			for (j = 0; j < nt - 1; j++) {
//				jj = j * 3;
//
//				// first set of triangles
//				elem1[jj] = (unsigned int)(xx + j);
//				elem1[jj + 1] = (unsigned int)(xx + nt + j);
//				elem1[jj + 2] = (unsigned int)(xx + j + 1);
//
//				// second set of triangles
//				elem2[jj] = (unsigned int)(xx + j + 1);
//				elem2[jj + 1] = (unsigned int)(xx + nt + j);
//				elem2[jj + 2] = (unsigned int)(xx + nt + j + 1);
//			}
//			register int numElem = 3 * (nt - 1);
//			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElem * sizeof(unsigned int), &elem1[0], GL_STATIC_DRAW);
//			glDrawElements(GL_TRIANGLES, numElem, GL_UNSIGNED_INT, (void*)0);
//
//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer2);
//			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElem * sizeof(unsigned int), &elem2[0], GL_STATIC_DRAW);
//			glDrawElements(GL_TRIANGLES, numElem, GL_UNSIGNED_INT, (void*)0);
//		}
//		glDisableVertexAttribArray(0);
//		glDisableVertexAttribArray(1);
//		free1(line);
//		free1(colour);
//		free1(elem1);
//		free1(elem2);
//		free2((void **)colorsArr);
//	}
//
//	myFBOtoDC(1500);
////	myFBOtoBitMapFile();
//}

//
//void OpenGLRendererFBO::FirstFrameBufferImage(float **xArr, int nx, int nt)
//{
//	// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
//
//	// Dark blue background
//	glClearColor(0.4f, 0.0f, 0.4f, 0.0f);
//
//	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
// 	glGenFramebuffers(1, &myGL.FramebufferName);
//	glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);
//
//	// Now we need to create the texture which will contain the RGB output of our shader.This code is very classic :
//	// The texture we're going to render to
//	GLuint renderedTexture;
//	glGenTextures(1, &renderedTexture);
//	glBindTexture(GL_TEXTURE_2D, renderedTexture); // "Bind" the newly created texture : all future texture functions will modify this texture
//
//	int ncol = this->myGL.m_rectPixel.Width();
//	int nrow = this->myGL.m_rectPixel.Height();
//
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 
//		ncol, nrow,
//		0, GL_RGB, GL_UNSIGNED_BYTE, 0);// Give an empty image to OpenGL ( the last "0" )
//
//	// Poor filtering. Needed !
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	// The depth buffer
//	GLuint depthrenderbuffer;
//	glGenRenderbuffers(1, &depthrenderbuffer);
//	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ncol, nrow);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
//
//	// Set "renderedTexture" as our colour attachement #0
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
//
//	// Set the list of draw buffers.
//	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
//	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
//
//								   // Always check that our framebuffer is ok
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		return;
//
//	// #############Rendering to the texture
//	// Rendering to the texture is straightforward. Simply bind your framebuffer, and draw your scene as usual. Easy !
//	// Render to our framebuffer
//	glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);
////	glViewport(0, 0, ncol, nrow); // Render on the whole framebuffer, complete from the lower left corner to the upper right
//
//								 // The fullscreen quad's FBO
//	GLuint quad_VertexArrayID;
//	glGenVertexArrays(1, &quad_VertexArrayID);
//	glBindVertexArray(quad_VertexArrayID);
//
//	static const GLfloat g_vertex_buffer_data[] = {
//		0.0f, 0.0f, 0.0f,
//		1.0f, 0.0f, 0.0f,
//		0.0f,  1.0f, 0.0f,
//	};
//	GLuint vertexbuffer;
//	glGenBuffers(1, &vertexbuffer);
//	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
//
//	// Create and compile our GLSL program from the shaders
//	GLuint programID = LoadShaders("E:/Jeff/Passthrough.vertexshader", "E:/Jeff/SimpleTexture.fragmentshader");
//	glClear(GL_COLOR_BUFFER_BIT);
//
//	// Use our shader
//	glUseProgram(programID);
//
//	// 1rst attribute buffer : vertices
//	glEnableVertexAttribArray(0);
//	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//	glVertexAttribPointer(
//		0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//		3,                  // size
//		GL_FLOAT,           // type
//		GL_FALSE,           // normalized?
//		0,                  // stride
//		(void*)0            // array buffer offset
//	);
//
//	// Draw the triangle !
//	glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
//
//	glDisableVertexAttribArray(0);
//
////	myFBOtoBitMapFile();
//	myFBOtoDC(1500);
//}

//bool OpenGLRendererFBO::myFBOtoDC(int memMaxMB)
//{
//	CDib dib;
//
//	// note: after this call, nrow or ncol can change
//	int ncol = this->myGL.m_rectPixel.Width();
//	int nrow = this->myGL.m_rectPixel.Height();
//	float reduceRate = dib.Create24b(ncol, nrow);
//	if (reduceRate == 0.f) {
//		return false;
//	}
//	dib.SetPalette(myGL.parentDC2);
//
//	byte *buff = (byte *)dib.getBits();
////	glReadBuffer(GL_BACK);
//
//	flushGLErrors();
//	glPixelStorei(GL_PACK_SWAP_BYTES, GL_FALSE);
//	glPixelStorei(GL_PACK_LSB_FIRST, GL_TRUE);
//	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
//	glPixelStorei(GL_PACK_IMAGE_HEIGHT, 0);
//	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);
//	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
//	glPixelStorei(GL_PACK_ALIGNMENT, 1);
//	//glReadPixels(0, 0, ncol, nrow, GL_BGR_EXT, GL_UNSIGNED_BYTE, buff);
//	glReadPixels(0, 0, ncol, nrow, GL_BGR, GL_UNSIGNED_BYTE, buff);
//
//	int aLeft = myGL.m_isSwapX ? this->myGL.m_rectDraw.left + this->myGL.m_rectDraw.Width() : this->myGL.m_rectDraw.left;
//	int aWidth = myGL.m_isSwapX ? -this->myGL.m_rectDraw.Width() : this->myGL.m_rectDraw.Width();
//	//if (!dib.Draw(myGL.parentDC, aLeft, this->myGL.m_rectDraw.top,
//	//	aWidth, this->myGL.m_rectDraw.Height(),
//	//	!myGL.m_isFaceUp)) {
//	//	throw;
//	//}
//	if (!dib.Draw(myGL.parentDC2, this->myGL.m_rectDraw.left, this->myGL.m_rectDraw.top,
//		this->myGL.m_rectDraw.Width(), this->myGL.m_rectDraw.Height(),
//		!myGL.m_isFaceUp, myGL.m_isSwapX)) {
//		throw;
//	}
//	return true;
//}


// this will need later
/*
void OpenGLRendererFBO::myDrawSceneLines(float **ampArr, float x[], float y[], int nx, int nt, double dxMin4Plot)
{
wglMakeCurrent(myGL.glDC->m_hDC, myGL.m_hMemRC);
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
SwapBuffers(myGL.glDC->m_hDC);
wglMakeCurrent(NULL, NULL);

// restore old DC and RC
//wglMakeCurrent(myGL.m_hOldDC, myGL.m_hOldRC);

free1(line);
}
*/


// https://www.khronos.org/opengl/wiki/Common_Mistakes#Image_precision
//void OpenGLRendererFBO::myDrawSceneLinesFBO(float **ampArr, float x[], float y[], int nx, int nt, double dxMin4Plot)
//{
//	glGenFramebuffers(1, &myGL.FramebufferName);
//	glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);
//	int ncol = this->myGL.m_rectPixel.Width();
//	int nrow = this->myGL.m_rectPixel.Height();
//	glViewport(0, 0, ncol, nrow);
//
//	GLuint renderedTexture;
//	glGenTextures(1, &renderedTexture);
//	glBindTexture(GL_TEXTURE_2D, renderedTexture); // "Bind" the newly created texture : all future texture functions will modify this texture
//
//	glClearColor(0.0, 0.8, 1.0, 0.0); //background to clear with.
//	glTexImage2D(GL_TEXTURE_2D, 0, 
//		GL_RGB, // GL_RGB means 3 components
//		ncol, nrow,
//		0, 
//		GL_RGB, 
//		GL_UNSIGNED_BYTE, 0);// Give an empty image to OpenGL ( the last "0" )
//
//										// Poor filtering. Needed !
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	// The depth buffer
//	glDepthFunc(GL_ALWAYS); // deping add
//	GLuint depthrenderbuffer;
//	glGenRenderbuffers(1, &depthrenderbuffer);
//	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ncol, nrow);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
//
//	// Set "renderedTexture" as our colour attachement #0
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
//
//	// Set the list of draw buffers.
//	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
//	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
//
//								   // Always check that our framebuffer is ok
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		return;
//	// #############Rendering to the texture
//	// Rendering to the texture is straightforward. Simply bind your framebuffer, and draw your scene as usual. Easy !
//	// Render to our framebuffer
//
//
////##################################
//	const int GLIntSize = 3; //number of floats per item. i.e. number of floats per point in space.
//	if (true) {
//		int GLSizeCount = nt; //number of items. ie sizeof(TriangleA)/m_GLIntSize
//		GLfloat *line = (GLfloat*)alloc1(nt * GLIntSize, sizeof(GLfloat));
//		double xPlotted = -99999.;
//
//		for (int j = 0; j < nt; j++) {
//			line[j * GLIntSize + 1] = (GLfloat)y[j];
//			line[j * GLIntSize + 2] = (GLfloat)-1.0;
//		}
//
//		int count = 0;
//		glGenVertexArrays(myGL.numvo, myGL.vaoID);
//		glClear(GL_COLOR_BUFFER_BIT);
//		for (int ix = 0; ix < nx; ix++) {
//			if (fabs(x[ix] - xPlotted) < dxMin4Plot) continue;  // skip too dense traces
//			xPlotted = x[ix];
//
//			for (int j = 0; j < nt; j++) {
//				line[j * GLIntSize] = (GLfloat)ampArr[ix][j];
//			}
//
//			glBindVertexArray(myGL.vaoID[ix]);
//			glGenBuffers(1, &myGL.vboID[ix]); //VBO allocation
//			glBindBuffer(GL_ARRAY_BUFFER, myGL.vboID[ix]);
//			glBufferData(GL_ARRAY_BUFFER, nt * GLIntSize * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
//			glBufferSubData(GL_ARRAY_BUFFER, 0, nt * GLIntSize * sizeof(GLfloat), line);
//			glVertexAttribPointer((GLuint)0, GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);
//			glEnableVertexAttribArray(0);
//			glDrawArrays(GL_LINE_STRIP, 0, GLSizeCount);
//			count++;
//		}
//		free1(line);
//	}
//
//	if (false) {
//		static const GLfloat g_vertex_buffer_data[] = {
//			-1.0f, -1.0f, 0.0f,
//			0.0f, -1.0f, 0.0f,
//			-1.0f,  0.0f, 0.0f,
//		};
//		GLuint vertexbuffer;
//		glGenBuffers(1, &vertexbuffer);
//		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
//
//		// Create and compile our GLSL program from the shaders
//		GLuint programID = LoadShaders("E:/Jeff/Passthrough.vertexshader", "E:/Jeff/SimpleTexture.fragmentshader");
//	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//
//		// Use our shader
//		glUseProgram(programID);
//
//		// 1rst attribute buffer : vertices
//		glEnableVertexAttribArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//		glVertexAttribPointer(
//			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//			GLIntSize,                  // size
//			GL_FLOAT,           // type
//			GL_FALSE,           // normalized?
//			0,                  // stride
//			(void*)0            // array buffer offset
//		);
//		glEnableVertexAttribArray(0);
//		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
//										  //	glDisableVertexAttribArray(0);
//	}
//
//
//
//
//	if (false) {
//		static const GLfloat g_vertex_buffer_data[] = {
//			0.0f, 0.0f, 0.0f,
//			1.0f, 0.0f, 0.0f,
//			0.0f,  1.0f, 0.0f,
//		};
//		GLuint vertexbuffer;
//		glGenBuffers(1, &vertexbuffer);
//		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
//
//		// Create and compile our GLSL program from the shaders
//		GLuint programID = LoadShaders("E:/Jeff/Passthrough.vertexshader", "E:/Jeff/SimpleTexture.fragmentshader");
//	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//
//		// Use our shader
//		glUseProgram(programID);
//
//		// 1rst attribute buffer : vertices
//		glEnableVertexAttribArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//		glVertexAttribPointer(
//			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//			GLIntSize,                  // size
//			GL_FLOAT,           // type
//			GL_FALSE,           // normalized?
//			0,                  // stride
//			(void*)0            // array buffer offset
//		);
//		glEnableVertexAttribArray(0);
//		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
//	//	glDisableVertexAttribArray(0);
//	}
//
//
//
//
//
//
//
//	myFBOtoDC(1500);
////	myFBOtoBitMapFile();
//
//}

//
//void OpenGLRendererFBO::myDrawSceneFills(float **xArr, int nx, int nt)
//{
//	wglMakeCurrent(myGL.m_hMemDC, myGL.m_hMemRC);
//	glClearColor(0.0, 0.8, 1.0, 0.0); //background to clear with.
//	const int GLIntSize = 3; //Number of floats per vertex (x,y,z, etc.)
//	const int GLSizeCount = 6; //Number of vertices in the polygon/triangle fan
//
//
//	GLuint shaderProgram = create_program("E:/project/cpp.net/myOpengl/shaders/vert.shader", "E:/project/cpp.net/myOpengl/shaders/frag.shader");
//	GLint position_attribute = glGetAttribLocation(shaderProgram, "position");
//	GLint color_attribute = glGetAttribLocation(shaderProgram, "color");
//	//Add Shader
//
//	glClear(GL_COLOR_BUFFER_BIT);
//	glGenVertexArrays(10, myGL.vaoID); //The 10 will need to be dynamic, aka amount of polygons to be drawn
//									   //// First VAO setup
//
//
//	for (int i = 0; i < 10; i++)
//	{
//
//		if (i % 2 == 0) {
//			GLfloat colors[] =
//			{
//				1.0, 0.0, 0.0,
//				1.0, 0.0, 0.0,
//				1.0, 0.0, 0.0,
//				1.0, 0.0, 0.0,
//				1.0, 0.0, 0.0,
//				1.0, 0.0, 0.0
//			};
//			//RGB colouring. One colour per vertex. Matches in order with the vertices of the polygon.
//			GLfloat TriangleA[] =
//			{
//				0.0f, 1.0f - i*0.2f, -1.0f,
//				0.0f, 0.8f - i*0.2f, -1.0f,
//				0.2f, 0.85f - i*0.2f, -1.0f,
//				0.3f, 0.9f - i*0.2f, -1.0f,
//				0.4f, 0.95f - i*0.2f, -1.0f,
//				0.0f, 1.0f - i*0.2f, -1.0f
//			};
//			//Vertices of the Polygon (number of vertices will be dynamic)
//			//Graphics card reads it from the top left corner going counter-clockwise
//
//			glBindVertexArray(myGL.vaoID[1]);  //<- These i's used in vao and vbo will be the i'th polygon being stored
//			glGenBuffers(1, &myGL.vaoID[1]);
//			glBindBuffer(GL_ARRAY_BUFFER, myGL.vboID[1]);
//			glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleA) + sizeof(colors), NULL, GL_STATIC_DRAW);
//			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TriangleA), TriangleA);   //This subdata is to hold the vertices polygon
//			glBufferSubData(GL_ARRAY_BUFFER, sizeof(TriangleA), sizeof(colors), colors); //This subdata holds the colours of the vertices
//			glVertexAttribPointer((GLuint)0, GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);   //This tells the system how many floats per vertex
//			glEnableVertexAttribArray(0);
//			glVertexAttribPointer(color_attribute, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(TriangleA)); //This tells the system how many floats per colour
//			glEnableVertexAttribArray(color_attribute);
//			glDrawArrays(GL_TRIANGLE_FAN, 0, GLSizeCount);
//		}
//		else {
//			GLfloat colors[] =
//			{
//				0.0, 0.0, 1.0,
//				0.0, 0.0, 1.0,
//				0.0, 0.0, 1.0,
//				0.0, 0.0, 1.0,
//				0.0, 0.0, 1.0,
//				0.0, 0.0, 1.0
//			};
//			GLfloat TriangleA[] =
//			{
//				0.0f, 1.0f - i*0.2f, -1.0f,
//				-0.2f, 0.95f - i*0.2f, -1.0f,
//				-0.3f, 0.9f - i*0.2f, -1.0f,
//				-0.2f, 0.85f - i*0.2f, -1.0f,
//				0.0f, 0.8f - i*0.2f, -1.0f,
//				0.0f, 1.0f - i*0.2f, -1.0f
//			};
//			glBindVertexArray(myGL.vaoID[1]);
//			glGenBuffers(1, &myGL.vaoID[1]);
//			glBindBuffer(GL_ARRAY_BUFFER, myGL.vboID[1]);
//			glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleA) + sizeof(colors), NULL, GL_STATIC_DRAW);
//			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(TriangleA), TriangleA);
//			glBufferSubData(GL_ARRAY_BUFFER, sizeof(TriangleA), sizeof(colors), colors);
//			glVertexAttribPointer((GLuint)0, GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);
//			glEnableVertexAttribArray(0);
//			glVertexAttribPointer(color_attribute, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)sizeof(TriangleA));
//			glEnableVertexAttribArray(color_attribute);
//			glDrawArrays(GL_TRIANGLE_FAN, 0, GLSizeCount);
//
//
//		}
//
//	}
//
//
//	//for (int i = 1; i<10; i++) {
//	//	glBindVertexArray(myGL.vaoID[i]);   //Load each i'th polygon to be displayed
//	//	glDrawArrays(GL_TRIANGLE_FAN, 0, GLSizeCount);   //Draws the shape using triangle fan from vertex 0 to the last vertex
//	//}
//
//	SwapBuffers(myGL.m_hMemDC);
//	wglMakeCurrent(NULL, NULL);
//}


//#include "../abc 3/ConsoleApplication2/glm-0.9.7.1/glm/glm.hpp"
//#include "../abc 3/ConsoleApplication2/glm-0.9.7.1/glm/gtc/matrix_transform.hpp"
//#include "../abc 3/ConsoleApplication2/common/texture.h"
//#include "../abc 3/ConsoleApplication2/common/objloader.h"
//#include "../abc 3/ConsoleApplication2/common/vboindexer.h"
//void OpenGLRendererFBO::myTestFBO(float **ampArr, CColorHelper::AMPCOLORDYN *ampColor, float x[], float y[], int nx, int nt, double dxMin4Plot, double dyMin4Plot)
//{
//
//	//Set up framebuffer
//	glGenFramebuffers(1, &myGL.FramebufferName);
//	glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);
//	int ncol = this->myGL.m_rectPixel.Width();
//	int nrow = this->myGL.m_rectPixel.Height();
//	glViewport(0, 0, ncol, nrow);
//
//	//Create texture buffer
//	GLuint renderedTexture;
//	glGenTextures(1, &renderedTexture);
//	glBindTexture(GL_TEXTURE_2D, renderedTexture); // "Bind" the newly created texture : all future texture functions will modify this texture
//
//												   //Generate background colour and backgrou
//	glClearColor(0.0, 0.8, 1.0, 0.0); //background to clear with.
//
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
//		ncol, nrow,
//		0, GL_RGB, GL_UNSIGNED_BYTE, 0);// Give an empty image to OpenGL ( the last "0" )
//
//										// Poor filtering. Needed !
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	// The depth buffer
//	//glDepthFunc(GL_ALWAYS); // deping add
//	GLuint depthrenderbuffer;
//	glGenRenderbuffers(1, &depthrenderbuffer);
//	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ncol, nrow);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
//
//	// Set "renderedTexture" as our colour attachement #0
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
//
//	// Set the list of draw buffers.
//	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
//	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
//
//								   // Always check that our framebuffer is ok
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		return;
//	// #############Rendering to the texture
//	// Rendering to the texture is straightforward. Simply bind your framebuffer, and draw your scene as usual. Easy !
//	// Render to our framebuffer
//
//	GLuint programID = LoadShaders("E:/Jeff/work85vs2015/shaders/TransformVertexShader.vertexshader", "E:/Jeff/work85vs2015/shaders/ColorFragmentShader.fragmentshader");
//	//GLint color_attribute = glGetAttribLocation(programID, "color");
//	// Get a handle for our "MVP" uniform
//	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
//
//	// Projection matrix : 45�� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
//	//glm::mat4 Projection = glm::perspective(15.0f, 1.0f, 0.1f, 100.0f);
//	// Or, for an ortho camera :
//	glm::mat4 Projection = glm::ortho(-2.0f,2.0f,-2.0f,2.0f,0.0f,100.0f); // In world coordinates
//
//	// Camera matrix
//	glm::mat4 View = glm::lookAt(
//		glm::vec3(0, 2, 15), // Camera is at (4,3,3), in World Space
//		glm::vec3(0, 0, 0), // and looks at the origin
//		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
//	);
//	// Model matrix : an identity matrix (model will be at the origin)
//	glm::mat4 Model = glm::mat4(1.0f);
//	// Our ModelViewProjection : multiplication of our 3 matrices
//	glm::mat4 MVP = Projection * View * Model;
//
//	//##################################
//	const int GLIntSize = 3; //number of floats per item. i.e. number of floats per point in space.
//	if (false) {
//
//		CColorHelper::rgb **colorsArr = (CColorHelper::rgb **)alloc2(nt, nx, sizeof(CColorHelper::rgb));
//		for (int ix = 0; ix < nx; ix++) {
//			ampColor->getColorRef4Ampf(ampArr[ix], colorsArr[ix], nt);
//		}
//
//	//	int GLSizeCount = nt*2; //number of items. each data point will need 2 triangles to plot
//		GLfloat *line = (GLfloat*)alloc1(6 * nt * GLIntSize, sizeof(GLfloat)); // a triangle has 3 points
//		double xPlotted = -99999.;
//		GLfloat *colour = (GLfloat*)alloc1(6 * nt * GLIntSize, sizeof(GLfloat));
//
//		//Set up y values for position and the green values of the colour
//		//Alternating patterns to mesh triangles
//		for (int j = 0; j < nt - 1; j++) {
//			line[j * 6 * GLIntSize + 0] = (GLfloat)y[j];
//			line[j * 6 * GLIntSize + 3] = (GLfloat)y[j + 1];
//			line[j * 6 * GLIntSize + 6] = (GLfloat)y[j];
//				
//			line[j * 6 * GLIntSize + 9] = (GLfloat)y[j];
//			line[j * 6 * GLIntSize + 12] = (GLfloat)y[j + 1];
//			line[j * 6 * GLIntSize + 15] = (GLfloat)y[j + 1];
//		}
//
//		int count = 0;
//		int skip = 1;
//		glGenVertexArrays(myGL.numvo, myGL.vaoID);
//		glClear(GL_COLOR_BUFFER_BIT);
//		for (int ix = 0; ix < nx-1; ix++) {
//			if (fabs(x[ix] - xPlotted) < dxMin4Plot) {
//				skip++;
//				continue;
//			}// skip too dense traces
//			xPlotted = x[ix];
//
//			//Set up x and z value for position and red and blue values for colour
//			//Alternating patterns to mesh triangles
//			for (int j = 0; j < nt - 1; j++) {
//
//				register int jj = j * 18;
//
//				if (true) {
//					line[j * 6 * GLIntSize + 1] = (GLfloat)ampArr[ix][j];
//					line[j * 6 * GLIntSize + 4] = (GLfloat)ampArr[ix][j + 1];
//
//					if (ix + skip < nx) {
//						line[j * 6 * GLIntSize + 2] = (GLfloat)x[ix];
//						line[j * 6 * GLIntSize + 5] = (GLfloat)x[ix];
//						line[j * 6 * GLIntSize + 8] = (GLfloat)x[ix + skip];
//
//						line[j * 6 * GLIntSize + 7] = (GLfloat)ampArr[ix + skip][j];
//						line[j * 6 * GLIntSize + 10] = (GLfloat)ampArr[ix + skip][j];
//						line[j * 6 * GLIntSize + 16] = (GLfloat)ampArr[ix + skip][j + 1];
//
//						line[j * 6 * GLIntSize + 11] = (GLfloat)x[ix + skip];
//						line[j * 6 * GLIntSize + 14] = (GLfloat)x[ix];
//						line[j * 6 * GLIntSize + 17] = (GLfloat)x[ix + skip];
//
//						colour[jj] = (GLfloat)colorsArr[ix][j].r;
//						colour[jj + 1] = (GLfloat)colorsArr[ix][j].g;
//						colour[jj + 2] = (GLfloat)colorsArr[ix][j].b;
//
//						colour[jj + 3] = (GLfloat)colorsArr[ix][j+1].r;
//						colour[jj + 4] = (GLfloat)colorsArr[ix][j+1].g;
//						colour[jj + 5] = (GLfloat)colorsArr[ix][j+1].b;
//
//						colour[jj + 6] = (GLfloat)colorsArr[ix + skip][j].r;
//						colour[jj + 7] = (GLfloat)colorsArr[ix + skip][j].g;
//						colour[jj + 8] = (GLfloat)colorsArr[ix + skip][j].b;
//
//						colour[jj + 9] = (GLfloat)colorsArr[ix + skip][j].r;
//						colour[jj + 10] = (GLfloat)colorsArr[ix + skip][j].g;
//						colour[jj + 11] = (GLfloat)colorsArr[ix + skip][j].b;
//
//						colour[jj + 12] = (GLfloat)colorsArr[ix][j + 1].r;
//						colour[jj + 13] = (GLfloat)colorsArr[ix][j + 1].g;
//						colour[jj + 14] = (GLfloat)colorsArr[ix][j + 1].b;
//
//						colour[jj + 15] = (GLfloat)colorsArr[ix + skip][j + 1].r;
//						colour[jj + 16] = (GLfloat)colorsArr[ix + skip][j + 1].g;
//						colour[jj + 17] = (GLfloat)colorsArr[ix + skip][j + 1].b;
//					}
//					else {
//						line[j * 6 * GLIntSize + 2] = (GLfloat)x[ix];
//						line[j * 6 * GLIntSize + 5] = (GLfloat)x[ix];
//						line[j * 6 * GLIntSize + 8] = (GLfloat)x[nx-1];
//
//						line[j * 6 * GLIntSize + 7] = (GLfloat)ampArr[nx - 1][j];
//						line[j * 6 * GLIntSize + 10] = (GLfloat)ampArr[nx - 1][j];
//						line[j * 6 * GLIntSize + 16] = (GLfloat)ampArr[nx - 1][j + 1];
//
//						line[j * 6 * GLIntSize + 11] = (GLfloat)x[nx - 1];
//						line[j * 6 * GLIntSize + 14] = (GLfloat)x[nx];
//						line[j * 6 * GLIntSize + 17] = (GLfloat)x[nx - 1];
//
//
//
//
//						colour[jj + 3] = (GLfloat)colorsArr[ix][j + 1].r;
//						colour[jj + 4] = (GLfloat)colorsArr[ix][j + 1].g;
//						colour[jj + 5] = (GLfloat)colorsArr[ix][j + 1].b;
//
//						colour[jj + 6] = (GLfloat)colorsArr[nx-1][j].r;
//						colour[jj + 7] = (GLfloat)colorsArr[nx - 1][j].g;
//						colour[jj + 8] = (GLfloat)colorsArr[nx - 1][j].b;
//
//						colour[jj + 9] = (GLfloat)colorsArr[nx - 1][j].r;
//						colour[jj + 10] = (GLfloat)colorsArr[nx - 1][j].g;
//						colour[jj + 11] = (GLfloat)colorsArr[nx - 1][j].b;
//
//						colour[jj + 12] = (GLfloat)colorsArr[ix][j + 1].r;
//						colour[jj + 13] = (GLfloat)colorsArr[ix][j + 1].g;
//						colour[jj + 14] = (GLfloat)colorsArr[ix][j + 1].b;
//
//						colour[jj + 15] = (GLfloat)colorsArr[nx - 1][j + 1].r;
//						colour[jj + 16] = (GLfloat)colorsArr[nx - 1][j + 1].g;
//						colour[jj + 17] = (GLfloat)colorsArr[nx - 1][j + 1].b;
//
//					}
//
//					line[j * 6 * GLIntSize + 13] = (GLfloat)ampArr[ix][j + 1];
//				}
//				skip = 1;
//				
//			}
//
//			//Use shader to load in matrix and set viewing direction
//			glUseProgram(programID);
//			glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
//
//			glBindVertexArray(myGL.vaoID[ix]);
//			glGenBuffers(1, &myGL.vboID[ix]); //VBO allocation
//			glBindBuffer(GL_ARRAY_BUFFER, myGL.vboID[ix]);
//			glBufferData(GL_ARRAY_BUFFER, 12 * nt * GLIntSize * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
//			glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * nt * GLIntSize * sizeof(GLfloat), line);
//			glBufferSubData(GL_ARRAY_BUFFER, 6 * nt * GLIntSize * sizeof(GLfloat), 6 * nt * GLIntSize * sizeof(GLfloat), colour);
//			glVertexAttribPointer((GLuint)0, GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);
//			glEnableVertexAttribArray(0);
//			GLuint temp = 6 * nt * GLIntSize * sizeof(GLfloat);
//			glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid *)temp);
//			glEnableVertexAttribArray(1);
//			glDrawArrays(GL_TRIANGLES, 0, 3 * nt);
//
//			glDisableVertexAttribArray(0);
//			glDisableVertexAttribArray(1);
//			count++;
//		}
//		free1(line);
//		free1(colour);
//		free2((void **)colorsArr);
//	}
//
//	int ix, j;
//
//	if (true) {
//		CColorHelper::rgb **colorsArr = (CColorHelper::rgb **)alloc2(nt, nx, sizeof(CColorHelper::rgb));
//		for (ix = 0; ix < nx; ix++) {
//			ampColor->getColorRef4Ampf(ampArr[ix], colorsArr[ix], nt);
//		}
//
//		//	each data point will need 2 triangles to plot
//		GLfloat *line = (GLfloat*)alloc1(nt * 3 * nx, sizeof(GLfloat)); // a triangle has 3 points
//		GLfloat *colour = (GLfloat*)alloc1(nt * 3 * nx, sizeof(GLfloat));
//		unsigned int *elem1 = (unsigned int*)alloc1(nt * 3, sizeof(unsigned int));
//		unsigned int *elem2 = (unsigned int*)alloc1(nt * 3, sizeof(unsigned int));
//
//		GLfloat *texCoord = (GLfloat*)alloc1(nt * 2 * nx, sizeof(GLfloat));
//
//		int count = 0;
//		glGenVertexArrays(myGL.numvo, myGL.vaoID); // not used at all
//
//		glEnable(GL_NORMALIZE);
//		GLfloat *temp = (GLfloat*)alloc1(nt * 2 * nx, sizeof(GLfloat));
//		glGetFloatv(GL_CURRENT_NORMAL, temp);
//
//		GLuint vao;
//		GLuint vbo[2];
//		glGenVertexArrays(1, &vao);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glGenBuffers(2, vbo);
//		GLuint elementbuffer1;
//		GLuint elementbuffer2;
//		glGenBuffers(1, &elementbuffer1);
//		glGenBuffers(1, &elementbuffer2);
//
//		// bool isUpHill = true; // this will need to be calculated later
//		//Set up x and z value for position and red and blue values for colour
//		//Alternating patterns to mesh triangles
//
//		double xPlotted = -99999.;
//		int xx, jj;
//		double yPlotted = -99999.;
//
//		dxMin4Plot = 1.0 / 2048;
//		dyMin4Plot = 1.0 / 2048;
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer1);
//		for (ix = 0; ix < nx; ix++) {
//			xx = ix*nt*3;
//			if (fabs(x[ix] - xPlotted) < dxMin4Plot) {
//				continue;
//			}// skip too dense traces
//			xPlotted = x[ix];
//
//			for (j = 0; j < nt; j++) {
//				if (fabs(y[j] - yPlotted) < dyMin4Plot) {
//					continue;
//				}// skip too dense traces
//				yPlotted = y[j];
//
//				jj = j * 3;
//
//				line[xx + jj + 0] = (GLfloat)x[ix];
//				line[xx + jj + 1] = (GLfloat)y[j];
//				line[xx + jj + 2] = (GLfloat)ampArr[ix][j];
//
//
//				colour[xx + jj] = (GLfloat)colorsArr[ix][j].r;
//				colour[xx + jj + 1] = (GLfloat)colorsArr[ix][j].g;
//				colour[xx + jj + 2] = (GLfloat)colorsArr[ix][j].b;
//
//			}
//		}
//
//		//Use shader to load in matrix and set viewing direction
//		glUseProgram(programID);
//		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
//
//		//Place data in an array to be drawn to the screen
//		glBindVertexArray(vao);
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
//		GLuint sizeBytes = (3 * nt*nx) * sizeof(GLfloat);
//		glBufferData(GL_ARRAY_BUFFER, sizeBytes, line, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_TRUE, 0, 0);
//		glEnableVertexAttribArray(0);
//
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
//		glBufferData(GL_ARRAY_BUFFER, sizeBytes, colour, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_TRUE, 0, 0);
//		glEnableVertexAttribArray(1);
//
//		//	glDrawArrays(GL_TRIANGLES, 0, 6 * (nt-1));
//		// now prepare for the other triangles using indexes
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer1);
//		for (ix = 0; ix < nx - 1; ix++) {
//			xx = ix*nt;
//			if (fabs(x[ix] - xPlotted) < dxMin4Plot)
//				continue;// skip too dense traces
//			xPlotted = x[ix];
//
//
//			for (j = 0; j < nt - 1; j++) {
//				if (fabs(y[j] - yPlotted) < dyMin4Plot) {
//					continue;
//				}// skip too dense traces
//				yPlotted = y[j];
//				jj = j * 3;
//
//				// first set of triangles
//				elem1[jj] = (unsigned int)(xx + j);
//				elem1[jj + 1] = (unsigned int)(xx + nt + j);
//				elem1[jj + 2] = (unsigned int)(xx + j + 1);
//
//				// second set of triangles
//				elem2[jj] = (unsigned int)(xx + j + 1);
//				elem2[jj + 1] = (unsigned int)(xx + nt + j);
//				elem2[jj + 2] = (unsigned int)(xx + nt + j + 1);
//			}
//			register int numElem = 3 * (nt - 1);
//			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElem * sizeof(unsigned int), &elem1[0], GL_STATIC_DRAW);
//			glDrawElements(GL_TRIANGLES, numElem, GL_UNSIGNED_INT, (void*)0);
//
//			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer2);
//			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElem * sizeof(unsigned int), &elem2[0], GL_STATIC_DRAW);
//			glDrawElements(GL_TRIANGLES, numElem, GL_UNSIGNED_INT, (void*)0);
//		}
//		glDisableVertexAttribArray(0);
//		glDisableVertexAttribArray(1);
//		free1(line);
//		free1(colour);
//		free1(elem1);
//		free1(elem2);
//		free2((void **)colorsArr);
//	}
//
//
//	if (false) {
//		static const GLfloat g_vertex_buffer_data[] = {
//			-1.0f, -1.0f, 0.0f,
//			1.0f, 0.0f, 0.0f,
//			0.0f,  1.0f, 0.0f,
//		};
//		GLuint vertexbuffer;
//		glGenBuffers(1, &vertexbuffer);
//		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
//
//		// Create and compile our GLSL program from the shaders
//		GLuint programID = LoadShaders("E:/Jeff/Passthrough.vertexshader", "E:/Jeff/SimpleTexture.fragmentshader");
//		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//
//		// Use our shader
//		glUseProgram(programID);
//
//		// 1rst attribute buffer : vertices
//		glEnableVertexAttribArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//		glVertexAttribPointer(
//			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//			GLIntSize,                  // size
//			GL_FLOAT,           // type
//			GL_TRUE,           // normalized?
//			0,                  // stride
//			(void*)0            // array buffer offset
//		);
//		glEnableVertexAttribArray(0);
//		glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
//										  //	glDisableVertexAttribArray(0);
//	}
//
//	if (false) {
//		glEnable(GL_DEPTH_TEST);
//
//		// Accept fragment if it closer to the camera than the former one
//		glDepthFunc(GL_LESS);
//
//		// Cull triangles which normal is not towards the camera
//		glEnable(GL_CULL_FACE);
//
//
//		GLuint VertexArrayID;
//		glGenVertexArrays(1, &VertexArrayID);
//		glBindVertexArray(VertexArrayID);
//
//		// Create and compile our GLSL program from the shaders
//		GLuint depthProgramID = LoadShaders("E:/Jeff/work85vs2015/shaders/DepthRTT.vertexshader", "E:/Jeff/work85vs2015/shaders/DepthRTT.fragmentshader");
//
//		// Get a handle for our "MVP" uniform
//		GLuint depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");
//
//		// Load the texture
//		GLuint Texture = loadDDS("E:/Jeff/work85vs2015/shaders/uvmap.DDS");
//
//		// Read our .obj file
//		std::vector<glm::vec3> vertices;
//		std::vector<glm::vec2> uvs;
//		std::vector<glm::vec3> normals;
//		bool res = loadOBJ("E:/Jeff/work85vs2015/shaders/room_thickwalls.obj", vertices, uvs, normals);
//
//		std::vector<unsigned short> indices;
//		std::vector<glm::vec3> indexed_vertices;
//		std::vector<glm::vec2> indexed_uvs;
//		std::vector<glm::vec3> indexed_normals;
//		indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
//
//		// Load it into a VBO
//
//		GLuint vertexbuffer;
//		glGenBuffers(1, &vertexbuffer);
//		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//		glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);
//
//		GLuint uvbuffer;
//		glGenBuffers(1, &uvbuffer);
//		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
//		glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);
//
//		GLuint normalbuffer;
//		glGenBuffers(1, &normalbuffer);
//		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
//		glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
//
//		// Generate a buffer for the indices as well
//		GLuint elementbuffer;
//		glGenBuffers(1, &elementbuffer);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);
//
//
//		// ---------------------------------------------
//		// Render to Texture - specific code begins here
//		// ---------------------------------------------
//
//		// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
//		GLuint FramebufferName = 0;
//		glGenFramebuffers(1, &FramebufferName);
//		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
//
//		// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
//		GLuint depthTexture;
//		glGenTextures(1, &depthTexture);
//		glBindTexture(GL_TEXTURE_2D, depthTexture);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 2048, 2048, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
//
//		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
//		glDrawBuffer(GL_NONE);
//
//		static const GLfloat g_quad_vertex_buffer_data[] = {
//			-1.0f, -1.0f, 0.0f,
//			1.0f, -1.0f, 0.0f,
//			-1.0f,  1.0f, 0.0f,
//			-1.0f,  1.0f, 0.0f,
//			1.0f, -1.0f, 0.0f,
//			1.0f,  1.0f, 0.0f,
//		};
//
//		GLuint quad_vertexbuffer;
//		glGenBuffers(1, &quad_vertexbuffer);
//		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
//
//		// Create and compile our GLSL program from the shaders
//		GLuint quad_programID = LoadShaders("E:/Jeff/work85vs2015/shaders/Passthrough.vertexshader", "E:/Jeff/work85vs2015/shaders/SimpleTexture.fragmentshader");
//		GLuint texID = glGetUniformLocation(quad_programID, "texture");
//
//
//		// Create and compile our GLSL program from the shaders
//		GLuint programID = LoadShaders("E:/Jeff/work85vs2015/shaders/ShadowMapping.vertexshader", "E:/Jeff/work85vs2015/shaders/ShadowMapping.fragmentshader");
//
//		// Get a handle for our "myTextureSampler" uniform
//		GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
//
//		// Get a handle for our "MVP" uniform
//		GLuint MatrixID = glGetUniformLocation(programID, "MVP");
//		GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
//		GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
//		GLuint DepthBiasID = glGetUniformLocation(programID, "DepthBiasMVP");
//		GLuint ShadowMapID = glGetUniformLocation(programID, "shadowMap");
//
//		// Get a handle for our "LightPosition" uniform
//		GLuint lightInvDirID = glGetUniformLocation(programID, "LightInvDirection_worldspace");
//
//		// Render to our framebuffer
//		glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
//		glViewport(0, 0, ncol, nrow); // Render on the whole framebuffer, complete from the lower left corner to the upper right
//
//									  // We don't use bias in the shader, but instead we draw back faces, 
//									  // which are already separated from the front faces by a small distance 
//									  // (if your geometry is made this way)
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
//
//							 // Clear the screen
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		// Use our shader
//		glUseProgram(depthProgramID);
//
//		glm::vec3 lightInvDir = glm::vec3(0.5f, 2, 2);
//
//		// Compute the MVP matrix from the light's point of view
//		glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
//		glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
//		// or, for spot light :
//		//glm::vec3 lightPos(5, 20, 20);
//		//glm::mat4 depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
//		//glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos-lightInvDir, glm::vec3(0,1,0));
//
//		glm::mat4 depthModelMatrix = glm::mat4(1.0);
//		glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
//
//		// Send our transformation to the currently bound shader, 
//		// in the "MVP" uniform
//		glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);
//
//		// 1rst attribute buffer : vertices
//		glEnableVertexAttribArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//		glVertexAttribPointer(
//			0,  // The attribute we want to configure
//			3,                  // size
//			GL_FLOAT,           // type
//			GL_FALSE,           // normalized?
//			0,                  // stride
//			(void*)0            // array buffer offset
//		);
//
//		// Index buffer
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
//
//		// Draw the triangles !
//		glDrawElements(
//			GL_TRIANGLES,      // mode
//			indices.size(),    // count
//			GL_UNSIGNED_SHORT, // type
//			(void*)0           // element array buffer offset
//		);
//
//		glDisableVertexAttribArray(0);
//
//
//
//		// Render to the screen
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glViewport(0, 0, ncol, nrow); // Render on the whole framebuffer, complete from the lower left corner to the upper right
//
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
//
//							 // Clear the screen
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		// Use our shader
//		glUseProgram(programID);
//
//		// Compute the MVP matrix from keyboard and mouse input
//		glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 1.0f, 0.1f, 100.0f);
//		glm::mat4 ViewMatrix = glm::lookAt(
//			glm::vec3(25, 25, -14), // Camera is at (4,3,3), in World Space
//			glm::vec3(0, 0, 0), // and looks at the origin
//			glm::vec3(0, -1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
//		);
//		//ViewMatrix = glm::lookAt(glm::vec3(14,6,4), glm::vec3(0,1,0), glm::vec3(0,1,0));
//		glm::mat4 ModelMatrix = glm::mat4(1.0);
//		glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
//
//		glm::mat4 biasMatrix(
//			0.5, 0.0, 0.0, 0.0,
//			0.0, 0.5, 0.0, 0.0,
//			0.0, 0.0, 0.5, 0.0,
//			0.5, 0.5, 0.5, 1.0
//		);
//
//		glm::mat4 depthBiasMVP = biasMatrix*depthMVP;
//
//		// Send our transformation to the currently bound shader, 
//		// in the "MVP" uniform
//		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
//		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
//		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
//		glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);
//
//		glUniform3f(lightInvDirID, lightInvDir.x, lightInvDir.y, lightInvDir.z);
//
//		// Bind our texture in Texture Unit 0
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, Texture);
//		// Set our "myTextureSampler" sampler to user Texture Unit 0
//		glUniform1i(TextureID, 0);
//
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, depthTexture);
//		glUniform1i(ShadowMapID, 1);
//
//		// 1rst attribute buffer : vertices
//		glEnableVertexAttribArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//		glVertexAttribPointer(
//			0,                  // attribute
//			3,                  // size
//			GL_FLOAT,           // type
//			GL_FALSE,           // normalized?
//			0,                  // stride
//			(void*)0            // array buffer offset
//		);
//
//		// 2nd attribute buffer : UVs
//		glEnableVertexAttribArray(1);
//		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
//		glVertexAttribPointer(
//			1,                                // attribute
//			2,                                // size
//			GL_FLOAT,                         // type
//			GL_FALSE,                         // normalized?
//			0,                                // stride
//			(void*)0                          // array buffer offset
//		);
//
//		// 3rd attribute buffer : normals
//		glEnableVertexAttribArray(2);
//		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
//		glVertexAttribPointer(
//			2,                                // attribute
//			3,                                // size
//			GL_FLOAT,                         // type
//			GL_FALSE,                         // normalized?
//			0,                                // stride
//			(void*)0                          // array buffer offset
//		);
//
//		// Index buffer
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
//
//		// Draw the triangles !
//		glDrawElements(
//			GL_TRIANGLES,      // mode
//			indices.size(),    // count
//			GL_UNSIGNED_SHORT, // type
//			(void*)0           // element array buffer offset
//		);
//
//		glDisableVertexAttribArray(0);
//		glDisableVertexAttribArray(1);
//		glDisableVertexAttribArray(2);
//
//
//		// Optionally render the shadowmap (for debug only)
//
//		// Render only on a corner of the window (or we we won't see the real rendering...)
//		//glViewport(0, 0, ncol, nrow);
//
//		// Use our shader
//		glUseProgram(quad_programID);
//
//		// Bind our texture in Texture Unit 0
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, depthTexture);
//		// Set our "renderedTexture" sampler to user Texture Unit 0
//		glUniform1i(texID, 0);
//
//		// 1rst attribute buffer : vertices
//		glEnableVertexAttribArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
//		glVertexAttribPointer(
//			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//			3,                  // size
//			GL_FLOAT,           // type
//			GL_FALSE,           // normalized?
//			0,                  // stride
//			(void*)0            // array buffer offset
//		);
//
//		// Draw the triangle !
//		// You have to disable GL_COMPARE_R_TO_TEXTURE above in order to see anything !
//		//glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
//		glDisableVertexAttribArray(0);
//	}
//
//
//
//
//	if (false) {
//		static const GLfloat g_vertex_buffer_data[] = {
//			-1.0f,-1.0f,-1.0f,
//			-1.0f,-1.0f, 1.0f,
//			-1.0f, 1.0f, 1.0f,
//			1.0f, 1.0f,-1.0f,
//			-1.0f,-1.0f,-1.0f,
//			-1.0f, 1.0f,-1.0f,
//			1.0f,-1.0f, 1.0f,
//			-1.0f,-1.0f,-1.0f,
//			1.0f,-1.0f,-1.0f,
//			1.0f, 1.0f,-1.0f,
//			1.0f,-1.0f,-1.0f,
//			-1.0f,-1.0f,-1.0f,
//			-1.0f,-1.0f,-1.0f,
//			-1.0f, 1.0f, 1.0f,
//			-1.0f, 1.0f,-1.0f,
//			1.0f,-1.0f, 1.0f,
//			-1.0f,-1.0f, 1.0f,
//			-1.0f,-1.0f,-1.0f,
//			-1.0f, 1.0f, 1.0f,
//			-1.0f,-1.0f, 1.0f,
//			1.0f,-1.0f, 1.0f,
//			1.0f, 1.0f, 1.0f,
//			1.0f,-1.0f,-1.0f,
//			1.0f, 1.0f,-1.0f,
//			1.0f,-1.0f,-1.0f,
//			1.0f, 1.0f, 1.0f,
//			1.0f,-1.0f, 1.0f,
//			1.0f, 1.0f, 1.0f,
//			1.0f, 1.0f,-1.0f,
//			-1.0f, 1.0f,-1.0f,
//			1.0f, 1.0f, 1.0f,
//			-1.0f, 1.0f,-1.0f,
//			-1.0f, 1.0f, 1.0f,
//			1.0f, 1.0f, 1.0f,
//			-1.0f, 1.0f, 1.0f,
//			1.0f,-1.0f, 1.0f
//		};
//
//		// One color for each vertex. They were generated randomly.
//		static const GLfloat g_color_buffer_data[] = {
//			0.583f,  0.771f,  0.014f,
//			0.609f,  0.115f,  0.436f,
//			0.327f,  0.483f,  0.844f,
//			0.822f,  0.569f,  0.201f,
//			0.435f,  0.602f,  0.223f,
//			0.310f,  0.747f,  0.185f,
//			0.597f,  0.770f,  0.761f,
//			0.559f,  0.436f,  0.730f,
//			0.359f,  0.583f,  0.152f,
//			0.483f,  0.596f,  0.789f,
//			0.559f,  0.861f,  0.639f,
//			0.195f,  0.548f,  0.859f,
//			0.014f,  0.184f,  0.576f,
//			0.771f,  0.328f,  0.970f,
//			0.406f,  0.615f,  0.116f,
//			0.676f,  0.977f,  0.133f,
//			0.971f,  0.572f,  0.833f,
//			0.140f,  0.616f,  0.489f,
//			0.997f,  0.513f,  0.064f,
//			0.945f,  0.719f,  0.592f,
//			0.543f,  0.021f,  0.978f,
//			0.279f,  0.317f,  0.505f,
//			0.167f,  0.620f,  0.077f,
//			0.347f,  0.857f,  0.137f,
//			0.055f,  0.953f,  0.042f,
//			0.714f,  0.505f,  0.345f,
//			0.783f,  0.290f,  0.734f,
//			0.722f,  0.645f,  0.174f,
//			0.302f,  0.455f,  0.848f,
//			0.225f,  0.587f,  0.040f,
//			0.517f,  0.713f,  0.338f,
//			0.053f,  0.959f,  0.120f,
//			0.393f,  0.621f,  0.362f,
//			0.673f,  0.211f,  0.457f,
//			0.820f,  0.883f,  0.371f,
//			0.982f,  0.099f,  0.879f
//		};
//		GLuint vertexbuffer;
//		glGenBuffers(1, &vertexbuffer);
//		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
//
//		GLuint colorbuffer;
//		glGenBuffers(1, &colorbuffer);
//		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
//
//		// Create and compile our GLSL program from the shaders
//		GLuint programID = LoadShaders("E:/Jeff/work85vs2015/shaders/TransformVertexShader.vertexshader", "E:/Jeff/work85vs2015/shaders/ColorFragmentShader.fragmentshader");
//		///GLuint programID = LoadShaders("E:/Jeff/Passthrough.vertexshader", "E:/Jeff/SimpleTexture.fragmentshader");
//		//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
//
//		// Use our shader
//		glUseProgram(programID);
//
//		///glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
//
//		// 1rst attribute buffer : vertices
//		glEnableVertexAttribArray(0);
//		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//		glVertexAttribPointer(
//			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
//			GLIntSize,                  // size
//			GL_FLOAT,           // type
//			GL_FALSE,           // normalized?
//			0,                  // stride
//			(void*)0            // array buffer offset
//		);
//
//		glEnableVertexAttribArray(1);
//		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
//		glVertexAttribPointer(
//			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
//			3,                                // size
//			GL_FLOAT,                         // type
//			GL_FALSE,                         // normalized?
//			0,                                // stride
//			(void*)0                          // array buffer offset
//		);
//
//
//		glDrawArrays(GL_TRIANGLES, 0, 36); // 3 indices starting at 0 -> 1 triangle
//										   //	glDisableVertexAttribArray(0);
//
//		glDisableVertexAttribArray(0);
//		glDisableVertexAttribArray(1);
//	}
//
//	myFBOtoDC(1500);
////	myFBOtoBitMapFile();
//
//}

//void OpenGLRendererFBO::myTest3FBO(float **ampArr, CColorHelper::AMPCOLORDYN *ampColor, float x[], float y[], int nx, int nt, double dxMin4Plot, double dyMin4Plot)
//{
//
//	//Set up framebuffer
//	glGenFramebuffers(1, &myGL.FramebufferName);
//	glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);
//	int ncol = this->myGL.m_rectPixel.Width();
//	int nrow = this->myGL.m_rectPixel.Height();
//	glViewport(0, 0, ncol, nrow);
//
//	//Create texture buffer
//	GLuint renderedTexture;
//	glGenTextures(1, &renderedTexture);
//	glBindTexture(GL_TEXTURE_2D, renderedTexture); // "Bind" the newly created texture : all future texture functions will modify this texture
//
//												   //Generate background colour and backgrou
//	glClearColor(0.0f, 0.8f, 1.0f, 0.0f); //background to clear with.
//
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//	glEnable(GL_CULL_FACE);
//
//	GLuint depthProgramID = LoadShaders("E:/Jeff/work85vs2015/shaders/DepthRTT.vertexshader", "E:/Jeff/work85vs2015/shaders/DepthRTT.fragmentshader");
//
//	// Get a handle for our "MVP" uniform
//	GLuint depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");
//
//	GLuint Texture = loadDDS("uvmap.DDS");
//
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
//		ncol, nrow,
//		0, GL_RGB, GL_UNSIGNED_BYTE, 0);// Give an empty image to OpenGL ( the last "0" )
//
//										// Poor filtering. Needed !
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	// The depth buffer
//	//glDepthFunc(GL_ALWAYS); // deping add
//	GLuint depthrenderbuffer;
//	glGenRenderbuffers(1, &depthrenderbuffer);
//	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ncol, nrow);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
//
//
//	GLuint depthTexture;
//	glGenTextures(1, &depthTexture);
//	glBindTexture(GL_TEXTURE_2D, depthTexture);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, ncol, nrow, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
//
//
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
//
//	// Set "renderedTexture" as our colour attachement #0
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
//
//	// Set the list of draw buffers.
//	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
//	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
//
//								   // Always check that our framebuffer is ok
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		return;
//	// #############Rendering to the texture
//	// Rendering to the texture is straightforward. Simply bind your framebuffer, and draw your scene as usual. Easy !
//	// Render to our framebuffer
//
//	GLuint quad_programID = LoadShaders("E:/Jeff/work85vs2015/shaders/Passthrough.vertexshader", "E:/Jeff/work85vs2015/shaders/SimpleTexture.fragmentshader");
//	GLuint texID = glGetUniformLocation(quad_programID, "texture");
//
//
//	// Create and compile our GLSL program from the shaders
//	GLuint programID = LoadShaders("E:/Jeff/work85vs2015/shaders/ShadowMapping.vertexshader", "E:/Jeff/work85vs2015/shaders/ShadowMapping.fragmentshader");
//
//	// Get a handle for our "myTextureSampler" uniform
//	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
//
//	// Get a handle for our "MVP" uniform
//	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
//	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
//	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
//	GLuint DepthBiasID = glGetUniformLocation(programID, "DepthBiasMVP");
//	GLuint ShadowMapID = glGetUniformLocation(programID, "shadowMap");
//
//	// Get a handle for our "LightPosition" uniform
//	GLuint lightInvDirID = glGetUniformLocation(programID, "LightInvDirection_worldspace");
//
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//	// Use our shader
//	glUseProgram(depthProgramID);
//
//	glm::vec3 lightInvDir = glm::vec3(0, 2, 7);
//
//	glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
//	glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
//
//	glm::mat4 depthModelMatrix = glm::mat4(1.0);
//	glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
//
//	// Send our transformation to the currently bound shader, 
//	// in the "MVP" uniform
//	glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);
//
//	// Projection matrix : 45�� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
//	//glm::mat4 Projection = glm::perspective(15.0f, 1.0f, 0.1f, 100.0f);
//	// Or, for an ortho camera :
//
//
//
//	//##################################
//	const int GLIntSize = 3; //number of floats per item. i.e. number of floats per point in space.
//
//	int ix, j;
//
//	if (true) {
//		CColorHelper::rgb **colorsArr = (CColorHelper::rgb **)alloc2(nt, nx, sizeof(CColorHelper::rgb));
//		for (ix = 0; ix < nx; ix++) {
//			ampColor->getColorRef4Ampf(ampArr[ix], colorsArr[ix], nt);
//		}
//
//		//	each data point will need 2 triangles to plot
//		GLfloat *line = (GLfloat*)alloc1(nt * 3 * nx, sizeof(GLfloat)); // a triangle has 3 points
//		GLfloat *colour = (GLfloat*)alloc1(nt * 3 * nx, sizeof(GLfloat));
//		unsigned int *elem1 = (unsigned int*)alloc1((nt - 1) * 6 * (nx - 1), sizeof(unsigned int));
//		//unsigned int *elem2 = (unsigned int*)alloc1(nt * 6, sizeof(unsigned int));
//		GLfloat *uvs1 = (GLfloat*)alloc1((nt - 1) * 12 * (nx - 1), sizeof(GLfloat));
//		//GLfloat *uvs2 = (GLfloat*)alloc1(nt * 6, sizeof(GLfloat));
//		GLfloat *normals1 = (GLfloat*)alloc1((nt - 1) * 6 * (nx - 1), sizeof(GLfloat));
//		//GLfloat *normals2 = (GLfloat*)alloc1(nt * 6, sizeof(GLfloat));
//
//		int count = 0;
//		glGenVertexArrays(myGL.numvo, myGL.vaoID); // not used at all
//
//
//		GLuint vao;
//		GLuint vbo[2];
//		glGenVertexArrays(1, &vao);
//		glGenBuffers(2, vbo);
//		GLuint elementbuffer1;
//		GLuint elementbuffer2;
//		glGenBuffers(1, &elementbuffer1);
//		glGenBuffers(1, &elementbuffer2);
//
//		// bool isUpHill = true; // this will need to be calculated later
//		//Set up x and z value for position and red and blue values for colour
//		//Alternating patterns to mesh triangles
//
//		double xPlotted = -99999.;
//		int xx, jj, xyz;
//		double yPlotted = -99999.;
//
//		dxMin4Plot = 1.0 / 2048;
//		dyMin4Plot = 1.0 / 2048;
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer1);
//		for (ix = 0; ix < nx; ix++) {
//			xx = ix*nt * 3;
//			if (fabs(x[ix] - xPlotted) < dxMin4Plot) {
//				continue;
//			}// skip too dense traces
//			xPlotted = x[ix];
//
//			for (j = 0; j < nt; j++) {
//				if (fabs(y[j] - yPlotted) < dyMin4Plot) {
//					continue;
//				}// skip too dense traces
//				yPlotted = y[j];
//
//				jj = j * 3;
//
//				line[xx + jj] = (GLfloat)x[ix];
//				line[xx + jj + 1] = (GLfloat)y[j];
//				line[xx + jj + 2] = (GLfloat)ampArr[ix][j];
//
//				//colour[xx + jj] = (GLfloat)colorsArr[ix][j].r;
//				//colour[xx + jj + 1] = (GLfloat)colorsArr[ix][j].g;
//				//colour[xx + jj + 2] = (GLfloat)colorsArr[ix][j].b;
//
//				colour[xx + jj] = (GLfloat)ampArr[ix][j];
//				colour[xx + jj + 1] = (GLfloat) 0.1;
//				colour[xx + jj + 2] = (GLfloat)-1.0 * ampArr[ix][j];
//			}
//		}
//		//double maxAMP = 0;
//		//for (int i = 0; i < nt * 3 * nx; i++) {
//		//	if (colour[i] > maxAMP)
//		//		maxAMP = colour[i];
//		//}
//
//		//Use shader to load in matrix and set viewing direction
//
//
//
//		//Place data in an array to be drawn to the screen
//		glBindVertexArray(vao);
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
//		GLuint sizeBytes = (3 * nt*nx) * sizeof(GLfloat);
//		glBufferData(GL_ARRAY_BUFFER, sizeBytes, line, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_TRUE, 0, 0);
//		glEnableVertexAttribArray(0);
//
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
//		glBufferData(GL_ARRAY_BUFFER, sizeBytes, colour, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_TRUE, 0, 0);
//		glEnableVertexAttribArray(3);
//
//		//	glDrawArrays(GL_TRIANGLES, 0, 6 * (nt-1));
//		// now prepare for the other triangles using indexes
//
//		GLfloat x1, y1, z1, x2, y2, z2, x3, y3, z3;
//		int offset = 0;
//
//		float o2 = 0.5f;
//
//		for (ix = 0; ix < nx - 1; ix++) {
//			xx = ix*nt;
//			if (fabs(x[ix] - xPlotted) < dxMin4Plot)
//				continue;// skip too dense traces
//			xPlotted = x[ix];
//
//			xyz = ix * (nt - 1) * 6;
//			//xyz = ix * nt * 3;
//			//maxxyz = nx * nt * 3;
//
//			for (j = 0; j < nt - 1; j++) {
//				if (fabs(y[j] - yPlotted) < dyMin4Plot) {
//					continue;
//				}// skip too dense traces
//				yPlotted = y[j];
//
//				/*jj = j * 3;
//
//				// first set of triangles
//				elem1[xyz + jj] = (unsigned int)(xx + j);
//				elem1[xyz + jj + 1] = (unsigned int)(xx + nt + j);
//				elem1[xyz + jj + 2] = (unsigned int)(xx + j + 1);
//
//				// second set of triangles
//				elem1[maxxyz + xyz + jj + 0] = (unsigned int)(xx + j + 1);
//				elem1[maxxyz + xyz + jj + 1] = (unsigned int)(xx + nt + j);
//				elem1[maxxyz + xyz + jj + 2] = (unsigned int)(xx + nt + j + 1);
//
//				x1 = line[elem1[xyz + jj] * 3];
//				y1 = line[elem1[xyz + jj] * 3 + 1];
//				z1 = line[elem1[xyz + jj] * 3 + 2];
//				x2 = line[elem1[xyz + jj + 1] * 3];
//				y2 = line[elem1[xyz + jj + 1] * 3 + 1];
//				z2 = line[elem1[xyz + jj + 1] * 3 + 2];
//				x3 = line[elem1[xyz + jj + 2] * 3];
//				y3 = line[elem1[xyz + jj + 2] * 3 + 1];
//				z3 = line[elem1[xyz + jj + 2] * 3 + 2];
//
//				CalculateVectorNormal(x1, y1, z1, x2, y2, z3, x3, y3, z3, &normals1[xyz + jj], &normals1[xyz + jj + 1], &normals1[xyz + jj + 2]);
//				uvs1[2 * xyz + jj * 2] = (x1 + 1.0) * o2;
//				uvs1[2 * xyz + jj * 2 + 1] = (y1 + 1.0) * o2;
//				uvs1[2 * xyz + jj * 2 + 2] = (x2 + 1.0) * o2;
//				uvs1[2 * xyz + jj * 2 + 3] = (y2 + 1.0) * o2;
//				uvs1[2 * xyz + jj * 2 + 4] = (x3 + 1.0) * o2;
//				uvs1[2 * xyz + jj * 2 + 5] = (y3 + 1.0) * o2;
//
//				x1 = line[elem1[maxxyz + xyz + jj + 0] * 3];
//				y1 = line[elem1[maxxyz + xyz + jj + 0] * 3 + 1];
//				z1 = line[elem1[maxxyz + xyz + jj + 0] * 3 + 2];
//				x2 = line[elem1[maxxyz + xyz + jj + 1] * 3];
//				y2 = line[elem1[maxxyz + xyz + jj + 1] * 3 + 1];
//				z2 = line[elem1[maxxyz + xyz + jj + 1] * 3 + 2];
//				x3 = line[elem1[maxxyz + xyz + jj + 2] * 3];
//				y3 = line[elem1[maxxyz + xyz + jj + 2] * 3 + 1];
//				z3 = line[elem1[maxxyz + xyz + jj + 2] * 3 + 2];
//
//				CalculateVectorNormal(x1, y1, z1, x2, y2, z3, x3, y3, z3, &normals1[maxxyz + xyz + jj + 0], &normals1[maxxyz + xyz + jj + 1], &normals1[maxxyz + xyz + jj + 2]);
//				uvs1[2 * maxxyz + 2 * xyz + jj * 2 + 0] = (x1 + 1.0) * o2;
//				uvs1[2 * maxxyz + 2 * xyz + jj * 2 + 1] = (y1 + 1.0) * o2;
//				uvs1[2 * maxxyz + 2 * xyz + jj * 2 + 2] = (x2 + 1.0) * o2;
//				uvs1[2 * maxxyz + 2 * xyz + jj * 2 + 3] = (y2 + 1.0) * o2;
//				uvs1[2 * maxxyz + 2 * xyz + jj * 2 + 4] = (x3 + 1.0) * o2;
//				uvs1[2 * maxxyz + 2 * xyz + jj * 2 + 5] = (y3 + 1.0) * o2;*/
//
//				///Set for Left-Right alternating triangle drawings
//				jj = j * 6;
//
//				offset = xyz + jj;
//
//				// first set of triangles
//
//				if (line[xx + nt + j] + line[xx + nt + j + 1] > (line[xx + j] + line[xx + j + 1])) {
//					elem1[offset] = (unsigned int)(xx + j);
//					elem1[offset + 1] = (unsigned int)(xx + nt + j + 1);
//					elem1[offset + 2] = (unsigned int)(xx + nt + j);
//
//					elem1[offset + 3] = (unsigned int)(xx + j);
//					elem1[offset + 4] = (unsigned int)(xx + j + 1);
//					elem1[offset + 5] = (unsigned int)(xx + nt + j + 1);
//				}
//				//DOWNHILL
//				else {
//					elem1[offset] = (unsigned int)(xx + j);
//					elem1[offset + 1] = (unsigned int)(xx + j + 1);
//					elem1[offset + 2] = (unsigned int)(xx + nt + j);
//
//					// second set of triangles
//					elem1[offset + 3] = (unsigned int)(xx + j + 1);
//					elem1[offset + 4] = (unsigned int)(xx + nt + j + 1);
//					elem1[offset + 5] = (unsigned int)(xx + nt + j);
//				}
//
//				x1 = line[elem1[offset] * 3];
//				y1 = line[elem1[offset] * 3 + 1];
//				z1 = line[elem1[offset] * 3 + 2];
//				x2 = line[elem1[offset + 1] * 3];
//				y2 = line[elem1[offset + 1] * 3 + 1];
//				z2 = line[elem1[offset + 1] * 3 + 2];
//				x3 = line[elem1[offset + 2] * 3];
//				y3 = line[elem1[offset + 2] * 3 + 1];
//				z3 = line[elem1[offset + 2] * 3 + 2];
//
//				float o2 = 0.5f;
//				int uvIndex = 2 * xyz + jj * 2;
//				//CalculateVectorNormal(x1, y1, z1, x2, y2, z3, x3, y3, z3, &normals1[xyz + jj], &normals1[xyz + jj + 1], &normals1[xyz + jj + 2]);
//				uvs1[uvIndex] = (x1 + 1.0f) * o2;
//				uvs1[uvIndex + 1] = (y1 + 1.0f) * o2;
//				uvs1[uvIndex + 2] = (x2 + 1.0f) * o2;
//				uvs1[uvIndex + 3] = (y2 + 1.0f) * o2;
//				uvs1[uvIndex + 4] = (x3 + 1.0f) * o2;
//				uvs1[uvIndex + 5] = (y3 + 1.0f) * o2;
//
//				x1 = line[elem1[offset + 3] * 3];
//				y1 = line[elem1[offset + 3] * 3 + 1];
//				z1 = line[elem1[offset + 3] * 3 + 2];
//				x2 = line[elem1[offset + 4] * 3];
//				y2 = line[elem1[offset + 4] * 3 + 1];
//				z2 = line[elem1[offset + 4] * 3 + 2];
//				x3 = line[elem1[offset + 5] * 3];
//				y3 = line[elem1[offset + 5] * 3 + 1];
//				z3 = line[elem1[offset + 5] * 3 + 2];
//
//				//CalculateVectorNormal(x1, y1, z1, x2, y2, z3, x3, y3, z3, &normals1[xyz + jj + 3], &normals1[xyz + jj + 4], &normals1[xyz + jj + 5]);
//				uvs1[uvIndex + 6] = (x1 + 1.0f) * o2;
//				uvs1[uvIndex + 7] = (y1 + 1.0f) * o2;
//				uvs1[uvIndex + 8] = (x2 + 1.0f) * o2;
//				uvs1[uvIndex + 9] = (y2 + 1.0f) * o2;
//				uvs1[uvIndex + 10] = (x3 + 1.0f) * o2;
//				uvs1[uvIndex + 11] = (y3 + 1.0f) * o2;
//
//			}
//
/*GLuint uvbuffer2;
glGenBuffers(1, &uvbuffer2);
glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
glBufferData(GL_ARRAY_BUFFER, sizeof(uvs2), &uvs2[0], GL_STATIC_DRAW);

GLuint normalbuffer2;
glGenBuffers(1, &normalbuffer2);
glBindBuffer(GL_ARRAY_BUFFER, normalbuffer2);
glBufferData(GL_ARRAY_BUFFER, sizeof(normals2), &normals2[0], GL_STATIC_DRAW);

glEnableVertexAttribArray(1);
glBindBuffer(GL_ARRAY_BUFFER, uvbuffer2);
glVertexAttribPointer(
1,                                // attribute
2,                                // size
GL_FLOAT,                         // type
GL_FALSE,                         // normalized?
0,                                // stride
(void*)0                          // array buffer offset
);

// 3rd attribute buffer : normals
glEnableVertexAttribArray(2);
glBindBuffer(GL_ARRAY_BUFFER, normalbuffer2);
glVertexAttribPointer(
2,                                // attribute
3,                                // size
GL_FLOAT,                         // type
GL_FALSE,                         // normalized?
0,                                // stride
(void*)0                          // array buffer offset
);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer2);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElem * sizeof(unsigned int), &elem2[0], GL_STATIC_DRAW);
glDrawElements(GL_TRIANGLES, numElem, GL_UNSIGNED_INT, (void*)0);*/

//		}
//		GLuint uvbuffer1;
//		glGenBuffers(1, &uvbuffer1);
//		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer1);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(uvs1), &uvs1[0], GL_STATIC_DRAW);
//
//		GLuint normalbuffer1;
//		glGenBuffers(1, &normalbuffer1);
//		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer1);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(normals1), &normals1[0], GL_STATIC_DRAW);
//
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer1);
//		register int numElem = (nx - 1) * 6 * (nt - 1);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numElem * sizeof(unsigned int), &elem1[0], GL_STATIC_DRAW);
//		glDrawElements(GL_TRIANGLES, numElem, GL_UNSIGNED_INT, (void*)0);
//
//
//		glDisableVertexAttribArray(0);
//		glDisableVertexAttribArray(3);
//
//
//		glBindFramebuffer(GL_FRAMEBUFFER, 0);
//		glViewport(0, 0, nrow, ncol);
//
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
//
//							 // Clear the screen
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		// Use our shader
//		glUseProgram(programID);
//
//		glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f); // In world coordinates
//
//																				   // Camera matrix
//		glm::mat4 View = glm::lookAt(
//			glm::vec3(0, 2, 14), // Camera is at (4,3,3), in World Space
//								 //lightInvDir,
//			glm::vec3(0, 0, 0), // and looks at the origin
//			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
//		);
//		// Model matrix : an identity matrix (model will be at the origin)
//		glm::mat4 Model = glm::mat4(1.0f);
//		// Our ModelViewProjection : multiplication of our 3 matrices
//		glm::mat4 MVP = Projection * View * Model;
//
//		glm::mat4 biasMatrix(
//			0.5, 0.0, 0.0, 0.0,
//			0.0, 0.5, 0.0, 0.0,
//			0.0, 0.0, 0.5, 0.0,
//			0.5, 0.5, 0.5, 1.0
//		);
//
//		glm::mat4 depthBiasMVP = biasMatrix*depthMVP;
//
//		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
//		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
//		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
//		glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);
//
//		glUniform3f(lightInvDirID, lightInvDir.x, lightInvDir.y, lightInvDir.z);
//
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, depthTexture);
//		glUniform1i(ShadowMapID, 1);
//
//		glEnableVertexAttribArray(1);
//		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer1);
//		glVertexAttribPointer(
//			1,                                // attribute
//			2,                                // size
//			GL_FLOAT,                         // type
//			GL_FALSE,                         // normalized?
//			0,                                // stride
//			(void*)0                          // array buffer offset
//		);
//
//		// 3rd attribute buffer : normals
//		glEnableVertexAttribArray(2);
//		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer1);
//		glVertexAttribPointer(
//			2,                                // attribute
//			3,                                // size
//			GL_FLOAT,                         // type
//			GL_FALSE,                         // normalized?
//			0,                                // stride
//			(void*)0                          // array buffer offset
//		);
//
//		glBindVertexArray(vao);
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
//		sizeBytes = (3 * nt*nx) * sizeof(GLfloat);
//		glBufferData(GL_ARRAY_BUFFER, sizeBytes, line, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_TRUE, 0, 0);
//		glEnableVertexAttribArray(0);
//
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
//		glBufferData(GL_ARRAY_BUFFER, sizeBytes, colour, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_TRUE, 0, 0);
//		glEnableVertexAttribArray(3);
//
//		glDisableVertexAttribArray(0);
//		glDisableVertexAttribArray(1);
//		glDisableVertexAttribArray(2);
//		glDisableVertexAttribArray(3);
//
//		free1(line);
//		free1(colour);
//		free1(elem1);
//		free1(normals1);
//		free1(uvs1);
//		free2((void **)colorsArr);
//	}
//
//	myFBOtoDC(1500);
////	myFBOtoBitMapFile();
//}

//void OpenGLRendererFBO::mySeis3DFBO(float **ampArr, CColorHelper::AMPCOLORDYN *ampColor, float x[], float y[], int nx, int nt)
//{
//
//	//Set up framebuffer
//	glGenFramebuffers(1, &myGL.FramebufferName);
//	glBindFramebuffer(GL_FRAMEBUFFER, myGL.FramebufferName);
//	int ncol = this->myGL.m_rectPixel.Width();
//	int nrow = this->myGL.m_rectPixel.Height();
//	glViewport(0, 0, ncol, nrow);
//
//	//Create texture buffer
//	GLuint renderedTexture;
//	glGenTextures(1, &renderedTexture);
//	glBindTexture(GL_TEXTURE_2D, renderedTexture); // "Bind" the newly created texture : all future texture functions will modify this texture
//
//	//Generate background colour and backgrou
//	glClearColor(0.9f, 0.9f, 1.f, 0.f); //background to clear with.
//
//	glEnable(GL_DEPTH_TEST);
//	glDepthFunc(GL_LESS);
//	//glDepthFunc(GL_CULL_FACE);
//
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
//		ncol, nrow,
//		0, GL_RGB, GL_UNSIGNED_BYTE, 0);// Give an empty image to OpenGL ( the last "0" )
//
//										// Poor filtering. Needed !
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//	// The depth buffer
//	//glDepthFunc(GL_ALWAYS); // deping add
//	GLuint depthrenderbuffer;
//	glGenRenderbuffers(1, &depthrenderbuffer);
//	glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, ncol, nrow);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);
//
//
//	GLuint depthTexture;
//	glGenTextures(1, &depthTexture);
//	glBindTexture(GL_TEXTURE_2D, depthTexture);
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, ncol, nrow, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
//
//
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
//
//	// Set "renderedTexture" as our colour attachement #0
//	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);
//
//	// Set the list of draw buffers.
//	//GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
//	//glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
//
//   // Always check that our framebuffer is ok
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//		return;
//
//
//	//##################################
//	// calculate the maximum trace spacing: if there is a big data gap, we will show the gap
//	float dxMaxPlot = getAvgInterval(x, nx) * 5.f;
//
//
//	//glDrawBuffer(GL_BACK_LEFT);
//	//glClearColor(1.0, 0.0, 0.0, 1.0); /* red */
//	//glClear(GL_COLOR_BUFFER_BIT);
//	//glDrawBuffer(GL_BACK_RIGHT);
//	//glClearColor(0.0, 0.0, 1.0, 1.0); /* blue */
//	//glClear(GL_COLOR_BUFFER_BIT);
//
//	int ix, j;
//
//	if (true) {
//		//	each data point will need 2 triangles to plot
//		int xx, jj, xyz, x4;
//		float o2 = 0.5f;
//
//		GLfloat *colour = (GLfloat*)alloc1(nt * 3 * nx, sizeof(GLfloat));
//		CColorHelper::rgb *colors1D = (CColorHelper::rgb *)alloc1(nt, sizeof(CColorHelper::rgb));
//		for (ix = 0; ix < nx; ix++) {
//			ampColor->getColorRef4Ampf(ampArr[ix], colors1D, nt);
//			xx = ix*nt * 3;
//			for (j = 0; j < nt; j++) {
//				jj = j * 3;
//				colour[xx + jj] = (GLfloat)colors1D[j].r;
//				colour[xx + jj + 1] = (GLfloat)colors1D[j].g;
//				colour[xx + jj + 2] = (GLfloat)colors1D[j].b;
//			}
//		} // for ix
//		free1((void *)colors1D); // colors1D* completed its mission
//
//		float ampMaxNeg = ampColor->percentAmp[0];
//		float ampMaxPos = ampColor->percentAmp[ampColor->num - 1];
//		float oMax = 1.f / (fabs(ampMaxNeg) < fabs(ampMaxPos) ? fabs(ampMaxPos) : fabs(ampMaxNeg));
//		for (int i = 0; i < nx; i++)
//			for (int j = 0; j < nt; j++) {
//				ampArr[i][j] = ampArr[i][j] * oMax;
//				if (ampArr[i][j] < -2.f) ampArr[i][j] = -2.f;
//				else if (ampArr[i][j] > 2.f) ampArr[i][j] = 2.f;
//			}
//
//		// compute down hill array
//		bool *isDownHillArr = (bool*)alloc1(nt * nx, sizeof(bool));
//		for (ix = 1; ix < nx - 1; ix++) {
//			xx = ix*nt;
//			for (j = 1; j < nt - 1; j++) {
//				isDownHillArr[xx + j] = isDownDip(ampArr[ix], ampArr[ix + 1], nt, j);
//			}
//		} // for ix
//
//
//		// compute vertex and color
//		GLfloat *line = (GLfloat*)alloc1(nt * 3 * nx, sizeof(GLfloat)); // a triangle has 3 points
//		for (ix = 0; ix < nx; ix++) {
//			xx = ix*nt * 3;
//			for (j = 0; j < nt; j++) {
//				jj = j * 3;
//				line[xx + jj] = (GLfloat)x[ix];
//				line[xx + jj + 1] = (GLfloat)y[j];
//				line[xx + jj + 2] = (GLfloat)ampArr[ix][j];
//
//				// test
//				//if (isDownHillArr[ix*nt + j]) {
//				//	colour[xx + jj] = (GLfloat)1.;
//				//	colour[xx + jj + 1] = (GLfloat)0.;
//				//	colour[xx + jj + 2] = (GLfloat)0.;
//				//}
//				//else {
//				//	colour[xx + jj] = (GLfloat)0.;
//				//	colour[xx + jj + 1] = (GLfloat)0.;
//				//	colour[xx + jj + 2] = (GLfloat)1.;
//				//}
//			}
//		} // for ix
//
//
//		// compute uv array
//		GLfloat *uvs1 = (GLfloat*)alloc1(nt * 2 * nx, sizeof(GLfloat));
//		uvs1[0] = 1.0;
//		for (ix = 0; ix < nx; ix++) {
//			x4 = ix*nt * 2;
//			for (j = 0; j < nt; j++) {
//				jj = j * 2;
//				uvs1[x4 + jj] = (x[ix] + 1.0f) * o2;
//				uvs1[x4 + jj + 1] = (y[j] + 1.0f) * o2;
//			}
//		} // for ix
//
//
//		// compute normals
//		GLfloat *normals1 = (GLfloat*)alloc1(nt * 3 * nx, sizeof(GLfloat));
//		for (ix = 1; ix < nx - 1; ix++) {
//			xx = ix*nt * 3;
//			for (j = 1; j < nt - 1; j++) {
//				jj = j * 3;
//				CalculateVectorNormal9(
//					ampArr[ix-1][j-1], ampArr[ix-1][j], ampArr[ix-1][j+1],
//					ampArr[ix][j - 1], ampArr[ix][j], ampArr[ix][j + 1],
//					ampArr[ix + 1][j - 1], ampArr[ix + 1][j], ampArr[ix + 1][j + 1],
//					x[ix] - x[ix-1], x[ix+1]-x[ix], y[j]-y[j-1], true,
//					&normals1[xx + jj], &normals1[xx + jj + 1], &normals1[xx + jj + 2]
//				);
//			}
//		} // for ix
//
//		// now set the border values to the same as middle ones
//		// first trace
//		for (j = 1; j < nt - 1; j++) {
//			jj = j * 3;
//			normals1[jj] = normals1[nt*3+jj];
//			normals1[jj+1] = normals1[nt * 3 + jj + 1];
//			normals1[jj+1] = normals1[nt * 3 + jj] + 1;
//			isDownHillArr[j] = isDownHillArr[nt + j];
//		}
//
//		// last trace
//		xx = (nx - 1)*nt * 3;
//		for (j = 1; j < nt - 1; j++) {
//			jj = j * 3;
//			normals1[xx + jj] = normals1[xx - nt * 3 + jj];
//			normals1[xx + jj+1] = normals1[xx - nt * 3 + jj+1];
//			normals1[xx + jj+2] = normals1[xx - nt * 3 + jj+2];
//			isDownHillArr[(nx-1)*nt + j] = isDownHillArr[(nx-2)*nt + j];
//		}
//
//		// first sample for all traces
//		for (ix = 0; ix < nx; ix++) {
//			xx = ix*nt * 3;
//			normals1[xx] = normals1[xx+3];
//			normals1[xx + 1] = normals1[xx + 4];
//			normals1[xx + 2] = normals1[xx + 5];
//			isDownHillArr[ix*nt] = isDownHillArr[ix*nt + 1];
//		}
//
//		// last sample for all traces
//		for (ix = 0; ix < nx; ix++) {
//			xx = ix*nt * 3 + (nt-1)*3;
//			normals1[xx] = normals1[xx - 3];
//			normals1[xx + 1] = normals1[xx - 2];
//			normals1[xx + 2] = normals1[xx - 1];
//			isDownHillArr[ix*nt + nt-1] = isDownHillArr[ix*nt + nt-2];
//		}
//		// #####now all samples of nx*nt are covered#######
//
//		//Use shader to load in matrix and set viewing direction
//
//		//	glDrawArrays(GL_TRIANGLES, 0, 6 * (nt-1));
//		// now prepare for the other triangles using indexes
//
////		GLfloat x1, y1, z1, x2, y2, z2, x3, y3, z3;
//		int offset = 0;
//
//		myGL.numElem = (nx - 1) * 6 * (nt - 1);
//		unsigned int *elem1 = (unsigned int*)alloc1(myGL.numElem, sizeof(unsigned int));
//		int countTrc = 0;
//		for (ix = 0; ix < nx - 1; ix++) {
//			xx = ix*nt;
//			if (fabs(x[ix] - x[ix + 1]) > dxMaxPlot) continue; // we do not plot data gap
//			xyz = countTrc * (nt - 1) * 6;
//			for (j = 0; j < nt - 1; j++) {
//				///Set for Left-Right alternating triangle drawings
//				//jj = j * 6;
//				offset = xyz + j * 6;
//
//				register bool isDownHill = isDownHillArr[xx + j];
//				if (isDownHill) {
//					//DOWNHILL
//					// first set of triangles
//					elem1[offset] = (unsigned int)(xx + j);
//					elem1[offset + 1] = (unsigned int)(xx + nt + j + 1);
//					elem1[offset + 2] = (unsigned int)(xx + nt + j);
//
//					// second set of triangles
//					elem1[offset + 3] = (unsigned int)(xx + j);
//					elem1[offset + 4] = (unsigned int)(xx + j + 1);
//					elem1[offset + 5] = (unsigned int)(xx + nt + j + 1);
//				}
//				else {
//					//UPHILL
//					// first set of triangles
//					elem1[offset] = (unsigned int)(xx + j);
//					elem1[offset + 1] = (unsigned int)(xx + j + 1);
//					elem1[offset + 2] = (unsigned int)(xx + nt + j);
//
//					// second set of triangles
//					elem1[offset + 3] = (unsigned int)(xx + j + 1);
//					elem1[offset + 4] = (unsigned int)(xx + nt + j + 1);
//					elem1[offset + 5] = (unsigned int)(xx + nt + j);
//				}
//			}
//			countTrc++;
//		} // for ix
//
//	//	glGenVertexArrays(myGL.numvo, myGL.vaoID); // not used at all
//
//
//		GLuint vao;
//		GLuint vbo[4];
//		glGenVertexArrays(1, &vao);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		glGenBuffers(4, vbo);
//		GLuint elementbuffer1;
//		glGenBuffers(1, &elementbuffer1);
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer1);
//
//		//GLint color_attribute = glGetAttribLocation(programID, "color");
//		// Get a handle for our "MVP" uniform
//
//
//		//Place data in an array to be drawn to the screen
//		glBindVertexArray(vao);
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
//		GLuint sizeBytes = (3 * nt*nx) * sizeof(GLfloat);
//		glBufferData(GL_ARRAY_BUFFER, sizeBytes, line, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_TRUE, 0, 0);
//
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
//		glBufferData(GL_ARRAY_BUFFER, sizeBytes, colour, GL_STATIC_DRAW);
//		glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_TRUE, 0, 0);
//
//		glEnable(GL_BLEND);
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
//		sizeBytes = (2 * nt*nx) * sizeof(GLfloat);
//		glBufferData(GL_ARRAY_BUFFER, sizeBytes, &uvs1[0], GL_STATIC_DRAW);
//		glVertexAttribPointer(
//			1,                                // attribute
//			2,                                // size
//			GL_FLOAT,                         // type
//			GL_TRUE,                         // normalized?
//			0,                                // stride
//			(void*)0                          // array buffer offset
//		);
//
//		glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
//		sizeBytes = (3 * nt*nx) * sizeof(GLfloat);
//		glBufferData(GL_ARRAY_BUFFER, sizeBytes, &normals1[0], GL_STATIC_DRAW);
//
//		// 3rd attribute buffer : normals
//		glVertexAttribPointer(
//			2,                                // attribute
//			3,                                // size
//			GL_FLOAT,                         // type
//			GL_TRUE,                         // normalized?
//			0,                                // stride
//			(void*)0                          // array buffer offset
//		);
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer1);
//		myGL.numElem = countTrc * 6 * (nt - 1);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, myGL.numElem * sizeof(unsigned int), &elem1[0], GL_STATIC_DRAW);
//
//		free1(line);
//		free1(colour);
//		free1(elem1);
//		free1(normals1);
//		free1(isDownHillArr);
//		free1(uvs1);
//
//
//
//		// #############Rendering to the texture
//		// Rendering to the texture is straightforward. Simply bind your framebuffer, and draw your scene as usual. Easy !
//		// Render to our framebuffer
//		CString s1 = myGL.shaderFolderSlash + "ShadowMapping.vertexshader";
//		CString s2 = myGL.shaderFolderSlash + "ShadowMapping.fragmentshader";
//		char * ascS1 = getCharFromCString(s1, 100);
//		char * ascS2 = getCharFromCString(s2, 100);
//		myGL.programID = LoadShaders(ascS1, ascS2);
//		free1(ascS1);
//		free1(ascS2);
//		glUseProgram(myGL.programID);
//
//
//		glEnableVertexAttribArray(0);
//		glEnableVertexAttribArray(1);
//		glEnableVertexAttribArray(2);
//		glEnableVertexAttribArray(3);
//
//
//		for (int iv = 10; iv < 30; iv += 2) {
//			mySeis3DFBO_Animate(myGL.parentDC2, iv);
//			break;
//			Sleep(500);
//		}
//		mySeis3DFBO_End();
//
//		//// Camera matrix
//		//// Projection matrix : 45�� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
//		////glm::mat4 Projection = glm::perspective(15.0f, 1.0f, 0.1f, 100.0f);
//		//// Or, for an ortho camera :
//		//for (int iv = 10; iv < 30; iv+=2) {
//		//	glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f); // In world coordinates
//		//	CString fOut;
//		//	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
//		//	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
//		//	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
//		//	GLuint DepthBiasID = glGetUniformLocation(programID, "DepthBiasMVP");
//		//	GLuint ShadowMapID = glGetUniformLocation(programID, "shadowMap");
//
//		//	// Get a handle for our "LightPosition" uniform
//		//	GLuint lightInvDirID = glGetUniformLocation(programID, "LightInvDirection_worldspace");
//		//	glm::mat4 View = glm::lookAt(
//		//		glm::vec3(0.1, 0.2, iv), // Camera is at (4,3,3), in World Space //lightInvDir,
//		//		glm::vec3(0, 0, 0), // and looks at the origin
//		//		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
//		//	);
//
//		//	// Model matrix : an identity matrix (model will be at the origin)
//		//	glm::mat4 Model = glm::mat4(1.0f);
//
//		//	// Our ModelViewProjection : multiplication of our 3 matrices
//		//	glm::mat4 MVP = Projection * View * Model;
//
//		//	glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
//		//	glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &Model[0][0]);
//		//	glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &View[0][0]);
//
//		//	glm::vec3 lightInvDir = glm::vec3(8, 3, 5);
//		//	glUniform3f(lightInvDirID, lightInvDir.x, lightInvDir.y, lightInvDir.z);
//
//		//	glDrawElements(GL_TRIANGLES, numElem, GL_UNSIGNED_INT, (void*)0);
//		//	fOut.Format(_T("e:/Jeff/abc%d.bmp"), iv);
//		//	myFBOtoBitMapFile(fOut);
//		//	myFBOtoDC(1500);
//		//	break;
//		//	Sleep(500);
//		//}
//		//glDisableVertexAttribArray(0);
//		//glDisableVertexAttribArray(1);
//		//glDisableVertexAttribArray(2);
//		//glDisableVertexAttribArray(3);
//	}
//
//}


//void OpenGLRendererFBO::PrepareScene2()
//{
//	wglMakeCurrent(myGL.m_hMemDC, myGL.m_hMemRC);
//	glClearColor(0.0, 0.0, 1.0, 0.0); //background to clear with.
//
//	//do other preparations here
//
//	wglMakeCurrent(NULL, NULL);
//}
//
//void OpenGLRendererFBO::Reshape(UINT nType, int w, int h)
//{
//	OnSize(nType, w, h);
//}

//
//void OpenGLRendererFBO::OnSize(UINT nType, int cx, int cy)
//{
//	//if(myGL.glDC != NULL)
//	//{
//	//	wglMakeCurrent(myGL.glDC->m_hDC, myGL.m_hMemRC);
//	//	//---------------------------------
//	//	glViewport(0, 0, (GLsizei)cx, (GLsizei)cy); 
//	//	//---------------------------------
//	//	myGL.m_rectPixel.right = cx;
//	//	myGL.m_rectPixel.bottom = cy;
//	//}
//
//	switch (nType)
//	{
//			case SIZE_RESTORED:
//			case SIZE_MAXIMIZED:
//				{
//				//	MoveWindow(m_WindowOld.left,m_WindowOld.top, m_Window.Width(), m_Window.Height(),TRUE);
//					MoveWindow(myGL.m_rectPixel, TRUE);
//					break;
//				}
//			default:
//				break;
//	}
////	wglMakeCurrent(NULL, NULL);
//}


//
//void OpenGLRendererFBO::myDrawSceneBasic1()
//{
//	// from prepare
//	wglMakeCurrent(myGL.pDC->m_hDC, myGL.m_hMemRC);
//	glClearColor(0.0, 0.8, 1.0, 0.0); //background to clear with.
////	wglMakeCurrent(NULL, NULL);
//	// end prepare
//
//	// copied from triangle
//	//wglMakeCurrent(m_hdc, m_hrc);
//	int m_GLIntSize = 4; //number of floats per item. i.e. number of floats per point in space.
//	int m_GLSizeCount = 3; //number of items. ie sizeof(TriangleA)/m_GLIntSize
//
//	GLfloat TriangleA[] =
//	{
//		-0.3f, 0.5f, -1.0f, 1.0f,
//		-0.8f, -0.5f, -1.0f, 1.0f,
//		0.2f, -0.5f, -1.0f, 1.0f
//	};
//
//	GLfloat TriangleB[] =
//	{
//		-0.2f,  0.5f, -1.0f, 1.0f,
//		0.3f,  -0.5f, -1.0f, 1.0f,
//		0.8f, 0.5f, -1.0f, 1.0f,
//	};
//
//	//VAOs allocation
//	glGenVertexArrays(2, &myGL.vaoID[0]);
//
//	// First VAO setup
//	glBindVertexArray(myGL.vaoID[0]);
//	glGenBuffers(1, myGL.vboID); //VBO allocation
//	glBindBuffer(GL_ARRAY_BUFFER, myGL.vboID[0]);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleA), TriangleA, GL_STATIC_DRAW);
//	glVertexAttribPointer((GLuint)0, m_GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);
//	glEnableVertexAttribArray(0);
//
//	// Second VAO setup     
//	glBindVertexArray(myGL.vaoID[1]);
//	glGenBuffers(1, &myGL.vboID[1]); //VBO allocation
//	glBindBuffer(GL_ARRAY_BUFFER, myGL.vboID[1]);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(TriangleB), TriangleB, GL_STATIC_DRAW);
//	glVertexAttribPointer((GLuint)0, m_GLIntSize, GL_FLOAT, GL_FALSE, 0, 0);
//	
//	glEnableVertexAttribArray(0);
//
//	glClear(GL_COLOR_BUFFER_BIT);
//	for(int i=0;i<2;i++) {
//		glBindVertexArray(myGL.vaoID[i]);
//		glDrawArrays(GL_TRIANGLE_STRIP, 0, m_GLSizeCount);
//	}
//	//--------------------------------
////	glFlush();
//
//
//	SwapBuffers(myGL.pDC->m_hDC);
//	wglMakeCurrent(NULL, NULL);
//
//	// Deping testing
//	//myDrawLine();
//}

//
//const GLchar* sceneVertexSource2 = R"glsl(
//    #version 150 core
//    in vec3 position;
//    in vec3 color;
//    in vec2 texcoord;
//    out vec3 Color;
//    out vec2 Texcoord;
//    uniform mat4 model;
//    uniform mat4 view;
//    uniform mat4 proj;
//    uniform vec3 overrideColor;
//    void main()
//    {
//        Color = overrideColor * color;
//        Texcoord = texcoord;
//        gl_Position = proj * view * model * vec4(position, 1.0);
//    }
//)glsl";
//const GLchar* sceneFragmentSource2 = R"glsl(
//    #version 150 core
//    in vec3 Color;
//    in vec2 Texcoord;
//    out vec4 outColor;
//    uniform sampler2D texKitten;
//    uniform sampler2D texPuppy;
//    void main()
//    {
//        outColor = vec4(Color, 1.0) * mix(texture(texKitten, Texcoord), texture(texPuppy, Texcoord), 0.5);
//    }
//)glsl";
//
//const GLchar* screenVertexSource2 = R"glsl(
//    #version 150 core
//    in vec2 position;
//    in vec2 texcoord;
//    out vec2 Texcoord;
//    void main()
//    {
//        Texcoord = texcoord;
//        gl_Position = vec4(position, 0.0, 1.0);
//    }
//)glsl";
//const GLchar* screenFragmentSource2 = R"glsl(
//    #version 150 core
//    in vec2 Texcoord;
//    out vec4 outColor;
//    uniform sampler2D texFramebuffer;
//    void main()
//    {
//        outColor = texture(texFramebuffer, Texcoord);
//    }
//)glsl";
