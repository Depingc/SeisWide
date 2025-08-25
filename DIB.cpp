// DIB.cpp

#include "stdafx.h"
#include "DIB.h"
#include <math.h>
#include "ximage.h"
#include "util.h"
#include "ealloc_d.h"

CDib::CDib()
{

	// Set the Dib pointer to
	// NULL so we know if it's
	// been loaded.
	//  delete [] m_pDib;   cannot delete if it has not started yet!
	m_pDib = NULL;
	m_rowSizeBytes = -1;
	m_isTransformed = false;

}

CDib::~CDib()
{

	// If a Dib has been loaded,
	// delete the memory.
	if( m_pDib != NULL ) {
	//	free(m_pDib);
		//delete []m_pDib;  // this line enables large memory usage!
		free1(m_pDib);
	}

	m_pDib = NULL;
}

float CDib::Create32Ex(int &ncol, int &nrow)  // deping's addition
{
	if( m_pDib != NULL ) {
		free1(m_pDib);
	}
	int memoryMaxMB = (int)getMemoryAvail();

	// return 0: not created
	// return 1.0:  created normally
	// return >1.0: created while reduced by this amount due to memory limitation
	/*
	BITMAPINFO i;
	ZeroMemory( &i.bmiHeader, sizeof(BITMAPINFOHEADER) );
	i.bmiHeader.biWidth=ncol;     // Set size you need
	i.bmiHeader.biHeight=nrow;    // Set size you need
	i.bmiHeader.biPlanes=1;
	i.bmiHeader.biBitCount=biForm; // Can be 8, 16, 32 bpp or 
	// other number
	//i.bmiHeader.biSizeImage=0;
	i.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
	//i.bmiHeader.biClrUsed= 0;
	//i.bmiHeader.biClrImportant= 0;
	VOID *pvBits = &m_pDibBits[0];


	HDC dc= CreateCompatibleDC(NULL);
	HBITMAP hbmp= CreateDIBSection( dc,
	&i,
	DIB_RGB_COLORS,
	&pvBits,  // this is now a double pointer because of &
	NULL,
	0 );
	// now use it
	HBITMAP oldBmp= (HBITMAP) SelectObject(dc, hbmp);
	RECT rect= {10,10,ncol,nrow};
	FillRect(dc, &rect, (HBRUSH) (COLOR_WINDOW));
	HRESULT hRezz= SaveBitmapToFile("d:\\test.bmp",hbmp, dc);
	SelectObject(dc,oldBmp);
	DeleteObject(hbmp);
	DeleteDC(dc); 
	*/


	// ULONGLONG dwDibSize = sizeof( BITMAPFILEHEADER ) + ncol*nrow*3;

	int bitsPerPixel = 32;  // can be 24 or 32 bits


	double reduceRate = 1.0;


	//  The size of each row is rounded up to a multiple of 4 bytes (a 32-bit DWORD) by padding.
	//double a = (bitsPerPixel * *ncol) / 32.0 * 4.0; 
	//m_rowSizeBytes = (int)ceil(a / 4.0) * 4;  // this rounds up 
	m_rowSizeBytes = (int)ceil(bitsPerPixel * ncol / 32.0) * 4;  // this rounds up 
	unsigned long long lenFileHead = sizeof(BITMAPFILEHEADER);
	unsigned long long lenInfoHead = sizeof(BITMAPINFOHEADER);

	ULONGLONG dwDibSize = lenFileHead + lenInfoHead + (int)ceil(m_rowSizeBytes * nrow / 4.0) * 4;

	// a = (bitsPerPixel * *ncol) / 32.0 * 4.0; 
	// ULONGLONG dwDibSize = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + (int)ceil(m_rowSizeBytes * *nrow / 4.0) * 4;
	
	if (dwDibSize > memoryMaxMB*MIL) {
		reduceRate = (double)dwDibSize / (double)(memoryMaxMB * MIL);
		nrow = (int)(nrow / reduceRate);
		ncol = (int)(ncol / reduceRate);
		//a = (bitsPerPixel * *ncol) / 32.0 * 4.0; 
		//m_rowSizeBytes = (int)ceil(a / 4.0) * 4;  // this rounds up 
		//dwDibSize = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + (int)ceil(m_rowSizeBytes * *nrow / 4.0) * 4;
		m_rowSizeBytes = (int)ceil(bitsPerPixel * ncol / 32.0) * 4;  // this rounds up 
		dwDibSize = lenFileHead + lenInfoHead + (int)ceil(m_rowSizeBytes * nrow / 4.0) * 4;
	}

	unsigned char *pDib = (unsigned char *)alloc1((size_t)dwDibSize, sizeof(unsigned char));
	if( pDib == NULL ) return  0.f;
	m_pDib = pDib;
	m_dwDibSize = dwDibSize;

	// for this empty bitmap, we do not want any m_Palette in the header
	if( m_Palette.GetSafeHandle() != NULL ) m_Palette.DeleteObject();

	m_pBIH = (BITMAPINFOHEADER *) m_pDib;
	ZeroMemory( m_pBIH, lenInfoHead);
	m_pBIH->biBitCount = bitsPerPixel;
	m_pBIH->biWidth = ncol;
	m_pBIH->biHeight = nrow;
	m_pBIH->biPlanes = 1;
	m_pBIH->biSize = (DWORD)lenInfoHead;
	m_nPaletteEntries = 0;
	m_pPalette = (RGBQUAD *) &m_pDib[lenFileHead]; // palette has zero length here
	m_pDibBits = &m_pDib[lenFileHead + lenInfoHead];

	return (float)reduceRate;
}

