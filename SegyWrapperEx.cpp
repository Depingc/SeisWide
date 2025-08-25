#include "stdafx.h"
#include "SegyWrapper.h"
#include "SegyWrapperEx.h"
#include "segyMeta.h"
#include "util.h"
#include "ealloc_d.h"
#include "sumig.h"
#include "math.h"
#include "float.h"
#include "xallocator.h"
#include "shepard_interp_1d.h"

bool is_test = false;  // if true, it will output whatever data displayed on screen to a file

/*
int loadSegy2Array(CString f, segydynamic *trArr, bhed *bh);
void freeSegyArray(segydynamic *trArr, int nxmax);

*/
SegyWrapperEx::SegyWrapperEx(CString t)
{
	this->m_f = t;
	this->m_tmin = 0.f;
	this->m_tmax = 0.f;
	this->m_drawMode = 6; // meaning only reading based on trace info
	m_formatInFile2 = 1;
	m_isUnixInFile = false;
	this->m_distShift = 0.f;
	this->m_dxMin = 0.001;
	this->m_dyMin = 0.001;
	this->m_isHeadersOnly = false;

	// this is for avoiding exception when the same class instantiation is called twice
	headRead.trArr = NULL;
	headRead.setnx(0);
	this->m_cacheTimeFile = 0;


	this->xFlatten = this->yFlatten = NULL;
	this->nFlatten = 0;
	this->m_isEntireTrace = false;
}


// if we want to read all traces, just put xmin=xmax=0.0 (3 decimal resolution for equalality comparison)
SegyWrapperEx::SegyWrapperEx(CString f, double xmin, double xmax, 
	double tmin, double tmax, int drawMode)
{
	this->m_f = f;
	m_formatInFile2 = 1;
	m_isUnixInFile = false;
	this->m_distShift = 0.f;
	this->m_dxMin = 0.001;
	this->m_dyMin = 0.001;
	this->m_isHeadersOnly = false;

	// this is for avoiding exception when the same class instantiation is called twice
	headRead.trArr = NULL;
	headRead.setnx(0);
	this->m_cacheTimeFile = 0;

	this->xFlatten = this->yFlatten = NULL;
	this->nFlatten = 0;

	this->m_xmin = xmin;
	this->m_xmax = xmax;
	this->m_tmin = tmin;  // note: m_tmin is reduced!
	this->m_tmax = tmax;
	this->m_drawMode = drawMode; 
	this->m_isEntireTrace = false;
}

void SegyWrapperEx::freeSegyDataArrayOnly()
{
	// after this, the segy data is freed, but the trace header and section headers will remain.
	CSegyMeta::freeSegyDataArrayOnly(headRead.trArr, headRead.getnx());
}

void SegyWrapperEx::freeSegyArray() 
{
	// clean up before return
	if (headRead.trArr == NULL) return;

	CSegyMeta::freeSegyArray(headRead.trArr, headRead.getnx());
	headRead.trArr = NULL;
}


SegyWrapperEx::~SegyWrapperEx(void)
{
	TRY {
		freeSegyArray();
		if (xFlatten) free1(xFlatten);
		if (yFlatten) free1(yFlatten);

		xFlatten = NULL;
		yFlatten = NULL;
	}
	CATCH_ALL(e)
	{ 
		// it's ok to have some exceptions here 
		// AfxMessageBox(_T("oooo");
	}
	END_CATCH_ALL
}


const bool SegyWrapperEx::isPlotTrace(float range, float rangeOld)
{
	// return true;
	if (isFloatEqual(m_xmin, 0.) && isFloatEqual(m_xmax, 0.)) return true;
	if (isFloatEqual(m_xmin, m_xmax)) return true;
	// note: for a double click, we may get m_xmin = m_xmax, and it can be big
	if(range < m_xmin || range > m_xmax) return false;
	  // do not include any traces outside the limits, otherwise the plotting may have extra stuff seen that can be annoying and not accurate

	// if (fabs(m_xmax - m_xmin) <= m_dxMin && fabs(range - m_xmin)<m_dxMin * 2) return true; // this line is not effective, can delete it?
	return fabs(range-rangeOld) > this->m_dxMin;
}


