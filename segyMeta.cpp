#include "stdafx.h"
#include "math.h"
#include "float.h"
#include "ealloc_d.h"
#include "segyMeta.h"
#include "segyWrapper.h"
#include "suSwapByte.h"
//#include "jp2_manage.h"

CSegyMeta::CSegyMeta() 
{
}

CSegyMeta::~CSegyMeta() 
{
}




//
//
//// ######################these are all static functions
//void CSegyMeta::getTraceMetasGenNotused(CString fileName, int *traces, float *dist, float *offset, int *cmp, int *ffid, int numTracesFile)
//{
//	CFile file;
//	if (!file.Open(fileName, CFile::modeRead | CFile::typeBinary)) {
//		return;
//	}
//	getTraceMetasGenNotused(&file, traces, dist, offset, cmp, ffid, numTracesFile);
//	file.Close();
//}
//
//
//
//void CSegyMeta::getTraceMetasGenNotused(CFile * file, int *traces, float *dist, float *offset, int *cmp, int *ffid, int numTracesFile){
//	// This routine tries to determine the smallest tmin for all teaces; should consider unix as well!
//	if (file == NULL) {
//		return;
//	}
//
//	ULONGLONG iold = file->Seek(0L, CFile::current);
//
//	segy tr;
//	bhed bh;
//	file->Seek(3200L, CFile::begin);
//	file->Read(&bh, 400L);
//	BOOL isUnix = IsUnix(bh.format);
//	if (isUnix) SwapTapeHeader(&bh);
//
//	int sampByte = (bh.format != 3) ? 4 : 2;
//	int traceDataBytes = sampByte * bh.hns;
//
//	float tminFinal = MIL;
//	float out;
//	int i = 0;
//	while (file->Read(&tr, 240) == (UINT)240) {
//		if (isUnix)
//			SwapTraceHeader(&tr);
//		out = getTraceDelay(&tr);
//		if (out < tminFinal)
//			tminFinal = out;
//		if (traces != NULL) {
//			traces[i] = tr.tracl;
//			cmp[i] = tr.cdp;
//			ffid[i] = tr.fldr;
//			dist[i] = getDistance(&tr);
//			offset[i] = getOffset(&tr);
//		}
//		i++;
//		if (i >= numTracesFile) break;  // no more traces can be read in
//		file->Seek(traceDataBytes, CFile::current);
//	}
//
//	file->Seek(iold, CFile::begin);
//
//	if (tminFinal == MIL) tminFinal = 0.f;
//	//this->m_tminFile = tminFinal;
//	//this->m_twinFile = (float)myFloat(  (double)(bh.hns-1) * (double) bh.hdt / (double)MIL  );
//
//	return;
//}



// if return 1, it is initialized, if return -1, it failed initialization
// what about if no need to initializing?  it return 0
// NOTE: the Init function will compute all head* 
// it is safe to call this function as many times as you want since the caching makes it very fast
int CSegyMeta::segyHeadInit(CString t, HEAD *head)
{
	if (!isFileExistNotDirectory(t)) return -1;

	int numTotalExisting = getNumTraceInSegy(t);
	if (numTotalExisting < 1) return 0; // no need to initialize anything

	CTime aTime = getFileModifiedTime(t);
	if (head->numTraces>0 &&
		head->numTraces <= numTotalExisting && 
		head->secName.CompareNoCase(t)==0 &&
		aTime == head->modifiedtime)
		return 1;

	CString fEncoded = CSegyMeta::getSegyMetaCacheName(t, false);  // if fEncoded does not exist, it may mean the user delete it on purpose, so we re-calculate
	if (isFileExistNotDirectory(fEncoded)) {
		// already fully initialized successfully beforehand
		if (CSegyMeta::getSegyMetaCache(fEncoded, head) && aTime == head->modifiedtime) return 1;
	}
	
	



	// start to initialize the segy
	int i = segyHeadInitArrays(head, numTotalExisting);
	
	getSegyMetaHead(t, head);

	// compute all the rest of meta info
	SegyWrapper sw(t, false);
	sw.m_isHeadersOnly = false;
	
	int numTracesToTest = (numTotalExisting < 100) ? numTotalExisting : 100;
		
	if (sw.openTraceFlatWhole(numTracesToTest)) {
		head->formatFile = sw.headRead.bh.format; // it might have changed
		head->offsetIntervalFile = sw.getAvgKmInterval(false);
		//return -1;
		head->distIntervalFile = sw.getAvgKmInterval(true);

		// NOTE: when the result is 18 million, if I do not have (double) cast, it returns 0. Why?
		head->maxAmp = (double)sw.getMaxAmp();
		head->aveAmp = (double)sw.getAvgAmp();
		if (head->formatFile==1 || head->formatFile == 5) {
			if (sw.m_countErr > 0 || head->maxAmp > MIL) {
				CString tStr;
				tStr.Format(_T("%s\nLarger than million values found in segy, which may be avoided by properly swaping between IBM and IEEE floating formats. Some older softwares might not distinquish between IBM and IEEE. Fix it by swapping?"), t);
				//AfxMessageBox(tStr);
				int iResult = AfxMessageBox(tStr, MB_YESNO | MB_ICONINFORMATION | MB_ICONQUESTION);
				if (iResult == IDYES) {
					sw.freeSegyArray(&sw.headRead);
					swapIbmIeee(t);
					return segyHeadInit(t, head);
				}
			}
		}
		sw.freeSegyArray(&sw.headRead);
	}
	else return 0;
	
	if (head->aveAmp < MINAMP) {
		// all zeros in the first 100 traces, so we read all
		sw.open(head, head->numTraces);
		head->maxAmp = (double)sw.getMaxAmp(head->numTraces);
		head->aveAmp = (double)sw.getAvgAmp(head->numTraces);
		if (head->aveAmp < MINAMP) head->aveAmp = 0.5;
	}

	if (head->maxAmp > 100. * head->aveAmp)
		head->maxAmp = 100. * head->aveAmp;  // this is because too high amplitude should be displayed the same as its 90% value
	if (head->maxAmp < MINAMP) head->maxAmp = 1.;

	setSegyMetaCache(fEncoded, head); // even if this may fail, we are ok with it.

	return i;
}













