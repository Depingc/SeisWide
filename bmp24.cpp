/*****************************************************************************
                            B  S  B  M  P  2  4
                            -------------------

Take 3 busstop grids. Use as R, G, and B component in a plain graphics file
in one of the following formats:

 *  24 bit BMP (Microsoft)
 
 *  24 bit Encapsulated PostScript (Adobe)


PROGRAMMERS:       Thomas Knudsen, thk@kms.dk
                   National Survey and Cadastre - Denmark (KMS)
                   Geodetic Office
                   Rentemestervej 8
                   DK-2400 Copenhagen NV
                   Denmark

COPYRIGHT:         Thomas Knudsen / KMS
                   This file is part of the Busstop system - see the
                   file bsrights.h for rights to use, copy etc.

HISTORY:           YYYY-MM-DD - init

METHOD:            -
REFERENCES:        -
SECTIONS:          -

FUNCTIONS:
      bsbmp24_status
      bsbmp24
      main

DEPENDENCIES:
      libbs.a,
      bs.h

DESCRIPTION:
// Old code, not used any more

*****************************************************************************/


#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "bmp24.h"

# include "segy.h"


// Old code, not used any more
FILE *BMP24CreateWriteStream (
   CString name,
   int   ncol,
   int   nrow,
   int   biForm)
{
   
   unsigned char
      bmp24_header[54] = {
         'B', 'M',               /* Magic number identifying Windows BMP files   */
    
         0x00, 0x00, 0x00, 0x00, /* File size                              ***   */
    
         0x00, 0x00, 0x00, 0x00, /* Reserved. Must be set to 0                   */
         0x36, 0x00, 0x00, 0x00, /* The actual bitmap starts at adress 0x36      */
         0x28, 0x00, 0x00, 0x00, /* Size of bitmapinfoheader chunk               */
    
         0x00, 0x00, 0x00, 0x00, /* Bitmap width                           ***   */
         0x00, 0x00, 0x00, 0x00, /* Bitmap height                          ***   */
    
         0x01, 0x00,             /* Number of target image planes. Must be 1     */
         0x18, 0x00,             /* 0x18 = 24 decimal. Magic no. for 24 bit BMP  */
         0x00, 0x00, 0x00, 0x00, /* Compression type. Equals 0 for 24 bit bitmap */
    
         0x00, 0x00, 0x00, 0x00, /* Image size                             ***   */
    
         0xCE, 0x0E, 0x00, 0x00, /* Target device bits/m horizontally            */
         0xC4, 0x0E, 0x00, 0x00, /* Target device bits/m vertically              */
                                 /* Values here taken from a Win Paintbrush img  */
    
         0x00, 0x00, 0x00, 0x00, /* No. of palette entries used (0 for 24 bit)   */
         0x00, 0x00, 0x00, 0x00  /* No. of palette entries important (0)         */
    
      };

   UINT buf;
   int  linesize, i;
   FILE *f;
   RGBQUAD colTab[2];

   if (biForm == 24)
   {
	   linesize = 3*ncol + ncol%4; //presumed line size.
	   /* file size */
		buf = 54 + linesize*nrow;
   }
   else
   {
	   linesize = ((ncol + 31) & ~31) >> 3;   //this is the scanline width for 1-bit BMPs, where ncol is number of pixels in x.
	   /* file size */
		buf = 54 + 2*sizeof(RGBQUAD) + linesize*nrow;
   }

//   if (4321==NATIVE_BYTEORDER)
  //    bswap32(&buf);
   memcpy(bmp24_header+2, &buf, 4);

   /* bitmap width */
   buf = ncol;
//   if (4321==NATIVE_BYTEORDER)
  //    bswap32(&buf);
   memcpy(bmp24_header+18, &buf, 4);
  
   /* bitmap height */
   buf = nrow;
//   if (4321==NATIVE_BYTEORDER)
  //    bswap32(&buf);
   memcpy(bmp24_header+22, &buf, 4);

   /* bitmap size */
   buf = nrow * linesize;
 //  if (4321==NATIVE_BYTEORDER)
   //   bswap32(&buf);
   memcpy(bmp24_header+34, &buf, 4);

   if(biForm==1) bmp24_header[28] = 1;

   if (0!=_tfopen_s(&f, name, _T("wb")))
      return 0;

   fwrite(bmp24_header, 1, 54, f);
   if(biForm==1)
   { //only if monochrome case.
	   bmp24_header[28] = 1;
	   colTab[0].rgbRed = colTab[0].rgbBlue = colTab[0].rgbGreen = 0;
	   colTab[1].rgbRed = colTab[1].rgbBlue = colTab[1].rgbGreen = 255;
	   colTab[0].rgbReserved = colTab[1].rgbReserved = 0;
	   fwrite(colTab, 1, 2*sizeof(RGBQUAD), f);
   }

   /* BMPs are stored upside down, so we must create a full size empty file
      and then fill it gradually from the end */
   for (i=0; i<nrow*linesize; i++)
      fputc(0, f);
   fseek(f, -linesize, SEEK_CUR);
   return f;
}

