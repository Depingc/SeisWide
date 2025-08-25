#pragma once
#include "ealloc_d.h"
#include "suSwapByte.h"

/* SeisWide Version: 
* Copyright (c) Colorado School of Mines, 1996.*/
/* All rights reserved.                       */
/* Copyright (c) Colorado School of Mines, 1994.*/
/* All rights reserved.                       */
/* segy.h - include file for SEGY traces
 *
 * declarations for:
 *	typedef struct {} segy - the trace identification header
 *	typedef struct {} bhed - binary header
 *
 * Note:
 *	If header words are added, run the makefile in this directory
 *	to recreate hdr.h.
 *
 * Reference:
 *	K. M. Barry, D. A. Cavers and C. W. Kneale, "Special Report:
 *		Recommended Standards for Digital Tape Formats",
 *		Geophysics, vol. 40, no. 2 (April 1975), P. 344-352.
 *	
 * $Author: jkc $
 * $Source: /usr/local/cwp/src/su/include/RCS/segy.h,v $
 * $Revision: 1.20 $ ; $Date: 1996/02/20 16:41:29 $
 */ 
#ifndef SEGY_H
#define SEGY_H
// #define SU_NFLTS	22768	/* Arbitrary limit on data array size	*/
#define SU_NFLTS	65535	/* Arbitrary limit on data array size	*/
#define ERRORVAL -99999.f

inline BOOL IsUnix(short a) {
	//	return a!=256 && !(a >= 0 && a < 6); 
	return !(a >= 0 && a < 6);
	// here if the headers are all zeros (an abnormal situation), I think it should be PC format
}

inline BOOL IsFormatValid(short a) {
	return !IsUnix(a);
	// a==256 || (a >= 0 && a < 6);  // here if the headers are all zeros (an abnormal situation), I think it should be PC format
}

static BOOL gIsZplotFormat = FALSE;  // not used????