int CSegyMeta::getNumTraceInSegy(CString f) {
	CString ext = GetFileExtension(f);
	if (ext.Compare(_T("sgy")) != 0  &&  ext.Compare(_T("segy")) != 0 && ext.Compare(_T("seg")) != 0)
		return 0;

		
	CFile file;
	if (!file.Open(f, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone))
		return 0;

//	ULONGLONG etrwettttt = file.GetLength();
	int out = getNumTraceInSegy(&file);
	file.Close();
	return out;
}

int CSegyMeta::getNumTraceInSegy(CFile *file) {
	bhed bh;
	int num = 0;
	if (file != NULL) {
		// ULONGLONG iend = file->SeekToEnd();
		if (file->GetLength() <= 3600) return num;

		ULONGLONG iold = file->Seek(0L, CFile::current);
		file->Seek(3200L, CFile::begin);
		file->Read(&bh, 400);
		if (IsUnix(bh.format)) SwapTapeHeader(&bh);
		int sampByte = (bh.format != 3) ? 4 : 2;
		int len = sampByte*bh.hns + 240;
	//	ULONGLONG etrwettttt = file->GetLength();
		num = (int)myRound((double)(file->GetLength() - 3600) / (double)len);
		file->Seek(iold, CFile::begin);
	}
	return num;
}

CString CSegyMeta::segyFormatToString(short iFormat)
{
	CString tStr;
	int isUnix;
	if (isUnix = IsUnix(iFormat)) 
		swap_short_2(&iFormat);

	if (iFormat==1)
		tStr.Format(_T("IBM floating data"));
	else if (iFormat==2)
		tStr.Format(_T("4-byte int data"));
	else if (iFormat==3)
		tStr.Format(_T("2-byte int data"));
	else if (iFormat==4)
		tStr.Format(_T("4-byte fixed point data"));
	else if (iFormat==5)
		tStr.Format(_T("IEEE floating data"));

	if (isUnix) {
tStr = _T("*Unix format* (big endian), ") + tStr;
	}
	else {
		tStr = _T("*PC format* (small endian), ") + tStr;
	}
	return tStr;
}