// Old code, not used any more
FILE *BMP24CreateReadStream(
   CString name,
   int  *ncol,
   int  *nrow,
   int  *biForm)
{

//	unsigned char
//      bmp24_header[54] = {
//         'B', 'M',               /* Magic number identifying Windows BMP files   */    
//         0x00, 0x00, 0x00, 0x00, /* File size                              ***   */   
//         0x00, 0x00, 0x00, 0x00, /* Reserved. Must be set to 0                   */
//         0x36, 0x00, 0x00, 0x00, /* The actual bitmap starts at adress 0x36      */
//         0x28, 0x00, 0x00, 0x00, /* Size of bitmapinfoheader chunk               */    
//         0x00, 0x00, 0x00, 0x00, /* Bitmap width                           ***   */
//         0x00, 0x00, 0x00, 0x00, /* Bitmap height                          ***   */
//         0x01, 0x00,             /* Number of target image planes. Must be 1     */
//         0x18, 0x00,             /* 0x18 = 24 decimal. Magic no. for 24 bit BMP  */
//         0x00, 0x00, 0x00, 0x00, /* Compression type. Equals 0 for 24 bit bitmap */    
//         0x00, 0x00, 0x00, 0x00, /* Image size                             ***   */
//         0xCE, 0x0E, 0x00, 0x00, /* Target device bits/m horizontally            */
//         0xC4, 0x0E, 0x00, 0x00, /* Target device bits/m vertically              */
                                 /* Values here taken from a Win Paintbrush img  */
//         0x00, 0x00, 0x00, 0x00, /* No. of palette entries used (0 for 24 bit)   */
//         0x00, 0x00, 0x00, 0x00  /* No. of palette entries important (0)         */
//      };

// typedef struct tagBITMAPFILEHEADER { // bmfh
//	WORD    bfType;  /* BM, Magic number identifying Windows BMP files   */
//  DWORD   bfSize; /* 0, File size                              ***   */
//	WORD    bfReserved1; //Must be set to 0
//	WORD    bfReserved2; //Must be set to 0
//  DWORD   bfOffBits; // 0x36000000 ???
//  } BITMAPFILEHEADER; 
	//typedef struct tagBITMAPCOREHEADER { // bmch
//	DWORD   bcSize; //0x28000000 ???
//  WORD    bcWidth;
//	WORD    bcHeight;
//	WORD    bcPlanes; 
//  WORD    bcBitCount; } BITMAPCOREHEADER
// typedef struct tagBITMAPINFOHEADER{ // bmih
//	DWORD  biSize; 
//  LONG   biWidth;
//	LONG   biHeight;
//	WORD   biPlanes; 
//  WORD   biBitCount
//	DWORD  biCompression;
//	DWORD  biSizeImage; 
//  LONG   biXPelsPerMeter;
//	LONG   biYPelsPerMeter;
//	DWORD  biClrUsed; 
//  DWORD  biClrImportant; } BITMAPINFOHEADER; 
/*typedef struct tagRGBQUAD { // rgbq 
    BYTE    rgbBlue; 
    BYTE    rgbGreen; 
    BYTE    rgbRed; 
    BYTE    rgbReserved; 
} RGBQUAD; //defined in vc++
 */

	BITMAPFILEHEADER hdr;
//	BITMAPCOREHEADER hCore;
	BITMAPINFOHEADER hInfo;
	FILE *f;
	int linesize, tabSize;
	RGBQUAD colTab[2];

	tabSize = 1<<(*biForm); //size of the color table.
	if (colTab == 0) return 0;

   if (0!=_tfopen_s(&f, name, _T("rb")))
	   return 0;
   fread(&hdr, sizeof(BITMAPFILEHEADER), 1, f);
   fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, f);
   fread(colTab, sizeof(RGBQUAD), tabSize, f);

   if (hdr.bfType != 0x4d42)
   {
	   fclose(f);
	   free (colTab);
	   AfxMessageBox(_T("Not a BMP format"), MB_ICONQUESTION);
	   return 0;
   }

//   memcpy(ncol, bmp24_header+18, 4);
  *ncol = hInfo.biWidth;
  
