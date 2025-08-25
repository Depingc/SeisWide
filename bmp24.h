FILE *BMP24CreateWriteStream (
   CString name,
   int   ncol,
   int   nrow,
   int   biForm);

void BMP24SetRow(
   unsigned char *linebuf,
   int   col,
   int   r,
   int   g,
   int   b,
   int   biForm
);
void BMP24SetRow(
   unsigned char *linebuf,
   int   col,
   COLORREF rgb,
   int   biForm
);
size_t BMP24WriteRow(
   unsigned char *buf,
   int ncol,
   FILE *stream, int biForm
);

int BMP24CloseStream(
   FILE *f);


FILE *BMP24CreateReadStream (
   CString name,
   int  * ncol,
   int  * nrow,
   int  *biForm);

unsigned char *BMP24CreateLineBuffer(
   int ncol, int biForm);

size_t BMP24ReadRow(
   unsigned char *buf,
   int ncol,
   FILE *stream, int biForm
);