float CDib::Create24b(int &ncol, int &nrow)  // deping's addition
{
	int memoryMaxMB = (int)getMemoryAvail();

	// return 0: not created
	// return 1.0:  created normally
	// return >1.0: created while reduced by this amount due to memory limitation
	if( m_pDib != NULL ) {
	//	free(m_pDib);
		free1(m_pDib);  // this line enables large memory usage!
		m_pDib = NULL;
	}

	double bitsPerPixel = 24.0;  // can be 24 or 32 bits, make it double for easier calculations
	double reduceRate = 1.0;

	//  The size of each row is rounded up to a multiple of 4 bytes (a 32-bit DWORD) by padding.
	// double a = (bitsPerPixel * *ncol) / 32.0; 
	m_rowSizeBytes = (int)ceil(bitsPerPixel * ncol / 32.0) * 4;  // this rounds up 

	unsigned long long lenFileHead = sizeof(BITMAPFILEHEADER);
	unsigned long long lenInfoHead = sizeof(BITMAPINFOHEADER);
	m_dwDibSize = lenFileHead + lenInfoHead + (int)ceil(m_rowSizeBytes * nrow / 4.0) * 4;

	if (memoryMaxMB>0 && m_dwDibSize > memoryMaxMB*MIL) {
		reduceRate = (double)m_dwDibSize / (double)(memoryMaxMB * MIL);
		nrow = (int)(nrow / reduceRate);
		ncol = (int)(ncol / reduceRate);
		m_rowSizeBytes = (int)ceil(bitsPerPixel * ncol / 32.0) * 4;  // this rounds up 
		m_dwDibSize = lenFileHead + lenInfoHead + (int)ceil(m_rowSizeBytes * nrow / 4.0) * 4;
	}
	 
	m_pDib = (unsigned char *)alloc1((size_t)m_dwDibSize, sizeof(unsigned char));
	if( m_pDib == NULL ) return  0.f;
//	int fgrsd = sizeof(char);
	// for this empty bitmap, we do not want any m_Palette in the header
	if( m_Palette.GetSafeHandle() != NULL ) m_Palette.DeleteObject();

	m_pBIH = (BITMAPINFOHEADER *) m_pDib;
	ZeroMemory( m_pBIH, lenInfoHead);
	m_pBIH->biBitCount = (WORD)bitsPerPixel;
	m_pBIH->biWidth = ncol;
	m_pBIH->biHeight = nrow;
	m_pBIH->biPlanes = 1;
	m_pBIH->biSize = (DWORD)lenInfoHead;
	m_nPaletteEntries = 0;
	m_pPalette = (RGBQUAD *) &m_pDib[lenFileHead];
	m_pDibBits = &m_pDib[lenFileHead + lenInfoHead];

	// pre-fill with all white values
	memset(m_pDibBits, 255, m_dwDibSize - lenFileHead - lenInfoHead);

	return (float)reduceRate;
}


