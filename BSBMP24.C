#define BSBMP24_C
#define NAME    "bsbmp24"
#define VERSION "1999-02-09"
#define BRIEF   "convert 3 busstop grids to one 24 bit bmp file\n"
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

*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <time.h>

#include <bs.h>
#include <bslow.h>
#include <o.h>

char
   bt_bsbmp24_help[] = {
     "\nsyntax:\n"
     "   bsbmp24 red.des{par} r0 r1 grn.des{par} g0 g1 blu.des{par} b0 b1 outfile\n"
     "   bsbmp24 [ --help | --status | --version]\n\n"
     "bsbmp24 creates a 24 bit  Windows BMP file from the three busstop grids\n"
     "specified by the 3 descriptor/parameter pairs given on the command line.\n"
     "The value ranges specified by (r0,r1), (g0,g1), and (b0,b1) will be\n"
     "stretched linearly to the BMP range of (0,255).\n"
   };




FILE *PS24CreateWriteStream (
   char *name,
   int   ncol,
   int   nrow
){

   FILE
      *f;

   if (0==(f=fopen(name, "wt")))
      return 0;

   /* Encapsulated 24 bit Color PS Prolog */
   fprintf(gf,
     "%%!PS-Adobe-3.0 EPSF-3.0\n"
     "%%%%BoundingBox: %d %d %d %d\n"
     "%%%%Extensions: CMYK\n"
     "%%%%EndComments\n"
     "%%%%EndProlog\n",
     0, 0, sx, sy
   );
 
   /* Scaling information */
   fprintf(gf, "%d %d scale\n", sx, sy);
 
   /* Image Header */
   fprintf(gf, "/picstr %d string def\n", 3*sx);
   fprintf(gf, "%d %d 8\n", sx, sy);
   fprintf(gf, "[%d 0 0 %d 0 %d]\n",   sx, -sy, sy);
   fprintf(gf, "{currentfile\n picstr readhexstring pop}\nfalse 3\ncolorimage");

   return f;
}



void ps_pixel(FILE *f, int *i, int r, int g, int b){
   r = (r<0)? 0: (r>255)? 255: r;
   g = (g<0)? 0: (g>255)? 255: g;
   b = (b<0)? 0: (b>255)? 255: b;
   fprintf(f, "%2.2x%2.2x%2.2x", r, g, b);
   if (0==(*i++)%13) {
      fprintf(f, "\n");
      *i=0;
   }
}

void PS24CloseStream(FILE *f){
   fprintf(f, "\nshowpage\n%%%%EOF\n");
   fclose(f);
}


unsigned char *PS24CreateLineBuffer(
   int ncol
){
   unsigned char  *p;
   int             linesize;

   linesize = 3*ncol;
   if (0==(p=malloc(linesize)))
      return 0;

   return memset(p, 0, linesize);
}




void PS24SetRow(
   unsigned char *linebuf,
   int   col,
   int   r,
   int   g,
   int   b
){
   r = (r<0)? 0: (r>255)? 255: r;
   g = (g<0)? 0: (g>255)? 255: g;
   b = (b<0)? 0: (b>255)? 255: b;
   col*=3;
   linebuf[col++] = r;
   linebuf[col++] = g;
   linebuf[col]   = b;
}


int PS24WriteRow(
   unsigned char *buf,
   int ncol,
   FILE *stream
){
   int i;
   for (i=0, n=0; i<ncol; i++, n++) {
      if (0==n%13) {
         fprintf(f, "\n");
         n=0;
      }
      fprintf(f, "%2.2x%2.2x%2.2x", buf[3*i], buf[3*i+1], buf[3*i+2]);
   }

}






