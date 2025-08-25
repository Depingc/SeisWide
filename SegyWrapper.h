#pragma once
#include "segy.h"
#include "util.h"
#include  "ximage.h"
#include "digitize.h"
#include "colorHelper.h"

class SegyWrapper
{
private:

public:
	CString m_f; // file name for the segy, the only input needed
	bool m_isHeadersOnly;
	int  m_cdp;  // 0=all, !=0 will read only 1 cdp (assume traces of 1 cdp are always close to each other.
	double m_tmin; // note: this is the absolute time, not reduced (e.g. reduction velocity) by any means
	double m_tmax; // time window. If m_tmax==m_tmin, Seiswide will take the entire trace, discard the m_tmin. 
					// NOTE: this is found to be useful since each trace can have different starting time. 
					// If I set m_tmin=0, all trace will start from zero time, which is not desirable in many cases!
	// NOTE: problem: if (m_tmax-m_tmin) is greater than the fixed record length, the output cannot work. So we have to change
	//		if they are smaller than total length, we should use it
	// bool m_isUnixInFile; // if it is unix format, for information only

	bool m_isEntireTrace;
	bool m_isTimeAlign;
	/*	 // default: false. 
		
		If true, each trace read in will have the same starting time. 

		If not, no alignment is done, in which the problem assises: if the second trace has a different starting time from the first trace, how 
		do we cut the output traces? The answer: we only output with cutting according to the first trace!!!

		When plotting, we need m_isTimeAlign=true. However, when updating a segy header, we need to have m_isTimeAlign=false.
	*/

	HEAD_READ headRead;  // everything read in shall be stored in this struct
	// in comparison, HEAD struct contains header info for every trace in file, 
	// regardless of how much is read into memory


	// these 4 are for backup only. 
	int m_cacheTraceStep;
	double m_cacheDyMin;
	double m_cacheDyDesired; // for a desired DyMin, the opentrace() may return a slightly different value m_cacheDyMin
	CTime m_cacheTimeFile; // Just set to to 0 at first, and assign a value after segy is read in
		// and copied to the cache

	int m_countErr;


	SegyWrapper(CString t, bool isHeadersOnly = false, int icdp = 0);
	SegyWrapper();

	~SegyWrapper(void);
	//void freeSegyArray(int nx);
	//inline bool isUnixFile(int valueReadfromFile) {
	//	return IsUnix(valueReadfromFile) == TRUE;
	//}
	// m_formatInFile is not a indicator for the unix detection any more!!!

	static void freeSegyArray(HEAD_READ *headRead2);
	//static bool isTraceNumberIrregular(HEAD *head);
	static void headReadZero(HEAD_READ *headTo);
	static bool headReadCopyReset(HEAD_READ *headFrom, HEAD_READ *headTo, int nxmaxTo = 0);
	static bool headReadReallocateArray(HEAD_READ *headRead, int nt, float tmin, float dt);
	static bool headReadCopy(HEAD_READ *headFrom, HEAD_READ *headTo, int nxmaxTo, bool headOnly);
	static bool headReadCopy(HEAD_READ *headFrom, HEAD_READ *headTo, int ix1, int ix2);
	static bool headReadCopy(HEAD_READ *headFrom, HEAD_READ *headTo,
		int ix1, int ix2, 
		int it1, int it2);
	// note: here I use int to avoid any coordinate/reduction issues!!!

	// note: trace numbers may frequently not be incrementing by 1, since we may skip some traces 
	//    during display. Then how do we assemble them together?
	// depreciated
	static bool headReadAssemble(HEAD_READ *headBase, HEAD_READ *headOut,
		HEAD_READ *headLeft,
		HEAD_READ *headRight,
		HEAD_READ *headTop,
		HEAD_READ *headBot,
		// int trace1, int trace2, 
		double t1, double t2);