float CDib::Create24_Orig(int *ncol, int *nrow)  // deping's addition
{
	int memoryMaxMB = (int)getMemoryAvail();

	// return 0: not created
	// return 1.0:  created normally
	// return >1.0: created while reduced by this amount due to memory limitation
	if (m_pDib != NULL) {
		//	free(m_pDib);
		free1(m_pDib);  // this line enables large memory usage!
		m_pDib = NULL;
	}

	double bitsPerPixel = 24.0;  // can be 24 or 32 bits, make it double for easier calculations
	double reduceRate = 1.0;

	//  The size of each row is rounded up to a multiple of 4 bytes (a 32-bit DWORD) by padding.
	// double a = (bitsPerPixel * *ncol) / 32.0; 
	m_rowSizeBytes = (int)ceil(bitsPerPixel * *ncol / 32.0) * 4;  // this rounds up 

	unsigned long long lenFileHead = sizeof(BITMAPFILEHEADER);
	unsigned long long lenInfoHead = sizeof(BITMAPINFOHEADER);
	m_dwDibSize = lenFileHead + lenInfoHead + (int)ceil(m_rowSizeBytes * *nrow / 4.0) * 4;

	if (memoryMaxMB > 0 && m_dwDibSize > memoryMaxMB * 1000000) {
		reduceRate = (double)m_dwDibSize / (double)(memoryMaxMB * 1000000.);
		*nrow = (int)(*nrow / reduceRate);
		*ncol = (int)(*ncol / reduceRate);
		// a = (bitsPerPixel * *ncol) / 32.0; 
		m_rowSizeBytes = (int)ceil(bitsPerPixel * *ncol / 32.0) * 4;  // this rounds up 
		m_dwDibSize = lenFileHead + lenInfoHead + (int)ceil(m_rowSizeBytes * *nrow / 4.0) * 4;
	}

	m_pDib = (unsigned char *)alloc1((size_t)m_dwDibSize, sizeof(unsigned char));
	if (m_pDib == NULL) return  0.f;
	//	int fgrsd = sizeof(char);
		// for this empty bitmap, we do not want any m_Palette in the header
	if (m_Palette.GetSafeHandle() != NULL) m_Palette.DeleteObject();

	m_pBIH = (BITMAPINFOHEADER *)m_pDib;
	ZeroMemory(m_pBIH, lenInfoHead);
	m_pBIH->biBitCount = (WORD)bitsPerPixel;
	m_pBIH->biWidth = *ncol;
	m_pBIH->biHeight = *nrow;
	m_pBIH->biPlanes = 1;
	m_pBIH->biSize = (DWORD)lenInfoHead;
	m_nPaletteEntries = 0;
	m_pPalette = (RGBQUAD *)&m_pDib[lenFileHead];
	m_pDibBits = &m_pDib[lenFileHead + lenInfoHead];

	// pre-fill with all white values
	memset(m_pDibBits, 255, m_dwDibSize - lenFileHead - lenInfoHead);

	return (float)reduceRate;
}
// this function actually convert from CXImage to CDIB
BOOL CDib::LoadXImage( CString filename, int &biForm, int &ncol, int &nrow )
{
	CString tStr;
	int type = CxImage::GetTypeIdFromName(GetFileExtension(filename));
	CxImage *image = new CxImage(filename, type);
	if (!image->IsValid()){
		CString s = image->GetLastError();
		AfxMessageBox(s);
		delete image;
		image = NULL;
		return FALSE;
	}

	// now the CXImage is loaded, we need to convert it to bmp24.dib object
	ncol = image->GetWidth();
	nrow = image->GetHeight();

	float reduceRate = this->Create24b(ncol, nrow);
	if (reduceRate == 0.f) {
		// tStr.Format(_T("Failed to create bitmap with %d MB max memory. Try reduce max memory of usage."), draw.memMaxMB);
		delete image;
		image = NULL;
		return FALSE;
	}

	if( m_pDib == NULL )
		m_pDib = new unsigned char [(unsigned int)m_dwDibSize];

	// note:  m_dwDibSize is auto calculated during Create24b(), and is a roundup number
	if( m_pDib == NULL ) {
		delete image;
		image = NULL;
		return FALSE;
	}

	memcpy(m_pDib, &image->head, sizeof(BITMAPINFOHEADER));
	m_pBIH = (BITMAPINFOHEADER *) m_pDib;
	m_pPalette = (RGBQUAD *) &m_pDib[sizeof(BITMAPINFOHEADER)];
	biForm = m_pBIH->biBitCount = image->head.biBitCount;
	m_nPaletteEntries = m_pBIH->biClrUsed = image->head.biClrUsed;
	m_pDibBits = &m_pDib[sizeof(BITMAPINFOHEADER)+m_nPaletteEntries*sizeof(RGBQUAD)];
	memcpy(m_pDibBits, image->GetBits(), image->head.biSizeImage);
	delete image;
	return( TRUE );

}