//   memcpy(nrow, bmp24_header+22, 4);
  *nrow = hInfo.biHeight;

//   memcpy(biForm, bmp24_header+28, 4);
  *biForm = hInfo.biBitCount;

   if (*biForm == 24) linesize = 3*(*ncol) + (*ncol)%4; //presumed line size.
   else
	   linesize = ((*ncol*(*biForm) + 31) & ~31) >> 3;   //this is the scanline width for other-bit BMPs, where ncol is number of pixels in x.

   if ((UINT)linesize*(*nrow) != hInfo.biSizeImage)
   {
	   fclose(f);
	   free (colTab);
	 //  AfxMessageBox(_T("Bitmap size mismatch, unknown format"));
	   return 0;
   }

   /* file size */
 //  memcpy(&buf, bmp24_header+2, 4);
 //  if (buf != (UINT)(54 + linesize*(*nrow)))
 //   AfxMessageBox(_T("File size mismatch, but continue anyway. ");

   /* BMPs are stored upside down, so we must read gradually from the end */
//   for (i=0; i<nrow*linesize; i++)
//      fputc(0, f);
//	   fseek(f, ((*nrow)-1)*linesize, SEEK_CUR);
   return f;

}


// Old code, not used any more
unsigned char *BMP24CreateLineBuffer(
   int ncol, int biForm)
{
   unsigned char
      *p;
   int
      linesize;

   if (biForm == 24)
	   linesize = 3*ncol + ncol%4; //presumed line size.
   else 
	   linesize = ((ncol + 31) & ~31) >> 3;   //this is the scanline width for 1-bit BMPs, where ncol is number of pixels in x.
   if (0==(p=(unsigned char*)malloc(linesize)))
      return 0;

   return (unsigned char*)memset(p, 0, linesize);

}

void BMP24SetRow( unsigned char *linebuf, int col, int r, int g, int b, int biForm ){ //for 24-bit only.
	int itemp, ishift;
	if (biForm == 24) {
		//Note: the physical representation of bitmap must be b, g, r, so we turn over it.
	   col*=3;
	   linebuf[col] = (unsigned char)b;
	   linebuf[col+1] = (unsigned char)g;
	   linebuf[col+2]   = (unsigned char)r;
	}
	else {
		itemp = col /8;
		ishift = col%8;
		if (ishift == 0) linebuf[itemp] = 0;
		if (r+g+b >0) linebuf[itemp] += (0x01 << (7-ishift));
	}
}

// Old code, not used any more
void BMP24SetRow(unsigned char *linebuf, int col, COLORREF rgb, int biForm){ //for 24-bit only.
	int itemp, ishift;
	if (biForm == 24) {
		//Note: the physical representation of bitmap must be b, g, r, so we turn over it.
	   col*=3;
	   linebuf[col] = (unsigned char)GetBValue(rgb);
	   linebuf[col+1] = (unsigned char)GetGValue(rgb);
	   linebuf[col+2]   = (unsigned char)GetRValue(rgb);
	}
	else {
		itemp = col /8;
		ishift = col%8;
		if (ishift == 0) linebuf[itemp] = 0;
		linebuf[itemp] += (0x01 << (7-ishift));
	}
}

// Old code, not used any more
size_t BMP24WriteRow(
   unsigned char *buf,
   int ncol,
   FILE *stream, int biForm)
{
   size_t
      linesize,
      ret;

   if (biForm == 24)
	   linesize = 3*ncol + ncol%4; //presumed line size.
   else 
	   linesize = ((ncol + 31) & ~31) >> 3;   //this is the scanline width for 1-bit BMPs, where ncol is number of pixels in x.
   ret = fwrite(buf, (unsigned int)1, (unsigned int)linesize, stream);
   int temp = fseek(stream, (long)(-2*linesize), SEEK_CUR);
   return ret;
}

// Old code, not used any more
size_t BMP24ReadRow(
   unsigned char *buf,
   int ncol,
   FILE *stream, int biForm)
{
   int linesize;
   size_t ret;

   if (biForm == 24)
	   linesize = 3*ncol + ncol%4; //presumed line size.
   else 
	   linesize = ((ncol + 31) & ~31) >> 3;   //this is the scanline width for 1-bit BMPs, where ncol is number of pixels in x.
   ret = fread(buf, (unsigned int)1, (unsigned int)linesize, stream);
   int temp = fseek(stream, (long)(-2*linesize), SEEK_CUR);
   return ret;
}

int BMP24CloseStream(
   FILE *f)
{
   return fclose(f);
}