/* TYPEDEFS */


	/*  This HEAD struct is useful for the currently active SEGY file. These numbers are
		saved as global so that they can be used many times without having to read them
		again and again.

		all parameters in this struct can be cached to file, so that they can be shared across many .sei files
		plan: cache the struct HEAD in seiswide\\cache\ , under a file name calculated as follows:
		    1. strip drive letter, such as  E:\\
			2. base64 the path\\name.sgy, restrict to pure ascii letters
	*/
	struct HEAD{
		CString secName; // if this secName is different from the draw.timeSecName, the HEAD is for a different segy
		CTime modifiedtime; // this is segy file modified time . If the file is changed by other process, it will be different
							// so it is easily detected
			// info will be retrieved, to save CPU time
 		// for extracting extra meta-data for the segy...added aug. 26, 2010
		double tminFile;  // useful info, storing segy header info
		double tmaxFile;  // useful info
		double offsetIntervalFile;
		double distIntervalFile;

		double aveAmp;	//average amp; 
		double maxAmp;
		
		// bool isTraceNumAllZero; // when all traces have zero trace number, this gives a warning
			// but HEAD struct will populate traces[] with sequencial numbers starting with 1

		// these 3 lines are redundant, because each time you load a segy file, they are inside its header, and should be obtained there
		// but for jp2 format or bitmap files, these 3 lines are important in properly displaying
		bool isUnixFile;
		short formatFile; // should be 1,2,3,4,5 or 0 for segy files, not applicable for JP2
			// if it is 6, it is RBG data
		//bool isDataRgb; // indicate if the data value is already RGB value
		//				// if false, it is in amplitude values and will need to be converted to
		//				// RGB before displaying 
		//double img_transform_gain;  // not used yet
		//double img_transform_offset;  // not used yet
		double xshotInSegy;

		double dtFile; // in sec
		unsigned short numsampFiles;  // to be accurate, this cannot exceed the max of unsigned short

		int numTraces;
		bool m_isUseSeqTraceNumber; // if true, traces[] stores sequencial trace numbers instead of readings from tracl field
		//bool m_isFloatSwappedIEEE; // older Dalhousie segy seems all not using IBM floats, so this para is very important and must be auto detected!
		unsigned char ascHead[3200];
		ULONGLONG *pDataArr;
		int *traces;
		float *tminArr;
		float *dist;
		float *offsets;
		float *sx;
		float *sy;
		int *cdp;
		int *cdpt;
		int *ffid;
		// float *timeStatic;  
		/* trace static time, such as correction for horizon flatten processing;
						   flattening will not change data at all, only change starting time.
						   When reading into headRead struct, it will be considered. After read, it will 
						   not be considered any more. After finished reading from file, please free its 
						   memory and set it to NULL, so that it is not effective any more.
						   So I will need to set it too zero unless I set it on purpose.*/
		inline const bool isStartTimeChangeDrastic() {
			if (numTraces < 2 || numsampFiles < 2) return false;
			
			// now I think it's better to read the whole segy file
			// if here I return false, Seiswide will try to read partial data. But if
			// there are some traces with large time delay, they will be cut off at the end
			// of the display. So I think it's better to load the entire data.

			//double halfLen = (numsampFiles - 1)*dtFile * .5;
			for (int i = 1; i < numTraces; i++) {
				if (fabs((double)tminArr[i] - (double)tminArr[i-1]) > 0.001) return true;
			}
			return false;
		}

		inline const int traceMin() {
			int iOut = 999999;
			for (int i = 0; i < numTraces; i++) {
				if (iOut > traces[i]) iOut = traces[i];
			}
			return iOut;
		}
		inline const int traceMax() {
			int iOut = 0;
			for (int i = 0; i < numTraces; i++) {
				if (iOut < traces[i]) iOut = traces[i];
			}
			return iOut;
		}
		inline const float offsetMin() {
			float out = 999999.f;
			for (int i = 0; i < numTraces; i++) {
				if (out > offsets[i]) out = offsets[i];
			}
			return out;
		}
		inline const float offsetMax() {
			float out = -999999.f;
			for (int i = 0; i < numTraces; i++) {
				if (out < offsets[i]) out = offsets[i];
			}
			return out;
		}

		inline const double offsetMaxAbs() {
			return max(fabs(offsetMax()), fabs(offsetMin()));
		}

		inline const bool isPrestackShotGather() {
			int i;

			// there is a ARTA case: All traces have FFID=1, CDP==0, CDPT>0.
			bool isSameFFID = true;
			for (i = 1; i < numTraces; i++) {
				if (ffid[i] != ffid[i - 1]) {
					isSameFFID = false;
					break;
				}
			}
			if (isSameFFID) return false; // this wide-angle data is not treated as pre-stack, although single shot without reflection stacking


			// most post-stack data will have cdpt all zeros.
			for (i = 1; i < numTraces; i++) {
				if (ffid[i]==ffid[i-1] && cdp[i]>100 && cdpt[i] > 3) {
					return true;
				}
			}
			return false;
		}

		inline const bool isTraceNumberIrregular()
		{
			bool isIrregular = false;
			for (int i = 0; i<numTraces; i++) {
				if (traces[i] < 0) {
					isIrregular = true;
					break;
				}
				else if (i > 0 && traces[i] != traces[i - 1] + 1) {
					isIrregular = true;
					break;
				}
			}

			return isIrregular;
		}

		inline const void conditionIrregularTraceNum() {
			if (isTraceNumberIrregular()) {
				int nFirst = traces[0];
				if (nFirst < 1 || nFirst > 40000)
					nFirst = 1;

				for (int i = 0; i<numTraces; i++)
					traces[i] = i + nFirst;

				m_isUseSeqTraceNumber = true;
			}
		}

		inline const float getOffsetFromTrace(int trace) {
			for (int i = 0; i < numTraces; i++) {
				if (traces[i] == trace) {
					return offsets[i];
				}
			}
			return 0.f;
		}

		// frequently, the distance array head->dist[i] contains no information. We need a proper markup for this
		inline const bool isDistArrayValid() {
			if (numTraces > 0) {
				if (numTraces>2 &&
					dist[0] == dist[1] &&
					dist[1] == dist[2]) return false;

				if (numTraces>2 &&
					(dist[0]<-99999. || dist[0]>99999. ||
					dist[1]<-99999. || dist[1]>99999. ||
					dist[2]<-99999. || dist[2]>99999.)
					) return false;
			}
			return true;
		}

		//inline void setAscHead(CString ascHeadc) {
		//	//CString ascHeadc = CA2T(ascHead2);
		//	ZeroMemory(ascHead, 3200);
		//	strcpy_s((char *)ascHead, 3200, CT2A(ascHeadc));
		//}

		inline void setAscHead(CString ascHeadc) {
			ZeroMemory(ascHead, 3200);
			strcpy_s((char *)ascHead, 3200, CT2A(ascHeadc));
		}

		// this func is not tested as in below
		inline void setAscHead(CString ascHeadc, int lenBytes) {
			if (lenBytes > 3200) lenBytes = 3200; // no more than 3200 bytes
			// ZeroMemory(ascHead, 3200);
			//char *c = CT2A(ascHeadc);
			//strcpy_s((char *)ascHead, lenBytes, CStringA(ascHeadc).GetString());
			memcpy(ascHead, CT2A(ascHeadc), lenBytes);  // We leave the rest of ascii strings untouched by memcpy() only partial arrays
		}

		inline const bool isSegyTaupDomain() {
			char b[99] = "XX8Slowness";
			return strncmp((char *)ascHead, b, strlen(b)) == 0;
			//return (ascHead[0] == 'X' &&
			//	ascHead[1] == 'X' &&
			//	ascHead[2] == '8' &&
			//	ascHead[3] == 'S' &&
			//	ascHead[4] == 'l' &&
			//	ascHead[5] == 'o' &&
			//	ascHead[6] == 'w' &&
			//	ascHead[7] == 'n' &&
			//	ascHead[8] == 'e' &&
			//	ascHead[9] == 's' &&
			//	ascHead[10] == 's'
			//	);
		}

		inline const bool isSegySpectrumDomain() {
			char b[99] = "XX8Spectrum";
			return strncmp((char *)ascHead, b, strlen(b)) == 0;
			//return (ascHead[0] == 'X' &&
			//	ascHead[1] == 'X' &&
			//	ascHead[2] == '8' &&
			//	ascHead[3] == 'S' &&
			//	ascHead[4] == 'p' &&
			//	ascHead[5] == 'e' &&
			//	ascHead[6] == 'c' &&
			//	ascHead[7] == 't' &&
			//	ascHead[8] == 'r' &&
			//	ascHead[9] == 'u' &&
			//	ascHead[10] == 'm'
			//	);
		}

		inline const bool isSegyBitmap() {
			char b[99] = "XX6BITMAP";
			return strncmp((char *)ascHead, b, strlen(b)) == 0;
			//return (ascHead[0] == 'X' &&
			//	ascHead[1] == 'X' &&
			//	ascHead[2] == '6' &&
			//	ascHead[3] == 'B' &&
			//	ascHead[4] == 'I' &&
			//	ascHead[5] == 'T' &&
			//	ascHead[6] == 'M' &&
			//	ascHead[7] == 'A' &&
			//	ascHead[8] == 'P'
			//	);
		}

		inline const bool isSegyBitmapC() {
			char b[99] = "XX7BITMAPC";
			return strncmp((char *)ascHead, b, strlen(b)) == 0;
			//return (ascHead[0] == 'X' &&
			//	ascHead[1] == 'X' &&
			//	ascHead[2] == '7' &&
			//	ascHead[3] == 'B' &&
			//	ascHead[4] == 'I' &&
			//	ascHead[5] == 'T' &&
			//	ascHead[6] == 'M' &&
			//	ascHead[7] == 'A' &&
			//	ascHead[8] == 'P' &&
			//	ascHead[8] == 'C'
			//	);
		}

		inline const bool isSegyDepthDomain() {
			char b[99] = "XX5Depth";
			return strncmp((char *)ascHead, b, strlen(b)) == 0;
			//return (ascHead[0] == 'X' &&
			//	ascHead[1] == 'X' &&
			//	ascHead[2] == '5' &&
			//	ascHead[3] == 'D' &&
			//	ascHead[4] == 'e' &&
			//	ascHead[5] == 'p' &&
			//	ascHead[6] == 't' &&
			//	ascHead[7] == 'h'
			//	);
		}
		HEAD() : offsetIntervalFile(.1), distIntervalFile(.1), aveAmp(-1.), maxAmp(-1.), 
			numTraces(0), pDataArr(NULL), traces(NULL), tminArr(NULL), 
			dist(NULL), offsets(NULL), cdp(NULL), cdpt(NULL), ffid(NULL), sx(NULL), sy(NULL),
			tminFile(0.), tmaxFile(5.), isUnixFile(false), m_isUseSeqTraceNumber(false)
		{
			xshotInSegy = ERRORVAL;
		};
	};



	//float d1;	/* 180: sample spacing for non-seismic data i2buf[90-91]; i4int[45]*/
	//float f1;	/* 184: First depth sample in meter  i2buf[92-93]; i4int[46]*/
	//float d2;	/* 188: sample spacing between traces i2buf[94-95]; i4int[47] */
	union CMP {
		float df[3];
		byte b[12];
		inline int* getInt1() {
			int *v = (int*)&b[1];
			return v;
		}
		inline int* getInt2() {
			return (int*)&b[5];
		}

		// only the first int is needed
		inline void setInt1(int anInt) {
			memset(&b[0], 0, 12);
			memcpy(&b[1], &anInt, sizeof(int));
		}
	};


