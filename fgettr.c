/* Copyright (c) Colorado School of Mines, 2002.*/
/* All rights reserved.                       */

/* FGETTR: $Revision: 1.40 $; $Date: 2002/03/15 18:16:01 $	*/

/*********************** self documentation **********************/
/****************************************************************************
FGETTR - Routines to get an SU trace from a file 

fgettr		get a fixed-length segy trace from a file by file pointer
fvgettr		get a variable-length segy trace from a file by file pointer
fgettra		get a fixed-length trace from disk file by trace number
gettr		macro using fgettr to get a trace from stdin
vgettr		macro using vfgettr to get a trace from stdin
gettra		macro using fgettra to get a trace from stdin by trace number
 
*****************************************************************************
Function Prototype:
int fgettr(FILE *fp, segy *tp);
int fvgettr(FILE *fp, segy *tp);
int fgettra(FILE *fp, segy *tp, int itr);

*****************************************************************************
Returns:
fgettr, fvgettr:
int: number of bytes read on current trace (0 after last trace)

fgettra:
int: number of traces in disk file
 
Macros defined in segy.h
#define gettr(x)	fgettr(stdin, (x))
#define vgettr(x)	fgettr(stdin, (x))

Usage example:
 	segy tr;
 	...
 	while (gettr(&tr)) {
 		tr.offset = abs(tr.offset);
 		puttr(&tr);
 	}
 	...

*****************************************************************************
Authors: SEP: Einar Kjartansson, Stew Levin CWP: Shuki Ronen, Jack Cohen
****************************************************************************/
/*
 * Revised: 7/2/95 Stewart A. Levin   Mobil
 *     Major rewrite:  Use xdr library for portable su output file
 *     format.   Merge fgettr and fgettra into same source file.
 *     Make input from multiple streams work (at long last!).
 * Revised: 11/22/95 Stewart A. Levin  Mobil
 *     Always set ntr for DISK input.  This fixes susort failure.
 * Revised: 1/9/96  jkc CWP
 *     Set lastfp on nread <=0 return, too.
 */
/**************** end self doc ********************************/


#ifdef SUXDR
/********************************************
code using XDR
*********************************************/

#include "su_xdr.h"
#include "header.h"

static struct insegyinfo {
	FILE *infp;		     /* FILE * ptr for search	 */
	struct insegyinfo *nextinfo; /* linked list pointer      */
	unsigned long itr;	     /* number of traces read	 */
	int nsfirst;		     /* samples from 1st header	 */
	unsigned short bytesper;  /* bytes per datum		 */
	int nsegy; 		     /* segy bytes from nsfirst	 */
	int ntr;                     /* traces in input,if known */
	FileType ftype;		     /* file type of input *fp	 */
	XDR *segy_xdr;		     /* allocated XDR structure  */
} *insegylist = (struct insegyinfo *) NULL;

static FILE *lastfp = (FILE *) NULL;
static struct insegyinfo *infoptr, **oldinfoptr;

static
void searchlist(FILE *fp)
{
	oldinfoptr = &insegylist;
	for(infoptr = insegylist; infoptr != ((struct insegyinfo *) NULL);
	    infoptr = infoptr->nextinfo) {
		if(fp == infoptr->infp) break;
		oldinfoptr = &infoptr->nextinfo;
	}
}

static
int dataread(struct insegyinfo *iptr, segy *tp, cwp_Bool fixed_length)
{
	unsigned int nsread = fixed_length?iptr->nsfirst:tp->ns;
	unsigned int databytes = infoptr->bytesper*nsread;
	int nread;

	/* read trace data */
	switch(tp->trid) {
	case CHARPACK:
	case SHORTPACK:
		nread = efread((char *) (&((tp->data)[0])),1,databytes,
			       iptr->infp);
	break;
	default:
		if(FALSE == xdr_vector(iptr->segy_xdr,
				       (char *) (&((tp->data)[0])),
				       nsread,sizeof(float),(xdrproc_t) xdr_float))
			nread = 0;
		else
			nread = databytes;
	break;
	}
	
	if(nread > 0 && nread != databytes) 
		err("%s: on trace #%ld, tried to read %d bytes, "
		    "read %d bytes",
		    __FILE__, (infoptr->itr)+1, databytes, nread);
	
	return(nread);
}