BOOL CDib::LoadBMP( CString filename, int &biForm, int &ncol, int &nrow )
{
	CString ext = GetFileExtension(filename);
	if (ext.CompareNoCase(_T("bmp")) != 0)
		return this->LoadXImage(filename, biForm, ncol, nrow);

	CFile file;

	// Attempt to open the Dib file for reading.
	if( !file.Open( filename, CFile::modeRead | CFile::shareDenyNone) )
		return( FALSE );
	//if(_tfopen_s(&ifh1, velMod.floatName, _T("r") ) != 0) 

	if( m_pDib != NULL ) {
		free1(m_pDib);  // this line enables large memory usage!
	}


	// Get the size of the file and store
	// in a local variable. Subtract the
	// size of the BITMAPFILEHEADER structure
	// since we won't keep that in memory.
	ULONGLONG dwDibSize;
	dwDibSize = file.GetLength() - sizeof( BITMAPFILEHEADER );

	// Attempt to allocate the Dib memory.
	if( m_pDib == NULL ) {
		m_pDib = new unsigned char [(unsigned int)dwDibSize];
		m_dwDibSize = dwDibSize;
	}

	BITMAPFILEHEADER BFH;

	// Read in the Dib header and data.
	try{

		// Did we read in the entire BITMAPFILEHEADER?
		if( file.Read( &BFH, sizeof( BITMAPFILEHEADER ) )
			!= sizeof( BITMAPFILEHEADER ) ||

			// Is the type 'MB'?
			BFH.bfType != 'MB' ||

			// Did we read in the remaining data?
			file.Read( m_pDib, (UINT)dwDibSize ) != (UINT)dwDibSize ){
				// Delete the memory if we had any
				// errors and return FALSE.
				free(m_pDib);
				m_pDib = NULL;
				file.Close();
				return( FALSE );
		}
	}

	// If we catch an exception, delete the
	// exception, the temporary Dib memory,
	// and return FALSE.
	catch( CFileException *e ){
		e->Delete();
		free(m_pDib);
		m_pDib = NULL;
		file.Close();
		return( FALSE );
	}

	file.Close();


	// Pointer our BITMAPINFOHEADER and RGBQUAD
	// variables to the correct place in the Dib data.
	m_pBIH = (BITMAPINFOHEADER *) m_pDib;
	m_pPalette =
		(RGBQUAD *) &m_pDib[sizeof(BITMAPINFOHEADER)];

	//added by Deping here.
	ncol = m_pBIH->biWidth;
	nrow = m_pBIH->biHeight;
	biForm = m_pBIH->biBitCount;

	// Calculate the number of palette entries.
	m_nPaletteEntries = 1 << m_pBIH->biBitCount;
	if( m_pBIH->biBitCount > 8 )
		m_nPaletteEntries = 0;
	else if( m_pBIH->biClrUsed != 0 )
		m_nPaletteEntries = m_pBIH->biClrUsed;

	// Point m_pDibBits to the actual Dib bits data.
	m_pDibBits =
		&m_pDib[sizeof(BITMAPINFOHEADER)+
		m_nPaletteEntries*sizeof(RGBQUAD)];

	// If we have a valid palette, delete it.
	if( m_Palette.GetSafeHandle() != NULL )
		m_Palette.DeleteObject();

	// If there are palette entries, we'll need
	// to create a LOGPALETTE then create the
	// CPalette palette.
	if( m_nPaletteEntries != 0 ){

		// Allocate the LOGPALETTE structure.
		int aSize = sizeof(LOGPALETTE) + m_nPaletteEntries*sizeof(PALETTEENTRY);
		// LOGPALETTE *pLogPal = (LOGPALETTE *) new char[aSize];
		LOGPALETTE *pLogPal = (LOGPALETTE *) alloc1(aSize, sizeof(char));

		if( pLogPal != NULL ){

			// Set the LOGPALETTE to version 0x300
			// and store the number of palette
			// entries.
			pLogPal->palVersion = 0x300;
			pLogPal->palNumEntries = m_nPaletteEntries;

			// Store the RGB values into each
			// PALETTEENTRY element.
			for( int i=0; i<m_nPaletteEntries; i++ ){
				pLogPal->palPalEntry[i].peRed =
					m_pPalette[i].rgbRed;
				pLogPal->palPalEntry[i].peGreen =
					m_pPalette[i].rgbGreen;
				pLogPal->palPalEntry[i].peBlue =
					m_pPalette[i].rgbBlue;
			}

			// Create the CPalette object and
			// delete the LOGPALETTE memory.
			m_Palette.CreatePalette( pLogPal );
			free1(pLogPal);
		}
	}

	return( TRUE );

}