typedef struct SEGYDYNAMIC {	/* SEGYDYNAMIC - trace identification header */
#pragma pack(push, 1) // note: this is required for .net
	int tracl;	/* 0 trace sequence number within line */
	int tracr;	/* 4 trace sequence number within reel */
	int fldr;	/* 8 field record number  or FFID */
	int tracf;	/* 12 trace number within field record */
	int ep;	/* 16 energy source point number */
	int cdp;	/* 20 CDP ensemble number */
	int cdpt;	/* 24 trace number within CDP ensemble */
	short trid;	/* 28 trace identification code: i2buf[14]
			1 = seismic data
			2 = dead
			3 = dummy
			4 = time break
			5 = uphole
			6 = sweep
			7 = timing
			8 = water break
			9---, N = optional use (N = 32,767)
			Following are CWP id flags:
			 9 = autocorrelation
			10 = Fourier transformed - no packing
			     xr[0],xi[0], ..., xr[N-1],xi[N-1]
			11 = Fourier transformed - unpacked Nyquist
			     xr[0],xi[0],...,xr[N/2],xi[N/2]
			12 = Fourier transformed - packed Nyquist
	 		     even N:
			     xr[0],xr[N/2],xr[1],xi[1], ...,
				xr[N/2 -1],xi[N/2 -1]
				(note the exceptional second entry)
			     odd N:
			     xr[0],xr[(N-1)/2],xr[1],xi[1], ...,
				xr[(N-1)/2 -1],xi[(N-1)/2 -1],xi[(N-1)/2]
				(note the exceptional second & last entries)
			13 = Complex signal in the time domain
			     xr[0],xi[0], ..., xr[N-1],xi[N-1]
			14 = Fourier transformed - amplitude/phase
			     a[0],p[0], ..., a[N-1],p[N-1]
			15 = Complex time signal - amplitude/phase
			     a[0],p[0], ..., a[N-1],p[N-1]
			16 = Real part of complex trace from 0 to Nyquist
			17 = Imag part of complex trace from 0 to Nyquist
			18 = Amplitude of complex trace from 0 to Nyquist
			19 = Phase of complex trace from 0 to Nyquist
			21 = Wavenumber time domain (k-t)
			22 = Wavenumber frequency (k-omega)
			23 = Envelope of the complex time trace
			24 = Phase of the complex time trace
			25 = Frequency of the complex time trace
			30 = Depth-Range (z-x) traces
			101 = Seismic data packed to bytes (by supack1)
			
			102 = Seismic data packed to 2 bytes (by supack2)
			*/
	short nvs;	/* 30;  number of vertically summed traces (see vscode
			   in bhed structure) */
	short nhs;	/* number of horizontally summed traces (see vscode
			   in bhed structure) */
	short duse;	/* 34: data use:
				1 = production
				2 = test */
	int offset;	/* 36: distance from source point to receiver i2buf[18-19]
			   group (negative if opposite to direction
			   in which the line was shot) */
	int gelev;	/* 40: receiver group elevation from sea level
			   (above sea level is positive) */
	int selev;	/* 44: source elevation from sea level
			   (above sea level is positive) */
	int sdepth;	/* 48: source depth (positive) i2buf[24-25]*/
	int gdel;	/* 52 (checked): datum elevation at receiver group */
	int sdel;	/* 56: datum elevation at source */
	int swdep;	/* 60: water depth at source */
	int gwdep;	/* 64: water depth at receiver group */
	short scalel;	/* 68 checked: scale factor for previous 7 entries
			   with value plus or minus 10 to the
			   power 0, 1, 2, 3, or 4 (if positive,
			   multiply, if negative divide) */
	short scalco;	/* 70 checked: scale factor for next 4 entries
			   with value plus or minus 10 to the
			   power 0, 1, 2, 3, or 4 (if positive,
			   multiply, if negative divide) */
	int  sx;	/* 72: X source coordinate  latitude i2buf[36-37] */
	int  sy;	/* 76: Y source coordinate longitude*/
	int  gx;	/* 80: X group coordinate */
	int  gy;	/*84  checked: Y group coordinate   i2buf[42-43] */
	short counit;	/* 88: coordinate units code:
				for previous four entries
				1 = length (meters or feet)
				2 = seconds of arc (in this case, the
				X values are longitude and the Y values
				are latitude, a positive value designates
				the number of seconds east of Greenwich
				or north of the equator */
	short wevel;	/* 90: weathering velocity */
	short swevel;	/* 22: subweathering velocity */
	short sut;	/* 94: uphole time at source */
	short gut;	/* 96: uphole time at receiver group */
	short sstat;	/* 98: source static correction */
	short gstat;	/* 100: group static correction i2buf[50]*/
	short tstat;	/* 102; total static applied */
	short laga;	/* 104: lag time A, time in ms between end of 240-
			   byte trace identification header and time
			   break, positive if time break occurs after
			   end of header, time break is defined as
			   the initiation pulse which maybe recorded
			   on an auxiliary trace or as otherwise
			   specified by the recording system */
	short lagb;	/* 106 checked:   96: lag time B, time in ms between the time break
			   and the initiation time of the energy source,
			   may be positive or negative i2buf[53]*/
	short delrt;	/* 108: delay recording time, time in ms between
			   initiation time of energy source and time
			   when recording of data samples begins
			   (for deep water work if recording does not
			   start at zero time) i2buf[54]*/
	short muts;	/* 110: mute time--start */
	short mute;	/* 112: mute time--end */
	unsigned short ns;	/* 114 checked: number of samples in this trace */
	unsigned short dt;	/* 116: sample interval; in micro-seconds */
	short gain;	/* 118: gain type of field instruments code:
				1 = fixed
				2 = binary
				3 = floating point
				4 ---- N = optional use */
	short igc;	/* 120: instrument gain constant */
	short igi;	/* 122: instrument early or initial gain */
	short corr;	/* 124: correlated:
				1 = no
				2 = yes */
	short sfs;	/* 126: sweep frequency at start */
	short sfe;	/* 128: sweep frequency at end i2buf[64]*/
	short slen;	/* 130 checked: sweep length in ms */
	short styp;	/* 132: sweep type code:
				1 = linear
				2 = cos-squared
				3 = other */
	short stas;	/* 134: sweep trace length at start in ms */
	short stae;	/* 136: sweep trace length at end in ms */
	short tatyp;	/* 138: taper type: 1=linear, 2=cos^2, 3=other */
	short afilf;	/* 140: alias filter frequency if used */
	short afils;	/* 142: alias filter slope */
	short nofilf;	/* 144: notch filter frequency if used */
	short nofils;	/* 146: notch filter slope */
	short lcf;	/* 148: low cut frequency if used */
	short hcf;	/* 150: high cut frequncy if used */
	short lcs;	/* 152: low cut slope */
	short hcs;	/* 154: high cut slope */
	short year;	/* 156: year data recorded */
	short day;	/* 158: day of year */
	short hour;	/* 160 checked: hour of day (24 hour clock) i2buf[80]*/
	short minute;	/* 162: minute of hour */
	short sec;	/* 164: second of minute */
	short timbas;	/* 166: time basis code:
				1 = local
				2 = GMT
				3 = other 
				Now revised for storing mili-seconds of shot, to be consistent
				with GSCA single channel segy */
	short trwf;	/* 168: trace weighting factor, defined as 1/2^N
			   volts for the least sigificant bit */
	short grnors;	/* 170: geophone group number of roll switch
			   position one */
	short grnofr;	/* 172: geophone group number of trace one within
			   original field record */
	short grnlof;	/* 174: geophone group number of last trace within
			   original field record */
	short gaps;	/* 176: gap size (total number of groups dropped) */
	short otrav;	/* 178: overtravel taper code:
				1 = down (or behind)
				2 = up (or ahead) i2buf[89]*/
	/* local assignments */
	//float d1;	/* 180: sample spacing for non-seismic data i2buf[90-91]; i4int[45]*/
	//float f1;	/* 184: First depth sample in meter  i2buf[92-93]; i4int[46]*/
	//float d2;	/* 188: sample spacing between traces i2buf[94-95]; i4int[47] */
	CMP cmp; // includes bytes 180-183, 184-197, 188-191 for d1, f1, d2
		// or we can extract bytes 181-184 for CMP locations in m

	float f2;	/* 192: first trace location i2buf[96-97]; i4int[48]*/
	float ungpow;	/* 196: negative of power used for dynamic range compression i2buf[98-99]; i4int[49]*/
	float unscale;	/* 200: reciprocal of scaling factor to normalize range i2buf[100-101]; i4int[50]*/
	int ntr; 	/* 204: number of traces i2buf[102-103]; i4int[51]*/
	short mark;	/* 208: mark selected traces i2buf[104]*/
	int delrtEx;  /* 210: trace delay time in msec, to be added to delrt, by Deping! i2buf[105-106]*/
	int cdpbak;	/* 214: CDP ensemble number backup i2buf[107-108]*/
	int cdptbak; /* 218: trace number within CDP ensemble backup i2buf[109-110]*/

	/* Added 2 more variables May 28, 2008 */
	short idistopt;	/* 222: 9=use the next float for distance; 0=use CDP for distance; idistopt=8 means that IBM floating is used together with idistopt as distance. */
	float trcdist;  /* 224 checked: trace distance; used only if idistopt>=9 */
	float trcOffset;  /* 228 checked: trace offset; */

	char unass[8];	
			/* 216:  unassigned--NOTE: last entry causes 
			   a break in the word alignment, if we REALLY
			   want to maintain 240 bytes, the following
			   entry should be an odd number of short/UINT2
			   OR do the insertion above the "mark" keyword */
#pragma pack(pop)
	float*  data;

	inline float getTraceDelay(){
		// fix up first, since some programs ...
		if (delrtEx > 90000000.) delrtEx = 0;

		return (delrt + delrtEx)*.001f;
	}
	inline void setTraceDelayF(double timeDelay){
		// will access segy headers:    short tr.delrt, int tr.t1
		int iTimeDelay = timeDelay>0. ?
			(int)(timeDelay * 1000 + .5)
			:
			(int)(timeDelay * 1000 - .5);
		if (abs(iTimeDelay) < MAXSHORT) {
			// if the traditional unsigned short header can handle it, we use it
			delrt = iTimeDelay;
			delrtEx = 0;
		}
		else {
			// if it cannot handle, we use our own extended header
			delrt = 0;
			delrtEx = iTimeDelay;
		}

		if (abs(delrtEx) > 90000000) {
			delrtEx = 0;
		}
	}

	float const getOffset() {
//		return offset; // testing
		if (fabs(trcOffset) >= 1.f) trcOffset = 0.f;
		return (float)offset * 0.001f + trcOffset;
	}
	void setOffset(float f) {
		offset = (int)(f*1000.f);
		trcOffset = (f*1000.f - offset) * 0.001f;
	}
	inline const float getDistance(){
		if (idistopt == 8 || idistopt == 9)
			return (float)trcdist;
		else
			return (float)cdp * 0.001f;
	}
	inline void setDistance(float fDistKm){
		idistopt = 8; // by using 8 here, we indicate that we will use IBM floating data from now on.
		trcdist = fDistKm;
	}

	// Given a time, we will return an interpolated value.
	// if error, it will return zero.
	inline const float getTraceAmp(float t, float sampRate){
		float out = 0.f;
		if (sampRate <= 0.f || sampRate > 10.f || !data)
			return out;  // unknown situation to come here to cause exception
		if (sampRate > 0.0000001f) {
			float ai = (t - getTraceDelay()) / sampRate;
			int i = (int)ai;
			if (i == ns - 1) {
				out = data[i];
			}
			else if (i >= 0 && i < ns - 1) {
				out = data[i] + (float)(ai - i) * (data[i + 1] - data[i]);
			}
		} // if
		return out;
	}

	inline const float getTraceXVal(int drawmode, BOOL isDistance) {
		if (drawmode == 6) return (float)tracl;  // this is trace display
		return (!isDistance) ? getOffset() : getDistance();
	}

	SEGYDYNAMIC() : 
		data(NULL)     // I need to initialize it to be NULL to avoid memory problems. If not NULL, related destructors will need to free it.
	{};

} segydynamic;
/* A typical usage of segydynamic:

	segydynamic *trArr = (segydynamic *)malloc(sizeof(segydynamic)*nxmax);
	if (ERROR == trArr) return;
	for (j=0; j<nxmax; j++) {
		trArr[j]->data = (float *)malloc(sizeof(float)*numsamp);
		if (ERROR == trArr[j]->data) {
			// clean up before return
			for (j=0; j<nxmax; j++) {
				free(trArr[j]->data);
			}
			AfxMessageBox("Sorry, failed to allocate memory for loading all traces into memory.");
			return;
		}
	}

	NOTE: sizeof(segydynamic)  is a little greater than  240 !
*/