static
int fgettr_internal(FILE *fp, segy *tp, cwp_Bool fixed_length)
{
	unsigned short bytesper;
	unsigned int startpos;	/* xdr stream offset */
	int nread;		/* bytes seen by fread calls	*/

	if(fp != lastfp)  /* search linked list for possible alternative */
			    searchlist(fp);

	if (infoptr == ((struct insegyinfo *) NULL)) {
	/* initialize new segy input stream */
		unsigned int databytes;	/* bytes from nsfirst		*/

		/* allocate new segy input information table */
		*oldinfoptr = (struct insegyinfo *)
			malloc(sizeof(struct insegyinfo));
		infoptr = *oldinfoptr;
		infoptr->nextinfo = (struct insegyinfo *) NULL;
		/* save FILE * ptr */
		infoptr->infp = fp;
		infoptr->itr = 0;
		infoptr->ntr = -1;
		/* allocate XDR struct and associate FILE * ptr */
		infoptr->segy_xdr = (XDR *) malloc(sizeof(XDR));

		switch (infoptr->ftype = filestat(fileno(fp))) {
		case DIRECTORY:
			err("%s: segy input can't be a directory", __FILE__);
		case TTY:
			err("%s: segy input can't be tty", __FILE__);
		break;
		default: /* the rest are ok */
		break;
		}
		xdrstdio_create(infoptr->segy_xdr,fp,XDR_DECODE);
		startpos = xdr_getpos(infoptr->segy_xdr);

		/* retrieve segy trace header */
		if(FALSE == xdrhdrsub(infoptr->segy_xdr,tp))
			err("%s: bad first header", __FILE__);
		
		/* Have the header, now for the data */
		infoptr->nsfirst = tp->ns;
		if (infoptr->nsfirst > SU_NFLTS)
			err("%s: unable to handle %d > %d samples per trace",
				    __FILE__, infoptr->nsfirst, SU_NFLTS);

		switch(tp->trid) {
		case CHARPACK:
		   infoptr->bytesper = sizeof(char); break;
		case SHORTPACK:
		   infoptr->bytesper = 2*sizeof(char); break;
		default:
		   infoptr->bytesper = BYTES_PER_XDR_UNIT; break;
		}

		databytes = infoptr->bytesper * tp->ns;

		infoptr->nsegy = xdr_getpos(infoptr->segy_xdr) +
						databytes - startpos;

		nread = dataread(infoptr, tp, fixed_length);

		switch (nread) {
		case 0:   err("%s: no data on first trace", __FILE__);
		default:  if (nread != databytes)
				 err("%s: first trace: tried to read %d bytes "
				     "read %d bytes",
				      __FILE__, databytes, nread);
			else nread += HDRBYTES;
		}

		if(infoptr->ftype == DISK) { /* compute ntr */
		    startpos = xdr_getpos(infoptr->segy_xdr);
		    efseeko(fp,(off_t) 0,SEEK_END);
		    infoptr->ntr = eftello(fp)/infoptr->nsegy;
		    if(FALSE == xdr_setpos(infoptr->segy_xdr,startpos))
			err("%s: DISK reposition failure",__FILE__);
		    }

	} else { /* Not first entry */

                startpos = xdr_getpos(infoptr->segy_xdr);
                if (FALSE == xdrhdrsub(infoptr->segy_xdr,tp)) nread=0;
		else nread = xdr_getpos(infoptr->segy_xdr)-startpos;
		nread += dataread(infoptr, tp, fixed_length);
		if (nread <= 0) {
			lastfp = infoptr->infp;
			return 0;
		}

		if (fixed_length && (tp->ns != infoptr->nsfirst))
			err("%s: on trace #%ld, "
			    "number of samples in header (%d) "
			    "differs from number for first trace (%d)", 
			     __FILE__, infoptr->itr, tp->ns, infoptr->nsfirst);
	}

	++(infoptr->itr);
	lastfp = infoptr->infp;
	return (nread);
}

int fgettr(FILE *fp, segy *tp)
{
 return(fgettr_internal(fp,tp,cwp_true));
}

