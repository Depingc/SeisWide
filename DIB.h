// DIB.h
#include "colorHelper.h"
#ifndef __DIB_H__
#define __DIB_H__

class CDib
{

public:
	CDib();
	~CDib();

	float Create32Ex(int &ncol, int &nrow); // deping's addition
	float Create24_Orig(int *ncol, int *nrow); // deping's addition
	float Create24b(int &ncol, int &nrow); // deping's addition

	BOOL LoadXImage( CString filename, int &biForm, int &ncol, int &nrow );
	BOOL LoadBMP( CString filename, int &biForm, int &ncol, int &nrow );
	BOOL Save( CString filename );
	BOOL Draw( CDC *pDC, int nX, int nY, int nWidth, int nHeight, bool isFacingDown);
	BOOL Draw(CDC *pDC, int nX, int nY, int nWidth, int nHeight, bool, bool);
	BOOL DrawEx( CDC *pDC, int nXSrc, int nYSrc, int nWidthSrc, int nHeightSrc,
		int nX = 0, int nY = 0, int nWidth = -1, int nHeight = -1, bool isFacingDown = false );

	BOOL DrawEx2( CDC *pDC, 
		int nX, int nY, int nWidth, int nHeight, 
		int nXSrc, int nYSrc, int nWidthSrc, int nHeightSrc, 
		bool isFacingDown = false );


	BOOL SetPalette( CDC * );
	void SetDibBit32( int *column, int *row, BYTE r,BYTE g,BYTE b);
	//void SetDibBit24( int *column, int *row, BYTE r,BYTE g,BYTE b);
	void SetDibBit24( int column, int row, COLORREF rgb);
	void SetDibBit24(int column, int nrow, COLORREF rgbs[]);
	void SetDibBit(ULONGLONG *pos, BYTE r, BYTE g, BYTE b);
	inline unsigned char *getBits() {
		return this->m_pDibBits;
	}
	
	inline CColorHelper::hsv *getHsvAll() {
		LONG nx = m_pBIH->biWidth; // note: m_rowSizeBytes is the actual length in CDib, and it longer than nx
		LONG ny = m_pBIH->biHeight;
		CColorHelper::hsv *hsv = (CColorHelper::hsv *)alloc1(nx*ny, sizeof(CColorHelper::hsv));
		ULONGLONG pos0, pos;
		ULONGLONG posHsv;
		for (int iy = 0; iy < ny; iy++) {
			pos0 = iy *m_rowSizeBytes;
			posHsv = iy*nx; 
			for (register int ix = 0; ix < nx; ix++) {
				pos = pos0 + ix * 3;
				hsv[posHsv + ix] = CColorHelper::rgb2hsvf(RGB(m_pDibBits[pos + 2], m_pDibBits[pos + 1], m_pDibBits[pos]));
			}
		}
		return hsv;
	}

	inline void filterLoG5_OLD(CColorHelper::hsv hsv[]) {
		register double f;
		LONG nx = m_pBIH->biWidth;
		LONG ny = m_pBIH->biHeight;
		for (int y = 2; y < ny - 2; y++) {
			register ULONGLONG pos0 = nx * (y -2);
			register ULONGLONG pos1 = nx * (y -1);
			register ULONGLONG pos2 = nx * y;
			register ULONGLONG pos3 = nx * (y +1);
			register ULONGLONG pos4 = nx * (y +2);
			for (int x = 2; x < nx - 2; x++) {
				f =
					-hsv[pos0 + x].v
					- hsv[pos1 + x - 1].v - 2. * hsv[pos1 + x].v - hsv[pos1 + x + 1].v
					- hsv[pos2 + x - 2].v - 2. * hsv[pos2 + x - 1].v + 16. * hsv[pos2 + x].v - 2.*hsv[pos2 + x + 1].v - hsv[pos2 + x + 2].v
					- hsv[pos3 + x - 1].v - 2. * hsv[pos3 + x].v - hsv[pos3 + x + 1].v
					- hsv[pos4 + x].v;
				// hsv[pos2 + x].v = f;
				//hsv[pos2 + x].v *= 0.5;
				register CColorHelper::rgb c = CColorHelper::hsv2rgbf(hsv[pos2 + x]);
				register COLORREF ref = RGB(
					(unsigned char)(c.r * 255.),
					(unsigned char)(c.g * 255.),
					(unsigned char)(c.b * 255.)
				);
				SetDibBit24(x, y, ref);
			}
		}
	}

	inline void filterLoG5(CColorHelper::hsv hsv[]) {
		register double f;
		LONG nx = m_pBIH->biWidth;
		LONG ny = m_pBIH->biHeight;
		CColorHelper::hsv hsvt;
		for (int y = 2; y < ny - 2; y++) {
			register ULONGLONG pos0 = nx * (y - 2);
			register ULONGLONG pos1 = nx * (y - 1);
			register ULONGLONG pos2 = nx * y;
			register ULONGLONG pos3 = nx * (y + 1);
			register ULONGLONG pos4 = nx * (y + 2);
			for (int x = 2; x < nx - 2; x++) {
				f =
					-hsv[pos0 + x].v
					- hsv[pos1 + x - 1].v - 2. * hsv[pos1 + x].v - hsv[pos1 + x + 1].v
					- hsv[pos2 + x - 2].v - 2. * hsv[pos2 + x - 1].v + 16. * hsv[pos2 + x].v - 2.*hsv[pos2 + x + 1].v - hsv[pos2 + x + 2].v
					- hsv[pos3 + x - 1].v - 2. * hsv[pos3 + x].v - hsv[pos3 + x + 1].v
					- hsv[pos4 + x].v;
				hsvt = hsv[pos2 + x];
				hsvt.v = f*0.5;
				register CColorHelper::rgb c = CColorHelper::hsv2rgbf(hsvt);
				register COLORREF ref = RGB(
					(unsigned char)(c.r * 255.),
					(unsigned char)(c.g * 255.),
					(unsigned char)(c.b * 255.)
				);
				SetDibBit24(x, y, ref);
			}
		}
	}
	HRESULT SaveBitmapToFileNotused(LPTSTR pszFile, HBITMAP hBMP, HDC hDC) ;
	BITMAPINFOHEADER *m_pBIH;

private:
	ULONGLONG m_dwDibSize;
	int m_nPaletteEntries;
	CPalette m_Palette;
	RGBQUAD *m_pPalette;
	unsigned char *m_pDib, *m_pDibBits;
	ULONGLONG m_rowSizeBytes;  // only used in Create new bitmap

	bool m_isTransformed;  // for color transformation
};

#endif