union DAT {
	float  data[SU_NFLTS];
	int  dataint[SU_NFLTS];
	short  datashort[SU_NFLTS*2];
	byte databyte[SU_NFLTS*4];
};


typedef struct segy {	/* segy - trace identification header */
#pragma pack(push, 1) // note: this is required for .net
	int tracl;	/* trace sequence number within line BYTE1-4 */
	int tracr;	/* trace sequence number within reel BYTE5-8 */
	int fldr;	/* field record number  or FFID   BYTE9-12*/
	int tracf;	/* trace number within field record  BYTE13-16 */
	int ep;	/* energy source point number  BYTE17-20 */
	int cdp;	/* CDP ensemble number  BYTE21-24*/
	int cdpt;	/* trace number within CDP ensemble  BYTE25-28 */
	short trid;	/* trace identification code: i2buf[14]  BYTE29-30
			1 = seismic data
			2 = dead
			3 = dummy
			4 = time break
			5 = uphole
			6 = sweep
			7 = timing
			8 = water break
			9---, N = optional use (N = 32,767)
			Following are CWP id flags:
			 9 = autocorrelation
			10 = Fourier transformed - no packing
			     xr[0],xi[0], ..., xr[N-1],xi[N-1]
			11 = Fourier transformed - unpacked Nyquist
			     xr[0],xi[0],...,xr[N/2],xi[N/2]
			12 = Fourier transformed - packed Nyquist
	 		     even N:
			     xr[0],xr[N/2],xr[1],xi[1], ...,
				xr[N/2 -1],xi[N/2 -1]
				(note the exceptional second entry)
			     odd N:
			     xr[0],xr[(N-1)/2],xr[1],xi[1], ...,
				xr[(N-1)/2 -1],xi[(N-1)/2 -1],xi[(N-1)/2]
				(note the exceptional second & last entries)
			13 = Complex signal in the time domain
			     xr[0],xi[0], ..., xr[N-1],xi[N-1]
			14 = Fourier transformed - amplitude/phase
			     a[0],p[0], ..., a[N-1],p[N-1]
			15 = Complex time signal - amplitude/phase
			     a[0],p[0], ..., a[N-1],p[N-1]
			16 = Real part of complex trace from 0 to Nyquist
			17 = Imag part of complex trace from 0 to Nyquist
			18 = Amplitude of complex trace from 0 to Nyquist
			19 = Phase of complex trace from 0 to Nyquist
			21 = Wavenumber time domain (k-t)
			22 = Wavenumber frequency (k-omega)
			23 = Envelope of the complex time trace
			24 = Phase of the complex time trace
			25 = Frequency of the complex time trace
			30 = Depth-Range (z-x) traces
			101 = Seismic data packed to bytes (by supack1)
			
			102 = Seismic data packed to 2 bytes (by supack2)
			*/
	short nvs;	/* 30;  number of vertically summed traces (see vscode
			   in bhed structure) */
	short nhs;	/* number of horizontally summed traces (see vscode
			   in bhed structure) */
	short duse;	/* 34: data use:
				1 = production
				2 = test */
	int offset;	/* 36: distance from source point to receiver i2buf[18-19]
			   group (negative if opposite to direction
			   in which the line was shot) */
	int gelev;	/* 40: receiver group elevation from sea level
			   (above sea level is positive) */
	int selev;	/* 44: source elevation from sea level
			   (above sea level is positive) */
	int sdepth;	/* 48: source depth (positive) i2buf[24-25]*/
	int gdel;	/* 52 (checked): datum elevation at receiver group */
	int sdel;	/* 56: datum elevation at source */
	int swdep;	/* 60: water depth at source */
	int gwdep;	/* 64: water depth at receiver group */
	short scalel;	/* 68 checked: scale factor for previous 7 entries
			   with value plus or minus 10 to the
			   power 0, 1, 2, 3, or 4 (if positive,
			   multiply, if negative divide) */
	short scalco;	/* 70 checked: scale factor for next 4 entries
			   with value plus or minus 10 to the
			   power 0, 1, 2, 3, or 4 (if positive,
			   multiply, if negative divide) */
	int  sx;	/* 72: X source coordinate  latitude i2buf[36-37] */
	int  sy;	/* 76: Y source coordinate longitude*/
	int  gx;	/* 80: X group coordinate */
	int  gy;	/*84  checked: Y group coordinate   i2buf[42-43] */
	short counit;	/* 88: coordinate units code:
				for previous four entries
				1 = length (meters or feet)
				2 = seconds of arc (in this case, the
				X values are longitude and the Y values
				are latitude, a positive value designates
				the number of seconds east of Greenwich
				or north of the equator */
	short wevel;	/* 90: weathering velocity */
	short swevel;	/* 22: subweathering velocity */
	short sut;	/* 94: uphole time at source */
	short gut;	/* 96: uphole time at receiver group */
	short sstat;	/* 98: source static correction */
	short gstat;	/* 100: group static correction i2buf[50]*/
	short tstat;	/* 102; total static applied */
	short laga;	/* 104: lag time A, time in ms between end of 240-
			   byte trace identification header and time
			   break, positive if time break occurs after
			   end of header, time break is defined as
			   the initiation pulse which maybe recorded
			   on an auxiliary trace or as otherwise
			   specified by the recording system */
	short lagb;	/* 106 checked:   96: lag time B, time in ms between the time break
			   and the initiation time of the energy source,
			   may be positive or negative i2buf[53]*/
	short delrt;	/* 108: delay recording time, time in ms between
			   initiation time of energy source and time
			   when recording of data samples begins
			   (for deep water work if recording does not
			   start at zero time) i2buf[54]*/
	short muts;	/* 110: mute time--start */
	short mute;	/* 112: mute time--end */
	unsigned short ns;	/* 114 checked: number of samples in this trace; i2buf[57] */
	unsigned short dt;	/* 116: sample interval; in micro-seconds; i2buf[58] */
	short gain;	/* 118: gain type of field instruments code:
				1 = fixed
				2 = binary
				3 = floating point
				4 ---- N = optional use ; i2buf[59]*/
	short igc;	/* 120: instrument gain constant ; i2buf[60]*/
	short igi;	/* 122: instrument early or initial gain */
	short corr;	/* 124: correlated:
				1 = no
				2 = yes */
	short sfs;	/* 126: sweep frequency at start */
	short sfe;	/* 128: sweep frequency at end i2buf[64]*/
	short slen;	/* 130 checked: sweep length in ms */
	short styp;	/* 132: sweep type code:
				1 = linear
				2 = cos-squared
				3 = other */
	short stas;	/* 134: sweep trace length at start in ms */
	short stae;	/* 136: sweep trace length at end in ms */
	short tatyp;	/* 138: taper type: 1=linear, 2=cos^2, 3=other */
	short afilf;	/* 140: alias filter frequency if used */
	short afils;	/* 142: alias filter slope */
	short nofilf;	/* 144: notch filter frequency if used */
	short nofils;	/* 146: notch filter slope */
	short lcf;	/* 148: low cut frequency if used */
	short hcf;	/* 150: high cut frequncy if used */
	short lcs;	/* 152: low cut slope */
	short hcs;	/* 154: high cut slope */
	short year;	/* 156: year data recorded */
	short day;	/* 158: day of year */
	short hour;	/* 160 checked: hour of day (24 hour clock) i2buf[80]*/
	short minute;	/* 162: minute of hour */
	short sec;	/* 164: second of minute */
	short timbas;	/* 166: time basis code:
				1 = local
				2 = GMT
				3 = other 
				Now revised for storing mili-seconds of shot, to be consistent
				with GSCA single channel segy */
	short trwf;	/* 168: trace weighting factor, defined as 1/2^N
			   volts for the least sigificant bit */
	short grnors;	/* 170: geophone group number of roll switch
			   position one */
	short grnofr;	/* 172: geophone group number of trace one within
			   original field record */
	short grnlof;	/* 174: geophone group number of last trace within
			   original field record */
	short gaps;	/* 176: gap size (total number of groups dropped) */
	short otrav;	/* 178: overtravel taper code:
				1 = down (or behind)
				2 = up (or ahead) i2buf[89]*/
	/* local assignments */
	//float d1;	/* 180: sample spacing for non-seismic data i2buf[90-91]; i4int[45]*/
	//float f1;	/* 184: First depth sample in meter  i2buf[92-93]; i4int[46]*/
	//float d2;	/* 188: sample spacing between traces i2buf[94-95]; i4int[47] */
	CMP cmp; // includes bytes 180-183, 184-197, 188-191 for d1, f1, d2
		// or we can extract bytes 181-184 for CMP locations in m (which is an integer)


	float f2;	/* 192: first trace location i2buf[96-97]; i4int[48]*/
	float ungpow;	/* 196: negative of power used for dynamic range compression i2buf[98-99]; i4int[49]*/
	float unscale;	/* 200: reciprocal of scaling factor to normalize range i2buf[100-101]; i4int[50]*/
	int ntr; 	/* 204: number of traces i2buf[102-103]; i4int[51]*/
	short mark;	/* 208: mark selected traces i2buf[104]*/
	int delrtEx;  /* 210: trace delay time in msec, to be added to delrt, by Deping! i2buf[105-106]*/
	int cdpbak;	/* 214: CDP ensemble number backup i2buf[107-108]; abandoned?*/
	int cdptbak; /* 218: trace number within CDP ensemble backup i2buf[109-110]; abandoned?*/

	/* Added 2 more variables May 28, 2008 */
	short idistopt;	/* 222: 9=use the next float for distance; 0=use CDP for distance; 
					idistopt=8 means that IBM floating is used together 
					with idistopt as distance.
					*/
	float trcdist;  /* 224 checked: trace distance; used only if idistopt=9 */
	float trcOffset;  /* 228 checked: decimal part of a trace offset; used only when its abs value is less than 1. Will be added to tr.offset */

	char unass[8];	
			/* 216:  unassigned--NOTE: last entry causes 
			   a break in the word alignment, if we REALLY
			   want to maintain 240 bytes, the following
			   entry should be an odd number of short/UINT2
			   OR do the insertion above the "mark" keyword */
#pragma pack(pop)
	 DAT dat; /* 240 checked */

	 inline const float getTraceDelay(){
		 // fix up first, since some programs ...
		 if (abs(delrtEx) > 260000) 
			 delrtEx = 0;  // here I assume 260 sec delay is the maximum
			// this is because some of my older segy has this field to be 262000, which is an invalid number

		 return (delrt + delrtEx)*.001f;
	 }

	 inline void setTraceDelayF(double timeDelay){
		 // will access segy headers:    short tr.delrt, int tr.t1
		 int iTimeDelay = timeDelay>0. ?  
			 (int)(timeDelay * 1000 + .5)
			 :
			 (int)(timeDelay * 1000 - .5);
		 if (abs(iTimeDelay) < MAXSHORT) {
			 // if the traditional unsigned short header can handle it, we use it
			 delrt = iTimeDelay;
			 delrtEx = 0;
		 }
		 else {
			 // if it cannot handle, we use our own extended header
			 delrt = 0;
			 delrtEx = iTimeDelay;
		 }

		 if (abs(delrtEx) > 90000000) {
			 delrtEx = 0;
		 }
	 }


	 inline const float getOffset() {
		 if (fabs(trcOffset) >= 1.f) trcOffset = 0.f;
		 return (float)offset * 0.001f + trcOffset;
	 }
	 inline void setOffset(float f) {
		 offset = (int)(f*1000.f);
		 trcOffset = (f*1000.f - offset) * 0.001f;
	 }

	 inline const float getDistance(){
		 if (idistopt == 8 || idistopt == 9)
			 return (float)trcdist;
		 else
			 return (float)cdp * 0.001f;
	 }

	 inline void setDistance(float fDistKm){
		 idistopt = 8; // by using 8 here, we indicate that we will use IBM floating data from now on.
		 trcdist = fDistKm;
	 }

	 inline void setDistancei(int distInMeter){
		 // note: here cdp is the actual distance in meter
		 idistopt = 8; // by using 8 here, we indicate that we will use IBM floating data from now on.
		 trcdist = (float)distInMeter*0.001f;
	 }
	 //void setDistance(segy* tr, double dDistKm){
		// tr->idistopt = 8; // by using 8 here, we indicate that we will use IBM floating data from now on.
		// tr->trcdist = (float)dDistKm;
	 //}

} segy;