int fvgettr(FILE *fp, segy *tp)
{
 return(fgettr_internal(fp,tp,cwp_false));
}
int fgettra(FILE *fp, segy *tp, int itr)
{
 int nread;
 if(lastfp != fp) /* search for match */
		     searchlist(fp);

 if(infoptr == (struct insegyinfo *) NULL) {
      /* get first trace */
      if(0 >= fgettr(fp, tp)) return(0); /* error return */

      switch(infoptr->ftype) {
	  case TTY:
	    warn("stdin not redirected");
	    break;
	  case DISK: /* correct */
	    break;
	  default:
	    err("%s: input must be disk file",__FILE__);
	    break;
	  }

      efseeko(fp,(off_t) 0,SEEK_END);
      infoptr->ntr = eftello(fp)/infoptr->nsegy;
      } /* end first entry initialization */

 /* Check on requested trace number */
  if(itr >= infoptr->ntr) err("%s: trying to read off end of file",__FILE__);

 /* Position file pointer at start of requested trace */
  if(FALSE == xdr_setpos(infoptr->segy_xdr, itr*infoptr->nsegy))
	err("%s: unable to seek xdr disk file to trace %d",__FILE__,itr);

 nread=fgettr(fp, tp);
 if(nread != infoptr->nsegy)
	err("%s: read %d bytes with %d bytes in trace",
		__FILE__,nread,infoptr->nsegy);

 if(tp->ns != infoptr->nsfirst)
       warn("%s: header ns field = %d differs from first trace = %d",
		__FILE__,tp->ns,infoptr->nsfirst);

 return(infoptr->ntr);
}


#else 
/**********************************************************
code without  XDR 
***********************************************************/

#include "su.h"
#include "segy.h"
#include "header.h"

static struct insegyinfo {
	FILE *infp;                  /* FILE * ptr for search	 */
	struct insegyinfo *nextinfo; /* linked list pointer      */
	unsigned long itr;	     /* number of traces read	 */
	int nsfirst;		     /* samples from 1st header	 */
	unsigned short bytesper;     /* bytes per datum		 */
	int nsegy; 		     /* segy bytes from nsfirst	 */
	int ntr;                     /* traces in input,if known */
	FileType ftype;		     /* file type of input *fp	 */
} *insegylist = (struct insegyinfo *) NULL;

static FILE *lastfp = (FILE *) NULL;
static struct insegyinfo *infoptr, **oldinfoptr;

static
void searchlist(FILE *fp)
{
	oldinfoptr = &insegylist;
	for(infoptr = insegylist; infoptr != ((struct insegyinfo *) NULL);
	    infoptr = infoptr->nextinfo) {
		if(fp == infoptr->infp) break;
		oldinfoptr = &infoptr->nextinfo;
	}
}

static
int dataread(segy *tp, struct insegyinfo *iptr, cwp_Bool fixed_length)
{
	unsigned int nsread = fixed_length?iptr->nsfirst:tp->ns;
	unsigned int databytes = infoptr->bytesper*nsread;
	int nread = (int) efread((char *) (&((tp->data)[0])),1, databytes,
			   iptr->infp);

	if(nread > 0 && nread != databytes) 
		err("%s: on trace #%ld, tried to read %d bytes, "
		    "read %d bytes ",
		    __FILE__, (infoptr->itr)+1, databytes, nread);

	return(nread);
}


