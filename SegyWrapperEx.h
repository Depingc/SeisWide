#pragma once
#include "segy.h"
#include "coords.h"
class SegyWrapperEx
{
public:
/*
int loadSegy2Array(CString f, segydynamic *trArr, bhed *bh);
void freeSegyArray(segydynamic *trArr, int nxmax);

*/
	// input parameters
	// ####if you want to load all existing data for processing, set  m_dxMin, m_dyMin, m_xmin, m_xmax, m_tmin, m_tmax   to all zero ####
	CString m_f; // file name for the segy, the only input needed
//	int m_trace1; // first trace to load
//	int m_trace2; // last trace to load
	double m_xmin;  // only used with distanceMode
	double m_xmax;  // only used with distanceMode
	double m_tmin; // note: this is the absolute time, not reduced (e.g. reduction velocity) by any means
	double m_tmax; // if m_tmin==m_tmax, Seiswide will take the entire trace, discard the m_tmin. 
					// NOTE: this is found to be useful since each trace can have different starting time. If I set m_tmin=0, all trace will start from zero time, which is not desirable in many cases!

	double m_dxMin; // if m_dxMin<=0, it will read all traces.
	double m_dyMin; // if m_dyMin<=0, it will read all samples
	double m_dyDesired; // actually read dy can be slightly different
	HEAD_READ headRead;
	int m_drawMode; // 6=read in trace mode; 0=km mode, 7=NMO mode; all other values are not implemented
	CCoords::REDUCE red2;
	float m_distShift; // Only in distance mode, all trace distance will add this amount.
		// NOTE: after opening, the trace distance will be already added by this amount!
	// bool m_isUnixInFile; // if it is unix format, for information only
	bool m_isHeadersOnly;
	//bool m_isReductionMarkedInHeader;

	// these are for caching and backup only.
	CTime m_cacheTimeFile; // Just set to to 0 at first, and assign a value after segy is read in
		// and copied to the cache
	bool m_cacheIsOffset;
	bool m_isEntireTrace;
	//bool m_isReduceAgainstOffset; // if !isReduceAgainstOffset, the reduction will be against the m_xmin, which is good for displaying reflection near the slope (for example)
		// this shall also be cached and used to determine if the cache is what needed to be extracted

	// for horizon flattening.
	float *xFlatten;
	float *yFlatten; // if not NULL, to be added to 
	int nFlatten;

	SegyWrapperEx(CString t);
	//SegyWrapperEx(CString t, int trc1, int trc2, double t1, double tmax);
	SegyWrapperEx(CString f, double xmin, double xmax, double tmin, double tmax, int drawMode);
	
	~SegyWrapperEx(void);
	//inline bool isUnixFile() {
	//	return IsUnix(m_formatInFile) == TRUE;
	//}
	inline void copyParaTo(SegyWrapperEx *sw2) {
		if (!sw2) return;
		// sw2->m_f = this->m_f;  // the name should not copy, otherwise it can cause problem
		sw2->m_xmin = this->m_xmin;
		sw2->m_xmax = this->m_xmax;
		sw2->m_tmin = this->m_tmin;
		sw2->m_tmax = this->m_tmax;
		sw2->m_dxMin = this->m_dxMin;
		sw2->m_dyMin = this->m_dyMin;
		sw2->m_dyDesired = this->m_dyDesired;
		sw2->m_drawMode = this->m_drawMode;
		this->red2.clone(&sw2->red2);
		sw2->m_distShift = this->m_distShift;
		sw2->m_isHeadersOnly = this->m_isHeadersOnly;
		sw2->nFlatten = this->nFlatten;
		if (this->nFlatten > 0) {
			sw2->xFlatten = ealloc1float(this->nFlatten);
			memcpy(sw2->xFlatten, this->xFlatten, sw2->nFlatten * sizeof(float));
			sw2->yFlatten = ealloc1float(this->nFlatten);
			memcpy(sw2->yFlatten, this->yFlatten, sw2->nFlatten * sizeof(float));
		}
		sw2->m_isEntireTrace = this->m_isEntireTrace;

	}

	void freeSegyDataArrayOnly();
	void freeSegyArray();
	const bool isPlotTrace(float range, float rangeOld);
	const int getClosestTrcIndexAtOffset(float offset);
	const int getClosestTrcIndexAtDistance(float dist);
	const float getShiftFromPrevTrc(int itrc, float tlen, float minlag, float maxlag);
	const float getShiftFromPrevTrcCoh(int itrc, float tlen, float tcenter, float terr);
	const float getShiftTrcCohDepreciated(int itrc1, int itrc2, float tlen, float tcenter, float terr);
	const float getShiftTrcCoh3(int itrc2, float terr, float slope, float lagWeight, int numSubdivide);




	// working
	const bool headReadCopyReduced(HEAD_READ *headFrom, HEAD_READ *headTo, double t1Reduced, double t2Reduced);
	const float getShiftTrcCoh3Reduced(HEAD_READ *headTo, int itrc2, float terr, float slope, float lagWeight, int numSubdivide);
	// end working



	inline const int getUnusedSegyCacheIndex(SegyWrapperEx **pCacheSegyWrapEx, int numCache) {
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
public:
	//inline void setReduceAgainstOffset(bool isRed) {
	//	this->m_isReduceAgainstOffset = isRed;
	//}
	bool openTraceNoAlign(HEAD *head, int trace1, int trace2, int numTracesDesired);

	// note: after this call, the actual num of sample is bh.nso or bh.hns 
	// which can be different from head.numsamp
	bool openDistance(bool isOffset, int numTracesDesired, HEAD *head);
	bool openDistanceNonZero(bool isOffset, int numTracesDesired, HEAD *head);
	void sortDistance(bool isOffset = true);  // we do need this. After this the trace number is not in sequencial order any more
	const float getAvgKmInterval2(bool isDistance);

	static bool processingGradientKm(HEAD_READ*, HEAD_READ*, HEAD_READ*, bool);
	int openDistanceCache(bool isOffset, int numTraceDesired, HEAD *head,
		SegyWrapperEx **cachePSegyWraps, int numCache, bool isCacheReadOnly, bool isReduceAgainstOffset = true);
	// note: if here I use SegyWrapper *cachePSegyWraps, then each cachePSegyWraps[i] is not a pointer.
	//       but if I define SegyWrapper *cachePSegyWraps[], then each cachePSegyWraps[i] becomes a pointer!!!
private:
	// these are saved privately when the segy file is open and read in. They will be useful when writing to file
	//    for making sure the new info will not conflict with existing ones!!!
	int m_formatInFile2;  //  m_formatInFile is not a indicator for the unix detection!!!
	bool m_isUnixInFile;
	int m_numsampInFile;
	double m_dtInFile;
};