//	short *i2buf;
//	i2buf = (short *) &tr;
//	float *trcdat;
//	trcdat = (float *) &i2buf[120];

//union TWO_FLOAT {
//	short hw[2];
//	float v;
//} fTwoFloat;

 
typedef struct bhed {	/* bhed - sectio binary header */
	int jobid;	/* i2buf[0-1] job identification number BYTE1-4 */
	int lino;	/* i2buf[2-3] line number (only one line per reel) BYTE5-8*/
	int reno;	/* i2buf[4-5] reel number BYTE9-12*/
	unsigned short ntrpr;	/* i2buf[6] number of data traces per record */
    short nart;	/* i2buf[7] number of auxiliary traces per record */
	unsigned short hdt; /* i2buf[8] sample interval in micro secs for this reel */
	unsigned short dto; /* i2buf[9] same for original field recording */
	unsigned short hns; /* i2buf[10] number of samples per trace for this reel */
	unsigned short nso; /* i2buf[11] same for original field recording */
	short format;	/* i2buf[12] data sample format code:
				1 = IBM floating point (4 bytes)
				2 = fixed point (4 bytes)
				3 = fixed point (2 bytes)
				4 = fixed point w/gain code (4 bytes) 
				5 = IEEE floating point (4 bytes) -- addition for segy1 specifications!

				Note: when segy data are read into memory, it's IEEE floating point. So naturally, all in-memory segy data have format=5
				but in disk files, oil industry uses IBM floating format=1. So most of segy files are in format=1
				*/
	short fold;	/* i2buf[13] CDP fold expected per CDP ensemble */
	short tsort;	/* i2buf[14] trace sorting code: 
				1 = as recorded (no sorting)
				2 = CDP ensemble
				3 = single fold continuous profile
				4 = horizontally stacked */
	short vscode;	/* i2buf[15] vertical sum code:
				1 = no sum
				2 = two sum ...
				N = N sum (N = 32,767) */
	short hsfs;	/* i2buf[16] sweep frequency at start */
	short hsfe;	/* i2buf[17] sweep frequency at end */
	short hslen;	/* i2buf[18] sweep length (ms) */
	short hstyp;	/* i2buf[19] sweep type code:
				1 = linear
				2 = parabolic
				3 = exponential
				4 = other */
	short schn;	/* i2buf[20] trace number of sweep channel */
	short hstas;	/* i2buf[21] sweep trace taper length at start if
			   tapered (the taper starts at zero time
			   and is effective for this length) */
	short hstae;	/* i2buf[22] sweep trace taper length at end (the ending
			   taper starts at sweep length minus the taper
			   length at end) */
//	float rv;  // reducing velocity  by Deping
	short htatyp;
		/* i2buf[23] sweep trace taper type code:  BYTE47-48
				1 = linear
				2 = cos-squared
				3 = other */
	short hcorr;
		/* i2buf[24] correlated data traces code:
				1 = no
				2 = yes */
	short bgrcv;	/* i2buf[25] binary gain recovered code:
				1 = yes
				2 = no */
	short rcvm;	/* i2buf[26] amplitude recovery method code:
				1 = none
				2 = spherical divergence
				3 = AGC
				4 = other */
	short mfeet;	/* i2buf[27] measurement system code:
				1 = meters
				2 = feet */
	short polyt;	/* i2buf[28] impulse signal polarity code:
				1 = increase in pressure or upward
				    geophone case movement gives
				    negative number on tape
				2 = increase in pressure or upward
				    geophone case movement gives
				    positive number on tape */
	short vpol;	/* i2buf[29] vibratory polarity code:
				code	seismic signal lags pilot by
				1	337.5 to  22.5 degrees
				2	 22.5 to  67.5 degrees
				3	 67.5 to 112.5 degrees
				4	112.5 to 157.5 degrees
				5	157.5 to 202.5 degrees
				6	202.5 to 247.5 degrees
				7	247.5 to 292.5 degrees
				8	293.5 to 337.5 degrees */

	/*
	The followings are for OBS refraction data usages:
				i2buf[31] = GunDel;
				i2buf[32] = offsetClockReset;
				i2buf[38] = refdayClockReset;
				i2buf[39] = refhrClockReset;
				i2buf[40] = refminClockReset;
				i2buf[41] = refsecClockReset;
*/

	short gunDelay;        // i2buf[30], C-bytes: 60-61
	short offsetClockReset;
	short refdayClockReset;
	short refhrClockReset;
	short refminClockReset;
	short refsecClockReset;   // i2buf[35], C-bytes: 70-71
	float lattCorr;   // i2buf[36-37], C-bytes: 72-75
	float longCorr;   // i2buf[38-39], C-bytes: 76-79
	float drftrate;   // i2buf[40-41], C-bytes: 80-83

	float obsPositionX;   // i2buf[42-43], C-bytes: 84-87
	
	int ntrExtra; // Deping's addition to add to ntrpr i2buf[44-45]
	
	short hunass[154];	/* unassigned */
	//inline float getReduceVel2006() {
	//	short i2buf[2];
	//	i2buf[0] = htatyp;
	//	i2buf[1] = hcorr;
	//	float *pf = (float *)&i2buf[0];
	//	float out = pf[0];
	//	if (out < 1.5f || out>9.f)
	//		out = 0.f;
	//	return out;
	//}

	inline int getTraceLenBytes() {
		unsigned short aNs = hns;
		if (IsUnix(format)) {
			swap_u_short_2(&aNs);
		}
		int len = (format != 3) ? aNs * 4 : aNs * 2;
		len += 240;
		return len;
	}

	inline int getnx() {
		if (ntrExtra < 0 || ntrExtra > 999999) 
			ntrExtra = 0;
		return ntrpr + ntrExtra;
	}

	inline void setnx(int nx) {
		ntrpr = nx;
		ntrExtra = 0;
		if (nx > 65535) {
			ntrpr = 65535;
			ntrExtra = nx - ntrpr;
		}
	}  // note: if any trArr[] beyond n traces exist, you have to free them manually

	bhed() : ntrExtra(0) {};

} bhed;