// if isReduceAgainstOffset not true, the reduction will be against the m_xmin, which is good for displaying reflection near the slope (for example)
int SegyWrapperEx::openDistanceCache(bool isOffset, int numTraceDesired, HEAD *head, 
	SegyWrapperEx **cachePSegyWraps, int numCache, bool isCacheReadOnly, bool isReduceAgainstOffset)
{
	this->m_cacheTimeFile = getFileModifiedTime(this->m_f);

	m_formatInFile2 = head->formatFile;
	m_isUnixInFile = head->isUnixFile;
	m_numsampInFile = head->numsampFiles;
	m_dtInFile = head->dtFile;

	int ic;  // index for caching
	int ix; // index for traces
	bool isWholeFile = isFloatEqual3((float)m_xmin, (float)m_xmax);
	// first, check if the cache already has the desired data.
	// if yes, get the data
	// if the cache contains partial data, get it and update the data with newer parts in the cache
	// if not, get the data, free old, and put the new to the cache
	//double xIntervalFile = isOffset ? head->offsetIntervalFile : head->distIntervalFile;
	//xIntervalFile *= 0.3333; // this shall be the minimum X interval to consider
	//if (m_dxMin < xIntervalFile) m_dxMin = xIntervalFile; // this needs to be conditioned after cache determination

	// condition some params
	if (m_dyMin < head->dtFile) m_dyMin = head->dtFile;
	if (this->m_tmax - this->m_tmin < 0.01) {
		this->m_tmin = head->tminFile;
		this->m_tmax = head->tmaxFile;
	}
	if (m_tmax - m_tmin < 0.01) m_tmax = m_tmin + 0.01;
	int num;
	for (ic = 0; ic < numCache; ic++) {
		register SegyWrapperEx *aWrap = cachePSegyWraps[ic];
		if (aWrap->m_cacheTimeFile != m_cacheTimeFile) {
			if (aWrap->m_cacheTimeFile == 0 || this->m_f.CompareNoCase(aWrap->m_f) == 0) {
				// since this segy file is already changed. We need to free up the cache memory for this file. 
				// It has happened I had 2GB cache file, and I change that file but the cache still holds old data
				SegyWrapper::freeSegyArray(&aWrap->headRead);
				aWrap->m_cacheTimeFile = 0;
				aWrap->m_f = BLANK;
			}
			continue; // this will save CPU time
		}

		register int nx = aWrap->headRead.getnx();
		register int ns = aWrap->headRead.getns();
		num = 0;
		if (aWrap->m_f.IsEmpty() || (num = nx) < 1)
			continue;
		

		// now it is ok to use this cache
		register double x;
		register int ix1 = 0;
		int ix2 = nx - 1;

		if (!isWholeFile) {
			for (ix = 0; ix < nx; ix++) {
				x = isOffset ?
					aWrap->headRead.trArr[ix].getOffset()
					:
					aWrap->headRead.trArr[ix].getDistance();
				if (x >= m_xmin) {
					ix1 = ix;
					break;
				}
			}
			for (ix = nx - 1; ix >= 0; ix--) {
				x = isOffset ?
					aWrap->headRead.trArr[ix].getOffset()
					:
					aWrap->headRead.trArr[ix].getDistance();
				if (x <= m_xmax) {
					ix2 = ix;
					break;
				}
			}

			// to avoid memory leaks, these are the measures, and they do work!!!
			if (ix1 < 0 || ix2 < 0 || ix2 >= nx) {
				// if here, the indexes are problemlamic. We just take the entire cache
				ix1 = 0;
				ix2 = nx;
			}
			if (ix1 > nx - 10)
				ix1 = nx - 10;
			if (ix2 < ix1 + 10)
				ix2 = ix1 + 10;
			if (ix2 > nx - 1)
				ix2 = nx - 1;
		} // isWholeFile


		bool isEntireTraceMatched = m_f.CompareNoCase(aWrap->m_f) == 0 && aWrap->m_isEntireTrace;
		if (isEntireTraceMatched) {
			SegyWrapper::headReadCopy(&aWrap->headRead, &this->headRead, ix1, ix2);
			return 2;
		}


		bool isDetailMatched = m_f.CompareNoCase(aWrap->m_f) == 0 &&
			m_drawMode == aWrap->m_drawMode &&
			isOffset == aWrap->m_cacheIsOffset &&
			m_distShift == aWrap->m_distShift &&
			(m_dyMin >= aWrap->m_dyMin- MINDIST || m_dyMin >= aWrap->m_dyDesired- MINDIST) &&
			m_dxMin >= aWrap->m_dxMin- MINDIST &&
			m_xmin >= aWrap->m_xmin- MINDIST &&
			m_xmax <= aWrap->m_xmax+ MINDIST &&
			m_tmin >= aWrap->m_tmin- MINDIST &&
			m_tmax <= aWrap->m_tmax+ MINDIST &&
			aWrap->m_cacheTimeFile == m_cacheTimeFile;
		if (isDetailMatched) {
			// found it from the cache

			// before using it, we need another compare
			if (m_drawMode == 0 && red2.rvred != aWrap->red2.rvred)
				continue; // this is not matching
			else if (m_drawMode == 7 && red2 != aWrap->red2)
				continue; // not matching

			// another compaire for horizon flattening
			if (nFlatten != aWrap->nFlatten)
				continue;  // not matching

			double odt = 1. / aWrap->headRead.getdt();
			int it1, it2;
			if (aWrap->m_isEntireTrace) {
				it1 = 0;
				it2 = ns - 1;
			}
			else {
				it1 = (int)((m_tmin - aWrap->m_tmin) * odt + 0.5);
				it2 = ns - 1 - (int)((aWrap->m_tmax - m_tmax) * odt + 0.5);
				if (it1 < 0 || it2<0 || it2 >= ns) {
					// if here, the indexes are problemlamic. We just take the entire cache
					it1 = 0;
					it2 = ns;
				}
			}
			SegyWrapper::headReadCopy(&aWrap->headRead, &this->headRead,
				ix1, ix2, it1, it2);
			return 2;
		} // if

	}  // for ic

	// if here, the cache is not found
	bool isTrue;
	// preparing to load data. since we will cache it, we need to format m_tmin and m_tmax

	double ddx = (this->m_xmax - this->m_xmin)*0.5;
	this->m_xmin -= ddx;
	this->m_xmax += ddx;
	double ddt = (this->m_tmax - this->m_tmin)*0.5;
	this->m_tmin -= ddt; 
	this->m_tmax += ddt;

	double maxReduction4ThisRun = CCoords::getReduceTimeEx2(head->offsetMaxAbs(), this->m_drawMode, &this->red2);
	if (this->m_tmin < head->tminFile- maxReduction4ThisRun) this->m_tmin = head->tminFile- maxReduction4ThisRun;
	if (this->m_tmax > head->tmaxFile) this->m_tmax = head->tmaxFile;

	isTrue = openDistance(isOffset, numTraceDesired*2, head); // why do we numTraceDesired*2? We want less amount of cache
	if (isTrue && (this->headRead.getnx() < 1 || this->headRead.getns() < 1))
		isTrue = false;

	if (isTrue && !isCacheReadOnly) {
		// now put the new data back to the cache pool
		int iCacheSelected = getUnusedSegyCacheIndex(cachePSegyWraps, numCache);
		register SegyWrapperEx *aWrap = cachePSegyWraps[iCacheSelected];
		bool isSuc = SegyWrapper::headReadCopy(&this->headRead, &aWrap->headRead, this->headRead.getnx(), false);
		if (isSuc) {
			aWrap->m_f = this->m_f;
			aWrap->m_drawMode = m_drawMode;
			aWrap->m_cacheIsOffset = isOffset;
			aWrap->m_distShift = this->m_distShift;
			aWrap->m_dxMin = this->m_dxMin;
			aWrap->m_dyDesired = this->m_dyMin;
			aWrap->m_dyMin = this->headRead.getdt(); // it can be different from desired value
			aWrap->m_xmin = this->m_xmin;
			aWrap->m_xmax = this->m_xmax;
			aWrap->m_tmin = this->m_tmin;
			aWrap->m_tmax = this->m_tmax;
			this->red2.clone(&aWrap->red2); // this will clone the values
			aWrap->m_cacheTimeFile = this->m_cacheTimeFile;
			aWrap->nFlatten = this->nFlatten;
			aWrap->m_isEntireTrace = this->m_isEntireTrace;
		}
	}

	// 2022-04-26: Previously I used 95% as memory threashold, but it happened when all memory consumed and the PC is extremely sloweven though I have 17 GB memory. So I now change to 75%
	bool isMemoryTooMuch = getMemoryUsagePhysPercent() > 75.;
	if (isMemoryTooMuch) {
		int iCache2Free = getUnusedSegyCacheIndex(cachePSegyWraps, numCache);
		SegyWrapper::freeSegyArray(&cachePSegyWraps[iCache2Free]->headRead);
		cachePSegyWraps[iCache2Free]->m_cacheTimeFile = 0;
	}

	return isTrue;
}

// this function is designed to deal with all-zero outputs.
// if 1/3 of the outputs are all zero, we read the complete trace length. This is useful 
// during on-screen digitizing trace doubled sections.
bool SegyWrapperEx::openDistanceNonZero(bool isOffset, int numTracesDesired, HEAD *head)
{
	bool isTrue = openDistance(isOffset, numTracesDesired, head);
	if (!isTrue) return false;
	int numsamp = headRead.getns();
	int countZero = 0;
	for (int n = 0; n < this->headRead.getnx(); n++) {
		if (getMedianAbsolute(&headRead.trArr[n].data[0], numsamp) < MINAMP)
			countZero++;
	}
	if (countZero * 3 > this->headRead.getnx()) {
		// more than 1/3 of traces are zero, so we need to read in the entire trace length
		m_tmax *= 20.;
		if (m_tmax > head->tmaxFile)
			m_tmax = head->tmaxFile;
		return openDistance(isOffset, numTracesDesired, head);
	}
	return true;
}