static
int fgettr_internal(FILE *fp, segy *tp, cwp_Bool fixed_length)
{
	int nread;	/* bytes seen by fread calls	*/

	/* search linked list for possible alternative */
	if(fp != lastfp)  searchlist(fp);

	if (infoptr == ((struct insegyinfo *) NULL)) {
		/* initialize new segy input stream */
		unsigned int databytes;	/* bytes from nsfirst	*/

		/* allocate new segy input information table */
		*oldinfoptr = (struct insegyinfo *)
			malloc(sizeof(struct insegyinfo));
		infoptr = *oldinfoptr;
		infoptr->nextinfo = (struct insegyinfo *) NULL;
		infoptr->infp = fp;  /* save FILE * ptr */
		infoptr->itr = 0;
		infoptr->ntr = -1;
		
		switch (infoptr->ftype = filestat(fileno(fp))) {
		case DIRECTORY:
			err("%s: segy input can't be a directory", __FILE__);
		case TTY:
			err("%s: segy input can't be tty", __FILE__);
		default:
			/* all others are ok */
		break;
		}

		/* Get the header */
		switch (nread = (int) efread(tp, 1, HDRBYTES, infoptr->infp)) {
		case 0:   return 0; /* no traces; trap in mains */
		default:  if (nread != HDRBYTES)
				err("%s: bad first header", __FILE__);
		}		

		/* Have the header, now for the data */
		infoptr->nsfirst = tp->ns;
		if (infoptr->nsfirst > SU_NFLTS)
			err("%s: unable to handle %d > %d samples per trace",
			    __FILE__, infoptr->nsfirst, SU_NFLTS);

		switch (tp->trid) {
		case CHARPACK:
			infoptr->bytesper = sizeof(char); break;
		case SHORTPACK:
			infoptr->bytesper = 2*sizeof(char); break;
		default:
			infoptr->bytesper = sizeof(float); break;
		}

		databytes = infoptr->bytesper * tp->ns;

		infoptr->nsegy = HDRBYTES + databytes;

		/* Inconvenient to bump nread here; do it in the switch */
		nread = dataread(tp, infoptr, fixed_length);

		switch (nread) {
		case 0:   err("%s: no data on first trace", __FILE__);
		default:  if (nread != databytes)
				err("%s: first trace: "
				    "read only %d bytes of %u",
				    __FILE__, nread, databytes);
			  else nread += HDRBYTES;
		}

		if (infoptr->ftype == DISK) { /* compute ntr */
			efseeko(fp, (off_t) 0,SEEK_END);
			infoptr->ntr = (int)(eftello(fp)/infoptr->nsegy);
			efseeko(fp, (off_t) infoptr->nsegy,SEEK_SET);
							/* reset fp */
	        }


	} else {		/* Not first entry */
		switch (nread = (int) efread(tp, 1, HDRBYTES, infoptr->infp)) {
		case 0: lastfp = infoptr->infp;
			return 0; /* finished */
		default:  if (nread != HDRBYTES)
				err("%s: on trace #%ld read %d bytes ",
				    "expected %d bytes",
				    __FILE__,(infoptr->itr)+1,nread,HDRBYTES);
		}

                nread += dataread(tp, infoptr, fixed_length);

		if (fixed_length && (tp->ns != infoptr->nsfirst))
			err("%s: on trace #%ld, "
			    "number of samples in header (%d) "
			    "differs from number for first trace (%d)", 
			    __FILE__, (infoptr->itr)+1, tp->ns,
			    infoptr->nsfirst);
	}

	++(infoptr->itr);
	lastfp = infoptr->infp;
	return (nread);
}

int fgettr(FILE *fp, segy *tp)
{
	return(fgettr_internal(fp,tp,cwp_true));
}

int fvgettr(FILE *fp, segy *tp)
{
	return(fgettr_internal(fp,tp,cwp_false));
}

int fgettra(FILE *fp, segy *tp, int itr)
{
	int nread;
	if(lastfp != fp)  searchlist(fp);  /* search for match */
		
	
	if(infoptr == (struct insegyinfo *) NULL) {
		/* get first trace */
		if(0 >= fgettr(fp, tp)) return(0); /* error return */

		switch(infoptr->ftype) {
		case TTY:
			warn("stdin not redirected");
		break;
		case DISK:	/* correct */
		break;
		default:
			err("%s: input must be disk file",__FILE__);
		}
      
		efseeko(fp,(off_t) 0,SEEK_END);
		infoptr->ntr = (int) (eftello(fp)/infoptr->nsegy);
	} /* end first entry initialization */
	
	/* Check on requested trace number */
	if(itr >= infoptr->ntr)
		err("%s: trying to read off end of file",__FILE__);
	
	/* Position file pointer at start of requested trace */
	efseeko(fp, (off_t) itr*infoptr->nsegy, SEEK_SET);
	
	nread=fgettr(fp, tp); /* let fgettr do the work */
	if(nread != infoptr->nsegy)
		err("%s: read %d bytes in trace of %d bytes",
		    __FILE__,nread,infoptr->nsegy);
	
	if(tp->ns != infoptr->nsfirst)
		warn("%s: header ns field = %d differs from first trace = %d",
		     __FILE__,tp->ns,infoptr->nsfirst);
	
	return(infoptr->ntr);
}

#endif /* end of XDR choice */
 

/*
#ifdef TEST

char *sdoc[] = {
"								",
" tgettr <stdin >stdout						",
"								",
" 	Test harness for gettr.c				",
"	Changes cdp to abs(cdp)					",
"	Contrast the following results:	 			",
"	suplane offset=-100 | sugethw offset 			",
"	suplane offset=-100 | tgettr | sugethw offset		",
"								",
NULL};

segy tr;

main(int argc, char **argv)
{
	initargs(argc, argv);
	requestdoc(1);

 	while (gettr(&tr)) {
 		tr.offset = abs(tr.offset);
 		puttr(&tr);
 	}

	return EXIT_SUCCESS;
}
#endif

*/