BOOL CDib::Save( CString filename )
{

	// If we have no data, we can't save.
	if( m_pDib == NULL )
		return( FALSE );

	CFile cf;

	// Attempt to create the file.
	if( !cf.Open( filename, CFile::modeCreate | CFile::modeWrite ) )
		return( FALSE );

	// Write the data.
	try{

		// First, create a BITMAPFILEHEADER
		// with the correct data.
		BITMAPFILEHEADER BFH;
		memset( &BFH, 0, sizeof( BITMAPFILEHEADER ) );
		BFH.bfType = 'MB';
		BFH.bfSize = sizeof( BITMAPFILEHEADER ) + (DWORD)m_dwDibSize;
		BFH.bfOffBits = sizeof( BITMAPFILEHEADER ) +
			sizeof( BITMAPINFOHEADER ) +
			m_nPaletteEntries * sizeof( RGBQUAD );

		// Write the BITMAPFILEHEADER and the
		// Dib data.
		cf.Write( &BFH, sizeof( BITMAPFILEHEADER ) );
		cf.Write( m_pDib, (UINT)m_dwDibSize );
	}

	// If we get an exception, delete the exception and
	// return FALSE.
	catch( CFileException *e ){
		e->Delete();
		return( FALSE );
	}

	return( TRUE );

}

BOOL CDib::Draw( CDC *pDC, int nX, int nY, int nWidth, int nHeight, bool isFacingDown, bool isSwapX)
{
	// If we have not data we can't draw.
	if( m_pDib == NULL ) return  FALSE;

	// Check for the default values of -1
	// in the width and height arguments. If
	// we find -1 in either, we'll set them
	// to the value that's in the BITMAPINFOHEADER.
	if( nWidth == -1 ) nWidth = m_pBIH->biWidth;
	if( nHeight == -1 ) nHeight = m_pBIH->biHeight;


	// NOTE: biHeight negative => Facing Down
	//       biHeight positive => Facing up
	m_pBIH->biHeight = isFacingDown  ?  -abs(m_pBIH->biHeight)  :  abs(m_pBIH->biHeight);


	int xPlot = isSwapX ? nX + nWidth : nX;
	int widthPlot = isSwapX ? -nWidth : nWidth;

	// Use StretchDIBits to draw the Dib.
	StretchDIBits( pDC->m_hDC, 
		xPlot,
		nY,
		widthPlot,
		nHeight,

		0, 
		0,
		m_pBIH->biWidth, 
		abs(m_pBIH->biHeight),
		m_pDibBits,
		(BITMAPINFO *) m_pBIH,
		DIB_RGB_COLORS, SRCCOPY );

	return  TRUE;

}