	// prefered
	static bool headReadAssemble(HEAD_READ *headOut, HEAD_READ *headLeft, HEAD_READ *headRight);
	static void sortTraceNum(HEAD_READ *headRead2);
	static bool saveToSegyFile(HEAD_READ *headRead2, CString outFile, unsigned char *ascHead, BOOL isIBMForced);
	static bool saveToSegyFile(HEAD_READ *headRead2, CString outFile, int drawmode, CCoords::REDUCE *red2, double tmin, double tmax, unsigned char *ascHead, BOOL isIBMForced);
	static bool saveToSegyIEEE(HEAD_READ *headRead2, CString outFile, unsigned char *ascHead);

	static bool initNewSegyHead(HEAD_READ *headRead, int nx, double tmin, double dt, int numsamp);
	static bool saveTifAsSegy2(CxImage *ximage, CString fSgyOut, double xmin, double xmax, double tmin, double tmax, int xmode, bool isFaceUp);
	static bool saveTifAsSegy(CxImage *ximage, CString fSgyOut, double xmin, double xmax, double tmin, double tmax, int xmode, bool isFaceUp, CColorHelper::AMPCOLORDYN *pAmpColorDyn);
	static bool saveDig2Trace(CDigitize::DIGITIZE *dig2, CString fSgyOut, int numsamp, double tmin, double dt);
	static bool processingGradient(HEAD_READ*, HEAD_READ*, HEAD_READ*);



	bool open(HEAD *head, int numTraceDesired);
	bool openTrace(int trace1, int trace2, int numTraceDesired, HEAD *head, 
		int traceStep = 1, double dyMin = 0.);
	// if dyMin<=dt, it will read all samples

	bool openSeqTrace(int trace1, int trace2, int numTraceDesired, HEAD *head,
		int traceStep = 1, double dyMin = 0.);

	bool openCacheDepre(int trace1, int trace2, int numTraceDesired, HEAD *head,
		SegyWrapper **cachePSegyWraps, int numCache, bool isCacheReadOnly,
		int traceStep = 1, double dyMin = 0.);
	// note: if here I use SegyWrapper *cachePSegyWraps, then each cachePSegyWraps[i] is not a pointer.
	//       but if I define SegyWrapper *cachePSegyWraps[], then each cachePSegyWraps[i] becomes a pointer!!!
	//       SegyWrapper **cachePSegyWraps  is used finally, which should be referenced as above

	const inline int getOldestSegyCacheIndex(SegyWrapper **pCacheSegyWraps, int numCache) {
		CTime aTime = CTime::GetCurrentTime();
		int icSel = -1;
		for (int ic = 0; ic < numCache; ic++) {
			// m_cacheTimeLoaded == 0 means that the cache item is not used
			if (pCacheSegyWraps[ic]->m_cacheTimeFile == 0)
				return ic;
		}

		for (int ic = 0; ic < numCache; ic++) {
			if (pCacheSegyWraps[ic]->m_cacheTimeFile > 0 && pCacheSegyWraps[ic]->m_cacheTimeFile < aTime) {
				aTime = pCacheSegyWraps[ic]->m_cacheTimeFile;
				icSel = ic;
			}
		}
		if (icSel < 0) icSel = 0;
		return icSel;
	}

	const inline int getUnusedSegyCacheIndex(SegyWrapper **pCacheSegyWrapEx, int numCache) {
		CTime aTime = CTime::GetCurrentTime();
		int icSel = -1;

		// m_cacheTimeLoaded == 0 means that the cache item is not used
		for (int ic = 0; ic < numCache; ic++) {
			if (pCacheSegyWrapEx[ic]->m_cacheTimeFile == 0) return ic;
		}

		// now all cache memory are filled. We need to use the oldest index
		for (int ic = 0; ic < numCache; ic++) {
			if (pCacheSegyWrapEx[ic]->m_cacheTimeFile > 0 && pCacheSegyWrapEx[ic]->m_cacheTimeFile < aTime) {
				aTime = pCacheSegyWrapEx[ic]->m_cacheTimeFile;
				icSel = ic;
			}
		}
		if (icSel < 0) icSel = 0;
		return icSel;
	}