BOOL CSegyMeta::getSegyMetaHead(CString f, HEAD *head)
{
	// This routine tries to determine the smallest tmin for all teaces; should consider unix as well!
	if (head->pDataArr == NULL || head->tminArr == NULL || head->dist == NULL || head->offsets == NULL || head->cdp == NULL || head->numTraces < 1) {
		return FALSE;
	}

	CTime aTime = getFileModifiedTime(f);
	//if (head->numTraces > 0 && aTime == head->modifiedtime) return FALSE;  // do not do this since when the cached info is not right, we want to do again even indicated cache is right

	CFile file;
	if (!file.Open(f, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
		return FALSE;
	}


	segy tr;
	bhed bh;
	//	static double omil = 1. / (double)MIL;
	static double oarc = 1. / 360000.;

	file.Read(head->ascHead, 3200L);
	autoAsc2ebc(head->ascHead, 3200);

	file.Read(&bh, 400L);
	head->isUnixFile = IsUnix(bh.format) == TRUE;
	if (head->isUnixFile) SwapTapeHeader(&bh);

	head->formatFile = bh.format;
	head->numsampFiles = bh.hns;
	if (bh.hdt == 0) bh.hdt = bh.dto;
	head->dtFile = bh.hdt * OMIL;
	int sampByte = (bh.format != 3) ? 4 : 2;
	int traceDataBytes = sampByte * bh.hns;

	head->xshotInSegy = bh.obsPositionX;

	int i = 0;
	register double factor = -0.1;
	//float reducedVelInsideSegy = bh.getReduceVel2006();
	//float rReducedVelInsideSegy = 
	//	rReducedVelInsideSegy == 0.f ? 0.f : (float)(1. / reducedVelInsideSegy);

	while (file.Read(&tr, 240) == (UINT)240) {
		if (head->isUnixFile)
			SwapTraceHeader(false, false, false, &tr);
		head->traces[i] = tr.tracl;
		if (tr.trid == 1 || tr.trid == 0 || tr.trid==200) {
			// need to skip non-valid seismic traces! Otherwise, any dead traces will kill the whole system!!!
			// tr.trid == 0 sometimes means it is not set but still valid trace
			head->tminArr[i] = tr.getTraceDelay();
			//if (head->tminArr[i] > 73.) {
			//	head->tminArr[i] = tr.getTraceDelay();
			//}
			head->cdp[i] = tr.cdp;
			head->cdpt[i] = tr.cdpt;
			head->ffid[i] = tr.fldr;
			head->dist[i] = tr.getDistance();
			head->offsets[i] = tr.getOffset();
			//if (rReducedVelInsideSegy != 0.f)
			//	head->tminArr[i] += head->offsets[i] * rReducedVelInsideSegy;
			head->pDataArr[i] = file.Seek(0, CFile::current);

			if (factor < 0.) {
				if (tr.counit == 0)
					factor = OMIL;
				else if (tr.counit == 2)
					factor = oarc;
				else
					factor = 1.;
			}
			head->sx[i] = (float)(tr.sx * factor);
			head->sy[i] = (float)(tr.sy * factor);
			i++;
		}
		file.Seek(traceDataBytes, CFile::current);
	}
	head->numTraces = i; // we must update this, since they may be different! Missed this line, and some older data would not read properly!
	head->conditionIrregularTraceNum();
	head->tminFile = getMinValue(head->tminArr, head->numTraces, 99999.);
	head->tmaxFile = getMaxValue(head->tminArr, head->numTraces) + (double)((bh.hns - 1) * head->dtFile);
	head->modifiedtime = aTime;
	head->secName = f;


	// now read data from the first 100 traces
	//file.Seek(3600, CFile::begin);
	//int nxmax = 0;
	//while (file.Read(&tr, 240+traceDataBytes) == (UINT)(240+traceDataBytes)) {
	//	if (head->isUnixFile)
	//		SwapTraceHeader(&tr);
	//	nxmax++;
	//	if (nxmax >= head->numTraces) break;  // no more traces can be read in
	//}
	file.Close();

	return TRUE;

}



double CSegyMeta::getAvgInterval(double x[], int n) 
{

	float dist[100], dx[100];
	n = min(100, n);
	if (n < 2) return 1.;  // cannot continue if so

	for (int i = 0; i<n; i++) {
		dist[i] = (float)x[i];
	} // for i


	sort1(&dist[0], n);

	for (int i = 1; i<n; i++) {
		dx[i - 1] = (float)fabs(dist[i] - dist[i - 1]);
	}
	dx[n-1] = 0.f;

	double out = getMedianAbsolute(&dx[0], n);
	if (out < MINAMP) out = MINAMP;
	return out;
}

// there is no need to consider the file modified time here, just store it to head->modifiedtime
//BOOL CSegyMeta::getSegyMetaAmpNotused(CString t, HEAD *pHead)
//{
//	if (!isFileExistNotDirectory(t)) return FALSE;
//	CString fEncoded = CSegyMeta::getSegyMetaCacheName(t, true);
//	if (!fEncoded.IsEmpty()) {
//		if (CSegyMeta::getSegyMetaCache(fEncoded, pHead)) {
//			// we have obtained the segy meta cache successfully. Now we should
//			// compare if this cache is update to date.
//			if (getFileModifiedTime(t) == pHead->modifiedtime) {
//				return TRUE;
//			}
//
//			// if here, the cache is out dated and should be discarded
//		}
//	}
//
//	if (segyHeadInit(t, pHead) != 1) return FALSE;
//	SegyWrapper sw(t);
//	sw.m_isHeadersOnly = false;
//	int numTrcInFile = pHead->numTraces;
//
//
//
//	// note: we cannot sort head.offsets[] or head.dist[], 
//	//  so we have to create offset[] and dist[] for sorting
//	if (!sw.open(pHead, 100)) {
//		pHead->aveAmp = 1.f;
//		pHead->maxAmp = 1.f;
//		pHead->offsetIntervalFile = .1;
//		pHead->distIntervalFile = 1.;
//		return FALSE;
//	}
//	//int numsamp = pHead->numsampFile;
//
//
//	int i;
//	int nxmax = min(100, sw.headRead.nx());
//	double offset[100], dist[100];
//	for (i = 0; i<nxmax; i++) {
//		dist[i] = sw.headRead.trArr[i].getDistance();
//		offset[i] = sw.headRead.trArr[i].getOffset();
//	} // for i
//	pHead->offsetIntervalFile = CSegyMeta::getAvgInterval(offset, nxmax);
//	pHead->distIntervalFile = CSegyMeta::getAvgInterval(dist, nxmax);
//
//	// NOTE: when the result is 18 million, if I do not have (double) cast, it returns 0. Why?
//	pHead->maxAmp = (double)sw.getMaxAmp();  
//	pHead->aveAmp = (double)sw.getAvgAmp(); 
//	sw.freeSegyArray(&sw.headRead);
//
//	if (pHead->aveAmp < MINAMP) {
//		// all zeros in the first 100 traces, so we read all
//		sw.open(pHead, numTrcInFile);
//		pHead->maxAmp = (double)sw.getMaxAmp(numTrcInFile);
//		pHead->aveAmp = (double)sw.getAvgAmp(numTrcInFile);
//		if (pHead->aveAmp < MINAMP) pHead->aveAmp = 0.5;
//	}
//
//	if (pHead->maxAmp > 1000. * pHead->aveAmp)
//		pHead->maxAmp = 1000. * pHead->aveAmp;  // this is because too high amplitude should be displayed the same as its 90% value
//
//	if (pHead->maxAmp < MINAMP) pHead->maxAmp = 1.;
//
//
//
//	//So, the average ampl should be "stretched" to cover the distance "temp*gain",
//	//But since this routine cannot use para.gain or para.clip (since this routine only
//	//executes once and yet para changed many times), the draw.trcIntOavgAmp and
//	//para.clip will have to be multiplied in separate drawing routines.
//	pHead->modifiedtime = getFileModifiedTime(t);
//	if (!fEncoded.IsEmpty())
//		CSegyMeta::setSegyMetaCache(fEncoded, pHead);
//	return TRUE;
//}



// ######################  End static functions





bool CSegyMeta::Serialize(CArchive &ar, HEAD *head)
{
	int i;
	CString tStr;
	try{

		if (ar.IsStoring()) {
			if (head->numTraces == 0)
				return false; // nothing to store
			ar << head->modifiedtime;
			ar << head->numTraces;
			ar << head->tminFile;
			ar << head->tmaxFile;
			ar << head->offsetIntervalFile;
			ar << head->distIntervalFile;
			ar << head->aveAmp;
			ar << head->maxAmp;
			for (i = 0; i < head->numTraces; i++)
				ar << head->pDataArr[i];
			for (i = 0; i < head->numTraces; i++)
				ar << head->traces[i];
			for (i = 0; i < head->numTraces; i++)
				ar << head->tminArr[i];
			for (i = 0; i < head->numTraces; i++)
				ar << head->dist[i];
			for (i = 0; i < head->numTraces; i++)
				ar << head->offsets[i];
			for (i = 0; i < head->numTraces; i++)
				ar << head->cdp[i];
			for (i = 0; i < head->numTraces; i++)
				ar << head->ffid[i];

			// extra additions, for importing JP2 or other imaging formats
			ar << (int)head->numsampFiles; // existing serialized file already uses (int) instead of (unsigned short)
			ar << head->dtFile;
			ar << head->formatFile;

			CString tStr(head->ascHead);
			ar << tStr;
			ar << head->isUnixFile;

			// additions Nov 16, 2015
			for (i = 0; i < head->numTraces; i++)
				ar << head->sx[i];
			for (i = 0; i < head->numTraces; i++)
				ar << head->sy[i];

			// addition Dec 2, 2016
			ar << head->m_isUseSeqTraceNumber;

			// addition Feb. 4, 2019
			ar << head->xshotInSegy;

			// addition Feb. 12, 2019
			for (i = 0; i < head->numTraces; i++)
				ar << head->cdpt[i];
		}
		else {
			CFile* file = ar.GetFile();
			ULONGLONG lenTot = file->GetLength();
			file->Seek(0, CFile::begin);



			// it is reading from archive
			ar >> head->modifiedtime;
			ar >> i; 
			// i is the num of traces in cache

			i = segyHeadInitArrays(head, i); 
			// i is now the actual num of traces read in
			if (i != 1) {
				head->numTraces = 0; // failed initializing arrays
				return false;
			}


			if (head->numTraces == 0)
				return false; // nothing to continue
			ar >> head->tminFile;
			if (fabs(head->tminFile) > fabs(ERRORVAL)) return false;
			ar >> head->tmaxFile;
			if (fabs(head->tmaxFile) < fabs(ERRORVAL)) return false;
			ar >> head->offsetIntervalFile;
			ar >> head->distIntervalFile;
			ar >> head->aveAmp;
			ar >> head->maxAmp;
			ar.Flush(); if (file->Seek(0, CFile::current) >= lenTot - 4) return false;
			for (i = 0; i < head->numTraces; i++)
				ar >> head->pDataArr[i];
			ar.Flush(); if (file->Seek(0, CFile::current) >= lenTot - 4) return false;
			for (i = 0; i < head->numTraces; i++)
				ar >> head->traces[i];
			ar.Flush(); if (file->Seek(0, CFile::current) >= lenTot - 4) return false;
			for (i = 0; i < head->numTraces; i++)
				ar >> head->tminArr[i];
			ar.Flush(); if (file->Seek(0, CFile::current) >= lenTot - 4) return false;
			for (i = 0; i < head->numTraces; i++)
				ar >> head->dist[i];
			ar.Flush(); if (file->Seek(0, CFile::current) >= lenTot - 4) return false;
			for (i = 0; i < head->numTraces; i++)
				ar >> head->offsets[i];
			ar.Flush(); if (file->Seek(0, CFile::current) >= lenTot - 4) return false;
			for (i = 0; i < head->numTraces; i++)
				ar >> head->cdp[i];
			ar.Flush(); if (file->Seek(0, CFile::current) >= lenTot - 4) return false;
			for (i = 0; i < head->numTraces; i++)
				ar >> head->ffid[i];

			ar.Flush();
			ULONGLONG lenCur = file->Seek(0, CFile::current);
			if(lenCur >= lenTot-4) {
				// no more data to read. We need to handle
				head->numsampFiles = 0;
				head->dtFile = -1.;
				head->formatFile = 0;
				head->isUnixFile = false;
				//head->img_transform_gain = 1.;
				//head->img_transform_offset = 0.;
				return true;
			}

			ar >> i;
			head->numsampFiles = (unsigned short)i;
			ar >> head->dtFile;
			ar >> head->formatFile;
			//ar >> head->img_transform_gain;
			//ar >> head->img_transform_offset;

			ar.Flush();
			lenCur = file->Seek(0, CFile::current);
			if(lenCur >= lenTot-4) {
				// no more data to read. We need to handle
			}
			else {
				ar >> tStr;
				//sprintf(head->ascHead, "%S", tStr);
				CT2A ch(tStr);
				for (i=0; i<3200; i++)
					head->ascHead[i] = ch[i];
			}

			ar.Flush();
			lenCur = file->Seek(0, CFile::current);
			if (lenCur >= lenTot - 4) {
				// no more data to read. We need to handle
			}
			else {
				ar >> head->isUnixFile;
			}



			// additions Nov 16, 2015
			ar.Flush();
			lenCur = file->Seek(0, CFile::current);
			if (lenCur >= lenTot - 4) {
				// no more data to read. We need to handle
				return false;
			}
			else {
				for (i = 0; i < head->numTraces; i++)
					ar >> head->sx[i];
				for (i = 0; i < head->numTraces; i++)
					ar >> head->sy[i];
			}

			// addition Dec 2, 2016
			ar.Flush();
			lenCur = file->Seek(0, CFile::current);
			if (lenCur >= lenTot - 1) {
				// no more data to read. We need to re-calculate. This is reading older cache files
				head->conditionIrregularTraceNum(); // this will modify m_isUseSeqTraceNumber
			}
			else {
				ar >> head->m_isUseSeqTraceNumber;
			}

			// addition Feb. 4, 2019
			ar.Flush();
			lenCur = file->Seek(0, CFile::current);
			if (lenCur >= lenTot - sizeof(double)) {
				// no more data to read. We need to re-calculate. This is reading older cache files
				return false; // will ignore this cache
			}
			else {
				ar >> head->xshotInSegy;
			}

			// addition Feb. 12, 2019
			ar.Flush();
			lenCur = file->Seek(0, CFile::current);
			if (lenCur >= lenTot - sizeof(int) * head->numTraces) {
				// no more data to read. We need to re-calculate. This is reading older cache files
				return false; // will ignore this cache
			}
			else {
				for (i = 0; i < head->numTraces; i++)
					ar >> head->cdpt[i];
			}
		}
	}
	catch (CArchiveException* pEx)
	{
		// An archive exception was thrown by MFC.  Report 
		// the error to the user. 
		pEx->Delete();
		return false;  // this archive is not good with exception
	}
	catch (CFileException* pEx)
	{
		// An archive exception was thrown by MFC.  Report 
		// the error to the user. 
		pEx->Delete();
		return false;  // this archive is not good with exception
	}
	catch (CMemoryException* pEx)
	{
		// An archive exception was thrown by MFC.  Report 
		// the error to the user. 
		pEx->Delete();
		return false;  // this archive is not good with exception
	}
	catch (CException* e) {
		//if (e->m_cause == CFileException::fileNotFound)
		//	AfxMessageBox("The File has not been opened.");
		e->Delete();
		return false;  // this archive is not good with exception
	}
	return true;

}

// output is the complete path to the cache file name, computed from input segy file named f
// Note: the best scenario is to 
// 1. use full path and name when in Seiswide's temp dir, or system temp dir, 
//    e.g.  arcticAll__sb2008__10__SB10_2008_245_00_10_09PCDubDespikeLOW$$sgy
// 2. use only name in segy's temp sub dir, e.g. SB10_2008_245_00_10_09PCDubDespikeLOW$$sgy
// This is because when a segy's parent dir is changed (which is frequent for me), the cach file is still valid
// But for case 1, if the cache filename does not contain path info, it will be confusing as 
// different dir may happen to have the same file names.
CString CSegyMeta::getSegyMetaCacheName(CString fSegy, bool isWriteNeeded) {
	if (!isFileExistNotDirectory(fSegy))
		return _T("");

	CString sgyName = GetBaseFileName(fSegy);
	// this is the prefered file name for the caching
	// e.g.  SB10_2008_245_00_10_09PCDubDespikeLOW$$sgy  without extension
	// since f will be changed later, please do sgyName beforehand!

	CString sgyPath = GetPathFromFileSlash(fSegy);

	// f must first be stripped drive letter, and then base64, and then check if the file exists
	int i = fSegy.Find(_T(":\\"));
	if (i == 1) {
		// now we need to strip the driver letter
		fSegy = fSegy.Mid(3);
	}
	fSegy.Replace(_T("\\"), _T("__"));
	fSegy.Replace(_T("."), _T("$$"));

	const CString strTemp = _T("temp\\");
	//const CString strCache = _T("\\cache\\");

	// no we need append the SeisWide.exe dir to cache
	CString cachePath;
	
	const CString seiPath = getSeisWideParaPath();
	cachePath = seiPath + strTemp + fSegy;
	if ( isFileExistNotDirectory(cachePath)) return cachePath;

	// now under the Segy's temp dir, the cache name should be the name only without path info in it
	cachePath = sgyPath + strTemp + sgyName + _T("$$sgy");
	if ( isFileExistNotDirectory(cachePath)) return cachePath;

	// now the cache file does not exist.
	if (isWriteNeeded) {
		if (isDirectoryWritable(sgyPath)) {
			cachePath = sgyPath + strTemp;
			if (!isDirectoryExist(cachePath))
				CreateDirectory(cachePath, NULL);
			fSegy = sgyName + _T("$$sgy"); // f now does not contain path info
		}
		else {
			cachePath = seiPath +strTemp;
			if (!isDirectoryExist(cachePath)) {
				CreateDirectory(cachePath, NULL);
			}
			if (!isDirectoryWritable(cachePath)) {
				// since both the segy and the seiswide dir are not writable, we use window's temp dir
				TCHAR lpszSystemInfo[MAX_PATH];   // buffer for concat'd. str.
				GetTempPath(MAX_PATH, lpszSystemInfo);
				cachePath = CString(lpszSystemInfo);
			}
			// f already contains path info
		}

		if (isDirectoryWritable(cachePath)) {
			// added Oct. 27, 2014
			// if this cache dir is writable, we want to add a readme_sei.txt  telling the user that
			// the caching files are safe to delete while their purpose is simply to speed up the work
			CString fStr = cachePath + _T("readme_sei.txt");
			if (!isFileExistNotDirectory(fStr)) {
				CStringArray *cstrArr = new CStringArray();
				cstrArr->Add(_T("#SeisWide's automatic caching files in this temp folder are safe to delete."));
				cstrArr->Add(_T("#Their only purpose is simply to speed up the work."));
				cstrArr->Add(_T("#Each caching file corresponds to a separate segy file. You may "));
				cstrArr->Add(_T("#manually remove these files, and SeisWide will regenerate them whenever it needs to."));
				cstrArr->Add(_T("#SeisWide's caching function can auto detect any changes to a segy file to make sure its caching is up to date."));
				saveStringArrayToFile(cstrArr, fStr);
				cstrArr->RemoveAll();
				delete cstrArr;
			}
			//CStringArray strArr;
			//saveStringArrayToFile(&strArr, path + _T
		}
	}
	else {
		// if we only need to read, we do not need the cache file to to modifiable
		cachePath = sgyPath + strTemp;
		fSegy = sgyName + _T("$$sgy"); // f now does not contain path info
	}

	return cachePath + fSegy;
}



bool CSegyMeta::getSegyMetaCache(CString fEncoded, HEAD *head) {
	// first, get the head struct data from cache
	// and then compare the head->time  with the time of file
	// if the file's time is different from cache's time (head->time), then load the data from segy file
	CFile fileS;
	if (!fileS.Open(fEncoded, CFile::modeRead)) return false;
	CArchive arLoad(&fileS, CArchive::load);
	try {
		bool iout = CSegyMeta::Serialize(arLoad, head);
		arLoad.Close();
		fileS.Close();  //  <--- close the file
		if (!iout) return false;
	}
	catch (CException* e) {
		e->Delete();
		arLoad.Close();
		fileS.Close();  //  <--- close the file
		return false;  // this archive is not good with exception
	}

	// before we are ok with this cache values, we want to test it first
	// this is important to avoid reading an invalid cache!!!
	return head->numTraces > 0 && head->numTraces<MIL && 
		head->numsampFiles>0 && head->numsampFiles<MIL && 
		head->dtFile>0. && head->dtFile<99999.;
}

bool CSegyMeta::setSegyMetaCache(CString fEncoded, HEAD *head) {
	// save head->  to file  fBase64, which has to be an absolute path
	CString aPath = GetPathExistingFromFileSlash(fEncoded);
	if (!isDirectoryExist(aPath))
		CreateDirectory(aPath, NULL);

	CFile fileS;
	if (!fileS.Open(fEncoded, CFile::modeWrite | CFile::modeCreate)) return false;
	CArchive arStore(&fileS, CArchive::store);
	bool iout = CSegyMeta::Serialize(arStore, head);
	arStore.Close();
	fileS.Close();  //  <--- close the file

	if (!iout && isFileExistNotDirectory(fEncoded)) {
		DeleteFile(fEncoded);
		return false;
	}
	return true;
}




void CSegyMeta::sortDistance(HEAD *pHead, bool offsetMode)
{
	int ival;
	int m;

	float val;
	float *ranges = ealloc1float(pHead->numTraces);

	for (int n = 0; n < pHead->numTraces - 1; n++) {
		for (m = n; m < pHead->numTraces; m++) {
			if (offsetMode)
				ranges[m] = pHead->offsets[m];
			else 
				ranges[m] = pHead->dist[m];
		}
		m = getMinIndex(&ranges[n], pHead->numTraces - n);

		// now m is not the index in trArr[], it must be added by an index shift of n! this a big programming trap!
		m += n;

		if (m > n) {
			// the range at m needs to swap with the trace at n. Note  trArr.data  is a separate pointer from trArr itself
			val = pHead->offsets[n]; pHead->offsets[n] = pHead->offsets[m]; pHead->offsets[m] = val;
			val = pHead->dist[n]; pHead->dist[n] = pHead->dist[m]; pHead->dist[m] = val;
			ival = pHead->traces[n]; pHead->traces[n] = pHead->traces[m]; pHead->traces[m] = ival;
			val = pHead->tminArr[n]; pHead->tminArr[n] = pHead->tminArr[m]; pHead->tminArr[m] = val;
			val = pHead->sx[n]; pHead->sx[n] = pHead->sx[m]; pHead->sx[m] = val;
			val = pHead->sy[n]; pHead->sy[n] = pHead->sy[m]; pHead->sy[m] = val;
			ival = pHead->cdp[n]; pHead->cdp[n] = pHead->cdp[m]; pHead->cdp[m] = ival;
			ival = pHead->cdpt[n]; pHead->cdpt[n] = pHead->cdpt[m]; pHead->cdpt[m] = ival;
			ival = pHead->ffid[n]; pHead->ffid[n] = pHead->ffid[m]; pHead->ffid[m] = ival;
			//memcpy((void *)&tr, (void *)&headRead.trArr[n], 240L);
			//memcpy((void *)&headRead.trArr[n], (void *)&headRead.trArr[m], 240L);
			//memcpy((void *)&headRead.trArr[m], (void *)&tr, 240L);
		}
	} // for n
	free1float(ranges);
}


double CSegyMeta::getlatLongFromSegyMod(HEAD *pHead, float dist,
	float &latitude, float &longitude)
{
	// note: this assumes data sorted . If the offsets are not sorted, the results may return -1.
	// run this beforehand CSegyMeta::sortDistance(pHead, false);
	double distOut = dist;  // will output the actual distance, which can be different from dist
	int n = pHead->numTraces;
	if (n < 1) return distOut;

	if (dist <= pHead->dist[0]) {
		latitude = pHead->sy[0];
		longitude = pHead->sx[0];
		distOut = pHead->dist[0];
	}
	else if (dist >= pHead->dist[n-1]) {
		latitude = pHead->sy[n-1];
		longitude = pHead->sx[n-1];
		distOut = pHead->dist[n-1];
	}
	else {
		float x1, x2, dx;
		for (int j = 0; j < n - 1; j++) {
			x1 = pHead->dist[j];
			x2 = pHead->dist[j + 1];
			if (isInsideDouble(dist, x1, x2)) {
				dx = x2 - x1;
				register float factor = (dx == 0.f) ? 0.f : (dist - x1) / dx;
				latitude = pHead->sy[j] + (pHead->sy[j + 1] - pHead->sy[j]) * factor;
				longitude = pHead->sx[j] + (pHead->sx[j + 1] - pHead->sx[j]) * factor;
				break;
			} // if
		} // for j
	}

	//latitude = pround(latitude, 6);
	//longitude = pround(longitude, 6);

	//if (longitude > 180.f) longitude -= 360.f; // this cause error with UTM
	//else
	//	if (longitude < -180.f) longitude += 360.f;

	return distOut;

}



float CSegyMeta::getXShotInSegy2(CString f) {
	float re = ERRORVAL;
	if (!isFileExistNotDirectory(f)) return re;
	bhed bh;
	BOOL isUnix = false;
	CFile file;
	if (!file.Open(f, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
	}
	else {
		file.Seek(3200L, SEEK_SET);
		file.Read(&bh, 400);
		if (isUnix = IsUnix(bh.format)) SwapTapeHeader(&bh);
		re = bh.obsPositionX;

		file.Close();
	}
	return re;
}


// this function gets a list of all xshot values in txName file, and stores to *sArr, nothing else will be stored
//#define MAXLINE 400     /* The max length of each text line input*/
BOOL CSegyMeta::getXshotListFromTxOut(CString txName, CStringArray *sArr) {
	if (sArr == NULL) return false;
	sArr->RemoveAll();
	CString tStr;

	FILE *stream;
	if (_tfopen_s(&stream, txName, _T("r")) != 0) {
		return FALSE;
	}

	TCHAR line[MAXLINE];
	long n;
	float t1, t2, t3, xshotOld = ERRORVAL;
	while (_fgetts(line, MAXLINE, stream) != NULL) {
		if (_tcslen(line) < 2) continue;
		if (line[0] == _T('#')) continue; // we do not need comment lines
		_stscanf_s(line, _T("%f %f %f %d"), &t1, &t2, &t3, &n);
		if (n < 0) {
			//No traveltime curve in file
			break;
		}

		if (isFloatEqual(0.f, t3) && n == 0 && !isFloatEqual(xshotOld, t1))  {
			tStr.Format(_T("%f"), t1);
			sArr->Add(tStr);
			xshotOld = t1;
		}
	}

	fclose(stream);
	return TRUE;

}