// all reading from file will be stored in this struct. If the screen window is very small, 
// one would have fewer samples and fewer traces than the data contain, and it can be a lot 
// faster to display. In this case, Only 1/10 (e.g.) traces are read in, with nxmax traces 
// in the arrays trArr[].data[]. Only 1/10 (e.g.) samples per trace are read in, with dtRead
// being the sample interval that is a lot bigger (0.04 sec) than the head->dtFile (0.004 sec, e.g.).
// Note the new sampling rate is also reflected in headRead.pBh->hdt or dto, nso, hns as well. 

// all outputs will be in 4-byte floating point format. If the original format was not floating point,
// we will output as floating point, and reset all markers to be as floating point, including
// setting bh.format=1. How does one knows the original segy format? It is still kept in head->formatFile.
struct HEAD_READ{
		// int nxmax; // total num of traces read in the array
				// this is in redundancy with bh.ntrpr, but I decided it is worth it to have this redundancy 
				// since it is convenient this way.
		//	int numsampRead; // actual num of samples per trace, regardless of the file's numsamp 
		//	double dtRead;  // can be smaller than head->dt; it is also set properly inside each trace header
		//  Previously I used numsampRead and dtRead, but they are redundant to deeper values, and it causes
		//  problems if we want to change the data length, so I just use inline functions to represent them
		CString ascHeadc;  // will cut it to 3200 bytes before writing
		bhed bh;  // binary header read in
		segydynamic *trArr; // actual data
		inline char * getAscHeadChar() {
			int nLeng = 3200 * 2;
			char *pInput = (char*)alloc1(nLeng, sizeof(char));
			ZeroMemory(pInput, nLeng);
			strcpy_s(pInput, nLeng, CT2A(ascHeadc));
			return pInput;
		}
		inline void setAscHead(char *ascHead) {
			//int nLeng = 3200 * 2;
			//char *pOutput = new char[nLeng];
			//ZeroMemory(pOutput, nLeng);
			ascHeadc = CA2T(ascHead);
		}
		inline int getnx() {
			return bh.getnx();
			//if (bh.ntrExtra < 0 || bh.ntrExtra > 999999) bh.ntrExtra = 0;
			//return bh.ntrpr + bh.ntrExtra;
		}
		inline void setnx(int nx) { 
			bh.setnx(nx);
		}  // note: if any trArr[] beyond n traces exist, you have to free them manually
		inline int getns()  { return bh.hns; }
		inline void setnumsamp(int num) { bh.hns = bh.nso = num;  }
		inline double getdt() { return bh.hdt * 0.000001; }
		inline void setdt(double delta) { 
			bh.hdt = bh.dto = (unsigned short)(delta * 1000000 + 0.5);
		}