bool SegyWrapperEx::openDistance(bool isOffset, int numTracesDesired, HEAD *head)
{
// 6=read in trace mode; 0=km mode, 7=NMO mode
//	if (numTracesDesired==1) numTracesDesired = 2; // this has happened, causing crash

// note:  the final result will be roughly aligned along the reduction velocity curve. 
	
	if (!isFileExistNotDirectory(this->m_f) || head->numsampFiles<1) return false;

	// if the user wants to load entire trace (by m_tmin==0==m_tmax), we just use trace mode
	m_formatInFile2 = head->formatFile;
	m_isUnixInFile = head->isUnixFile;
	m_numsampInFile = head->numsampFiles;
	m_dtInFile = head->dtFile;

	if (this->m_tmin >= this->m_tmax || m_drawMode == 6 || head->isStartTimeChangeDrastic())
		return this->openTraceNoAlign(head, 0, (int)MIL, 0); // read the entire segy file into memory
 


	// condition some params
	double xIntervalFile = isOffset ? head->offsetIntervalFile : head->distIntervalFile;

	if (numTracesDesired == 0) numTracesDesired = head->numTraces;
	if (m_dxMin < xIntervalFile*0.333) m_dxMin = xIntervalFile *0.333;
	if (m_dyMin < head->dtFile) m_dyMin = head->dtFile;
	if (this->m_tmin == this->m_tmax) {
		this->m_tmin = head->tminFile;
		this->m_tmax = head->tmaxFile;
	}
	if (this->m_tmax > head->tmaxFile)
		this->m_tmax = head->tmaxFile; // this prevents the user to specify a too big tmax to crash

	// to estimate how small the starting time can be, we need to estimate the maximum reducing time
	// otherwise we may miss the beginning part of the section!!!
	double reduceTimeMax =
		this->red2.isAvgReducedNMO ?
		CCoords::getReduceTimeExOld2(head->offsetMaxAbs(), this->m_drawMode, &red2) :
		CCoords::getReduceTimeEx2(head->offsetMaxAbs(), this->m_drawMode, &red2);
	if (this->m_tmin < head->tminFile - reduceTimeMax)
		this->m_tmin = head->tminFile - reduceTimeMax;


	if (m_tmax - m_tmin < 0.01) m_tmax = m_tmin + 0.01;
	m_cacheIsOffset = isOffset;
	

	CFile file;
	segy tr;
	CString tStr;

	if (!file.Open(m_f, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
		return false;
	}

	if (headRead.trArr) {
		// if here, the array headRead.trArr already contains data. We have to free them before continue.
		freeSegyArray();
	}

	// memcpy(this->ascHead, head->ascHead, 3200);
	headRead.setAscHead((char *)head->ascHead);

	//file.Read(&ascHead, 3200L); // skip ascii header section
	file.Seek(3200, CFile::begin);
	file.Read(&headRead.bh, 400L);
	if (m_isUnixInFile) SwapTapeHeader(&headRead.bh);
	if (headRead.bh.hdt == 0) headRead.bh.hdt = headRead.bh.dto;
	double odt = (double)MIL / (double) headRead.bh.hdt;
	double dt = 1. / odt;
	int sampByte = (headRead.bh.format != 3) ?  4  :  2;
	int numsamp = headRead.bh.hns;

	// fix up for *head info
	if (head->numsampFiles <= 0) {
		// this could happen if the file cache is old version or it is in image only format
		head->numsampFiles = headRead.bh.hns;
		head->dtFile = dt;
		head->formatFile = headRead.bh.format;
	}
	unsigned short numsampOut = (unsigned short)ceil((m_tmax-m_tmin) * odt) + 1;  
	
	// there is a case: numsampFiles=15001, but numsampOut=15002
	if (numsampOut < 4) {
		numsampOut = head->numsampFiles;
		if (numsampOut < 4) {
			numsampOut = 4;
		}
	}
	else if (numsampOut > USHRT_MAX) {
		numsampOut = USHRT_MAX;
	}
	this->m_tmax = this->m_tmin + (numsampOut - 1)*dt;


	bool isMonoDecreasing = false;
	if (isOffset) 
		isMonoDecreasing =
		head->numTraces > 4 &&
		head->offsets[0] > head->offsets[1] &&
		head->offsets[1] > head->offsets[2] &&
		head->offsets[2] > head->offsets[3];
	else
		isMonoDecreasing =
		head->numTraces > 4 &&
		head->dist[0] > head->dist[1] &&
		head->dist[1] > head->dist[2] &&
		head->dist[2] > head->dist[3];


	float range;
	float rangeOld = -MIL;

	// cumpute how much space should be allocated.
	// note: we MUST use exactly the same criterion as the actual reading

	// NOTE: numTracesDesired is the desired number of traces for output
	// nxmax is the finally determined number of traces for output
	// head->numTraces is the total number of traces in segy file
	int ind;
	int nxmax = 0;
	for (ind=0; ind< head->numTraces; ind++) {
		if (numTracesDesired>0 && nxmax >= numTracesDesired) {
			break; // no more traces needed 
		}
		range = (!isOffset) ?  
			head->dist[ind]+this->m_distShift  :  head->offsets[ind];
		if(!isPlotTrace(range, rangeOld) ) continue;
		rangeOld = range;
		nxmax++;
	} // for j 

	headRead.setnx(nxmax);
	if (nxmax == 0) return false;

	// now we have determined we only need memory for nxmax  traces
	headRead.trArr = (segydynamic *)alloc1(headRead.getnx(), sizeof(segydynamic));
	for (ind = 0; ind<headRead.getnx(); ind++) {
		headRead.trArr[ind].data = ealloc1float(numsampOut);
		if (ERROR == headRead.trArr[ind].data) {
			// when error allocation of memory, this whole segy will not work. We we need to de-allocate
			file.Close();
			//for (int jj=0; jj<ind; jj++)
			freeSegyArray();
			tStr.Format(_T("Sorry, failed to allocate memory: nx=%d; ns=%d. Try reducing the display array."), headRead.getnx(), numsampOut);
			AfxMessageBox(tStr);
			return false;
		}
	}

	// now we have successcully allocated for nxmax  traces
	// NOTE: numsamp is the amp length of each trace in segy
	//  numsampOut is the output amp length
	int sampByteOut = sizeof(float);  // always 4 bytes per sample in output
	float *fData = (float *)ealloc1float((size_t)numsamp);

	// start the loop of reading
	rangeOld = -MIL;

	int lenEachTraceData = numsamp*sampByte;
	//int i;
	int j;
	int nxStore = 0;
	ind = (!isMonoDecreasing)  ?  -1  :  head->numTraces;
	float zeroValue = head->isSegyBitmap() ? 255.f : 0.f;

	while (true) {
		if (!isMonoDecreasing) {
			if (ind >= head->numTraces-1) break;
			ind++;
		}
		else {
			if (ind < 1) break;
			ind--;
		}
		
		range = (!isOffset) ?  
			head->dist[ind]+this->m_distShift  :  head->offsets[ind];
		if (!isPlotTrace(range, rangeOld)) continue;
		rangeOld = range;

		//if (nxStore > nxmax - 3) {
		//	int testtest = 1;
		//}

		file.Seek(head->pDataArr[ind]-240, CFile::begin);
		file.Read(&tr, 240);
		file.Read(&tr.dat.data[0], lenEachTraceData);
		if (m_isUnixInFile) {
			SwapTraceHeader(false, false, false, &tr);
			SwapN(&tr.dat, headRead.bh.format, numsamp);
		}

		tr.tracl = head->traces[ind];  // head->traces[] has fixed a few odds so we have to use it

		if(headRead.bh.format==3)
			for (j=0; j<numsamp; j++) fData[j] = (float)tr.dat.datashort[j];
		else if (headRead.bh.format==1) {
			//if (tr.idistopt==8 || this->m_isIBMFloatSegy)
			ibm_to_float((int *) tr.dat.data, (int *) tr.dat.data, (int)numsamp);
			//for (j=0; j<numsamp; j++) fData[j] = (float)tr.dat.data[j];
			memcpy( (void *)&fData[0], (void *)&tr.dat.data[0], lenEachTraceData);
		}
		else if (headRead.bh.format==2) 
			for (j=0; j<numsamp; j++) fData[j] = (float)tr.dat.dataint[j];
		else if (headRead.bh.format==5) {
			//for (j=0; j<numsamp; j++) fData[j] = (float)tr.dat.data[j];
			memcpy( (void *)&fData[0], (void *)&tr.dat.data[0], lenEachTraceData);
		}
		else break;


		for (j=0; j<numsamp; j++) {
			if (_isnan(fData[j])) 
				fData[j] = zeroValue;
		}

		register double aReduceTime =
			this->red2.isAvgReducedNMO ?
			CCoords::getReduceTimeExOld2(head->offsets[ind], this->m_drawMode, &red2) :
			CCoords::getReduceTimeEx2(head->offsets[ind], this->m_drawMode, &red2);
		// note: m_tmin is reduced!
		// register float traceDelayInFile = head->tminArr[i];
		if (yFlatten && nFlatten > 0) {
			double aaa = interpolLinear(xFlatten, yFlatten, nFlatten, range);
			aReduceTime += aaa;
		}



		// assign only the trace header to the trArr[]
		// actuall trace delay is slightly different from  m_tmin+aReduceTime due to round off error
		// it should be:   i0 * dt
		register int i0 = (int)((m_tmin + aReduceTime - head->tminArr[ind]) * odt);
		register double trcDelayInOutput = i0 * dt + head->tminArr[ind];
		tr.setTraceDelayF(trcDelayInOutput);
		//tr.setTraceDelayF(m_tmin + aReduceTime);
		//register int i0 = (int)((m_tmin + aReduceTime - head->tminArr[ind]) * odt + 0.5);

		if (head->isSegyBitmap()) {
			for (j = 0; j < numsampOut; j++)
				headRead.trArr[nxStore].data[j] = zeroValue;
			// if I set it to zero here, the empty areas will be all black
		}
		else
			memset((void *)&headRead.trArr[nxStore].data[0], (int) '\0', numsampOut*sampByteOut);

		if (i0 < 0) {
			// i0<0 likely indicate that the required data to plot starts earlier than data can provide. 
			// in this case, we just pad some zeros at the beginning of each trace, already done with memset()
			register int iStart = abs(i0);
			register int numsamp2write = min(numsamp, numsampOut - iStart);
			if (numsamp < numsampOut - iStart) {
				int ddff = 1;
			}
			if (iStart<numsampOut && numsamp2write>0)
				memcpy( (void *)&headRead.trArr[nxStore].data[iStart], (void *)&fData[0], numsamp2write*sampByteOut);
		}
		else {
			register int numsamp2write = min(numsamp-i0, numsampOut);
			if (i0>=0 && numsamp2write>0)
				memcpy( (void *)&headRead.trArr[nxStore].data[0], (void *)&fData[i0], numsamp2write*sampByteOut);
		}

		tr.ns = numsampOut;
		if (!isOffset)
			tr.setDistance((float)range);
		memcpy( (void *)&headRead.trArr[nxStore].tracl, (void *)&tr, 240L);
		nxStore++;
		if (nxStore >= headRead.getnx()) {
			break; // no more room in array
		}
	} // while file2.Read
	file.Close();

	//CStringArray arr;
	//saveStringArrayToFile(&arr, _T("E:\\a.txt"));
	//arr.RemoveAll();

	// now remove any array elements not in use
	if (nxStore < headRead.getnx()) {
		for (j = nxStore; j<headRead.getnx(); j++) {
			free1float(headRead.trArr[j].data); 
			headRead.trArr[j].data = NULL;
		} // for
		headRead.setnx(nxStore);
	} // if

	free1float(fData);

	this->headRead.setnumsamp(numsampOut);
	this->headRead.setdt(dt);

	int resampFactor = (int)myRound(this->m_dyMin * odt);
	if (myRound(headRead.bh.hdt * resampFactor) > USHRT_MAX) {
		// too big, we need to reduce it
		resampFactor = (int)floor((double)headRead.bh.hdt / (double)USHRT_MAX);
	}
	if (resampFactor > 1) {
		// if here, the data has a lot more samples than the display device can handle. 
		// to save CPU, we need to skip some samples, which will change the 
		// trArr[nxStore].dt, trArr[nxStore].ns, bh.dt, bh.hns. To avoid confusion, we will 
		// revise these values when all the data are processed.

		// first, do the skipping of samples
		int jNew = 0;
		for (nxStore = 0; nxStore < this->headRead.getnx(); nxStore++) {
			register int count = resampFactor;
			jNew = 0;
			for (j=0; j<numsampOut; j++) {
				if (count==0) count = resampFactor;
				if (count==resampFactor) {
					this->headRead.trArr[nxStore].data[jNew] = this->headRead.trArr[nxStore].data[j];
					jNew++;
				}
				count--;
			}
		} // for nxStore
		 
		// now updating the headers
		numsampOut = jNew;
		dt *= (double)resampFactor;
		odt = 1. / dt; // odt will change accordingly
		headRead.bh.hdt = (unsigned short)myRound(dt * MIL);
		for (nxStore = 0; nxStore < this->headRead.getnx(); nxStore++) {
			this->headRead.trArr[nxStore].ns = numsampOut;
			this->headRead.trArr[nxStore].dt = headRead.bh.hdt;
		} // for nxStore

	} // if

	headRead.setnumsamp(numsampOut);
	headRead.setdt(dt);
	headRead.bh.format = 5; 
	// the actual data arrays inside headRead are actually IEEE floating point, regardless of the original format in file.
	// to avoid issues, we force headRead.bh.format =5.
	// The original format in file can be 3 which is 2 bytes short integer.

	// testing for outputing the SegyWrapperEx data to a new segy
	if (is_test) {
		CFile fileout;
		if (!fileout.Open(this->m_f+".test.sgy", CFile::modeCreate  |CFile::modeWrite | CFile::typeBinary)) {
			return TRUE;
		}
		fileout.Write(head->ascHead, 3200L);
		fileout.Write(&this->headRead.bh, 400);
		for (nxStore = 0; nxStore < this->headRead.getnx(); nxStore++) {
			this->headRead.trArr[nxStore].ns = (unsigned short)numsampOut;

			if (this->headRead.bh.format==1) {
				float_to_ibm((int *) headRead.trArr[nxStore].data, (int *) tr.dat.data, (int)numsampOut);
			}

			fileout.Write(&this->headRead.trArr[nxStore], 240);
			fileout.Write(&tr.dat.data[0], numsampOut*sizeof(float));
		}
		fileout.Close();
	}
	// end testing


	return true;
}



// This will use trace mode to read segy data (with SegyWrapper class)
bool SegyWrapperEx::openTraceNoAlign(HEAD *head, int trace1, int trace2, int numTracesDesired)
{
	m_formatInFile2 = head->formatFile;
	m_isUnixInFile = head->isUnixFile;
	m_numsampInFile = head->numsampFiles;
	m_dtInFile = head->dtFile;

	SegyWrapper sw(this->m_f);	
	sw.m_tmin = this->m_tmin;
	sw.m_tmax = this->m_tmax;

	// do we want to align the time (i.e. make every trace to have the same trace delay)? No.
	sw.m_isTimeAlign = false;


	bool isTrue;
	isTrue = sw.openTraceFlatWhole(numTracesDesired);
	this->m_isEntireTrace = true;
	if (!isTrue) {
		return false;
	}

	// now clone sw.headRead  to this->headRead
	this->headRead.ascHeadc = sw.headRead.ascHeadc;
	this->headRead.setnx(sw.headRead.getnx());
	int numsamp = sw.headRead.getns();
	this->headRead.setnumsamp(numsamp);
	this->headRead.setdt(sw.headRead.getdt());
	int lenBytes = numsamp * ((sw.headRead.bh.format != 3) ? 4 : 2);

	memcpy( (void *)&this->headRead.bh, (void *)&sw.headRead.bh, 400L);
	this->headRead.trArr = (segydynamic *)xmalloc(sizeof(segydynamic) * this->headRead.getnx());
	for (int i = 0; i<headRead.getnx(); i++) {
		headRead.trArr[i].data = ealloc1float(numsamp);
		memcpy( (void *)&this->headRead.trArr[i].tracl, (void *)&sw.headRead.trArr[i].tracl, 240L);
		memcpy( (void *)&this->headRead.trArr[i].data[0], (void *)&sw.headRead.trArr[i].data[0], lenBytes);
	}

	return true;
}



const int SegyWrapperEx::getClosestTrcIndexAtOffset(float offset)
{
	if (this->headRead.getnx() <= 1) return 0;

	// int iOffset = (int)myRound(offset*1000.f); 
	for (int j = 0; j< this->headRead.getnx() - 1; j++) {
		
		if (isInsideFloat(offset, this->headRead.trArr[j].getOffset(),
			this->headRead.trArr[j + 1].getOffset())) {
			if (fabs(offset - this->headRead.trArr[j].getOffset())   <   
				fabs(this->headRead.trArr[j + 1].getOffset() - offset))
				return j;
			else
				return j+1;
		} // if
	} // for j

	return -1;
}


const int SegyWrapperEx::getClosestTrcIndexAtDistance(float dist)
{
	if (this->headRead.getnx() <= 1) return 0;
	for (int j = 0; j< this->headRead.getnx() - 1; j++) {
		
		if (isInsideFloat(dist, this->headRead.trArr[j].getDistance(), 
			this->headRead.trArr[j + 1].getDistance())) {
			if (fabs(dist - this->headRead.trArr[j].getDistance())   <   
				fabs(this->headRead.trArr[j + 1].getDistance() - dist))
				return j;
			else
				return j+1;
		} // if
	} // for j

	return -1;
}

// calculate relative static shift using cross correlation, relative to the previous trace index.
// will ignore any trace delays in the trace headers, ND reduction vel, otherwise it's hard to say what is a relative time shift!
// only <b>fromt</b>  will consider the trace delay though
const float SegyWrapperEx::getShiftFromPrevTrc(int itrc, float tlen, float minlag, float maxlag) {
	if (itrc <= 0) return ERRORVAL;

	double dt = (double) this->headRead.bh.hdt / (double)MIL;
	double odt = 1. / dt;
	//float delay = getTraceDelay(&this->trArr[itrc-1]);
	//int ifromt = (int)myRound( (fromt-delay) * (float)odt );
	int itlen = (int)ceil(tlen * (float)odt) + 1;

	//note: minlag can be negative! but xcor only has positive indeces 0:n-1

	int iminlag = (int)myRound(minlag * (float)odt);
	int nmaxlag = (int)myRound( (maxlag-minlag) * (float)odt);
	if (nmaxlag > this->headRead.bh.hns) nmaxlag = this->headRead.bh.hns;
	if (iminlag > nmaxlag ) iminlag = 0;
//	if (iminlag < -nmaxlag ) iminlag = -imaxlag;

	float *crosscorr = ealloc1float(nmaxlag);

	xcor(itlen, 0, &this->headRead.trArr[itrc-1].data[0], 
		itlen, 0, &this->headRead.trArr[itrc].data[0], 
		nmaxlag, iminlag, &crosscorr[0]);


	int ind = getMaxIndex(crosscorr, nmaxlag);
	free1float(crosscorr);

	// now ind is the index corresponding to the maximum cross correlation
	if (ind>=0) {
		return (float)(ind*dt);
	}
	
	return ERRORVAL;
}

// tcenter is the time without considering any reduction velocity, 
// but trace static delay needs to consider if you use tcenter
const float SegyWrapperEx::getShiftFromPrevTrcCoh(int itrc, float tlen, float tcenter, float terr) {
	if (itrc <= 0) return ERRORVAL;

	double dt = (double) this->headRead.bh.hdt / (double)MIL;
	double odt = 1. / dt;
	float delay = this->headRead.trArr[itrc].getTraceDelay();
	int ntlen = (int)ceil(tlen * (float)odt) + 1;
	int ift = (int)myRound( (tcenter-delay-terr*0.5f) * (float)odt);
	int nerr = (int)ceil( terr * (float)odt) + 1;
	int hnerr = nerr/2;
	nerr = hnerr * 2 + 1;

	int ir,i,j;

	float **famtc = ealloc2float(ntlen, nerr);
	float **semb = ealloc2float(ntlen, nerr);
 	memset((void *) famtc[0],(int) '\0',nerr*ntlen*sizeof(float)); 
 	memset((void *) semb[0],(int) '\0',nerr*ntlen*sizeof(float)); 

	float *adata2 = ealloc1float(ntlen);
	float *bdata2 = ealloc1float(ntlen);
	float *temp = ealloc1float(nerr);
	float *temp1 = ealloc1float(nerr);
 	memset((void *) &temp[0],(int) '\0',nerr*sizeof(float)); 
 	memset((void *) &temp1[0],(int) '\0',nerr*sizeof(float)); 

	for (i=0; i<ntlen; i++) {  
		adata2[i] = this->headRead.trArr[itrc-1].data[i] * this->headRead.trArr[itrc-1].data[i];
		bdata2[i] = this->headRead.trArr[itrc].data[i] * this->headRead.trArr[itrc].data[i];
	}

	for (i=0; i<ntlen; i++) {  // i is index for the previous trace
		for (ir=0; ir<nerr; ir++) { 
			j = i+ir-hnerr;
			if (j>=0 && j<ntlen) {
				famtc[ir][i] = this->headRead.trArr[itrc-1].data[i] + this->headRead.trArr[itrc].data[j];
				semb[ir][i] = adata2[i] + bdata2[j];
			} // if j
		} // for ir
	} // for i

	for (ir=0; ir<nerr; ir++) {
		for (i=0; i<ntlen; i++) {
			temp[ir] += famtc[ir][i] * famtc[ir][i];
			temp1[ir] += semb[ir][i];
		} // for i
		if (temp1[ir] < MINAMP) {
			// temp1[ir] = 1.f;
			temp[ir] = 0.f;
		}
		else {
			temp[ir] /= temp1[ir];
		}
	} // for ir
	int ind = getMaxIndex(temp, nerr);

	free1float(adata2);
	free1float(bdata2);
	free2float(famtc);
	free2float(semb);
	free1float(temp);
	free1float(temp1);


	// now ind is the index corresponding to the maximum cross correlation
	//tStr.Format(_T("Existing %d %d; Prev %d %d; ind: %d", xTrc, nExisting, xTrcPrev, nPrevExisting, ind);
	//AfxMessageBox(tStr);

	if (ind>=0) {
		return (float)( (ind-hnerr)*dt);
	}
	
	return ERRORVAL;
}

// tcenter does not include reduction velocity!
const float SegyWrapperEx::getShiftTrcCohDepreciated(int itrc1, int itrc2, float tlen, float tcenter, float terr) {
	if (this->m_drawMode != 0 && this->m_drawMode != 6 && this->m_drawMode != 7) return ERRORVAL;  
		// this method works in NMO display  and normal reducing display, and trace dispaly
	if (itrc1 < 0) return ERRORVAL;
	if (itrc2 <= 0) return ERRORVAL;

	double dt = (double) this->headRead.bh.hdt / (double)MIL;
	double odt = 1. / dt;
	int ntlen = (int)ceil(tlen * (float)odt) + 1;

	float delay1 = this->headRead.trArr[itrc1].getTraceDelay();
	float delay2 = this->headRead.trArr[itrc2].getTraceDelay();

	int ift = (int)myRound( (tcenter-delay2-terr*0.5f) * (float)odt);
	int nerr = (int)ceil( terr * (float)odt) + 1;
	int hnerr = nerr/2;
	nerr = hnerr * 2 + 1;

	int ir,i,j;

	float **famtc = ealloc2float(ntlen, nerr);
	float **semb = ealloc2float(ntlen, nerr);
 	memset((void *) famtc[0],(int) '\0',nerr*ntlen*sizeof(float)); 
 	memset((void *) semb[0],(int) '\0',nerr*ntlen*sizeof(float)); 

	float *adata2 = ealloc1float(ntlen);
	float *bdata2 = ealloc1float(ntlen);
	float *temp = ealloc1float(nerr);
	float *temp1 = ealloc1float(nerr);
 	memset((void *) &temp[0],(int) '\0',nerr*sizeof(float)); 
 	memset((void *) &temp1[0],(int) '\0',nerr*sizeof(float)); 

	for (i=0; i<ntlen; i++) {  
		adata2[i] = this->headRead.trArr[itrc1].data[i] * this->headRead.trArr[itrc1].data[i];
		bdata2[i] = this->headRead.trArr[itrc2].data[i] * this->headRead.trArr[itrc2].data[i];
	}

	for (i=0; i<ntlen; i++) {  // i is index for the previous trace
		for (ir=0; ir<nerr; ir++) { 
			j = i+ir-hnerr;
			if (j>=0 && j<ntlen) {
				famtc[ir][i] = this->headRead.trArr[itrc1].data[i] + this->headRead.trArr[itrc2].data[j];
				semb[ir][i] = adata2[i] + bdata2[j];
			} // if j
		} // for ir
	} // for i

	for (ir=0; ir<nerr; ir++) {
		for (i=0; i<ntlen; i++) {
			temp[ir] += famtc[ir][i] * famtc[ir][i];
			temp1[ir] += semb[ir][i];
		} // for i
		if (temp1[ir] < MINAMP) {
			// temp1[ir] = 1.f;
			temp[ir] = 0.f;
		}
		else {
			temp[ir] /= temp1[ir];
		}
	} // for ir
	int ind = getMaxIndex(temp, nerr);

	free1float(adata2);
	free1float(bdata2);
	free2float(famtc);
	free2float(semb);
	free1float(temp);
	free1float(temp1);


	// now ind is the index corresponding to the maximum cross correlation
	//tStr.Format(_T("Existing %d %d; Prev %d %d; ind: %d", xTrc, nExisting, xTrcPrev, nPrevExisting, ind);
	//AfxMessageBox(tStr);

	if (ind>=0) {
		return (float)( (ind-hnerr)*dt);
	}
	
	return ERRORVAL;
}




// If no valid shift can be detected, return ERRORVAL
// in this routine, we assume the data is already perfectly assigned along desired reduction velocity. Since any reduced amount is included in the trace delay value
// as a result, we cannot use trace delay for coherency calculations here!
const float SegyWrapperEx::getShiftTrcCoh3(int itrc2, float terr, float slope, float lagWeight, int numSubdivide) {
	if (itrc2 <= 0) return ERRORVAL;
	if (this->m_drawMode != 0 && this->m_drawMode != 7)  
		return 0.f;


	double dtOrig = (double)headRead.bh.hdt / (double)MIL;
	int ntlenOrig = headRead.bh.hns;

	double dt = dtOrig /(double)numSubdivide;  
	double odt = 1. / dt;   

	int ntlen;
	if (this->m_tmax == this->m_tmin) {
		ntlen = (this->headRead.bh.hns - 1) * numSubdivide + 1;
	}
	else {
		ntlen = (int)ceil((m_tmax-m_tmin) * odt) + 1;
	}

	if (ntlen > 30000) {
		// the user has mistakenly done a too much over-sampling. We need to revert back
		odt = 30000. / (m_tmax - m_tmin);
		dt = 1. / odt;
		if (this->m_tmax == this->m_tmin) {
			ntlen = (int)ceil((this->headRead.bh.hns - 1) * dtOrig * odt) + 1;
		}
		else {
			ntlen = (int)ceil((m_tmax-m_tmin) * odt) + 1;
		}
	}





	int nerr = (int)ceil( terr * (float)odt) + 1;
	int hnerr = nerr/2;
	nerr = hnerr * 2 + 1;

	float *fam1 = ealloc1float(ntlen);
	float *semb1 = ealloc1float(ntlen);
 	memset((void *) &fam1[0],(int) '\0',ntlen*sizeof(float)); 
 	memset((void *) &semb1[0],(int) '\0',ntlen*sizeof(float)); 

	float **famtc = ealloc2float(ntlen, nerr);
	float **semb = ealloc2float(ntlen, nerr);
 	memset((void *) famtc[0],(int) '\0',nerr*ntlen*sizeof(float)); 
 	memset((void *) semb[0],(int) '\0',nerr*ntlen*sizeof(float)); 

	float *temp = ealloc1float(nerr);
	float *temp1 = ealloc1float(nerr);
 	memset((void *) &temp[0],(int) '\0',nerr*sizeof(float)); 
 	memset((void *) &temp1[0],(int) '\0',nerr*sizeof(float)); 

	float **datas = ealloc2float(ntlen, this->headRead.getnx());
	float **datas2 = ealloc2float(ntlen, this->headRead.getnx());
	float *tTrace = ealloc1float(ntlen);

	int ir,i;
	for (int n = 0; n<this->headRead.getnx(); n++) {
		// note: we want to sum along a straight line defined by "slope" as if there is no trace delay. So we do an interpolation. 
		// in this routine, we assume the data is already perfectly assigned along desired reduction velocity, which is included in the trace delay value
		//    as a result, we cannot use trace delay for coherency calculations
		for (i=0; i<ntlen; i++) {
			tTrace[i] = (float)(this->m_tmin + i * dt );  // actual time in seconds
		} // for i

		// float adelay = getTraceDelay(&this->trArr[n]);
		float adelay = (float)this->m_tmin; // do not use trace delay here!!!
		ints8r(         ntlenOrig, // ntin
						(float)dtOrig,   // dtin
						adelay,   // ftin
						&this->headRead.trArr[n].data[0], // yin[t=ntin], with yin[0] = y(fxin)
						0.f,  // yinl    value used to extrapolate yin values to left of yin[0]
						0.f,  // yinr	 value used to extrapolate yin values to right of yin[ntin-1]
						ntlen,   // nxout
						(float *)&tTrace[0],  // 
						(float *)&datas[n][0]);    // tout
 
		for (i=0; i<ntlen; i++) {  
			datas2[n][i] = datas[n][i] * datas[n][i];
		}
	} // for n




	int j;
	for (int n = 0; n<this->headRead.getnx(); n++) {
		if (n == itrc2) continue; // skip the target trace
		// first calculate shifts before stacking

		
		float delta = (this->headRead.trArr[n].getOffset() - 
			this->headRead.trArr[itrc2].getOffset()) * slope;
		int idelta = (int)myRound( delta * odt);
		for (i=0; i<ntlen; i++) {  
			j = i + idelta;
			if (j>=0 && j<ntlen) {
				fam1[i] += datas[n][j]; 
				semb1[i] += datas2[n][j];
			} // if j
		} // for i
	} // for n

	for (ir=0; ir<nerr; ir++) { 
		// the center point is at  ir==hnerr
		for (i=0; i<ntlen; i++) { 
			j = i+ir-hnerr;
			if (j>=0 && j<ntlen) {
				famtc[ir][i] = datas[itrc2][j];
				semb[ir][i] = datas2[itrc2][j];
			} // if j
		} // for i
		for (i=0; i<ntlen; i++) { 
			famtc[ir][i] += fam1[i];
			semb[ir][i] += semb1[i];
		} // for i
	} // for ir

	for (ir=0; ir<nerr; ir++) {
		for (i=0; i<ntlen; i++) {
			temp[ir] += famtc[ir][i] * famtc[ir][i];
			temp1[ir] += semb[ir][i];
		} // for i
		if (temp1[ir] < MINAMP) {
			temp[ir] = 0.f;
		}
		else {
			temp[ir] /= temp1[ir];
		}
	} // for ir

	// do a weighting to semblance array temp[] ?
	if (lagWeight != 1.f) {
		for (ir=0; ir<nerr; ir++) {
			register float aWeight = lagWeight * ( 1.f + (float)sin(90.*PIO * ((float)ir/(float)hnerr)) );
			temp[ir] *= aWeight;
		}
	}



	int ind = getMaxIndex(temp, nerr);


	free1float(tTrace);
	free1float(fam1);
	free1float(semb1);

	free2float(datas);
	free2float(datas2);
	free2float(famtc);
	free2float(semb);
	free1float(temp);
	free1float(temp1);

	// now ind is the index corresponding to the maximum cross correlation
	//tStr.Format(_T("Existing %d %d; Prev %d %d; ind: %d", xTrc, nExisting, xTrcPrev, nPrevExisting, ind);
	//AfxMessageBox(tStr);

	if (ind>=0) {
		return (float)( (ind-hnerr)*dt);
	}
	
	return 0.f;
}




const bool SegyWrapperEx::headReadCopyReduced(HEAD_READ *headFrom, HEAD_READ *headTo, double t1Reduced, double t2Reduced)
{
	int nxmax = headFrom->getnx();
	double dt = headFrom->getdt();
	double odt = 1. / dt;
	int numsampIn = headFrom->getns();
	double traceLen = t2Reduced - t1Reduced;
	int numsampOut = (int)ceil(traceLen * odt) + 1;

	CSegyMeta::freeSegyDataArrayOnly(headTo->trArr, headTo->getnx()); // only need to free data

	memcpy((void *)&headTo->bh, (void *)&headFrom->bh, 400L);
	headTo->setnx(nxmax);
	headTo->setnumsamp(numsampOut);
	headTo->setdt(headFrom->getdt());
	int lenBytes = numsampOut * 4;
	headTo->trArr = (segydynamic *)realloc1(headTo->trArr, nxmax, sizeof(segydynamic));
	for (int ix = 0; ix < nxmax; ix++) {
		headTo->trArr[ix].data = ealloc1float(numsampOut);
		register double trOffset = headFrom->trArr[ix].getOffset();
		register double tDelay = headFrom->trArr[ix].getTraceDelay();
		register double t1Real = t1Reduced + CCoords::getReduceTimeEx2(trOffset, m_drawMode, &red2) - tDelay; // (t1Real+tDelay) shall be the final trace delay in the output

		memcpy((void *)&headTo->trArr[ix].tracl, (void *)&headFrom->trArr[ix].tracl, 240L);
		memset ((void*)&headTo->trArr[ix].data[0], (int)'\0', lenBytes);
		register int it1 = (int)myRound(t1Real * odt); 
		if (it1 < numsampIn) {
			register int it2 = it1 + numsampOut - 1;
			if (it2 >= numsampIn)
				it2 = numsampIn - 1;
			if (it2 > it1) {
				lenBytes = (it2-it1+1) * 4;
				memcpy((void*)&headTo->trArr[ix].data[0], (void*)&headFrom->trArr[ix].data[it1], lenBytes);
			}
		}

		if (numsampOut != numsampIn) {
			// since the trace length is different, we need to adjust the header
			headTo->trArr[ix].setTraceDelayF((float)(t1Real+ tDelay));
			headTo->trArr[ix].ns = (unsigned short)numsampOut;
		}
	}

	return true;
}


// If no valid shift can be detected, return ERRORVAL
// in this routine, we assume the data is already perfectly assigned along desired reduction velocity. Since any reduced amount is included in the trace delay value
// as a result, we cannot use trace delay for coherency calculations here!
const float SegyWrapperEx::getShiftTrcCoh3Reduced(HEAD_READ *headTo, int itrc2, float terr, float slope, float lagWeight, int numSubdivide) {
	if (itrc2 <= 0) return ERRORVAL;
	headReadCopyReduced(&headRead, headTo, m_tmin, m_tmax);

	double dtOrig = (double)headTo->bh.hdt * OMIL;
	int ntlenOrig = headTo->bh.hns;

	double dt = dtOrig / (double)numSubdivide;
	double odt = 1. / dt;

	int ntlen;
	if (m_tmax == m_tmin) {
		ntlen = (headTo->bh.hns - 1) * numSubdivide + 1;
	}
	else {
		ntlen = (int)ceil((m_tmax - m_tmin) * odt) + 1;
	}

	if (ntlen > 30000) {
		// the user has mistakenly done a too much over-sampling. We need to revert back
		odt = 30000. / (m_tmax - m_tmin);
		dt = 1. / odt;
		if (m_tmax == m_tmin) {
			ntlen = (int)ceil((headTo->bh.hns - 1) * dtOrig * odt) + 1;
		}
		else {
			ntlen = (int)ceil((m_tmax - m_tmin) * odt) + 1;
		}
	}





	int nerr = (int)ceil(terr * (float)odt) + 1;
	int hnerr = nerr / 2;
	nerr = hnerr * 2 + 1;

	float *fam1 = ealloc1float(ntlen);
	float *semb1 = ealloc1float(ntlen);
	memset((void *)&fam1[0], (int) '\0', ntlen * sizeof(float));
	memset((void *)&semb1[0], (int) '\0', ntlen * sizeof(float));

	float **famtc = ealloc2float(ntlen, nerr);
	float **semb = ealloc2float(ntlen, nerr);
	memset((void *)famtc[0], (int) '\0', nerr*ntlen * sizeof(float));
	memset((void *)semb[0], (int) '\0', nerr*ntlen * sizeof(float));

	float *temp = ealloc1float(nerr);
	float *temp1 = ealloc1float(nerr);
	memset((void *)&temp[0], (int) '\0', nerr * sizeof(float));
	memset((void *)&temp1[0], (int) '\0', nerr * sizeof(float));

	float **datas = ealloc2float(ntlen, headTo->getnx());
	float **datas2 = ealloc2float(ntlen, headTo->getnx());
	float *tTrace = ealloc1float(ntlen);

	int ir, i;
	for (int n = 0; n<headTo->getnx(); n++) {
		// note: we want to sum along a straight line defined by "slope" as if there is no trace delay. So we do an interpolation. 
		// in this routine, we assume the data is already perfectly assigned along desired reduction velocity, which is included in the trace delay value
		//    as a result, we cannot use trace delay for coherency calculations
		for (i = 0; i<ntlen; i++) {
			tTrace[i] = (float)(m_tmin + i * dt);  // actual time in seconds
		} // for i

		  // float adelay = getTraceDelay(&this->trArr[n]);
		float adelay = (float)m_tmin; // do not use trace delay here!!!
		float yinl = headTo->trArr[n].data[0];
		float yinr = headTo->trArr[n].data[headTo->bh.hns - 1];
		ints8r(ntlenOrig, // ntin
			(float)dtOrig,   // dtin
			adelay,   // ftin
			&headTo->trArr[n].data[0], // yin[t=ntin], with yin[0] = y(fxin)
			yinl,  // yinl    value used to extrapolate yin values to left of yin[0]
			yinr,  // yinr	 value used to extrapolate yin values to right of yin[ntin-1]
			ntlen,   // nxout
			(float *)&tTrace[0],  // 
			(float *)&datas[n][0]);    // tout

		for (i = 0; i<ntlen; i++) {
			datas2[n][i] = datas[n][i] * datas[n][i];
		}
	} // for n




	int j;
	for (int n = 0; n<headTo->getnx(); n++) {
		if (n == itrc2) continue; // skip the target trace
								  // first calculate shifts before stacking


		float delta = (headTo->trArr[n].getOffset() -
			headTo->trArr[itrc2].getOffset()) * slope;
		int idelta = (int)myRound(delta * odt);
		for (i = 0; i<ntlen; i++) {
			j = i + idelta;
			if (j >= 0 && j<ntlen) {
				fam1[i] += datas[n][j];
				semb1[i] += datas2[n][j];
			} // if j
		} // for i
	} // for n

	for (ir = 0; ir<nerr; ir++) {
		// the center point is at  ir==hnerr
		for (i = 0; i<ntlen; i++) {
			j = i + ir - hnerr;
			if (j >= 0 && j<ntlen) {
				famtc[ir][i] = datas[itrc2][j];
				semb[ir][i] = datas2[itrc2][j];
			} // if j
		} // for i
		for (i = 0; i<ntlen; i++) {
			famtc[ir][i] += fam1[i];
			semb[ir][i] += semb1[i];
		} // for i
	} // for ir

	for (ir = 0; ir<nerr; ir++) {
		for (i = 0; i<ntlen; i++) {
			temp[ir] += famtc[ir][i] * famtc[ir][i];
			temp1[ir] += semb[ir][i];
		} // for i
		if (temp1[ir] < MINAMP) {
			temp[ir] = 0.f;
		}
		else {
			temp[ir] /= temp1[ir];
		}
	} // for ir

	  // do a weighting to semblance array temp[] ?
	if (lagWeight != 1.f) {
		for (ir = 0; ir<nerr; ir++) {
			register float aWeight = lagWeight * (1.f + (float)sin(90.*PIO * ((float)ir / (float)hnerr)));
			temp[ir] *= aWeight;
		}
	}



	int ind = getMaxIndex(temp, nerr);


	free1float(tTrace);
	free1float(fam1);
	free1float(semb1);

	free2float(datas);
	free2float(datas2);
	free2float(famtc);
	free2float(semb);
	free1float(temp);
	free1float(temp1);

	// now ind is the index corresponding to the maximum cross correlation
	//tStr.Format(_T("Existing %d %d; Prev %d %d; ind: %d", xTrc, nExisting, xTrcPrev, nPrevExisting, ind);
	//AfxMessageBox(tStr);

	if (ind >= 0) {
		return (float)((ind - hnerr)*dt);
	}

	return 0.f;
}


void SegyWrapperEx::sortDistance(bool isOffset)
{
	if (m_drawMode == 6) return;
	if (!this->headRead.trArr) return;

	segy tr;
	float *ranges = ealloc1float(headRead.getnx());

	for (int i = 0; i<this->headRead.getnx(); i++) {
		ranges[i] = isOffset ? 
			this->headRead.trArr[i].getOffset() :
			this->headRead.trArr[i].getDistance() +
			this->m_distShift;
	}

	int sampByte = (this->headRead.bh.format != 3) ? 4 : 2;
	int len = sampByte*headRead.bh.hns;
	int n, m;
	for (n = 0; n<this->headRead.getnx() - 1; n++) {
		//for (m = n; m<this->headRead.nx(); m++) {
		//	ranges[m] = isOffset ?  getOffset(&this->headRead.trArr[m])  :   getDistance(&this->headRead.trArr[m])+this->m_distShift;
		//}
		m = getMinIndex(&ranges[n], headRead.getnx() - n);

		// now m is not the index in trArr[], it must be added by an index shift of n! this a big programming trap!
		m += n;

		if (m > n) {
			// the range at m needs to swap with the trace at n. Note  trArr.data  is a separate pointer from trArr itself
			memcpy( (void *)&tr,   (void *)&headRead.trArr[n],  240L);
			memcpy( (void *)&tr.dat,   (void *)&headRead.trArr[n].data[0],  len);
			memcpy( (void *)&headRead.trArr[n],   (void *)&headRead.trArr[m],  240L);
			memcpy( (void *)&headRead.trArr[n].data[0],   (void *)&headRead.trArr[m].data[0],  len);
			memcpy( (void *)&headRead.trArr[m],   (void *)&tr,  240L);
			memcpy( (void *)&headRead.trArr[m].data[0],   (void *)&tr.dat,  len);
			
			register float temp = ranges[n];
			ranges[n] = ranges[m];
			ranges[m] = temp;
		}
	} // for n
	free1float(ranges);

	// after the sorting, the first trace should have the m_xmin, so let's change that as well
	this->m_xmin = isOffset ? this->headRead.trArr[0].getOffset() :
		this->headRead.trArr[0].getDistance();
	n = this->headRead.getnx() - 1;
	this->m_xmax = isOffset ? this->headRead.trArr[n].getOffset() :
		this->headRead.trArr[n].getDistance();
}


// depreciated, although still works ok. Not used. Please Use a similar function in CSegyMeta class
const float SegyWrapperEx::getAvgKmInterval2(bool isDistance) {
	int n,i;
	float *ranges = ealloc1float(headRead.getnx());
	for (n = 0; n<this->headRead.getnx(); n++) {
		ranges[n] = !isDistance ? this->headRead.trArr[n].getOffset() :
			this->headRead.trArr[n].getDistance() + this->m_distShift;
	}

	sort1(&ranges[0], this->headRead.getnx());

	for (i = 0; i<this->headRead.getnx() - 1; i++)
		ranges[i] = ranges[i+1] - ranges[i];
	// getMedian3(&ranges[0], this->nxmax-1, n);	//This is the average trace interval in km.
	n = getMedian3(&ranges[0], this->headRead.getnx() - 1);	//This is the average trace interval in km.
	float trcInterval = ranges[n];
	free1float(ranges);

	return trcInterval;
}

// all functions below are static functions

// processingGradientKm will only work in km mode. If no km info, please use another function
// headDx and headDt must already have proper headers copied from headRead, which is not checked for!!!
// range[] in input headRead must be already sorted, which is not checked for.
bool SegyWrapperEx::processingGradientKm(HEAD_READ *headIn, HEAD_READ *headDx, HEAD_READ *headDt, bool isDist)
{
	int ix, it, it1, it2;
	int nx = headIn->getnx();
	int nt = headIn->getns();

	if (nx < 2 || nt<2) return false;
	if (headDx->getnx() < nx || headDx->getns() < nt ||
		headDt->getnx() < nx || headDt->getns() < nt) return false;

	double *range = (double *)ealloc1double((size_t)nx);
	if (isDist) {
		for (ix = 0; ix < nx; ix++) {
			range[ix] = headIn->trArr[ix].getDistance();
		}
	}
	else {
		for (ix = 0; ix < nx; ix++) {
			range[ix] = headIn->trArr[ix].getOffset();
		}
	}

	double dxMin = fabs(getAvgInterval(range, nx)) * 0.2;
	if (dxMin < MINAMP) {
		free1double(range);
		return false; // no good to continue
	}
	double oDxMin = 1. / (2.*dxMin);
	float odt = 1.f / (float)(2. * headIn->getdt());

	double *transpose = (double *)ealloc1double((size_t)nx);
	double *range2 = (double *)ealloc1double((size_t)nx * 2);
	double *ampArr = (double *)ealloc1double(nx * 2);

	// now compute for headDt*  
	for (it = 0; it < nt; it++) {
		it1 = (it == 0) ? 0 : it - 1;
		it2 = it == nt - 1 ? nt - 1 : it + 1;
		register float fac = (it2 - it1 == 1) ? 2.f*odt : odt;
		for (ix = 0; ix < nx; ix++) {
			headDt->trArr[ix].data[it] = (headIn->trArr[ix].data[it2] - headIn->trArr[ix].data[it1]) * fac;
		}
	}




	// now compute for headDx*. Shift range to half interval in X. Note that it is uniform along X
	for (ix = 0; ix < nx; ix++) {
		range2[ix * 2] = range[ix] - dxMin;
		range2[ix * 2 + 1] = range[ix] + dxMin;
	}
	for (it = 0; it < nt; it++) {
		for (ix = 0; ix < nx; ix++) {
			transpose[ix] = headIn->trArr[ix].data[it];
		}

		//  now interpolate
		shepard_value_1d(nx, range, transpose, 2.0, nx*2, range2, ampArr);
		for (ix = 0; ix < nx; ix++)
			headDx->trArr[ix].data[it] = (float)((ampArr[ix * 2 + 1] - ampArr[ix * 2])*oDxMin); // this is now the real gradient along X-axis
	}

	// make a running mean along t-axis to reduce noise
	for (ix = 0; ix < nx; ix++) {
		for (it = 1; it < nt - 1; it++) {
			headDx->trArr[ix].data[it] = 
				(headDx->trArr[ix].data[it - 1] + headDx->trArr[ix].data[it] * 2.f + headDx->trArr[ix].data[it + 1]) * 0.25f;
		}
	}

	if (range != NULL) free1double(range);
	if (range2 != NULL) free1double(range2);
	if (transpose != NULL) free1double(transpose);
	if (ampArr != NULL) free1double(ampArr);
	return true;
}