	const float getAvgShotIntervalPrestack(int cdpi);
	int openCacheSmart(int trace1, int trace2, HEAD *head,
		SegyWrapper **cachePSegyWraps, int numCache, bool isCacheReadOnly,
		int traceStep = 1, double dyMin = 0.);
	// openCacheSmart() is a lot faster than openCache() except that it works similar to google map
	//    by considering the 4 corners pf existing cache



	// this openTraceFlat() will read traces with each starting at tmin sharp and 
	// with trace length the same. Therefore, if the segy contains traces with tmin changing
	// among traces, there are some data not read when they go beyond the readable area
	bool openTraceFlat2_deprec(int trace1, int trace2, int numTraceDesired, HEAD *head,
		int traceStep = 1, double dyMin = 0.);

	bool openTraceFlatWhole(int numTraceDesired);

	void sortFFID();
	void sortDistance(int offsetMode);
	void reverseTrace();
	int const getClosestTrcIndexAtOffset(double offset);
	int const getClosestTrcIndexAtDistance(double aDist);
	int const getClosestTrcIndexAtTraceNum(double atrc);
	float const getAvgKmInterval(bool isDistance);

	float const getAvgAmp(int numTrc=0);
	inline const float getAvgAmpTrace(int tracel) {
		if (this->m_isHeadersOnly || this->headRead.getnx() < 1) return 0.f;

		int ixSelected = 0;
		for (int ix = 0; ix < this->headRead.getnx(); ix++) {
			if (this->headRead.trArr[ix].tracl == tracel) {
				ixSelected = ix;
				break;
			}
		}
		float *trc = ealloc1float(this->headRead.getns());
		register int jcount = 0;
		for (int j = 0; j<this->headRead.getns(); j++) {
			trc[jcount] = (float)fabs(headRead.trArr[ixSelected].data[j]);
			if (trc[jcount] > MINAMP && trc[jcount]<MAXAMP)
				jcount++;
		} // for j
		float avgAmp = (float)getRmsValue(trc, jcount);
		free1float(trc);
		return avgAmp;
	}

	float const getMaxAmp(int numTrc=0);
	inline const float getMaxAmpTrace(int tracel) {
		int ns = this->headRead.getns();
		if (this->m_isHeadersOnly || this->headRead.getnx() < 1 || ns<1) return 0.f;
		int ixSelected = 0;
		for (int ix = 0; ix < ns; ix++) {
			if (this->headRead.trArr[ix].tracl == tracel) {
				ixSelected = ix;
				break;
			}
		}

		float maxAmp = 0.f;
		float a;
		for (int j = 0; j < ns; j++) {
			a = (float)fabs(headRead.trArr[ixSelected].data[j]);
			if (maxAmp < a && a < MAXAMP)
				maxAmp = a;
		} // for j
		return maxAmp;
	}

	int const getArrIndexAtTrace(int itrc);
	int const getArrIndexAtFFID(int iFFID);
	bool staticShift(int itrc, float deltat);
	int writeHeaderOnly();
	int writeHeader4ZeroTrcNum();
	int writeDataBetweenTraces(int i1, int i2);
	int writeDataAll(CString fNew, bool isIbmEnforced = true);
	int writeDataAll();
	bool deleteTrc(int itrc);

	int getCountNonMonoOffset(int offsetMode = 1);
	float getShiftTrcCoh2(int itrc1, int itrc2, float tIncCenter, float terr, int numSubdivide);
private:
	// these are saved privately when the segy file is open and read in. They will be useful when writing to file
	//    for making sure the new info will not conflict with existing ones!!!
	int m_formatInFile2;  //  m_formatInFile is not a indicator for the unix detection!!!
	bool m_isUnixInFile;
	int m_numsampInFile;
	double m_dtInFile;

};