		// test if the HEAD_READ is a complete representation of the segy in file. 
		// if true, it can save to overwrite the segy file
		inline bool isNotCompletelyMatched(bool isUnixFile, int formatFile, int numsampFile, double dtFile) {
			if (isUnixFile) return true;

			int iDiff = abs(formatFile - bh.format); // if format 5 or 1, it is safe to write since the HEAD_READ in memory is always 5 meaning IEEE floating,
				// while the HEAD_READ in industry file is mostly 1 meaning IBM floating point format
			bool isTrue = (iDiff>0 && iDiff<4) ||
				fabs(dtFile - getdt()) > 0.0000001 ||
				numsampFile != getns();
			return isTrue;
		}

		inline bool isStartEndSwapped(double startDist, double endDist) {
			int ix1 = -1;
			int ix2 = -1;
			for (int nxStore = 0; nxStore<getnx()-1; nxStore++) {
				register double x1 = trArr[nxStore].getDistance();
				register double x2 = trArr[nxStore+1].getDistance();
				if (startDist >= x1 && startDist <= x2) {
					ix1 = nxStore;
					break;
				}
			}
			if (ix1 < 0) return false;

			for (int nxStore = 0; nxStore<getnx()-1; nxStore++) {
				register double x1 = trArr[nxStore].getDistance();
				register double x2 = trArr[nxStore + 1].getDistance();
				if (endDist >= x1 && endDist <= x2) {
					ix2 = nxStore;
					break;
				}
			}
			if (ix2 < 0) return false;

			int time1 = trArr[ix1].day * 24*3600 + trArr[ix1].hour * 3600 + trArr[ix1].minute * 60;
			int time2 = trArr[ix2].day * 24 * 3600 + trArr[ix2].hour * 3600 + trArr[ix2].minute * 60;
			return time2 < time1;
		}


