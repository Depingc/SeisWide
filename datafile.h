/* TYPEDEFS */
typedef struct {	/* datafile - trace identification header */

	short t0;	/*  */
	short blocksize;	/* blocksize of each record */
	short numrecs;	/* num of total records in a file */
	short t3;	/*  */
	short t4;	/*  */
	short datlen;	/* length of data in bytes */
	short numchan;  /* num of channels */
	short unass[249];	/* short numbered at 7-255 */
} datafile;