// Note: to swap in X-axis, make nWidth to be negative, and make nX to point to the right side of the image.
// to swap Y-axis, set isFacingDown=false, but CDC draws assuming facing down usually!;
BOOL CDib::Draw(CDC *pDC, int nX, int nY, int nWidth, int nHeight, bool isFacingDown)
{
	// If we have not data we can't draw.
	if (m_pDib == NULL)
		return  FALSE;

	// Check for the default values of -1
	// in the width and height arguments. If
	// we find -1 in either, we'll set them
	// to the value that's in the BITMAPINFOHEADER.
	if (nWidth == -1)
		nWidth = m_pBIH->biWidth;
	if (nHeight == -1)
		nHeight = m_pBIH->biHeight;


	// NOTE: biHeight negative => Facing Down
	//       biHeight positive => Facing up
	m_pBIH->biHeight = isFacingDown ? -abs(m_pBIH->biHeight) : abs(m_pBIH->biHeight);


	// Use StretchDIBits to draw the Dib.
	StretchDIBits(pDC->m_hDC,
		nX,
		nY,
		nWidth,
		nHeight,

		0,
		0,
		m_pBIH->biWidth,
		abs(m_pBIH->biHeight),
		m_pDibBits,
		(BITMAPINFO *)m_pBIH,
		DIB_RGB_COLORS, SRCCOPY);


	// SRCAND will keep overlapped colors, good for color map plus seismics. But you may also use SRCCOPY if you want purer colors

	// in printing to PDf, the image dimensions are very big, e.g. 12000x8000, and we have problems. So I try other functions
	//pDC->StretchBlt(logicalAxes.left, logicalAxes.top, logicalAxes.Width(), logicalAxes.Height(), 
	//	draw.gDC, 
	//	draw.gDC_rect.left, draw.gDC_rect.top, draw.gDC_rect.Width(), draw.gDC_rect.Height(), dw);




	return  TRUE;

}
BOOL CDib::DrawEx( CDC *pDC, int nXSrc, int nYSrc, int nWidthSrc, int nHeightSrc, 
				  int nX, int nY, int nWidth, int nHeight, bool isFacingDown ) {
					  // If we have not data we can't draw.
					  if( m_pDib == NULL ) return( FALSE );

					  // Check for the default values of -1
					  // in the width and height arguments. If
					  // we find -1 in either, we'll set them
					  // to the value that's in the BITMAPINFOHEADER.
					  if( nWidth == -1 )
						  nWidth = m_pBIH->biWidth;
					  if( nHeight == -1 )
						  nHeight = m_pBIH->biHeight;

					  // Use StretchDIBits to draw the Dib.
					  // NOTE: biHeight negative => Facing Down
					  //       biHeight positive => Facing up
					  m_pBIH->biHeight = isFacingDown  ?  -abs(m_pBIH->biHeight)  :  abs(m_pBIH->biHeight);





					  StretchDIBits( pDC->m_hDC, 
						  nX, nY,
						  nWidth, 
						  nHeight,

						  nXSrc,   // these are the bitmap pixels, not considering any client coords
						  abs(nYSrc),
						  nWidthSrc, 
						  abs(nHeightSrc),
						  m_pDibBits,
						  (BITMAPINFO *) m_pBIH,
						  DIB_RGB_COLORS, SRCCOPY );

					  // if( m_pDib != NULL ) free1(m_pDib);  // we just delete after draw, do not drawing again.
					  // m_pDib = NULL;

					  return( TRUE );

}