		HEAD_READ() : 
			trArr(NULL)   // this shall be an array of nx() traces; if not NULL, related destructors will need to free it
		{
			setnx(0);
		};
};


inline BOOL isTimeDomainUtil(int mode) {
	//if true then in time domain.
	return
		mode == 0 ||
		mode == 5 ||
		mode == 6 ||
		mode == 7 ||
		mode == 9 ||
		mode == 10
		;

	// Note:
	// mode==10 can mean both a time or a depth domain depending on draw.labelAsTWTT.
	// but all other modes has only 1-to-1 fixed time or depth domain relationship
}


typedef struct datafile {	/* datafile - trace identification header */
	short t0;	/*  */
	short blocksize;	/* blocksize of each record */
	short numrecs;	/* num of total records in a file */
	short t3;	/*  */
	short t4;	/*  */
	short datlen;	/* length of data in bytes */
	short numchan;  // num of channels at byte#12-13.
	char u[18];	// short at byte#14-31 
	float freq;		//frequency at byte#32-35.
	char u1[61];		//at bytes#36-96.
} datafile;


typedef struct SEI_PAIR{
	double x;
	double y;
	bool isValid;
	inline void set(double a, double b) {
		x = a;
		y = b;
	}
	SEI_PAIR() : isValid(false) {};
} seiPair;

struct SEI_TRIPLE {
	double x;
	double y;
	double z;
	inline void set(double a, double b, double c) {
		x = a;
		y = b;
		z = c;
	}
	SEI_TRIPLE() {
		x = 0.;
		y = 0.;
		z = 0.;
	}

};

// for convenience and speed, sometimes I need to compute 4 numbers at once and return from a function
// such as  X12Y12
struct SEI_QUAD{
	double x1;
	double y1;
	double x2;
	double y2;
	inline void set(double xx1, double yy1, double xx2, double yy2) {
		x1 = xx1;
		y1 = yy1;
		x2 = xx2;
		y2 = yy2;
	}
	inline double width() {
		return x2 - x1;
	}
	inline double height() {
		return y2 - y1;
	}

	SEI_QUAD() : x1(0.), y1(0.), x2(0.), y2(0.){};
};

static CTime zeroTime(1999, 3, 19, 22, 15, 0);


/* DEFINES */
#define gettr(x)	fgettr(stdin, (x))
#define vgettr(x)	fvgettr(stdin, (x))
#define puttr(x)	fputtr(stdout, (x))
#define gettra(x, y)    fgettra(stdin, (x), (y))
/* The following refer to the trid field in segy.h		*/
/* CHARPACK represents byte packed seismic data from supack1	*/
#define		CHARPACK	101
/* SHORTPACK represents 2 byte packed seismic data from supack2	*/
#define		SHORTPACK	102
/* TREAL represents real time traces 				*/
#define		TREAL		1
/* TDEAD represents dead time traces 				*/
#define		TDEAD		2
/* TDUMMY represents dummy time traces 				*/
#define		TDUMMY		3
/* TBREAK represents time break traces 				*/
#define		TBREAK		4
/* UPHOLE represents uphole traces 				*/
#define		UPHOLE		5
/* SWEEP represents sweep traces 				*/
#define		SWEEP		6
/* TIMING represents timing traces 				*/
#define		TIMING		7
/* WBREAK represents timing traces 				*/
#define		WBREAK		8
/* TCMPLX represents complex time traces 			*/
#define		TCMPLX		13
/* TAMPH represents time domain data in amplitude/phase form	*/
#define		TAMPH		15
/* FPACK represents packed frequency domain data 		*/
#define		FPACK		12
/* FUNPACKNYQ represents complex frequency domain data 		*/
#define		FUNPACKNYQ	11
/* FCMPLX represents complex frequency domain data 		*/
#define		FCMPLX		10
/* FAMPH represents freq domain data in amplitude/phase form	*/
#define		FAMPH		14
/* REALPART represents the real part of a trace to Nyquist	*/
#define		REALPART	16
/* IMAGPART represents the real part of a trace to Nyquist	*/
#define		IMAGPART	17
/* AMPLITUDE represents the amplitude of a trace to Nyquist	*/
#define		AMPLITUDE	18
/* PHASE represents the phase of a trace to Nyquist		*/
#define		PHASE		19
/* KT represents wavenumber-time domain data 			*/
#define		KT		21
/* KOMEGA represents wavenumber-frequency domain data		*/
#define		KOMEGA		22
/* ENVELOPE represents the envelope of the complex time trace	*/
#define		ENVELOPE	23
/* INSTPHASE represents the phase of the complex time trace	*/
#define		INSTPHASE	24
/* INSTFREQ represents the frequency of the complex time trace	*/
#define		INSTFREQ	25
/* DEPTH represents traces in depth-range (z-x)			*/
#define		TRID_DEPTH	30
#define ISSEISMIC(id) ( (id)==0 || (id)==TREAL || (id)==TDEAD || (id)==TDUMMY )
/* FUNCTION PROTOTYPES */
#ifdef __cplusplus /* if C++, specify external linkage to C functions */
extern "C" {
#endif
/* hdrpkge */
//void gethval(const segy *tp, int index, Value *valp);
//void puthval(segy *tp, int index, Value *valp);
//void getbhval(const bhed *bhp, int index, Value *valp);
//void putbhval(bhed *bhp, int index, Value *valp);
//void gethdval(const segy *tp, char *key, Value *valp);
//void puthdval(segy *tp, char *key, Value *valp);
//char *hdtype(const char *key);
//char *getkey(const int index);
//int getindex(const char *key);
#ifdef __cplusplus /* if C++, end external linkage specification */
}
#endif
#endif