FILE *BMP24CreateWriteStream (
   char *name,
   int   ncol,
   int   nrow
){
   
   unsigned char
      bmp24_header[56] = {
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

   bs_uint32
      buf;

   int
      linesize,
      i;

   FILE
      *f;

   linesize = 3*ncol + 4 - (3*ncol)%4;

   /* file size */
   buf = 54 + linesize*nrow;
   if (4321==NATIVE_BYTEORDER)
      bswap32(&buf);
   memcpy(bmp24_header+2, &buf, 4);

   /* bitmap width */
   buf = ncol;
   if (4321==NATIVE_BYTEORDER)
      bswap32(&buf);
   memcpy(bmp24_header+18, &buf, 4);

   /* bitmap height */
   buf = nrow;
   if (4321==NATIVE_BYTEORDER)
      bswap32(&buf);
   memcpy(bmp24_header+22, &buf, 4);

   /* bitmap size */
   buf = nrow * linesize;
   if (4321==NATIVE_BYTEORDER)
      bswap32(&buf);
   memcpy(bmp24_header+34, &buf, 4);

   if (0==(f=fopen(name, "wb")))
      return 0;

   fwrite(bmp24_header, 1, 56, f);

   /* BMPs are stored upside down, so we must create a full size empty file
      and then fill it gradually from the end */
   for (i=0; i<nrow*linesize; i++)
      fputc(0, f);
   fseek(f, -linesize, SEEK_CUR);
   return f;
}

unsigned char *BMP24CreateLineBuffer(
   int ncol
){
   unsigned char
      *p;
   int
      linesize;

   linesize = 3*ncol + 4 - (3*ncol)%4;
   if (0==(p=malloc(linesize)))
      return 0;

   return memset(p, 0, linesize);

}

void BMP24SetRow(
   unsigned char *linebuf,
   int   col,
   int   r,
   int   g,
   int   b
){
   col*=3;
   linebuf[col++] = r;
   linebuf[col++] = g;
   linebuf[col]   = b;
}

int BMP24WriteRow(
   unsigned char *buf,
   int ncol,
   FILE *stream
){
   int
      linesize,
      ret;

   linesize = 3*ncol + 4 - (3*ncol)%4;
   ret = fwrite(buf, 1, linesize, stream);
   fseek(stream, -2*linesize, SEEK_CUR);
   return ret;
}

int BMP24CloseStream(
   FILE *f
){
   return fclose(f);
}


int BtBsbmp24 (
   garage *gar,
   char *fnin[],
   char *enin[],
   char *fnou,
   double *a,
   double *b,
   int verbose
){
   int
      err=0;
   garage
      *g;
   bus
      *p[]   = {0, 0, 0};

   double
      *bin[] = {0, 0, 0};

   int
      i, j;

   FILE
      *fou;

   int
      dim=0,
       s[3],
       nrow,
       ncol,
       row;

   double
      x0[3],
      dx[3],
      x1[3];

   unsigned char
      *bmpline;

   if(0==gar)
     g = BsGarageCreate("bsbmp24");
   else
      g = gar;
   if (!g)
      bserror(EXIT_FAILURE, "bsbmp24: out of core for garage - bye!");

   /* The allocation of memory etc. is a little bit tricky because:
      1) two or three grids may be from the same input file
      2) two or three grids may be exactly the same (one grid may
         occupy one, two or three colour guns. The latter will yield
         a greyscale image, unless different ranges are specified   */

   /* The first is always plain */
   if (0==(p[0]=BsBusCreate(g, "red"))) {
      err = 2;
      goto err_ret;
   }

   BsReadDescFile(p[0], fnin[0]);

   dim = BsNumFields(p[0], fc_implied);
   if (verbose)
      fprintf(stdout, "dim=%d ", dim);
   if ((dim>3)||(dim<2)) {
      err = 3;
      goto err_ret;
   }

   BsGridPar(p[0], s, x0, dx, x1);
   switch (dim) {
     case 2:  ncol = s[1]; nrow = s[0]; break;
     case 3:  ncol = s[2]; nrow = s[1]; break;
     default: ncol = nrow = 0;
   }
   if (verbose)
      fprintf(stdout, "ncol=%d, nrow=%d\n", ncol, nrow);

   if (0==(bin[0] = malloc(ncol*sizeof(double)))){
      err = 4;
      goto err_ret;
   }

   /* if element name is not specified, we use the first express field */
   if (0==enin[0])
      enin[0] = BsFieldName(p[0], 0, fc_express);

   BsSetBufDouble(p[0], enin[0], bin[0]);

   for (i=1; i<3; i++) {
      for (j=0; j<i; j++) {

         /* same file name? */
         if (0==strcmp(fnin[i], fnin[j])) {

            if (0==enin[i])
               enin[i] = BsFieldName(p[j], 0, fc_express);
            
            /* same element? just point to the same buffer */
            if (0==strcmp(enin[i], enin[j])) {
               bin[i] = bin[j];
               break;
            }

            /* not same element? add another buffer to the file */
            if (0==(bin[i] = malloc(ncol*sizeof(double)))){
               err = 4;
               goto err_ret;
            }
            BsSetBufDouble(p[j], enin[i], bin[i]);
            break;
         }
      }

      /* neither same file, nor same element - then we need a new bus */
      if (j==i) {
         if (0==(p[i]=BsBusCreate(g, (i==1)? "green": "blue"))) {
            err = 5;
            goto err_ret;
         }
         
         BsReadDescFile(p[i], fnin[i]);
         
         if (0==(bin[i] = malloc(ncol*sizeof(double)))) {
            err = 6;
            goto err_ret;
         }
         
         if (0==enin[i])
            enin[i] = BsFieldName(p[i], 0, fc_express);
            
         BsSetBufDouble(p[i], enin[i], bin[i]);
      }
   }

   /* Start creating the BMP file */
   fou     = BMP24CreateWriteStream(fnou, ncol, nrow);
   bmpline = BMP24CreateLineBuffer(ncol);

   for (row=0; row<nrow; row++) {
      int col;
      for (i=0; i<3; i++)
         if (0!=p[i])
            BsRead(p[i], ncol);

      for(col=0; col<ncol; col++) {
         int  red, green, blue;
         red   = 256*(bin[0][col] - a[0])/(b[0]-a[0]);
         green = 256*(bin[1][col] - a[1])/(b[1]-a[1]);
         blue  = 256*(bin[2][col] - a[2])/(b[2]-a[2]);
         BMP24SetRow(bmpline, col, red, green, blue);
      }
      BMP24WriteRow(bmpline, ncol, fou);
      if (verbose)
         fprintf(stdout, "%s: %d rows total. %3d rows remaining        \r", fnou, nrow, nrow-1-row);
      fflush(stdout);
   }
   if (verbose)
         fprintf(stdout, "\n");

   BMP24CloseStream(fou);

   BsGarageDestroy(g);
   return 0;

   err_ret:
   /* todo: deallocate */
   if (verbose)
      fprintf(stdout, "err=%d\n", err);
   BsGarageDestroy(g);
   return err;
}







int bsbmp24(int argc, char *argv[]){
   char
      *fnin[3]  = {0,0,0},
      *enin[3]  = {0,0,0},
      *fnou,
      *fnoudef  = {"a.bmp"},
      *s;
   double
      a[3] = {0,0,0},
      b[3] = {0,0,0};
   int   verbose=0, ret, j;
   O    *o;

   if (argc<10)
      return fprintf(stderr,bt_bsbmp24_help), 1;
 
   fnou = fnoudef;
   
   if (0==(o=o_init(argc, argv)))
      return fprintf(stderr, "%s: out of memory for options - bye!\n", argv[0]), 2;
 
   o_decode (o) {
      o_decode_long (o, "version")
         return BsStatus(stdout, BS_STATUS, "   (none)\n");
      o_decode_long (o, "status")
         return BsStatus(stdout, BS_STATUS, "   (none)\n");
      o_decode_long (o, "help")
         return fprintf(stdout,bt_bsbmp24_help), 0;
      o_decode_long (o, "verbose")
         verbose++;
      o_default_long (o)
         return fprintf(stderr, "%s: unknown option `%s' - bye!\n", argv[0], o_name(o)), 3;
 
      o_decode_short (o) {
         case 'h':
            return fprintf(stdout,bt_bsbmp24_help), 0;
            break;
         case 'v':
            verbose++;
            break;
         case 'o':
            if (0==(fnou=o_get_string_arg(o)))
               return fprintf(stderr, "%s: bad or missing argument for  option `o' - bye!\n", argv[0]), 4;
               break;
         default:
            return fprintf(stderr, "%s: unknown option `%s' - bye!\n", argv[0], o_name(o)), 3;
      }
   }
   
      
   /* Decode input file names, element names and ranges */
   for (j=0; j<3; j++) {
      int   k, n;
      char *endp;

      if (0==(fnin[j] = o_next_file_name(o)))
         return printf(bt_bsbmp24_help), 1;

      /* split the file name and the element name */
      for (k=0, n=strlen(fnin[j]); k<n; k++) {
         /* null-terminate the file name, init the element name */
         if (fnin[j][k]=='{') {
            fnin[j][k] = '\0';
            enin[j] = fnin[j]+k+1;
         }

         /* null-terminate the element name */
         if (fnin[j][k]=='}')
            fnin[j][k] = '\0';
      }

      /* Range begin */
      if (0==(s = o_next_file_name(o)))
         return printf("channel %d: missing range begin\n", j), 1;
      a[j] = strtod(s, &endp);
      if ('\0'!=*endp)
         return printf("channel %d: bad range begin\n", j), 1;

      /* Range end */
      if (0==(s = o_next_file_name(o)))
         return printf("channel %d: missing range end\n", j), 1;
      a[j] = strtod(s, &endp);
      if ('\0'!=*endp)
         return printf("channel %d: bad range end\n", j), 1;

   }

   ret = BtBsbmp24(0, fnin, enin, fnou, a, b, verbose);

   o_exit(o);
   return ret;
}



#ifdef ROUNDTRIP
int main(int argc, char *argv[]){
  int r=0;
  r = bsbmp24(argc, argv);
  return r;
}
#endif

#undef BSBMP24_C