BOOL CDib::DrawEx2( CDC *pDC, 
				   int nX, int nY, int nWidth, int nHeight, 
				   int nXSrc, int nYSrc, int nWidthSrc, int nHeightSrc, 
				   bool isFacingDown ) {
					   // If we have not data we can't draw.
					   if( m_pDib == NULL ) return( FALSE );

					   if( nWidth == -1 ) nWidth = m_pBIH->biWidth;
					   if( nHeight == -1 ) nHeight = m_pBIH->biHeight;

					   // Use StretchDIBits to draw the Dib.
					   // NOTE: biHeight negative => Facing Down
					   //       biHeight positive => Facing up
					   m_pBIH->biHeight = isFacingDown  ?  -abs(m_pBIH->biHeight)  :  abs(m_pBIH->biHeight);

					   if (nXSrc < 0) {
						   // if here, the area of the whole bitmap is smaller than display area.
						   // Negative pixels cannot be plotted in the source bitmap, so we need to clip
						   double percent2Clip = (double)abs(nXSrc) / (double)nWidthSrc;
						   nWidthSrc += nXSrc;
						   nXSrc =  (int)(nWidthSrc * percent2Clip + 0.5);
					   }
					   else if (nX < 0) {
						   // if here, 
					   }


					   StretchDIBits( pDC->m_hDC, 
						   nX, nY,
						   nWidth, 
						   nHeight,

						   nXSrc,   // these are the bitmap pixels, not considering any client coords
						   abs(nYSrc),
						   nWidthSrc, 
						   abs(nHeightSrc),
						   m_pDibBits,
						   (BITMAPINFO *) m_pBIH,
						   DIB_RGB_COLORS, SRCCOPY );

					   return  TRUE ;
}


BOOL CDib::SetPalette( CDC *pDC )
{

	// If we have not data we
	// won't want to set the palette.
	if( m_pDib == NULL )
		return( FALSE );

	// Check to see if we have a palette
	// handle. For Dibs greater than 8 bits,
	// this will be NULL.
	if( m_Palette.GetSafeHandle() == NULL )
		return( TRUE );

	// Select the palette, realize the palette,
	// then finally restore the old palette.
	CPalette *pOldPalette;
	pOldPalette = pDC->SelectPalette( &m_Palette, FALSE );
	pDC->RealizePalette();
	pDC->SelectPalette( pOldPalette, FALSE );

	return( TRUE );

}




HRESULT CDib::SaveBitmapToFileNotused(LPTSTR pszFile, HBITMAP hBMP, HDC hDC) {
	HANDLE hf;
	BITMAPFILEHEADER hdr;
	PBITMAPINFOHEADER pbih;
	LPBYTE lpBits;
	DWORD dwTotal;
	DWORD cb;
	BYTE *hp;
	DWORD dwTmp;
	BITMAP bmp;
	PBITMAPINFO pbi;
	WORD cClrBits;

	if (!GetObject(hBMP, sizeof(BITMAP), (LPSTR)&bmp))
		return 0;

	cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel);
	if (cClrBits == 1)
		cClrBits = 1;
	else if (cClrBits <= 4)
		cClrBits = 4;
	else if (cClrBits <= 8)
		cClrBits = 8;
	else if (cClrBits <= 16)
		cClrBits = 16;
	else if (cClrBits <= 24)
		cClrBits = 24;
	else cClrBits = 32;

	if (cClrBits != 24)
	{
		pbi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER) +
			sizeof(RGBQUAD) * (1i64<< cClrBits));
	}
	else
		pbi = (PBITMAPINFO) LocalAlloc(LPTR, sizeof(BITMAPINFOHEADER));

	pbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbi->bmiHeader.biWidth = bmp.bmWidth;
	pbi->bmiHeader.biHeight = bmp.bmHeight;
	pbi->bmiHeader.biPlanes = bmp.bmPlanes;
	pbi->bmiHeader.biBitCount = bmp.bmBitsPixel;
	if (cClrBits < 24)
		pbi->bmiHeader.biClrUsed = (1<<cClrBits);

	pbi->bmiHeader.biCompression = BI_RGB;
	pbi->bmiHeader.biSizeImage = ((pbi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
		* pbi->bmiHeader.biHeight;
	pbi->bmiHeader.biClrImportant = 0;

	pbih = (PBITMAPINFOHEADER) pbi;
	lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

	if (!lpBits)
		return E_FAIL;

	if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi,
		DIB_RGB_COLORS))
	{
		return E_FAIL;
	}

	hf = CreateFile(pszFile,
		GENERIC_READ | GENERIC_WRITE,
		(DWORD) 0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE) NULL);
	if (hf == INVALID_HANDLE_VALUE)
		return E_FAIL;
	hdr.bfType = 0x4d42;
	hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof(RGBQUAD) + pbih->biSizeImage);
	hdr.bfReserved1 = 0;
	hdr.bfReserved2 = 0;

	hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) +
		pbih->biSize + pbih->biClrUsed
		* sizeof (RGBQUAD);

	if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER),
		(LPDWORD) &dwTmp, NULL))
	{
		return E_FAIL;
	}

	if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER)
		+ pbih->biClrUsed * sizeof (RGBQUAD),
		(LPDWORD) &dwTmp, ( NULL)) )
		return E_FAIL;

	dwTotal = cb = pbih->biSizeImage;
	hp = lpBits;
	if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL))
		return E_FAIL;

	CloseHandle(hf);
	GlobalFree((HGLOBAL)lpBits);
	LocalFree(pbi);

	return S_OK;

}


void CDib::SetDibBit32( int *column, int *row, BYTE r,BYTE g,BYTE b) {
	//m_pBIH->biWidth = ncol;
	//m_pBIH->biHeight = nrow;
	if (m_rowSizeBytes < 0) return;
	ULONGLONG pos = (*row) * m_rowSizeBytes + (*column) * 4;

	this->m_pDibBits[pos] = b;  // blue
	this->m_pDibBits[pos+1] = g;  // green
	this->m_pDibBits[pos+2] = r;  // red
	this->m_pDibBits[pos+3] = 0;  // alpha
}


//void CDib::SetDibBit24( int *column, int *row, BYTE r,BYTE g,BYTE b) {
//	if (m_rowSizeBytes < 0) return;
//	ULONGLONG pos = (*row) * m_rowSizeBytes + (*column) * 3;
//	this->m_pDibBits[pos] = b;  // blue
//	this->m_pDibBits[pos+1] = g;  // green
//	this->m_pDibBits[pos+2] = r;  // red
//}

void CDib::SetDibBit24( int column, int row, COLORREF rgb) {
	if (m_rowSizeBytes < 0) return;
	ULONGLONG pos = (row) * m_rowSizeBytes + (column) * 3;
	this->m_pDibBits[pos] = GetBValue(rgb);  // blue
	this->m_pDibBits[pos+1] = GetGValue(rgb);  // green
	this->m_pDibBits[pos+2] = GetRValue(rgb);  // red
}

void CDib::SetDibBit24(int column, int nrow, COLORREF rgbs[]) {
	if (m_rowSizeBytes < 0) return;
	ULONGLONG pos;
	ULONGLONG col3 = column * 3;
	for (int i = 0; i < nrow; i++) {
		pos = i* m_rowSizeBytes + col3;
		this->m_pDibBits[pos] = GetBValue(rgbs[i]);  // blue
		this->m_pDibBits[pos + 1] = GetGValue(rgbs[i]);  // green
		this->m_pDibBits[pos + 2] = GetRValue(rgbs[i]);  // red
	}
}


void CDib::SetDibBit( ULONGLONG *pos, BYTE r, BYTE g, BYTE b) {
	// note: *pos can be very big number, bigger than int
	this->m_pDibBits[*pos] = b;
	this->m_pDibBits[*pos+1] = g;
	this->m_pDibBits[*pos+2] = r;
}
