#include "stdafx.h"
#include "SegyWrapper.h"
#include "segyMeta.h"
#include "math.h"
#include "float.h"
#include "ealloc_d.h"
#include "suSwapByte.h"
#include "xallocator.h"

/*
int loadSegy2Array(CString f, segydynamic *trArr, bhed *bh);
void freeSegyArray(segydynamic *trArr, int nxmax);

*/

SegyWrapper::SegyWrapper()
{
	SegyWrapper(_T(""), true, 0);
}

SegyWrapper::SegyWrapper(CString t, bool isHeadersOnly, int icdp)
{
	this->m_f = t;
	this->m_isHeadersOnly = isHeadersOnly;
	m_isTimeAlign = false;
	m_isEntireTrace = false;
	//	this->m_isIBMFloatSegy2 = true;
	//	this->headRead.trArr = NULL;
	// this->m_isUnixInFile = false;
	this->m_cdp = icdp;
	// this->nxDesired = 0;
	//	int ii = sizeof(bhed);

	// this is for avoiding exception when the same class instantiation is called twice
	//	headRead.trArr = NULL;
	//	headRead.pBh = NULL;
	//	this->headRead.setnx(0);  // this will be used important !!!

	// one will need to modify them
	this->m_tmin = 0.f;
	this->m_tmax = 0.f;
	this->m_cacheTraceStep = 1;
	this->m_cacheTimeFile = 0;
	m_formatInFile2 = 1;
	m_isUnixInFile = false;
	m_countErr = 0;
	//	this->m_tmaxFile = this->m_tminFile = ERRORVAL;
}

//void SegyWrapper::freeSegyArray(int nx)
//{
//	// clean up before return
//	// example of data:	headRead.trArr[10].data[1000] is an amplitude;
//	if (headRead.trArr && nx > 0) {
//		CSegyMeta::freeSegyDataArrayOnly(headRead.trArr, nx);
//	} // if
//
//	// now headRead.trArr[10].data==NULL;
//	if (headRead.trArr) {
//		free1(headRead.trArr);  // note: if nx==0, I cannot free trArr since it would give me an exception.
//		//	delete headRead.trArr;
//	}
//	headRead.trArr = NULL;
//	headRead.setnx(0);
//}

void SegyWrapper::freeSegyArray(HEAD_READ *headRead2)
{
	// example of data:	headRead2->trArr[10].data[1000] is an amplitude;
	if (!headRead2) return;
	CSegyMeta::freeSegyArray(headRead2->trArr, headRead2->getnx());
	//if (headRead2->trArr != NULL && headRead2->nx() > 0) {
	//	for (int jj = 0; jj < headRead2->nx(); jj++) {
	//		if (headRead2->trArr[jj].data != NULL) {
	//			free1float(headRead2->trArr[jj].data);
	//			headRead2->trArr[jj].data = NULL;
	//		}
	//	}
	//	xfree(headRead2->trArr);  // note: if nx==0, I cannot free trArr since it would give me an exception.
	//} // if
	headRead2->trArr = NULL;
}


SegyWrapper::~SegyWrapper(void)
{
	TRY{
		freeSegyArray(&headRead);
	}
		CATCH_ALL(e)
	{
		// it's ok to have some exceptions here 
	}
	END_CATCH_ALL
}




bool SegyWrapper::open(HEAD *head, int numTraceDesired) {
	return this->openTrace(0, 999999, numTraceDesired, head, 1, 0.);
}



// this static function helps to reallocate trace data length. It does not do memset to save CPU. 
// it returns false if not needed to reallocate. returns true if succeed
bool SegyWrapper::headReadReallocateArray(HEAD_READ *headRead, int nt, float tmin, float dt)
{
	if (!headRead || headRead->getnx() < 1) return false;
	if (headRead->getns() == nt && headRead->getdt() == dt &&
		headRead->trArr[0].getTraceDelay() == tmin)
		return false;

	headRead->setnumsamp(nt);
	headRead->setdt(dt);
	// headRead->bh.hdt = (unsigned short)myRound(dt*MIL);
	for (int ix = 0; ix < headRead->getnx(); ix++) {
		if (headRead->trArr[ix].data)
			free1float(headRead->trArr[ix].data);
		headRead->trArr[ix].data = ealloc1float(nt);
		headRead->trArr[ix].ns = nt;
		headRead->trArr[ix].dt = headRead->bh.hdt;
		headRead->trArr[ix].setTraceDelayF(tmin);
	}
	return true;
}

void SegyWrapper::headReadZero(HEAD_READ *headTo)
{
//	int lenBytes = headTo->numsamp() * ((headTo->bh.format != 3) ? 4 : 2);
	int lenBytes = headTo->getns() * 4;
	for (int i = 0; i<headTo->getnx(); i++) {
		memset((void *)&headTo->trArr[i].data[0], (int) '\0', lenBytes);
	}
}

// same as headReadCopy() except the new data will be all zero
bool SegyWrapper::headReadCopyReset(HEAD_READ *headFrom, HEAD_READ *headTo, int nxmaxTo)
{

	if (!SegyWrapper::headReadCopy(headFrom, headTo, nxmaxTo, true)) return false;
	SegyWrapper::headReadZero(headTo);
	//int lenBytes = headTo->numsamp() * ((headTo->bh.format != 3) ?  4  :  2);
	//for (int i = 0; i<headTo->nx(); i++) {
	//	memset((void *) &headTo->trArr[i].data[0],(int) '\0', lenBytes); 
	//}

	return true;

}
// To save CPU, if headOnly==true, the data will be mem copied.
// But regardless of headOnly being true or false, the data array must be 
//    properly assigned after this call. 
bool SegyWrapper::headReadCopy(HEAD_READ *headFrom, HEAD_READ *headTo, int nxmaxTo, bool headOnly)
{
	int nxmaxOut = (nxmaxTo == 0) ? headFrom->getnx() : nxmaxTo;
	if (!headTo) return false;
	//	SegyWrapper::freeSegyArray(headTo);
	CSegyMeta::freeSegyDataArrayOnly(headTo->trArr, headTo->getnx()); // only need to free data

	memcpy((void *)&headTo->bh, (void *)&headFrom->bh, 400L);
	headTo->setnx(nxmaxOut);
	headTo->setnumsamp(headFrom->getns());
	headTo->setdt(headFrom->getdt());
	// NOTE: HEAD_READ is always in floating point, but bh.format may point to other formats!!!
	int lenBytes = headFrom->getns() * 4;

	// headTo->bh.ntrpr = headTo->nxmax;  // since it is already changed, maybe
	// headTo->trArr = (segydynamic *)xmalloc(sizeof(segydynamic) * headTo->nx());
	headTo->trArr = (segydynamic *)realloc1(headTo->trArr, nxmaxOut, sizeof(segydynamic));

	bool isSuc = true;
	for (int i = 0; i < headTo->getnx(); i++) {
		headTo->trArr[i].data = ealloc1float(headTo->getns());
		if (!headTo->trArr[i].data) {
			isSuc = false;
			break;
		}
		if (i < headFrom->getnx()) {
			memcpy((void *)&headTo->trArr[i].tracl, (void *)&headFrom->trArr[i].tracl, 240L);
			if (!headOnly)
				memcpy((void *)&headTo->trArr[i].data[0], (void *)&headFrom->trArr[i].data[0], lenBytes);
		}
		else {
			memcpy((void *)&headTo->trArr[i].tracl, (void *)&headTo->trArr[i - 1].tracl, 240L);
			headTo->trArr[i].tracl = headTo->trArr[i - 1].tracl + 1;
			headTo->trArr[i].cdp = headTo->trArr[i - 1].cdp + 1;
			headTo->trArr[i].fldr = headTo->trArr[i - 1].fldr + 1;
			if (!headOnly)
				memset((void *)&headTo->trArr[i].data[0], (int) '\0', lenBytes);
		} // if
	} // for i

	if (!isSuc) {
		// memory alloc failed, so we need to scratch all and declare failure. This is normal if we work on huge segy files
		SegyWrapper::freeSegyArray(headTo);
	}

	return isSuc;

}



bool SegyWrapper::headReadCopy(HEAD_READ *headFrom, HEAD_READ *headTo, int ix1, int ix2)
{
	if (ix1 < 0) ix1 = 0;
	if (ix2 < ix1) return false;
	if (ix2 > headFrom->getnx() - 1) ix2 = headFrom->getnx() - 1;

	int nxmaxOut = ix2 - ix1 + 1;
	int numsamp = headFrom->getns();

	CSegyMeta::freeSegyDataArrayOnly(headTo->trArr, headTo->getnx()); // only need to free data

	memcpy((void *)&headTo->bh, (void *)&headFrom->bh, 400L);
	headTo->setnx(nxmaxOut);
	headTo->setnumsamp(numsamp);
	headTo->setdt(headFrom->getdt());
	int lenBytes = numsamp * 4;
	//double t1 = it1 * headFrom->getdt();

	headTo->trArr = (segydynamic *)realloc1(headTo->trArr, nxmaxOut, sizeof(segydynamic));
	for (int ix = ix1; ix <= ix2; ix++) {
		headTo->trArr[ix - ix1].data = ealloc1float(numsamp);
		memcpy((void *)&headTo->trArr[ix - ix1].tracl, (void *)&headFrom->trArr[ix].tracl, 240L);
		memcpy((void *)&headTo->trArr[ix - ix1].data[0], (void *)&headFrom->trArr[ix].data[0], lenBytes);
	}

	return true;
}



bool SegyWrapper::headReadCopy(HEAD_READ *headFrom, HEAD_READ *headTo, int ix1, int ix2, int it1, int it2)
{
	if (ix1 < 0) ix1 = 0;
	if (it1 < 0) it1 = 0;
	if (ix2 < ix1 || it2 < it1) return false;
	if (ix2 > headFrom->getnx() - 1) ix2 = headFrom->getnx() - 1;
	if (it2 > headFrom->getns() - 1) it2 = headFrom->getns() - 1;

	int nxmaxOut = ix2 - ix1 + 1;
	int numsampOut = it2 - it1 + 1;

	CSegyMeta::freeSegyDataArrayOnly(headTo->trArr, headTo->getnx()); // only need to free data

	memcpy((void *)&headTo->bh, (void *)&headFrom->bh, 400L);
	headTo->setnx(nxmaxOut);
	headTo->setnumsamp(numsampOut);
	headTo->setdt(headFrom->getdt());
//	int lenBytes = numsampOut * ((headFrom->bh.format != 3) ? 4 : 2);
	int lenBytes = numsampOut * 4;
	double t1 = it1*headFrom->getdt();

//	headTo->trArr = (segydynamic *)xmalloc(sizeof(segydynamic) * nxmaxOut);
	headTo->trArr = (segydynamic *)realloc1(headTo->trArr, nxmaxOut, sizeof(segydynamic));
	for (int ix = ix1; ix <= ix2; ix++) {
		headTo->trArr[ix - ix1].data = ealloc1float(numsampOut);
		memcpy((void *)&headTo->trArr[ix - ix1].tracl, (void *)&headFrom->trArr[ix].tracl, 240L);
		memcpy((void *)&headTo->trArr[ix - ix1].data[0], (void *)&headFrom->trArr[ix].data[it1], lenBytes);
		if (numsampOut != headFrom->getns()) {
			// since the trace length is different, we need to adjust the header
			register double fDelay = headFrom->trArr[ix].getTraceDelay() + t1;
			headTo->trArr[ix - ix1].setTraceDelayF(fDelay);
			headTo->trArr[ix - ix1].ns = (unsigned short)numsampOut;
		}
	}

	return true;
}

// before saving to segy file, we need to make sure the trace number is in order, otherwise it has portability problems
void SegyWrapper::sortTraceNum(HEAD_READ *headRead2)
{
	segy tr;
//	int sampByte = (headRead2->bh.format != 3) ? 4 : 2;
	int sampByte = 4;
	int n, m;
	int nx = headRead2->getnx();
	float *traces = ealloc1float(nx);
	int len = sampByte*headRead2->bh.hns;

	bool isSortNeeded = false;
	for (int i = 0; i < nx - 1; i++) {
		if (traces[i] > traces[i+1]) {
			isSortNeeded = true;
			break;
		}
	}
	if (isSortNeeded) {
		for (n = 0; n<nx - 1; n++) {
			for (m = n; m<nx; m++) {
				traces[m] = (float)headRead2->trArr[m].tracl;
			}
			m = getMinIndex(&traces[n], nx - n);

			// now m is not the index in trArr[], it must be added by an index shift of n! this a big programming trap!
			m += n;

			if (m > n) {
				// the range at m needs to swap with the trace at n. Note  trArr.data  is a separate pointer from trArr itself
				memcpy((void *)&tr, (void *)&headRead2->trArr[n], 240L);
				memcpy((void *)&tr.dat, (void *)&headRead2->trArr[n].data[0], len);
				memcpy((void *)&headRead2->trArr[n], (void *)&headRead2->trArr[m], 240L);
				memcpy((void *)&headRead2->trArr[n].data[0], (void *)&headRead2->trArr[m].data[0], len);
				memcpy((void *)&headRead2->trArr[m], (void *)&tr, 240L);
				memcpy((void *)&headRead2->trArr[m].data[0], (void *)&tr.dat, len);
			}
		} // for n
	} // if isSortNeeded

	free1float(traces);
}

bool SegyWrapper::saveToSegyIEEE(HEAD_READ *headRead2, CString outFile, unsigned char *ascHead)
{
	headRead2->bh.format = 5;
	return saveToSegyFile(headRead2, outFile, ascHead, FALSE);
}


bool SegyWrapper::saveToSegyFile(HEAD_READ *headRead2, CString outFile, int drawmode, CCoords::REDUCE *red2, double tmin, double tmax,
	unsigned char *ascHead, BOOL isIBMForced) {
	CFile fileout;
	if (!fileout.Open(outFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
		return false;
	}
	fileout.Write(ascHead, 3200L);

	double dt = headRead2->getdt();
	double odt = 1. / dt;
	int nsIn = headRead2->getns();
	int nsOut = (int)((tmax - tmin) * odt + 1.5);
	headRead2->setnumsamp(nsOut);

	// fix up for un-careful values, important
	bhed bh; // we do not want to change headRead2->bh
	memcpy(&bh, &headRead2->bh, 400);
	if (bh.format != 1 && bh.format != 5) bh.format = 1;
	if (isIBMForced)  bh.format = 1;
	fileout.Write(&bh, 400);
	int nxOut = headRead2->getnx();

	// note: after some processing, the traces may be re-organized, and the trace number field may not be in sequencial order.
	// so we need a fix here

	bool isTraceNumNormal = true;
	for (int nxStore = 0; nxStore < nxOut - 1; nxStore++) {
		if (headRead2->trArr[nxStore].tracl >= headRead2->trArr[nxStore + 1].tracl) {
			isTraceNumNormal = false;
			break;
		}
	}
	if (!isTraceNumNormal) {
		// now fix the tracl field
		for (int nxStore = 1; nxStore < nxOut; nxStore++) {
			headRead2->trArr[nxStore].tracl = headRead2->trArr[nxStore - 1].tracl + 1;
		}
	}

	segy tr;
	int j;
	float *trOut = (float *)alloc1float(nsOut);
	for (int nxStore = 0; nxStore < nxOut; nxStore++) {
		if (bh.format == 1) {
			float_to_ibm((int *)headRead2->trArr[nxStore].data, (int *)tr.dat.data, nsIn);
			// we always output as floating pt
		}
		else
			memcpy(tr.dat.data, headRead2->trArr[nxStore].data, nsIn * sizeof(float));

		// now need to calculate i1 that points to tmin + tReduced
		register double offset = headRead2->trArr[nxStore].getOffset();
		register double tReduced = CCoords::getReduceTimeEx2(offset, drawmode, red2);
		register double tDelay = headRead2->trArr[nxStore].getTraceDelay();
		register int i1 = (int)((tmin + tReduced - tDelay) * odt + 0.5);

		memset(trOut, 0, nsOut * sizeof(float));
		for (j = i1; j < nsIn; j++) {
			if (j>=0 && j - i1>=0 && j-i1 < nsOut)
				trOut[j - i1] = tr.dat.data[j];
		}

		headRead2->trArr[nxStore].setTraceDelayF(tmin + tReduced);
		fileout.Write(&headRead2->trArr[nxStore].tracl, 240L);
		fileout.Write(&trOut[0], nsOut * sizeof(float));
	} // for nxStore
	fileout.Close();
	free1float(trOut);

	return true;
}
	

bool SegyWrapper::saveToSegyFile(HEAD_READ *headRead2, CString outFile, 
	unsigned char *ascHead, BOOL isIBMForced)
{
	CFile fileout;
	if (!fileout.Open(outFile, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
		return false;
	}
	fileout.Write(ascHead, 3200L);

	// fix up for un-careful values, important
	bhed bh; // we do not want to change headRead2->bh
	memcpy(&bh, &headRead2->bh, 400);
	if (bh.format != 1 && bh.format != 5) bh.format = 1;
	if (isIBMForced)  bh.format = 1;
	fileout.Write(&bh, 400);
	int nxOut = headRead2->getnx();

	// note: after some processing, the traces may be re-organized, and the trace number field may not be in sequencial order.
	// so we need a fix here
	
	bool isTraceNumNormal = true;
	for (int nxStore = 0; nxStore < nxOut - 1; nxStore++) {
		if (headRead2->trArr[nxStore].tracl >= headRead2->trArr[nxStore + 1].tracl) {
			isTraceNumNormal = false;
			break;
		}
	}
	if (!isTraceNumNormal) {
		// now fix the tracl field
		for (int nxStore = 1; nxStore < nxOut; nxStore++) {
			headRead2->trArr[nxStore].tracl = headRead2->trArr[nxStore - 1].tracl + 1;
		}
	}

	segy tr;
	int nsOut = headRead2->getns();
	for (int nxStore = 0; nxStore < nxOut; nxStore++) {
		fileout.Write(&headRead2->trArr[nxStore].tracl, 240L);
		if (bh.format == 1) {
			float_to_ibm((int *)headRead2->trArr[nxStore].data, (int *)tr.dat.data, nsOut);
			// we always output as floating pt
		}
		else
			memcpy(tr.dat.data, headRead2->trArr[nxStore].data, nsOut*sizeof(float));

		fileout.Write(&tr.dat.data[0], nsOut*sizeof(float));
	} // for nxStore
	fileout.Close();

	return true;
}

bool SegyWrapper::headReadAssemble(HEAD_READ *headOut, HEAD_READ *headLeft, HEAD_READ *headRight)
{

	// note: trace numbers may frequently not be incrementing by 1, since we may skip some traces 
	//    during display. Then how do we assemble them together?
	// double odt = 1. / headOut->dt();
	int numsampOut = headOut->getns();
	int nxmaxOut = headLeft->getnx() + headOut->getnx() + headRight->getnx();
//	int sampSize = (headOut->bh.format != 3) ? 4 : 2;
	int sampSize = 4;
	int lenBytes = numsampOut * sampSize;
	int ix;

	HEAD_READ *out = new HEAD_READ();
	out->trArr = (segydynamic *)xmalloc(sizeof(segydynamic) * nxmaxOut);
	for (ix = 0; ix < nxmaxOut; ix++) {
		out->trArr[ix].data = ealloc1float(numsampOut);
	}

	// if we have different trace length, we have to pre-set 0 to the trace data
	if (headLeft->getns() != numsampOut || headRight->getns() != numsampOut) {
		for (ix = 0; ix < nxmaxOut; ix++) {
			memset(out->trArr[ix].data, 0, lenBytes);
		}
	}
	memcpy((void *)&out->bh, (void *)&headOut->bh, 400L);
	out->setnx(nxmaxOut);

	// now start the copying
	int ixCount = 0;
	int lenBytesLeft = (headLeft->getns() < numsampOut) ? headLeft->getns() * sampSize : lenBytes;
	for (ix = 0; ix < headLeft->getnx(); ix++) {
		memcpy((void *)&out->trArr[ixCount].tracl, (void *)&headLeft->trArr[ix].tracl, 240L);
		memcpy((void *)&out->trArr[ixCount].data[0], (void *)&headLeft->trArr[ix].data[0], lenBytesLeft);
		ixCount++;
	}

	for (ix = 0; ix < headOut->getnx(); ix++) {
		memcpy((void *)&out->trArr[ixCount].tracl, (void *)&headOut->trArr[ix].tracl, 240L);
		memcpy((void *)&out->trArr[ixCount].data[0], (void *)&headOut->trArr[ix].data[0], lenBytes);
		ixCount++;
	}

	int lenBytesRight = (headRight->getns() < numsampOut) ? headRight->getns() * sampSize : lenBytes;
	for (ix = 0; ix < headRight->getnx(); ix++) {
		memcpy((void *)&out->trArr[ixCount].tracl, (void *)&headRight->trArr[ix].tracl, 240L);
		memcpy((void *)&out->trArr[ixCount].data[0], (void *)&headRight->trArr[ix].data[0], lenBytesRight);
		ixCount++;
	}


	SegyWrapper::freeSegyArray(headOut);
	headOut->trArr = (segydynamic *)xmalloc(sizeof(segydynamic) * nxmaxOut);
	memcpy((void *)&headOut->bh, (void *)&out->bh, 400L);
	for (ix = 0; ix<nxmaxOut; ix++) {
		memcpy((void *)&headOut->trArr[ix].tracl, (void *)&out->trArr[ix].tracl, 240L);
		headOut->trArr[ix].data = ealloc1float(numsampOut);
		memcpy((void *)&headOut->trArr[ix].data[0], (void *)&out->trArr[ix].data[0], lenBytes);
	}
	SegyWrapper::freeSegyArray(out); 
	delete out;  // only because out is a pointer. We need to delete it

	return true;
}

// this is depreciated
bool SegyWrapper::headReadAssemble(HEAD_READ *headBase, HEAD_READ *headOut,
	HEAD_READ *headLeft,
	HEAD_READ *headRight,
	HEAD_READ *headTop,
	HEAD_READ *headBot,
	//	int trace1, int trace2, 
	double t1, double t2)
{
	if (
		(headTop->getnx() > 0 && headBase->getnx() != headTop->getnx())
		||
		(headBot->getnx() > 0 && headBase->getnx() != headBot->getnx())
		)
		return false;


	// note: trace numbers may frequently not be incrementing by 1, since we may skip some traces 
	//    during display. Then how do we assemble them together?
	//    We will need to build the data arrays and trace headers based on
	//    5 HEAD_READ structs
	double odt = 1. / headBase->getdt();
	int numsampOut = (int)ceil((t2 - t1) * odt + 1.5);
	if (numsampOut < headBase->getns())
		return false;

	int nxmaxOut = headLeft->getnx() + headBase->getnx() + headRight->getnx();
	int ix;
//	int sampSize = (headBase->bh.format != 3) ? 4 : 2;
	int sampSize = 4;
	int lenBytes = numsampOut * sampSize;


	if (headOut->trArr) {
		for (ix = 0; ix<headOut->getnx(); ix++) {
			if (headOut->trArr[ix].data != NULL) {
				free1float(headOut->trArr[ix].data);
				headOut->trArr[ix].data = NULL;
			}
		}
		free1(headOut->trArr);  // note: if nx==0, I cannot free trArr since it would give me an exception.
	}
	headOut->trArr = (segydynamic *)xmalloc(sizeof(segydynamic) * nxmaxOut);
	for (ix = 0; ix < nxmaxOut; ix++) {
		headOut->trArr[ix].data = ealloc1float(numsampOut);
		memset((void *)&headOut->trArr[ix].data[0], (int) '\0', lenBytes);
	}


	memcpy((void *)&headOut->bh, (void *)&headBase->bh, 400L);
	headOut->setnx(nxmaxOut);
	headOut->setnumsamp(numsampOut);
	headOut->setdt(headBase->getdt());

	// now start the copying
	double deltat;
	int it1;
	int lenCopy;
	int ixCount = 0;
	if (headLeft->getns() == headOut->getns()) {
		lenCopy = headLeft->getns()*sampSize;
		for (ix = 0; ix < headLeft->getnx(); ix++) {
			memcpy((void *)&headOut->trArr[ixCount].tracl, (void *)&headLeft->trArr[ix].tracl, 240L);
			memcpy((void *)&headOut->trArr[ixCount].data[0], (void *)&headLeft->trArr[ix].data[0], lenCopy);
			ixCount++;
		}
	}

	// for simplicity, we assume headOut is always longer than headBase
	for (ix = 0; ix < headBase->getnx(); ix++) {
		memcpy((void *)&headOut->trArr[ixCount].tracl, (void *)&headBase->trArr[ix].tracl, 240L);
		deltat = headBase->trArr[ix].getTraceDelay() - t1;
		it1 = (int)(fabs(deltat) * odt + 0.5);
		lenCopy = headBase->getns();
		if (it1 + lenCopy >headOut->getns())
			lenCopy = headOut->getns() - it1;
		if (lenCopy > 0)
			memcpy((void *)&headOut->trArr[ixCount].data[it1], (void *)&headBase->trArr[ix].data[0], lenCopy*sampSize);

		// copy headTop
		if (headTop->getnx() > 0) {
			deltat = headTop->trArr[ix].getTraceDelay() - t1;
			it1 = (int)(fabs(deltat) * odt + 0.5);
			lenCopy = headTop->getns();
			if (it1 + lenCopy > headOut->getns())
				lenCopy = headOut->getns() - it1;
			if (lenCopy > 0)
				memcpy((void *)&headOut->trArr[ixCount].data[it1], (void *)&headTop->trArr[ix].data[0], lenCopy*sampSize);
		}

		// copy headBot
		if (headBot->getnx() > 0) {
			deltat = headBot->trArr[ix].getTraceDelay() - t1;
			it1 = (int)(fabs(deltat) * odt + 0.5);
			lenCopy = headBot->getns();
			if (it1 + lenCopy > headOut->getns())
				lenCopy = headOut->getns() - it1;
			if (lenCopy > 0)
				memcpy((void *)&headOut->trArr[ixCount].data[it1], (void *)&headBot->trArr[ix].data[0], lenCopy*sampSize);
		}

		ixCount++;
	}

	if (headRight->getns() == headOut->getns()) {
		lenCopy = headRight->getns()*sampSize;
		for (ix = 0; ix < headRight->getnx(); ix++) {
			memcpy((void *)&headOut->trArr[ixCount].tracl, (void *)&headRight->trArr[ix].tracl, 240L);
			memcpy((void *)&headOut->trArr[ixCount].data[0], (void *)&headRight->trArr[ix].data[0], lenCopy);
			ixCount++;
		}
	}


	for (ix = 0; ix < nxmaxOut; ix++) {
		headOut->trArr[ix].setTraceDelayF(t1);
		headOut->trArr[ix].ns = (unsigned short)numsampOut;
	}

	return true;
}


bool SegyWrapper::openCacheDepre(int trace1, int trace2, int numTraceDesired, HEAD *head,
	SegyWrapper **cachePSegyWraps, int numCache, bool isCacheReadOnly,
	int traceStep, double dyMin)
{
	this->m_cacheTimeFile = getFileModifiedTime(this->m_f);
	m_formatInFile2 = head->formatFile;
	m_isUnixInFile = head->isUnixFile;
	m_numsampInFile = head->numsampFiles;
	m_dtInFile = head->dtFile;

	if (trace2 <= trace1) trace2 = trace1 + 1;
	if (traceStep < 1) traceStep = 1;
	if (dyMin < head->dtFile) dyMin = head->dtFile;
	int ic;  // index for caching
	int ix;


	// first, check if the cache already has the desired data.
	// if yes, get the data
	// if the cache contains partial data, get it and update the data with newer parts in the cache
	// if not, get the data, free old, and put the new to the cache
	/*	int  m_cdp;  // 0=all, !=0 will read only 1 cdp (assume traces of 1 cdp are always close to each other.
	double m_tmin; // note: this is the absolute time, not reduced (e.g. reduction velocity) by any means
	double m_tmax; // time window. If m_tmax==m_tmin, Seiswide will take the entire trace, discard the m_tmin. */
	int num;
	for (ic = 0; ic < numCache; ic++) {
		num = 0;
		if (cachePSegyWraps[ic]->m_f.IsEmpty() ||
			(num = cachePSegyWraps[ic]->headRead.getnx()) < 1)
			continue;

		register bool isSameRes = m_f.CompareNoCase(cachePSegyWraps[ic]->m_f) == 0 &&
			traceStep >= cachePSegyWraps[ic]->m_cacheTraceStep &&
			dyMin >= cachePSegyWraps[ic]->m_cacheDyMin &&
			m_cdp == cachePSegyWraps[ic]->m_cdp;
		if (!isSameRes) continue;

		register int cacheTrc1 = cachePSegyWraps[ic]->headRead.trArr[0].tracl;
		register int cacheTrc2 = cachePSegyWraps[ic]->headRead.trArr[num - 1].tracl;
		register double cacheTmin = cachePSegyWraps[ic]->m_tmin;
		register double cacheTmax = cachePSegyWraps[ic]->m_tmax;
		register bool isCacheComplete =
			cacheTrc1 <= head->traceMin() &&
			cacheTrc2 >= head->traceMax() &&
			cacheTmin <= head->tminFile &&
			cacheTmax >= head->tmaxFile;
		register bool isSameResData =
			trace1 >= cacheTrc1 && trace2 <= cacheTrc2 &&
			m_tmin >= cacheTmin && m_tmax <= cacheTmax &&
			cachePSegyWraps[ic]->m_cacheTimeFile == m_cacheTimeFile;

		if (isCacheComplete || isSameResData) {
			// found it from the cache
			register double x;
			register int ix1 = 0;
			for (ix = 0; ix < cachePSegyWraps[ic]->headRead.getnx(); ix++) {
				x = cachePSegyWraps[ic]->headRead.trArr[ix].tracl;
				if (x >= trace1) {
					ix1 = ix;
					break;
				}
			}
			int ix2 = cachePSegyWraps[ic]->headRead.getnx() - 1;
			for (ix = cachePSegyWraps[ic]->headRead.getnx() - 1; ix >= 0; ix--) {
				x = cachePSegyWraps[ic]->headRead.trArr[ix].tracl;
				if (x <= trace2) {
					ix2 = ix;
					break;
				}
			}
			double odt = 1. / cachePSegyWraps[ic]->headRead.getdt();
			int it1 = (int)((m_tmin - cachePSegyWraps[ic]->m_tmin) * odt + 0.5);
			int it2 = cachePSegyWraps[ic]->headRead.getns() - 1 -
				(int)((cachePSegyWraps[ic]->m_tmax - m_tmax) * odt + 0.5);
			SegyWrapper::headReadCopy(&cachePSegyWraps[ic]->headRead, &this->headRead,
				ix1, ix2, it1, it2);
			//	SegyWrapper::headReadCopy(&cachePSegyWraps[ic]->headRead, &this->headRead, cachePSegyWraps[ic]->headRead.nx());
			// cachePSegyWraps[ic]->m_cacheTimeLoaded = CTime::GetCurrentTime();  // this cache should be upgraded

			return true;
		}

	}

	// if here, the cache is not found
	bool isTrue;
	if (fabs(m_tmin - head->tminFile)<0.00001 && fabs(m_tmax - head->tmaxFile)<0.00001)
		isTrue = openTraceFlatWhole(0);
		// when the whole section is needed, better to read flat mode, otherwise problem can occur when starting time varies drastically
	else
		isTrue = openTrace(trace1, trace2, numTraceDesired, head, traceStep, dyMin);
//	bool isTrue = openTrace(trace1, trace2, numTraceDesired, head, traceStep, dyMin);
	if (isTrue && !isCacheReadOnly) {
		// now put the new data back to the cache pool
		// this->m_cacheTimeLoaded = CTime::GetCurrentTime();



		// first, find the oldest cache
		int iCacheSelected = getUnusedSegyCacheIndex(cachePSegyWraps, numCache);
		//int iCacheSelected = -1;
		//// note: at first, I didn't same segy file to be cached differently. But it should be allowed, 
		////       since a big display cannot show details, but a zoom up shows details without big picture
		////for (i = 0; i < numCache; i++) {
		////	if (m_f.CompareNoCase(cachePSegyWraps[i]->m_f) == 0) {
		////		iCacheSelected = i;
		////		break;
		////	}
		////}

		//if (iCacheSelected < 0) {
		//	iCacheSelected = (int)MIL;
		//	for (ic = 0; ic < numCache; ic++) {
		//		// m_cacheTimeLoaded == 0 means that the cache item is not used
		//		if (cachePSegyWraps[ic]->m_cacheTimeLoaded == 0 ||
		//			cachePSegyWraps[ic]->m_cacheTimeLoaded < iCacheSelected) {
		//			iCacheSelected = ic;
		//			break;
		//		}
		//	}
		//}
		//if (iCacheSelected < 0 || iCacheSelected >= numCache) iCacheSelected = 0;
		//SegyWrapper::freeSegyArray(&cachePSegyWraps[iCacheSelected]->headRead);



		bool isSuc = SegyWrapper::headReadCopy(&this->headRead, &cachePSegyWraps[iCacheSelected]->headRead, this->headRead.getnx(), false);
		if (isSuc) {
			cachePSegyWraps[iCacheSelected]->m_f = this->m_f;
			cachePSegyWraps[iCacheSelected]->m_cacheTraceStep = traceStep;
			cachePSegyWraps[iCacheSelected]->m_cacheDyMin = dyMin;
			cachePSegyWraps[iCacheSelected]->m_cacheTimeFile = this->m_cacheTimeFile;
			cachePSegyWraps[iCacheSelected]->m_cdp = this->m_cdp;
			cachePSegyWraps[iCacheSelected]->m_tmin = this->m_tmin;
			cachePSegyWraps[iCacheSelected]->m_tmax = this->m_tmax;
		}
	}

	return isTrue;
}




const float SegyWrapper::getAvgShotIntervalPrestack(int cdpi) {
	if (cdpi < 1) cdpi = 1;
	int n, i;
	float *ranges = ealloc1float(headRead.getnx());
	int count = 0;
	for (n = 0; n < this->headRead.getnx(); n++) {
		if (headRead.trArr[n].cdpt == cdpi) {
			ranges[count] = this->headRead.trArr[n].getDistance();
			count++;
		}
	}

	sort1(&ranges[0], count);

	for (i = 0; i < count - 1; i++)
		ranges[i] = ranges[i + 1] - ranges[i];
	n = getMedian3(&ranges[0], count - 1);
	float trcInterval = ranges[n];
	free1float(ranges);

	return trcInterval;
}




int SegyWrapper::openCacheSmart(int trace1, int trace2, HEAD *head,
	SegyWrapper **cachePSegyWraps, int numCache, bool isCacheReadOnly,
	int traceStep, double dyMin)
{
	this->m_cacheTimeFile = getFileModifiedTime(this->m_f);

	if (trace2 <= trace1) trace2 = trace1 + 1;
	if (traceStep < 1) traceStep = 1;
	if (dyMin < head->dtFile) dyMin = head->dtFile;
	int ic;  // index for caching
	int ix;

	m_formatInFile2 = head->formatFile;
	m_isUnixInFile = head->isUnixFile;
	m_numsampInFile = head->numsampFiles;
	m_dtInFile = head->dtFile;

	// first, check if the cache already has the desired data.
	// if yes, get the data
	// if the cache contains partial data, get it and update the data with newer parts in the cache
	// if not, get the data, free old, and put the new to the cache
	register double bakTmin = this->m_tmin;  // we will need to temporarily change m_tmin/m_tmax
	register double bakTmax = this->m_tmax;
	this->m_tmin = head->tminFile;
	this->m_tmax = head->tmaxFile;
	for (ic = 0; ic < numCache; ic++) {
		register int nxmaxCache = 0;
		register SegyWrapper *aWrap = cachePSegyWraps[ic];

		if (aWrap->m_cacheTimeFile==0 || aWrap->m_cacheTimeFile != m_cacheTimeFile) {
			if (this->m_f.CompareNoCase(aWrap->m_f) == 0) {
				// since this segy file is already changed. We need to free up the cache memory for this file. 
				// It has happened I had 2GB cache file, and I change that file but the cache still holds old data
				SegyWrapper::freeSegyArray(&aWrap->headRead);
				aWrap->m_cacheTimeFile = 0;
				aWrap->m_f = BLANK;
			}
			continue; // this will save CPU time
		}

		if (aWrap->m_f.IsEmpty() ||
			(nxmaxCache = aWrap->headRead.getnx()) < 1 ||
			aWrap->m_cacheTimeFile != m_cacheTimeFile)
			continue;
		if (m_f.CompareNoCase(aWrap->m_f) != 0) continue;

		if (aWrap->m_isEntireTrace) {
			SegyWrapper::headReadCopy(&aWrap->headRead, &this->headRead, 0, false);
			return 2;
		}

		register bool isSameStep = traceStep >= aWrap->m_cacheTraceStep
			&& m_cdp == aWrap->m_cdp;
		if (!isSameStep) continue;

		register int cacheTrc1 = aWrap->headRead.trArr[0].tracl;
		register int cacheTrc2 = aWrap->headRead.trArr[nxmaxCache - 1].tracl;
		register bool isCachedAll =
			cacheTrc1 <= head->traceMin() &&
			cacheTrc2 >= head->traceMax() &&
			aWrap->m_tmin <= head->tminFile &&
			aWrap->m_tmax >= head->tmaxFile;
			// here, if isCachedAll is true, no more data than what the cache holds  exist.
			// This will determine if the program will continue to look for more data other than the cache

		register bool isDataWithin = trace1 >= cacheTrc1 
			&& trace2 <= cacheTrc2 
			&& bakTmin >= aWrap->m_tmin
			&& bakTmax <= aWrap->m_tmax;

		// dyMin is typically 0.0240756 or 0.002, so we need it to be within 1 ms for a match
		register bool isSampRateAcceptable = 
			isCachedAll && isDataWithin
			?
			dyMin >= aWrap->m_cacheDyMin || dyMin >= aWrap->m_cacheDyDesired
			:
			fabs(dyMin - aWrap->m_cacheDyMin)<0.001;
		// note: here if isDataWithin!=true, we do not want to use a cache that of different samprate 
		if (!isSampRateAcceptable) continue;


		if (!isCachedAll && !isDataWithin) {
			// now, we have the cache available, but need adjustment for its content
			HEAD_READ headResLeft;
			HEAD_READ headResRight;

			// if the cache is already covering the complete data indicated in *head, we do not update
			if (trace1 < cacheTrc1 && cacheTrc1 > head->traceMin()) {
				// starting trace is left of desired trace number, for speeding up
				openTrace(2 * trace1 - cacheTrc1, cacheTrc1 - 1, 0, head, traceStep, dyMin);
				// exact num of output traces may be equal or smaller than desired
				SegyWrapper::headReadCopy(&headRead, &headResLeft, headRead.getnx(), false);
			}

			if (trace2 > cacheTrc2 && cacheTrc2 < head->traceMax()) {
				// In the new reading, we want to skip the last trace of cache
				openTrace(cacheTrc2 + 1, 2*trace2 - cacheTrc2, 0, head, traceStep, dyMin);  // read double the required num of traces
				SegyWrapper::headReadCopy(&headRead, &headResRight, headRead.getnx(), false);
			}

			if (headResLeft.getnx() > 0 || headResRight.getnx() > 0) {
				// note: the following cache pool update may be expensive
				// SegyWrapper::headReadCopy(&cachePSegyWraps[ic]->headRead, &headBase, cachePSegyWraps[ic]->headRead.nx());
				SegyWrapper::headReadAssemble(&aWrap->headRead, &headResLeft, &headResRight);
			}

			SegyWrapper::freeSegyArray(&headResLeft);
			SegyWrapper::freeSegyArray(&headResRight);
		}
		

		// now the same data can be copied from a smaller area of the cache, 
		// indexed as [ix1, ix2] and  [it1, it2]
		register double odt = 1. / aWrap->headRead.getdt();
		register int ix1 = 0;
		register int ix2 = aWrap->headRead.getnx() - 1;
		register double x;
		for (ix = 0; ix < aWrap->headRead.getnx(); ix++) {
			x = aWrap->headRead.trArr[ix].tracl;
			if (x >= trace1) {
				ix1 = ix;
				break;
			}
		}
		for (ix = aWrap->headRead.getnx() - 1; ix >= 0; ix--) {
			x = aWrap->headRead.trArr[ix].tracl;
			if (x <= trace2) {
				ix2 = ix;
				break;
			}
		}

		if (ix1 < 0 || ix2<0 || ix2 >= aWrap->headRead.getnx()) {
			// if here, the indexes are problemlamic. We just take the entire cache
			ix1 = 0;
			ix2 = aWrap->headRead.getnx();
		}

		register int it1, it2;
		if (aWrap->m_isEntireTrace) {
			it1 = 0;
			it2 = aWrap->headRead.getns() - 1;
		}
		else {
			it1 = (int)((m_tmin - aWrap->m_tmin) * odt + 0.5);
			it2 = aWrap->headRead.getns() - 1 -
				(int)((aWrap->m_tmax - m_tmax) * odt + 0.5);

			if (it1 < 0 || it2<0 || it2 >= aWrap->headRead.getns()) {
				// if here, the indexes are problemlamic. We just take the entire cache
				it1 = 0;
				it2 = aWrap->headRead.getns();
			}
		}

		SegyWrapper::headReadCopy(&aWrap->headRead, &this->headRead,
			ix1, ix2, it1, it2);
		return 2;
	}

	// if here, the cache is not found
	double ddx = (trace2 - trace1)*0.5;
	trace1 -= (int)ddx;
	trace2 += (int)ddx;

	double ddt = (this->m_tmax - this->m_tmin)*0.5;
	this->m_tmin -= ddt;
	if (this->m_tmin < head->tminFile) this->m_tmin = head->tminFile;
	this->m_tmax += ddt;
	if (this->m_tmax > head->tmaxFile) this->m_tmax = head->tmaxFile;

	bool isTrue = openTrace(trace1, trace2, 0, head, traceStep, dyMin);
	int iCacheSelected = getUnusedSegyCacheIndex(cachePSegyWraps, numCache);
	if (isTrue) {
		// now put the new data back to the cache pool
		//this->m_cacheTimeLoaded = CTime::GetCurrentTime();
		//SegyWrapper::freeSegyArray(&cachePSegyWraps[iCacheSelected]->headRead);
		register SegyWrapper *aWrap = cachePSegyWraps[iCacheSelected];
		bool isSuc = SegyWrapper::headReadCopy(&this->headRead, &aWrap->headRead, this->headRead.getnx(), false);
		if (isSuc) {
			aWrap->m_f = this->m_f;
			aWrap->m_cacheTraceStep = traceStep;
			aWrap->m_cacheDyDesired = dyMin;
			aWrap->m_cacheDyMin = headRead.getdt(); // actual dt usually has changed slightly
			aWrap->m_cacheTimeFile = this->m_cacheTimeFile;
			aWrap->m_cdp = this->m_cdp;
			aWrap->m_tmin = this->m_tmin;
			aWrap->m_tmax = this->m_tmax;
			aWrap->m_isEntireTrace = this->m_isEntireTrace;
			// note: no need to cache the trace1 and trace2 since they can be easily calculated from cache array. 
			// But xmin, xmax, and tmin and tmax are not easy to be calculated, so we cache them
		}
	}

	bool isMemoryTooMuch = getMemoryUsagePhysPercent() > 75.;
	if (isMemoryTooMuch) {
		int iCache2Free = getOldestSegyCacheIndex(cachePSegyWraps, numCache);
		SegyWrapper::freeSegyArray(&cachePSegyWraps[iCache2Free]->headRead);
		cachePSegyWraps[iCache2Free]->m_cacheTimeFile = 0;
	}

	return isTrue;
}



// Main function to read trace samples
// dyMin is the min resolution desired, or the time interval between 2 adjacent pixels. 
// e.g. if data sample interval is 0.004 sec, and the pixel resolution is 0.02 sec, we can only output 
// 1 sample every 4 samples.
// 
// no need to update the header info that already exist in *head, unless they need to update
bool SegyWrapper::openSeqTrace(int trace1, int trace2, int numTraceDesired, HEAD *head, int traceStep, double dyMin)
{
	// trace1 == 1 means the first trace!
	int i1 = (trace1>0 && trace1<=head->numTraces) ?
		head->traces[trace1-1] : head->traceMin();

	int i2 = (trace2>0 && trace2 <= head->numTraces) ?
		head->traces[trace2 - 1] : head->traceMax();

	return openTrace(i1, i2, numTraceDesired, head, traceStep, dyMin);
}

// head->aveAmp amd head->maxAmp  will be calculated 
// in a function that calls openTrace() to supply 100 traces for calculation
// if numTraceDesired==0, no need to control how many traces are within [trace1, trace2]
bool SegyWrapper::openTrace(int trace1, int trace2, int numTraceDesired, HEAD *head, int traceStep, double dyMin)
{
	if (!isFileExistNotDirectory(this->m_f)) return false;
	m_formatInFile2 = head->formatFile;
	if (m_formatInFile2 < 0 || m_formatInFile2 > 5) {
		return false;
	}

	

	m_isEntireTrace = (isFloatEqual(this->m_tmin, head->tminFile) && isFloatEqual(this->m_tmax, head->tmaxFile))
		||
		this->m_tmin >= this->m_tmax-0.001;
	if (m_isEntireTrace) {
		this->m_tmin = head->tminFile;
		this->m_tmax = head->tmaxFile;
	}
	else {
		if (head->isStartTimeChangeDrastic()) {
			bool isTrue = openTraceFlatWhole(0);
			// when the whole section is needed, better to read flat mode, otherwise problem can occur when starting time varies drastically
			this->m_tmin = head->tminFile; // the actual data reading is changed and therefore needs to reflect here
			this->m_tmax = head->tmaxFile;
			return isTrue;
		}
		if (m_tmin != m_tmax && (m_tmax < head->tminFile || m_tmin > head->tmaxFile))
			return false;  // no data possible for this

		if (fabs(m_tmax - m_tmin) < 0.01) {
			this->m_tmin = head->tminFile;
			this->m_tmax = head->tmaxFile; // this prevents the user to specify a too big tmax to crash
		}
		else {
			if (this->m_tmax > head->tmaxFile)
				this->m_tmax = head->tmaxFile; // this prevents the user to specify a too big tmax to crash
			if (this->m_tmin < head->tminFile)
				this->m_tmin = head->tminFile;
		}
	}

	//// after the above adjustments, there is a chance for memory exception with too little points.
	//if (this->m_tmin != this->m_tmax && this->m_tmax - this->m_tmin < 0.01) {
	//	this->m_tmax = this->m_tmin + 0.01;
	//}

	m_isUnixInFile = head->isUnixFile;
	m_numsampInFile = head->numsampFiles;
	m_dtInFile = head->dtFile;

	if (trace2 < head->traceMin() || trace1>head->traceMax()) {
		// this would imply we will not get any traces, so do a fix
		trace1 = head->traceMin();
		trace2 = trace1 + numTraceDesired - 1;
	}

	if (trace2 == trace1 +1) trace2 = trace1 + 1; // this has happened, causing crash


	if (headRead.trArr) {
		freeSegyArray(&headRead);
	}

	// condition some params
	if (trace2 <= trace1) trace2 = trace1;
	if (traceStep < 1) traceStep = 1;
	this->m_cacheTraceStep = traceStep;
	if (dyMin < head->dtFile) dyMin = head->dtFile;
	this->m_cacheDyMin = dyMin;

	CFile file;
	segy tr;

	int j;
	if (!file.Open(m_f, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
		return false;
	}

	headRead.setAscHead((char *)head->ascHead);
	file.Seek(3200, CFile::begin); // no need to read from file again about the asc head
	file.Read(&headRead.bh, 400L);
	
	if (head->isUnixFile) SwapTapeHeader(&headRead.bh);
	if (headRead.bh.hdt == 0) headRead.bh.hdt = headRead.bh.dto;
	double odt = (double)MIL / (double)headRead.bh.hdt;
	double dt = 1. / odt;  // duplicate with head->dt
	int sampByte = (headRead.bh.format != 3) ? 4 : 2;
	int lenIn = headRead.bh.hns * sampByte;

	// fix up for *head info
	if (head->numsampFiles <= 0) {
		// this could happen if the file cache is old version or it is in image only format
		head->numsampFiles = headRead.bh.hns;
		head->dtFile = dt;
		head->formatFile = headRead.bh.format;
	}


	int numsamp = headRead.bh.hns;
	unsigned short numsampOut; //  

	// we have to simply use the same length
	if (this->m_isTimeAlign) {
		// we cannot have time align and whole trace set at the same time. so we will ignore whole trace option
		if (m_isEntireTrace) {
			// only take the time limits of the first trace
			this->m_tmin = head->tminArr[0];
			this->m_tmax = this->m_tmin + (numsamp - 1)*dt;
		}
		numsampOut = (unsigned short)ceil((m_tmax - m_tmin) * odt) + 1;
	}
	else {
		// if time align is not required, we try to make the output as close as the existing whole trace
		numsampOut = m_isEntireTrace ?
				numsamp : 
				(unsigned short)ceil((m_tmax - m_tmin) * odt) + 1;
	}

	// there is a case: numsampFiles=15001, but numsampOut=15002
	if (numsampOut < 4 || numsampOut > head->numsampFiles) {
		numsampOut = head->numsampFiles;
		if (numsampOut < 4) {
			numsampOut = 4;
		}
	}
	this->m_tmax = this->m_tmin + (numsampOut - 1)*dt;

	headRead.setnumsamp(numsampOut);
	// int sampByteOut = sizeof(float);  // always 4 bytes per sample in output
	int lenOut = numsampOut * sizeof(float);

	// test on the first 3 traces in case of irregular segy not using 
	// the first 4 bytes for storing trace numbers
	//bool isNumberedIrregular = head->isTraceNumberIrregular();

	int stepCount = 0;
	int nxmax = 0;
	for (int i = 0; i < head->numTraces; i++) {
		if (this->m_cdp > 0) {
			if (head->cdp[i] < this->m_cdp) continue;
			else if (head->cdp[i] > this->m_cdp) break;
		}
		if (head->traces[i] > 0) {
			if (head->traces[i]<trace1 || head->traces[i]>trace2) //unwanted traces.
				continue;
		}
		if (stepCount > 1) {
			stepCount--;
			continue;
		}
		else stepCount = traceStep;
		nxmax++;
		if (numTraceDesired > 0 && nxmax >= numTraceDesired) break;
	}
	headRead.setnx(nxmax);

	if (headRead.getnx() < 1 || headRead.getnx() > MIL) {
		file.Close();
		return false;
	}



	if (headRead.getnx() < 2) {
		headRead.setnx(2);
		trace2 += 1;
	}
	headRead.trArr = (segydynamic *)xmalloc(sizeof(segydynamic) * headRead.getnx());
//	size_t numsampArraySize = (size_t)(ceil(numsampOut * 0.5) * 2.0);   // this trick does not help with memory issue
//	size_t numsampArraySize = numsampOut;
	for (j = 0; j < headRead.getnx(); j++) {
		if (this->m_isHeadersOnly) {
			headRead.trArr[j].data = NULL;
		}
		else {
			headRead.trArr[j].data = ealloc1float(numsampOut);
			if (ERROR == headRead.trArr[j].data) {
				freeSegyArray(&headRead);
				file.Close();
				AfxMessageBox(_T("Sorry, failed to allocate memory for loading all traces into memory."));
				return false;
			} // if
		} // else
	} // for j

	//float *trcdat = (float *) alloc1float(numsampOut);
	int nxStore = 0;
	int nxSeq = 0;
	stepCount = 0;
	float zeroValue = head->isSegyBitmap() ? 255.f : 0.f;
	// NOTE: we want to output a group of traces alighned for proper processing such as coherency mixing
	//       so if the output tmin=-2 sec, and we have no data before 0 sec, we still need to pad zeros at each trace begingning
	for (int i = 0; i < head->numTraces; i++) {
		if (this->m_cdp > 0) {
			if (head->cdp[i] < this->m_cdp) continue;
			else if (head->cdp[i] > this->m_cdp) break;
		}

		if (head->traces[i]<trace1 || head->traces[i]>trace2) //unwanted traces.
			continue;
		if (stepCount > 1) {
			stepCount--;
			continue;
		}
		else stepCount = traceStep;

		file.Seek(head->pDataArr[i] - 240, CFile::begin);
		file.Read(&tr, 240);  // read the rest of trace header
		if (head->isUnixFile) {
			SwapTraceHeader(false, false, false, &tr);
		}

		
		//// we do not want zero trace number, since it will cause problems
		//// head->traces[] has fixed a few odds so we have to use it
		//if (head->m_isUseSeqTraceNumber)
		//	tr.tracl = head->traces[i];  
		//// there is a case when tr.tracl is not zero, and not in proper ordering
		//// even head.traces[] already fixed it, we do not want to follow it here
		tr.tracl = head->traces[i];  // head->traces[] has fixed a few odds so we have to use it


		int j0 = 0;
		double actualDelay = head->tminArr[i];
		if (!m_isEntireTrace) {
			j0 = (int)myRound((m_tmin - head->tminArr[i]) * odt);
			// now j0 points to  m_tmin for array tr.data[] exactly!
			// NOTE:  if j0<0 and trace starts from zero time, it means we want extra zero padding at beginning of data
			//        so we need to pre-calculate output data index beginning j1 and data end index j2
			//register long j1 = (head->tminArr[i] > m_tmin)  ?  myRound((head->tminArr[i] - m_tmin) * odt)  :  0;
			//register long numCopy = min(numsamp-j0, numsampOut);
			actualDelay += j0 * dt;
			// this is supposed to be m_tmin, but due to wrap-around error, we need to re-calculate
		}


		// now deal with trace data
		if (!this->m_isHeadersOnly) {
			if (head->isSegyBitmap()) {
				for (j = 0; j < numsampOut; j++)
					headRead.trArr[nxStore].data[j] = zeroValue;
				// if I set it to zero here, the empty areas will be all black
			}
			else
				memset((void *)&headRead.trArr[nxStore].data[0], (int) '\0', lenOut);
			if (file.Read(&tr.dat.data[0], lenIn) == (UINT)(lenIn)) {
				if (head->isUnixFile) {
					SwapN(&tr.dat, headRead.bh.format, numsamp);  // trace header was already swapped
				}

				int jj;

				if (headRead.bh.format == 3) {
					for (j = 0; j < numsampOut; j++) {
						jj = j0 + j;
						if (jj >= 0 && jj <= numsamp - 1)
							headRead.trArr[nxStore].data[j] = tr.dat.datashort[jj];
					}
				}
				else if (headRead.bh.format == 1 || headRead.bh.format == 5) {
					if (headRead.bh.format == 1)
						ibm_to_float((int *)tr.dat.data, (int *)tr.dat.data, (int)numsamp);

					for (j = 0; j < numsampOut; j++) {
						jj = j0 + j;
						if (jj >= 0 && jj <= numsamp - 1)
							headRead.trArr[nxStore].data[j] = tr.dat.data[jj];
					}
				}
				else if (headRead.bh.format == 2) {
					for (j = 0; j < numsampOut; j++) {
						jj = j0 + j;
						if (jj >= 0 && jj <= numsamp - 1)
							headRead.trArr[nxStore].data[j] = (float)tr.dat.dataint[jj];
					}
				}
				else break;

				for (j = 0; j < numsampOut; j++) {
					if (_isnan(headRead.trArr[nxStore].data[j]))
						headRead.trArr[nxStore].data[j] = zeroValue;
				}
			}
		}  // if !m_isHeadersOnly

		tr.setTraceDelayF(actualDelay);
		tr.ns = numsampOut;  // this must be set after SwapN(&tr, ) is called, since it uses tr.ns inside the function
		// assign only the trace header to the trArr[]
		memcpy((void *)&headRead.trArr[nxStore], (void *)&tr, 240L);



		nxStore++;
		if (nxStore >= headRead.getnx()) break;
	} // for
	file.Close();
	//free(trcdat);

	// now remove any array elements not in use
	if (nxStore < headRead.getnx()) {
		for (j = nxStore; j < headRead.getnx(); j++) {
			if (headRead.trArr[j].data != NULL) {
				free1float(headRead.trArr[j].data);
				headRead.trArr[j].data = NULL;
			}
		} // for
		headRead.setnx(nxStore);
	} // if


	int resampFactor = (int)myRound(dyMin / dt);
	if (myRound(headRead.bh.hdt * resampFactor) > 65500) {
		// too big, we need to reduce it
		resampFactor = (int)floor((double)headRead.bh.hdt / (double)65500);
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
			for (j = 0; j < headRead.bh.hns; j++) {
				if (count == 0) count = resampFactor;
				if (count == resampFactor) {
					this->headRead.trArr[nxStore].data[jNew] = this->headRead.trArr[nxStore].data[j];
					jNew++;
				}
				count--;
			}
		} // for nxStore

		dt *= (double)resampFactor;
		headRead.bh.hdt = (unsigned short)myRound(dt*MIL);
		numsampOut = jNew;

		for (nxStore = 0; nxStore < this->headRead.getnx(); nxStore++) {
			this->headRead.trArr[nxStore].ns = numsampOut;
			this->headRead.trArr[nxStore].dt = headRead.bh.hdt;
		} // for nxStore
		// resampFactor = 1;  // no more resamp adjustment!
	} // if


	// now updating the headers
	headRead.setdt(dt);
	headRead.setnumsamp(numsampOut);


	headRead.bh.format = 5;  
	// the actual data arrays inside headRead are actually IEEE floating point, regardless of the original format in file.
	// to avoid issues, we force headRead.bh.format =5.
	// The original format in file can be 3 which is 2 bytes short integer.

	return true;
}

// ????
bool SegyWrapper::openTraceFlat2_deprec(int trace1, int trace2, int numTraceDesired, HEAD *head, int traceStep, double dyMin)
{
	if (!isFileExistNotDirectory(this->m_f)) return false;
	m_formatInFile2 = head->formatFile;
	m_isUnixInFile = head->isUnixFile;
	m_numsampInFile = head->numsampFiles;
	m_dtInFile = head->dtFile;

	// condition some params
	if (trace2 < head->traceMin() || trace1>head->traceMax()) {
		// this would imply we will not get any traces, so do a fix
		trace1 = head->traceMin();
		trace2 = trace1 + numTraceDesired - 1;
	}
	if (trace2 <= trace1) trace2 = trace1 + 1;
	if (traceStep < 1) traceStep = 1;
	if (dyMin < MINAMP) dyMin = MINAMP;

	CFile file;
	segy tr;

	int j;
	if (!file.Open(m_f, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
		return false;
	}

	file.Seek(3200, CFile::begin);
	file.Read(&headRead.bh, 400L);
	if (head->isUnixFile) SwapTapeHeader(&headRead.bh);
	if (headRead.bh.hdt == 0) headRead.bh.hdt = headRead.bh.dto;
	double odt = (double)MIL / (double)headRead.bh.hdt;
	double dt = 1. / odt;  // duplicate with head->dt
	int sampByte = (headRead.bh.format != 3) ? 4 : 2;
	int lenIn = headRead.bh.hns * sampByte;

	int numsamp = headRead.bh.hns;
	unsigned short numsampOut = (unsigned short)ceil((m_tmax - m_tmin) * odt) + 1;

	if (this->m_tmin == this->m_tmax) {
		// we have to simply use the same length
		this->m_tmin = head->tminFile;
		this->m_tmax = head->tmaxFile;
		numsampOut = numsamp;
		this->m_tmax = this->m_tmin + (numsampOut - 1) * dt;
	}
	else {
		numsampOut = (unsigned short)ceil((m_tmax - m_tmin) * odt) + 1;
	}

	if (numsampOut < 4) {
		numsampOut = 4;
		this->m_tmax = this->m_tmin + (numsampOut - 1) * dt;
	}
	headRead.setnumsamp(numsampOut);
	// int sampByteOut = sizeof(float);  // always 4 bytes per sample in output
	int lenOut = numsampOut * sizeof(float);

	// test on the first 3 traces in case of irregular segy not using 
	// the first 4 bytes for storing trace numbers
	// bool isNumberedIrregular = head->isTraceNumberIrregular();

	int stepCount = 0;
	int nxmax = 0;
	for (int i = 0; i < head->numTraces; i++) {
		if (this->m_cdp > 0) {
			if (head->cdp[i] < this->m_cdp) continue;
			else if (head->cdp[i] > this->m_cdp) break;
		}
		if (head->traces[i]<trace1 || head->traces[i]>trace2) //unwanted traces.
			continue;
		if (stepCount > 1) {
			stepCount--;
			continue;
		}
		else stepCount = traceStep;
		nxmax++;
		if (numTraceDesired > 0 && nxmax >= numTraceDesired) break;
	}
	headRead.setnx(nxmax);

	if (headRead.getnx() < 1 || headRead.getnx() > MIL) {
		file.Close();
		return false;
	}



	if (headRead.getnx() < 2) {
		headRead.setnx(2);
		trace2 += 1;
	}
	headRead.trArr = (segydynamic *)xmalloc(sizeof(segydynamic) * headRead.getnx());
	size_t numsampArraySize = (size_t)(ceil(numsampOut * 0.5) * 2.0);   // this trick does not help with memory issue
	for (j = 0; j < headRead.getnx(); j++) {
		if (this->m_isHeadersOnly) {
			headRead.trArr[j].data = NULL;
		}
		else {
			headRead.trArr[j].data = ealloc1float(numsampArraySize);
			if (ERROR == headRead.trArr[j].data) {
				freeSegyArray(&headRead);
				file.Close();
				AfxMessageBox(_T("Sorry, failed to allocate memory for loading all traces into memory."));
				return false;
			} // if
		} // else
	} // for j

	//float *trcdat = (float *) alloc1float(numsampOut);
	int nxStore = 0;
	int nxSeq = 0;
	stepCount = 0;
	// NOTE: we want to output a group of traces alighned for proper processing such as coherency mixing
	//       so if the output tmin=-2 sec, and we have no data before 0 sec, we still need to pad zeros at each trace begingning
	for (int i = 0; i < head->numTraces; i++) {
		if (this->m_cdp > 0) {
			if (head->cdp[i] < this->m_cdp) continue;
			else if (head->cdp[i] > this->m_cdp) break;
		}

		if (head->traces[i]<trace1 || head->traces[i]>trace2) //unwanted traces.
			continue;
		if (stepCount > 1) {
			//	file.Seek(len-4, CFile::current); 
			stepCount--;
			continue;
		}
		else stepCount = traceStep;

		file.Seek(head->pDataArr[i] - 240, CFile::begin);
		file.Read(&tr, 240);  // read the rest of trace header
		if (head->isUnixFile) {
			SwapTraceHeader(false, false, false, &tr);
		}

		tr.tracl = head->traces[i];  // head->traces[] has fixed a few odds so we have to use it
		// we do not want zero trace number, since it will cause problems
		// if (tr.tracl <= 0) tr.tracl = tr.cdp;   // check again


		int j0 = (int)myRound((m_tmin - head->tminArr[i]) * odt);
		// now j0 points to  m_tmin for array tr.data[] exactly!
		// NOTE:  if j0<0 and trace starts from zero time, it means we want extra zero padding at beginning of data
		//        so we need to pre-calculate output data index beginning j1 and data end index j2
		//register long j1 = (head->tminArr[i] > m_tmin)  ?  myRound((head->tminArr[i] - m_tmin) * odt)  :  0;
		//register long numCopy = min(numsamp-j0, numsampOut);
		double actualDelay = j0 * dt + head->tminArr[i];
		// this is supposed to be m_tmin, but due to wrap-around error, we need to re-calculate

		// now deal with trace data
		if (!this->m_isHeadersOnly) {
			memset((void *)&headRead.trArr[nxStore].data[0], (int) '\0', lenOut);
			if (file.Read(&tr.dat.data[0], lenIn) == (UINT)(lenIn)) {
				if (head->isUnixFile) {
					SwapN(&tr.dat, headRead.bh.format, numsamp);   // trace header was already swapped
				}

				int jj;

				if (headRead.bh.format == 3) {
					for (j = 0; j < numsampOut; j++) {
						jj = j0 + j;
						if (jj >= 0 && jj <= numsamp - 1)
							headRead.trArr[nxStore].data[j] = tr.dat.datashort[jj];
					}
				}
				else if (headRead.bh.format == 1 || headRead.bh.format == 5) {
					if (headRead.bh.format == 1)
						ibm_to_float((int *)tr.dat.data, (int *)tr.dat.data, (int)numsamp);
					for (j = 0; j < numsampOut; j++) {
						jj = j0 + j;
						if (jj >= 0 && jj <= numsamp - 1)
							headRead.trArr[nxStore].data[j] = tr.dat.data[jj];
					}
					// memcpy( (void *)&tr2.dat.data[j1], (void *)&tr.dat.data[j0], numsamp*sampByte);
				}
				// for (j=0; j<numsamp; j++) trArr[nxStore].data[j] = (float)tr.dat.data[j];
				else if (headRead.bh.format == 2) {
					for (j = 0; j < numsampOut; j++) {
						jj = j0 + j;
						if (jj >= 0 && jj <= numsamp - 1)
							headRead.trArr[nxStore].data[j] = (float)tr.dat.dataint[jj];
					}
				}
				else break;

				for (j = 0; j < numsampOut; j++) {
					if (_isnan(headRead.trArr[nxStore].data[j]))
						headRead.trArr[nxStore].data[j] = 0.f;
				}
			}
			//	if (nxStore < 50)
			//	memcpy( (void *)&trArr[nxStore].data[0], (void *)&trcdat[0], lenOut);
		}  // if !m_isHeadersOnly

		tr.setTraceDelayF(actualDelay);
		tr.ns = numsampOut;  // this must be set after SwapN(&tr, ) is called, since it uses tr.ns inside the function
		// assign only the trace header to the trArr[]
		memcpy((void *)&headRead.trArr[nxStore], (void *)&tr, 240L);

		nxStore++;
		if (nxStore >= headRead.getnx()) break;
	} // for
	file.Close();
	//free(trcdat);

	// now remove any array elements not in use
	if (nxStore < headRead.getnx()) {
		for (j = nxStore; j < headRead.getnx(); j++) {
			if (headRead.trArr[j].data != NULL) {
				free1float(headRead.trArr[j].data);
				headRead.trArr[j].data = NULL;
			}
		} // for
		headRead.setnx(nxStore);
	} // if

	// headRead.bh.ntrpr = (unsigned short)headRead.nxmax;
	headRead.bh.format = 5;




	int resampFactor = (int)myRound(dyMin / dt);
	if (myRound(headRead.bh.hdt * resampFactor) > 65500) {
		// too big, we need to reduce it
		resampFactor = (int)floor((double)headRead.bh.hdt / (double)65500);
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
			for (j = 0; j < headRead.bh.hns; j++) {
				if (count == 0) count = resampFactor;
				if (count == resampFactor) {
					this->headRead.trArr[nxStore].data[jNew] = this->headRead.trArr[nxStore].data[j];
					jNew++;
				}
				count--;
			}
		} // for nxStore

		dt *= (double)resampFactor;
		headRead.bh.hdt = (unsigned short)myRound(dt*MIL);
		numsampOut = jNew;

		for (nxStore = 0; nxStore < this->headRead.getnx(); nxStore++) {
			this->headRead.trArr[nxStore].ns = numsampOut;
			this->headRead.trArr[nxStore].dt = headRead.bh.hdt;
		} // for nxStore
		// resampFactor = 1;  // no more resamp adjustment!
	} // if

	// now updating the headers
	headRead.setdt(dt);
	headRead.setnumsamp(numsampOut);
	return true;
}

// This function will read entire segy into memory, so no need to have *head struct
// note: the format in this function will not change to 5, although the data array in memory is IEEE
bool SegyWrapper::openTraceFlatWhole(int numTraceDesired)
{
	if (!isFileExistNotDirectory(this->m_f)) return false;
	if (numTraceDesired < 1) numTraceDesired = (int)MIL;

	// note: sometimes the num of traces as reported in the headers is not valid
	// so we need to grab the actual num of traces in file
	int numTotalExisting = CSegyMeta::getNumTraceInSegy(m_f);
	int nxmax = (numTotalExisting >= numTraceDesired) ?
		numTraceDesired : numTotalExisting;

	CFile file;
	segy tr;
	int j;
	if (!file.Open(m_f, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
		return false;
	}

	file.Seek(3200, CFile::begin);
	file.Read(&headRead.bh, 400L);
	m_isUnixInFile = (headRead.bh.format < 0 || headRead.bh.format>5);
	if (m_isUnixInFile) SwapTapeHeader(&headRead.bh);

	m_formatInFile2 = headRead.bh.format;

	m_numsampInFile = headRead.getns();
	unsigned short numsampOut = m_numsampInFile;
	if (headRead.bh.hdt == 0) headRead.bh.hdt = headRead.bh.dto;
	m_dtInFile = headRead.getdt();
	int sampByte = (headRead.bh.format != 3) ? 4 : 2;
	int lenIn = headRead.bh.hns * sampByte;
	int lenOut = numsampOut * sizeof(float);

	headRead.setnx(nxmax);
	headRead.trArr = (segydynamic *)xmalloc(sizeof(segydynamic) * nxmax);
	//float a;

	if (headRead.bh.format == 1) {
		// test on the first trace
		static bool isPrompt = true;
		if (isPrompt) {
			isPrompt = false;
			file.Read(&tr, 240);  // read the rest of trace header
			if (m_isUnixInFile) SwapTraceHeader(false, false, false, &tr);
			if (file.Read(&tr.dat.data[0], lenIn) == (UINT)(lenIn)) {
				if (m_isUnixInFile) SwapN(&tr.dat, headRead.bh.format, numsampOut);   // trace header was already swapped
				float aMaxBefore = getMaxValueAbsolute(tr.dat.data, numsampOut);
				ibm_to_float((int *)tr.dat.data, (int *)tr.dat.data, (int)numsampOut);
				float aMaxAfter = getMaxValueAbsolute(tr.dat.data, numsampOut);
				if (aMaxBefore < MIL && aMaxAfter > MIL) {
					// prompt
					CString tStr;
					tStr.Format(_T("Data may be in IEEE float even marked as IBM float, because max amp of first trace is %g if in former format, but is %g in latter format."), aMaxBefore, aMaxAfter);
					AfxMessageBox(tStr);
				}
			}
			file.Seek(3600, CFile::begin);
		} // end isPrompt
	}

	for (int i = 0; i < nxmax; i++) {
		file.Read(&tr, 240);  // read the rest of trace header
		if (m_isUnixInFile) SwapTraceHeader(false, false, false, &tr);
		headRead.trArr[i].data = ealloc1float(numsampOut);
		memset((void *)&headRead.trArr[i].data[0], (int) '\0', lenOut);
		if (file.Read(&tr.dat.data[0], lenIn) == (UINT)(lenIn)) {
			if (m_isUnixInFile) SwapN(&tr.dat, headRead.bh.format, numsampOut);   // trace header was already swapped
			if (headRead.bh.format == 1 || headRead.bh.format == 5) {
				if (headRead.bh.format == 1) {
					ibm_to_float((int *)tr.dat.data, (int *)tr.dat.data, (int)numsampOut);
				}
				memcpy( (void *)&headRead.trArr[i].data[0], (void *)&tr.dat.data[0], numsampOut*sampByte);
			}
			else if (headRead.bh.format == 2) {
				for (j = 0; j < numsampOut; j++) {
					headRead.trArr[i].data[j] = (float)tr.dat.dataint[j];
				}
			}
			else if (headRead.bh.format == 3) {
				for (j = 0; j < numsampOut; j++) {
					headRead.trArr[i].data[j] = tr.dat.datashort[j];
				}
			}
			else {
				file.Close();
				return false;
			}

			for (j = 0; j < numsampOut; j++) {
					if (_isnan(headRead.trArr[i].data[j]))
						headRead.trArr[i].data[j] = 0.f;
			}
		}
		memcpy((void *)&headRead.trArr[i], (void *)&tr, 240L);

	//	a = headRead.trArr[i].getTraceDelay();

	} // for
	file.Close();
	this->m_isEntireTrace = true;
	//headRead.bh.format = 5;
	this->m_formatInFile2 = headRead.bh.format;

	return true;
}


void SegyWrapper::sortFFID()
{
	segy tr;
	//	int sampByte = (this->headRead.bh.format != 3) ? 4 : 2;
	int sampByte = 4;
	int nx = this->headRead.getnx();
	int i;

	int *ffid = ealloc1int(nx);
	for (i = 0; i < nx; i++)
		ffid[i] = this->headRead.trArr[i].fldr;


	register bool isSortNeeded = false;
	for (i = 0; i < nx - 1; i++) {
		if (ffid[i] > ffid[i + 1]) {
			isSortNeeded = true;
			break;
		}
	}
	if (!isSortNeeded) {
		free1int(ffid);
		return;
	}


	for (int n = 0; n < nx - 1; n++) {
		register int m = getMinIndex(&ffid[n], nx - n) + n;
		// here m must be added by an index shift of n! this a big programming trap!
		if (m > n) {
			// the range at m needs to swap with the trace at n. Note  trArr.data  is a separate pointer from trArr itself
			memcpy((void *)&tr, (void *)&headRead.trArr[n], 240L);
			if (!this->m_isHeadersOnly)
				memcpy((void *)&tr.dat, (void *)&headRead.trArr[n].data[0], sampByte*headRead.bh.hns);
			memcpy((void *)&headRead.trArr[n], (void *)&headRead.trArr[m], 240L);
			if (!this->m_isHeadersOnly)
				memcpy((void *)&headRead.trArr[n].data[0], (void *)&headRead.trArr[m].data[0], sampByte*headRead.bh.hns);
			memcpy((void *)&headRead.trArr[m], (void *)&tr, 240L);
			if (!this->m_isHeadersOnly)
				memcpy((void *)&headRead.trArr[m].data[0], (void *)&tr.dat, sampByte*headRead.bh.hns);

			register int temp = ffid[n];
			ffid[n] = ffid[m];
			ffid[m] = temp;
		}

	} // for n
	free1int(ffid);

	for (i = 1; i < nx; i++)
		headRead.trArr[i].tracl = headRead.trArr[i - 1].tracl + 1;
}

void SegyWrapper::sortDistance(int offsetMode)
{
	segy tr;
//	int sampByte = (this->headRead.bh.format != 3) ? 4 : 2;
	int sampByte = 4;
	int nx = this->headRead.getnx();
	float *ranges = ealloc1float(nx);

	int i;
	if (offsetMode == 0) {
		for (i = 0; i < nx; i++)
			ranges[i] = this->headRead.trArr[i].getDistance();
	}
	else if (offsetMode == 1) {
		for (i = 0; i < nx; i++)
			ranges[i] = this->headRead.trArr[i].getOffset();
	}
	else {
		for (i = 0; i < nx; i++)
			ranges[i] = (float)this->headRead.trArr[i].gx;
	}

	register bool isSortNeeded = false;
	for (i = 0; i < nx - 1; i++) {
		if (ranges[i] > ranges[i + 1]) {
			isSortNeeded = true;
			break;
		}
	}
	if (!isSortNeeded) {
		free1float(ranges);
		return;
	}


	for (int n = 0; n < nx - 1; n++) {
		register int m = getMinIndex(&ranges[n], nx - n) + n;
		// here m must be added by an index shift of n! this a big programming trap!
		//if (m > n-1) {
		//	int frdfs = 1;
		//}
		if (m > n) {
			// the range at m needs to swap with the trace at n. Note  trArr.data  is a separate pointer from trArr itself
			memcpy((void *)&tr, (void *)&headRead.trArr[n], 240L);
			if (!this->m_isHeadersOnly)
				memcpy((void *)&tr.dat, (void *)&headRead.trArr[n].data[0], sampByte*headRead.bh.hns);
			memcpy((void *)&headRead.trArr[n], (void *)&headRead.trArr[m], 240L);
			if (!this->m_isHeadersOnly)
				memcpy((void *)&headRead.trArr[n].data[0], (void *)&headRead.trArr[m].data[0], sampByte*headRead.bh.hns);
			memcpy((void *)&headRead.trArr[m], (void *)&tr, 240L);
			if (!this->m_isHeadersOnly)
				memcpy((void *)&headRead.trArr[m].data[0], (void *)&tr.dat, sampByte*headRead.bh.hns);

			register float temp = ranges[n];
			ranges[n] = ranges[m];
			ranges[m] = temp;
		}

	} // for n
	free1float(ranges);

	for (i = 1; i < nx; i++)
		headRead.trArr[i].tracl = headRead.trArr[i-1].tracl + 1;
}

void SegyWrapper::reverseTrace()
{
	segy tr;
//	int sampByte = (this->headRead.bh.format != 3) ? 4 : 2;
	int sampByte = 4;
	int n, m;
	int traceNumFirst = headRead.trArr[0].tracl;
	for (n = 0; n < this->headRead.getnx() / 2; n++) {
		memcpy((void *)&tr, (void *)&headRead.trArr[n], 240L);
		memcpy((void *)&tr.dat, (void *)&headRead.trArr[n].data[0], sampByte*headRead.bh.hns);

		m = this->headRead.getnx() - 1 - n;
		memcpy((void *)&headRead.trArr[n], (void *)&headRead.trArr[m], 240L);
		memcpy((void *)&headRead.trArr[n].data[0], (void *)&headRead.trArr[m].data[0], sampByte*headRead.bh.hns);
		memcpy((void *)&headRead.trArr[m], (void *)&tr, 240L);
		memcpy((void *)&headRead.trArr[m].data[0], (void *)&tr.dat, sampByte*headRead.bh.hns);
	}

	headRead.trArr[0].tracl = traceNumFirst; // Note: headRead.trArr[0] has been changed!!!
	for (n = 1; n < this->headRead.getnx(); n++) {
		headRead.trArr[n].tracl = traceNumFirst + n;
	}
}


const int SegyWrapper::getClosestTrcIndexAtOffset(double offset)
{
	// note: this assumes data sorted in offset. If the offsets are not sorted, the results may return -1.
	if (this->headRead.getnx() <= 1) return -1;

	//	int iOffset = (int)myRound(offset*1000.f); 
	for (int j = 0; j < this->headRead.getnx() - 1; j++) {
		if (isInsideFloat((float)offset, this->headRead.trArr[j].getOffset(), 
			this->headRead.trArr[j + 1].getOffset())) {
			if (fabs(offset - this->headRead.trArr[j].getOffset()) <
				fabs(this->headRead.trArr[j + 1].getOffset() - offset))
				return j;
			else
				return j + 1;
		} // if
	} // for j

	return -1;
}

const int SegyWrapper::getClosestTrcIndexAtDistance(double aDist)
{
	// note: this assumes data sorted in offset. If the offsets are not sorted, the results may return -1.
	if (this->headRead.getnx() <= 1) return -1;

	//	int iOffset = (int)myRound(offset*1000.f);
	float x1 = this->headRead.trArr[0].getDistance();
	float x2 = this->headRead.trArr[this->headRead.getnx() - 1].getDistance();
	if (x1 < x2) {
		if (aDist <= x1) return 0;
	}
	else {
		if (aDist >= x2) return this->headRead.getnx() - 1;
	}


	for (int j = 0; j < this->headRead.getnx() - 1; j++) {
		x1 = this->headRead.trArr[j].getDistance();
		x2 = this->headRead.trArr[j + 1].getDistance();
		if (isInsideFloat((float)aDist, x1, x2)) {
			if (fabs(aDist - x1) < fabs(x2 - aDist))
				return j;
			else
				return j + 1;
		} // if
	} // for j

	return this->headRead.getnx() - 1;
}

// calc the closest index for a trace in trArr[]
// why not simply grab integer trace number instead of a floating point trace number?
//    this is because when digitizing on screen, the point is an approximate X-coordinate, which is not to be traated as integer at all. 
const int SegyWrapper::getClosestTrcIndexAtTraceNum(double atrc)
{
	if (this->headRead.getnx() <= 1) return -1;

	float test = myFloat(39528.f);

	for (int j = 0; j < this->headRead.getnx() - 1; j++) {
		// note: here when the atrc is >39000, the isInsideFloat() gives incorrect result since it tries to wrap using ...
		if (isInsideDouble((double)atrc, (double)this->headRead.trArr[j].tracl, (double)this->headRead.trArr[j + 1].tracl)) {
			if (fabs(atrc - this->headRead.trArr[j].tracl) < fabs(this->headRead.trArr[j + 1].tracl - atrc))
				return j;
			else
				return j + 1;
		} // if
	} // for j

	return -1;
}


const int SegyWrapper::getArrIndexAtTrace(int itrc)
{
	if (this->headRead.getnx() < 1) return -1;

	for (int i = 0; i < this->headRead.getnx(); i++) {
		if (itrc == this->headRead.trArr[i].tracl) return i;
	} // for i

	return -1;
}

const int SegyWrapper::getArrIndexAtFFID(int iFFID)
{
	if (this->headRead.getnx() < 1) return -1;

	for (int i = 0; i < this->headRead.getnx(); i++) {
		if (iFFID == this->headRead.trArr[i].fldr) return i;
	} // for i

	return -1;
}


bool SegyWrapper::staticShift(int itrc, float deltat) {
	if (GetFileExtension(this->m_f).Compare(_T("jp2")) == 0) return 0;

	segy tr;
	int len;
	if (headRead.bh.format != 3)
		len = headRead.bh.hns * 4 + 240;
	else
		len = headRead.bh.hns * 2 + 240;

	CFile file;
	if (!file.Open(m_f, CFile::modeReadWrite | CFile::typeBinary)) {
		return false;
	}
	file.Seek(3600L, CFile::begin);
	while (file.Read(&tr, 4) == (UINT)4)
	{
		if (tr.tracl != itrc) {
			file.Seek(len - 4, CFile::current);
			continue;
		}

		// now we do 
		file.Read(&tr.tracr, 240 - 4);  // read the rest of thetrace header
		float adelay = tr.getTraceDelay();
		tr.setTraceDelayF(adelay + deltat);
		file.Seek(-240, CFile::current);
		file.Write(&tr, 240);
		break;
	}
	file.Close();

	return false;
}


int SegyWrapper::writeHeader4ZeroTrcNum()
{
	if (!isFileExistNotDirectory(m_f)) return 0;

	segy tr;

	int sampByte = (m_formatInFile2 != 3) ? 4 : 2;
	int trcDataBytes = m_numsampInFile*sampByte;

	CFile file;

	// segy already existing
	if (!file.Open(m_f, CFile::modeReadWrite | CFile::typeBinary)) {
		return 0;
	}
	file.Seek(3600L, CFile::begin);

	int itrcFile = 0;
	while (file.Read(&tr, 4) == (UINT)4) {
		itrcFile++;
		if (m_isUnixInFile) {
			swap_int_4(&tr.tracl);
		}

		if (tr.tracl != 0) {
			file.Seek(trcDataBytes + 240 - 4, CFile::current); // skip this trace
			continue;
		}
		tr.tracl = itrcFile;
		file.Seek(-4, CFile::current);
		if (m_isUnixInFile) {
			swap_int_4(&tr.tracl);
		}
		file.Write(&tr.tracl, 4);
		file.Seek(trcDataBytes + 240 - 4, CFile::current); // skip the data section to point to the next trace
	} // while


	file.Close();
	return itrcFile;
}

// **** THE FOLLOWING ROUTINE IS TESTED to write from headRead struct to SEGY file******
// HEAD *head  cannot be used during writing since head->traces[] is always non-zero even the segy data has all zero trace numbers
int SegyWrapper::writeHeaderOnly() {
	// this routine will only write all segy headers to the same file (assuming they are already changed in memory. If the segy file is new, it will not work since it only deals with headers only.
	// ****if the original segy file is unix format, the header must be converted to unix before writing, otherwise it will be a mess.****
	// if a trace in segy file is dead, and it does not exist in the this->trArr[] and will not update the trace header.
	if (m_isUnixInFile) {
		if (!this->headRead.trArr[0].data) 
			return 0;  // cannot continue
		
		// since it is unix file, we backup first before overwriting it
		CString bac = getFileNameAppend(m_f, _T("_UnixBac"));
		if (!CopyFile(m_f, bac, FALSE)) return 0;

		return writeDataAll(m_f);
	}


	segy tr;

	int sampByte = (headRead.bh.format != 3) ? 4 : 2;
//	int trcDataBytes = headRead.bh.hns*sampByte;
	int trcDataBytesFile = m_numsampInFile*sampByte;
	
	CFile file;
	int count = 0;

	// note: if the data format of *headRead is different from HEAD in file, then it does not match
	bool isNotMatching = headRead.isNotCompletelyMatched(m_isUnixInFile, m_formatInFile2, 
		m_numsampInFile, m_dtInFile);
		//m_formatInFile2 != headRead.bh.format ||
		//abs(m_dtInFile - headRead.dt()) > MINAMP ||
		//m_numsampInFile != headRead.numsamp();

	if (isNotMatching || !isFileExistNotDirectory(m_f)) {
		return writeDataAll(m_f);
	}

	// segy now already existing
	if (!file.Open(m_f, CFile::modeReadWrite | CFile::typeBinary)) {
			return 0;
	}

	if (headRead.ascHeadc.IsEmpty())
		file.Seek(3200L, CFile::begin);
	else {
		// now write it 
		// if (this->m_isUnixInFile) asc2ebc((unsigned char *)headRead.ascHead, 3200L);
		char * p = headRead.getAscHeadChar();
		file.Write(p, 3200L);
		free1(p);
	}

	short hrFormatBac = headRead.bh.format;
	if ( abs(m_formatInFile2 - headRead.bh.format) == 4)
		headRead.bh.format = m_formatInFile2;
	file.Write(&this->headRead.bh, 400L);//
	for (int i = 0; i < this->headRead.getnx(); i++) {
		int itrc = this->headRead.trArr[i].tracl; // this loop only deals with trace itrc only
		while (file.Read(&tr, 4) == (UINT)4) {
			if (tr.tracl != itrc) {
				file.Seek(trcDataBytesFile + 240 - 4, CFile::current); // skip this trace
				continue;
			}

			file.Seek(-4, CFile::current);
			file.Write(&this->headRead.trArr[i], 240);
			count++;
			file.Seek(trcDataBytesFile, CFile::current); // skip the data section to point to the next trace
			break;
		} // while
	} // for i
	headRead.bh.format = hrFormatBac;


	file.Close();

	return count;

}
int SegyWrapper::writeDataAll() {
	return writeDataAll(m_f);  // this will overwrite everything it matches


	// the following is ignored.
	bool isNotMatching = m_isUnixInFile ||
		m_formatInFile2 != headRead.bh.format ||
		fabs(m_dtInFile - headRead.getdt()) > MINAMP ||
		m_numsampInFile != headRead.getns();
	if (isNotMatching) return writeDataAll(m_f);  // this will overwrite everything it matches

	// if here, we only need to update the partial data. Is that safe to do so???
	return this->writeDataBetweenTraces(this->headRead.trArr[0].tracl, this->headRead.trArr[this->headRead.getnx() - 1].tracl);
}



// **** THE FOLLOWING ROUTINE IS TESTED to write data from headRead struct to SEGY file.
// **** DATA format to be written depends on   headRead.bh.format={0-5}, which is very important number!!!
// **** But the data in memory is always floating point:  headRead.trArr[i].data[j]
// ******
int SegyWrapper::writeDataBetweenTraces(int i1, int i2) {
	// this routine will only write all segy data to the same file (assuming they are already changed in memory. If the segy file is new, it will not work.
	// ****if the original segy file is unix format, the header/data must be converted to unix before writing, otherwise it will be a mess.****
	// if a trace in segy file is dead, and it does not exist in the this->trArr[] and will not update the trace header.
	// I don't want to bother update the ascii headers here.
	// ****Note: since we write to the same segy file, we have to keep the original format, 
	// i.e. if the original is integer, we have to write as integer
	if (m_isUnixInFile || headRead.getnx() < 1) return 0;

	if (i1 == 0) i1 = -999999;
	if (i2 == 0) i2 = 999999;

	segy tr;

	bool isNotMatching = 
		m_isUnixInFile || 
		m_formatInFile2 != headRead.bh.format ||
		fabs(m_dtInFile - headRead.getdt()) > MINAMP ||
		m_numsampInFile != headRead.getns();
	if (isNotMatching) return writeDataAll(m_f);  // this will overwrite everything it matches


	int sampByte = (headRead.bh.format != 3) ? 4 : 2;
	int numsamp = headRead.bh.hns;

	CFile file;
	if (!file.Open(this->m_f, CFile::modeReadWrite | CFile::modeCreate | CFile::typeBinary)) {
		return 0;
	}
	//if (this->m_isUnixInFile) asc2ebc(this->ascHead, 3200L);
	char * p = headRead.getAscHeadChar();
	file.Write(p, 3200L);
	free1(p);

	file.Write(&this->headRead.bh, 400);
	int count = 0;
	int j;
	for (int i = 0; i < this->headRead.getnx(); i++) {
		register int itrc = this->headRead.trArr[i].tracl; // this loop only deals with trace itrc only
		if (itrc < i1 || itrc > i2) continue;

		while (file.Read(&tr, 4) == (UINT)4) {
			if (m_isUnixInFile) {
				swap_int_4(&tr.tracl);
			}
			if (tr.tracl != itrc) {
				file.Seek(numsamp*sampByte + 240 - 4, CFile::current); // skip this trace
				continue;
			}

			// now we do 
			if (headRead.bh.format == 1) {
					memcpy((void *)&tr.dat.data[0], (void *)&headRead.trArr[i].data[0], numsamp*sampByte);
					//tr.idistopt = 8;
					float_to_ibm((int *)tr.dat.data, (int *)tr.dat.data, numsamp);
					// we always output as floating pt
			}
			else if (headRead.bh.format == 2) {
					// integer data format
					for (j = 0; j < numsamp; j++) tr.dat.dataint[j] = (int)headRead.trArr[i].data[j];
			}
			else if (headRead.bh.format == 3) {
					// short int format
					for (j = 0; j < numsamp; j++) tr.dat.datashort[j] = (int)headRead.trArr[i].data[j];
			}
			else if (headRead.bh.format == 5) {
					memcpy((void *)&tr.dat.data[0], (void *)&headRead.trArr[i].data[0], numsamp*sampByte);
			}
			if (m_isUnixInFile) SwapN(&tr.dat, headRead.bh.format, numsamp);
			file.Write(&tr.dat.data[0], numsamp*sampByte);
			count++;
			break;
		} // while
	} // for i
	file.Close();

	return count;

}


// this function now writes in IBM floating point format instead of IEEE floating point, to make the Segy file more compatible with industry standard!
int SegyWrapper::writeDataAll(CString fNew, bool isIbmEnforced) {
	// note: this routine cannot share with writeDataAll() at all, since if the segy file is different from this->f, it has to be written differently!!!
	if (headRead.getnx()>0 && !headRead.trArr[0].data) return 0;  // no data exists, even though headers may be ok

	int sampByte = (headRead.bh.format != 3) ? 4 : 2;
	int numsamp = headRead.getns();

	CFile file;

	if (!isFileExistNotDirectory(fNew)) {
		if (!file.Open(fNew, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
			return 0;
		}
	}
	else {
		if (!file.Open(fNew, CFile::modeWrite | CFile::typeBinary)) {
			return 0;
		}
	}


	if (headRead.ascHeadc.GetLength() > 5) {
		char * p = headRead.getAscHeadChar();
		file.Write(p, 3200L);
		free1(p);
	}
	else
		file.Seek(3200L, CFile::begin);
	
	//float a = headRead.trArr[0].getTraceDelay();

	bool isFormatIeeeFloat = this->headRead.bh.format == 5; // this has been the case most of the times!
	if (isFormatIeeeFloat && isIbmEnforced) this->headRead.bh.format = 1; // we want to write in IBM floats to be more compatible with current industry vendors!
	file.Write(&this->headRead.bh, 400);

	segy tr;
	int count = 0;
	int j;
	for (int i = 0; i < this->headRead.getnx(); i++) {
		//a = headRead.trArr[0].getTraceDelay();
		file.Write(&this->headRead.trArr[i], 240);

		if (headRead.trArr[i].data) {
			if (headRead.bh.format == 1) {
				memcpy((void *)&tr.dat.data[0], (void *)&headRead.trArr[i].data[0], numsamp*sampByte);
				//tr.idistopt = 8;
				float_to_ibm((int *)tr.dat.data, (int *)tr.dat.data, numsamp);
				// we always output as floating pt
			}
			else if (headRead.bh.format == 2) {
				// integer data format
				for (j = 0; j < numsamp; j++) tr.dat.dataint[j] = (int)headRead.trArr[i].data[j];
			}
			else if (headRead.bh.format == 3) {
				// short int format
				for (j = 0; j < numsamp; j++) tr.dat.datashort[j] = (int)headRead.trArr[i].data[j];
			}
			else if (headRead.bh.format == 5) {
				memcpy((void *)&tr.dat.data[0], (void *)&headRead.trArr[i].data[0], numsamp*sampByte);
				// we always output as floating pt
			}
			file.Write(&tr.dat.data[0], numsamp*sampByte);
		}

		count++;
	} // for i
	file.Close();
	if (isFormatIeeeFloat && isIbmEnforced) this->headRead.bh.format = 5; // we change the format back to Ieee otherwise you may be supprized that the data format is changed!

	return count;
}


bool SegyWrapper::deleteTrc(int itrc) {
	if (GetFileExtension(this->m_f).Compare(_T("jp2")) == 0) return false;

	segy tr;
	int len;
	if (headRead.bh.format != 3)
		len = headRead.bh.hns * 4 + 240;
	else
		len = headRead.bh.hns * 2 + 240;

	CFile file;
	if (!file.Open(m_f, CFile::modeReadWrite | CFile::typeBinary)) {
		return false;
	}
	file.Seek(3600L, CFile::begin);
	while (file.Read(&tr, 4) == (UINT)4)
	{
		if (tr.tracl != itrc) {
			file.Seek(len - 4, CFile::current);
			continue;
		}

		// now we do 
		file.Read(&tr.tracr, 240 - 4);  // read the rest of thetrace header
		tr.trid = 2;  // mark it dead
		file.Seek(-240, CFile::current);
		file.Write(&tr, 240);
		break;
	}
	file.Close();

	return false;
}

int SegyWrapper::getCountNonMonoOffset(int offsetMode) {
	float *ranges = ealloc1float(headRead.getnx());
	int m;
	for (m = 0; m < this->headRead.getnx(); m++) {
		if (offsetMode == 0)
			ranges[m] = this->headRead.trArr[m].getDistance();
		else if (offsetMode == 1)
			ranges[m] = this->headRead.trArr[m].getOffset();
		else
			ranges[m] = (float)this->headRead.trArr[m].gx;
	}

	int count = 0;
	for (int j = 0; j<this->headRead.getnx() - 1; j++) {
		if (ranges[j] > ranges[j + 1])
			count++;
	} // j
	free1float(ranges);

	return count;
}

#include "sumig.h"
float SegyWrapper::getShiftTrcCoh2(int itrc1, int itrc2, float tIncCenter, float terr, int numSubdivide) {
	if (itrc1 < 0) return ERRORVAL;
	if (itrc2 <= 0) return ERRORVAL;

	double dtOrig = this->headRead.getdt();
	int ntlenOrig = headRead.getns();
	double deltat = m_tmax - m_tmin;
	if (deltat == 0.)
		deltat = dtOrig * (ntlenOrig - 1);

	double dt = dtOrig / (double)numSubdivide;
	double odt = 1. / dt;
	int ntlen = (int)ceil(deltat * odt) + 1;
	if (ntlen > 20000) {
		// the user has mistakenly done a too much over-sampling. We need to revert back
		odt = 20000. / deltat;
		dt = 1. / odt;
		ntlen = (int)ceil(deltat * odt) + 1;
	}



	int nerr = (int)ceil(terr * (float)odt) + 1;
	int hnerr = nerr / 2;
	nerr = hnerr * 2 + 1;

	float **famtc = ealloc2float(ntlen, nerr);
	float **semb = ealloc2float(ntlen, nerr);
	memset((void *)famtc[0], (int) '\0', nerr*ntlen*sizeof(float));
	memset((void *)semb[0], (int) '\0', nerr*ntlen*sizeof(float));

	float *adata2 = ealloc1float(ntlen);
	float *bdata2 = ealloc1float(ntlen);
	float *temp = ealloc1float(nerr);
	float *temp1 = ealloc1float(nerr);
	memset((void *)&temp[0], (int) '\0', nerr*sizeof(float));
	memset((void *)&temp1[0], (int) '\0', nerr*sizeof(float));




	float *adata = ealloc1float(ntlen);
	float *bdata = ealloc1float(ntlen);
	float *tTrace = ealloc1float(ntlen);

	int ir, i, j;

	for (i = 0; i<ntlen; i++) {
		tTrace[i] = (float)(m_tmin + (float)i*dt);
	} // for i

	double delay1 = m_tmin; // since we assume the data array has already been perfectly aligned, we cannot consider trace delay at all!
	double delay2 = m_tmin;
	ints8r(ntlenOrig, // ntin
		(float)dtOrig,   // dtin
		(float)delay1,   // ftin
		&this->headRead.trArr[itrc1].data[0], // yin[t=ntin], with yin[0] = y(fxin)
		0.f,  // yinl    value used to extrapolate yin values to left of yin[0]
		0.f,  // yinr	 value used to extrapolate yin values to right of yin[ntin-1]
		ntlen,   // nxout
		(float *)&tTrace[0],  // 
		(float *)&adata[0]);    // tout
	for (i = 0; i<ntlen; i++) {
		tTrace[i] += tIncCenter;
	} // for i
	ints8r(ntlenOrig, // ntin
		(float)dtOrig,   // dtin
		(float)delay2,   // ftin
		&this->headRead.trArr[itrc2].data[0], // yin[t=ntin], with yin[0] = y(fxin)
		0.f,  // yinl    value used to extrapolate yin values to left of yin[0]
		0.f,  // yinr	 value used to extrapolate yin values to right of yin[ntin-1]
		ntlen,   // nxout
		(float *)&tTrace[0],  // 
		(float *)&bdata[0]);    // tout




	for (i = 0; i<ntlen; i++) {
		adata2[i] = adata[i] * adata[i];
		bdata2[i] = bdata[i] * bdata[i];
	}

	for (i = 0; i<ntlen; i++) {  // i is index for the previous trace
		for (ir = 0; ir<nerr; ir++) {
			j = i + ir - hnerr;
			if (j >= 0 && j<ntlen) {
				famtc[ir][i] = adata[i] + bdata[j];
				semb[ir][i] = adata2[i] + bdata2[j];
			} // if j
		} // for ir
	} // for i

	for (ir = 0; ir<nerr; ir++) {
		for (i = 0; i<ntlen; i++) {
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

	free1float(adata);
	free1float(bdata);
	free1float(adata2);
	free1float(tTrace);

	free1float(bdata2);
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

	return ERRORVAL;
}



const float SegyWrapper::getAvgKmInterval(bool isDistance) {
	// need to use head* instead of trArr[] ???


	int n, i;
	long num = headRead.getnx();
	float *ranges = ealloc1float(num);
	for (n = 0; n < num; n++) {
		ranges[n] = !isDistance ? this->headRead.trArr[n].getOffset() :
			this->headRead.trArr[n].getDistance();
	}

	sort1(&ranges[0], num);

	float xPrev = ranges[0];
	for (i = 1; i < num; i++) {
		ranges[i-1] = ranges[i] - xPrev;
		xPrev = ranges[i];
	}


	// getMedian3(&ranges[0], this->nxmax-1, n);	//This is the average trace interval in km.
	n = getMedian3(&ranges[0], num - 1);	//This is the average trace interval in km.
	float trcInterval = ranges[n];
	free1float(ranges);

	//return  0.1f;
	return trcInterval;
}

// This function will return RMS amp instead of average amp
const float SegyWrapper::getAvgAmp(int numTrc) {
	if (this->m_isHeadersOnly || this->headRead.getnx() < 1) return 0.f;

	float avgAmp = 0.f;

	int i, j;

	int num = (numTrc > 0) ? numTrc : min(100, this->headRead.getnx());
	if (num > this->headRead.getnx()) num = this->headRead.getnx();

	float *amps = ealloc1float(num);
	int xcount = 0;

	float *trc = ealloc1float(this->headRead.getns());
	for (i = 0; i<num; i++) {
		register int jcount = 0;
		for (j = 0; j<this->headRead.getns(); j++) {
			trc[jcount] = (float)fabs(headRead.trArr[i].data[j]);
			if (trc[jcount] > MINAMP && trc[jcount]<MAXAMP)
				jcount++;
		} // for j

		amps[xcount] = (float)getRmsValue(trc, jcount);
		if (amps[xcount] > MINAMP) {
			xcount++;
		}
		// if this amp is too small, it should not count
	} // for i
	free1float(trc);

	avgAmp = (float)getRmsValue(amps, xcount);
	free1float(amps);

	//if (numTrc == 0 && avgAmp<MINAMP) {
	//	//  this is the case when the default case we only use about 100 traces to test on avg amp calculation.
	//	// but in rare cases, the first 100 traces are all zeros
	//	return this->getAvgAmp(this->headRead.nx());
	//}
	return avgAmp;

}
const float SegyWrapper::getMaxAmp(int numTrc) {
	if (this->m_isHeadersOnly || this->headRead.getnx() < 1) return 0.f;

	int i, j;
	int num = (numTrc > 0) ? numTrc : min(100, this->headRead.getnx());
	if (num > this->headRead.getnx()) num = this->headRead.getnx();
	int ns = this->headRead.getns();

	float *amps = ealloc1float(num);
	float *trc = ealloc1float(ns);
	m_countErr = 0;
	for (i = 0; i < num; i++) {
		register int jc = 0;
		for (j = 0; j < ns; j++) {
			trc[jc] = (float)fabs(headRead.trArr[i].data[j]);
			if (trc[jc] < 1000.*MAXAMP)
				jc++;
			else
				m_countErr++;
		} // for j

		amps[i] = (float)getMaxValue(trc, jc);
	} // for i
	free1float(trc);

	float maxAmp = getMaxValue(amps, num);
	free1float(amps);
	return maxAmp;

}



bool SegyWrapper::initNewSegyHead(HEAD_READ *headRead, int nx, double tmin, double dt, int numsamp)
{
	// after this call, the headRead struct is quarenteed to form a valid segy file if saved
	memset(&headRead->bh, (int) '\0', 400);
	headRead->trArr = (segydynamic *)xmalloc(sizeof(segydynamic) * nx);
	if (!headRead->trArr) return false;
	headRead->bh.format = 5;
	headRead->setnx(nx);
	headRead->setnumsamp(numsamp);
	headRead->setdt(dt);
	for (int ix = 0; ix < nx; ix++) {
		memset((void *)&headRead->trArr[ix], (int) '\0', 240);
		headRead->trArr[ix].tracl = ix+1;
		headRead->trArr[ix].setOffset((float)tmin);

		// assign some other trace header info: tmin, dt, ns
		headRead->trArr[ix].setTraceDelayF(tmin);
		headRead->trArr[ix].dt = headRead->bh.dto;
		headRead->trArr[ix].delrt = (short)(tmin * 1000. + 0.5);
		headRead->trArr[ix].trid = 1; // valid seismic trace
		headRead->trArr[ix].ns = numsamp;
	}

	return true;
}

// xmode==0 : offset
// xmode==1 : distance
// xmode==2 : trace
// isFaceUp: bitmap always face down. But a face up coordinate will have to reverse the pixels

bool SegyWrapper::saveTifAsSegy2(CxImage *ximage, CString fSgyOut, double xmin, double xmax, double tmin, double tmax, int xmode, bool isFaceUp)
{
	CColorHelper::AMPCOLORDYN ampColorDyn2(fSgyOut); // only used in color mode
	ampColorDyn2.getAmpColorArray(255., false, 100, 1.); // this will grab the color scale from existing file
	CColorHelper::getDefaultAmpColor(&ampColorDyn2, 0, false); // this will overwrite the color scale to make it a gray scale
	bool out = saveTifAsSegy(ximage, fSgyOut, xmin, xmax, tmin, tmax, xmode, isFaceUp, &ampColorDyn2);
	ampColorDyn2.freeAmpColorArray();
	return out;
}

bool SegyWrapper::saveTifAsSegy(CxImage *ximage, CString fSgyOut, double xmin, double xmax, double tmin, double tmax, int xmode, bool isFaceUp,
	CColorHelper::AMPCOLORDYN *pAmpColorDyn)
{
//	if (ximage->GetType() != CXIMAGE_SUPPORT_TIF) return false;

	// CXImage class itself only supports GetBpp() == 1,4,8,24
	// WORD a = ximage->get
	if (!ximage || ximage->GetBpp() == 16) return false;

	int nx = ximage->GetWidth();
	int numsamp = ximage->GetHeight(); if (numsamp < 2) return false;
	double dt = (tmax - tmin) / (double)(numsamp - 1);
	SegyWrapper sw(fSgyOut);
	bool isSuc = initNewSegyHead(&sw.headRead, nx, tmin, dt, numsamp);
	if (!isSuc) {
		// XML section failed to read. So just clean up and return
		// clean up before return
		sw.freeSegyArray(&sw.headRead);
		return false;
	}

	// allocate data array
	for (int ix = 0; ix < nx; ix++) {
		sw.headRead.trArr[ix].data = ealloc1float(numsamp);
	}
	if (ERROR == sw.headRead.trArr[nx - 1].data) {
		sw.freeSegyArray(&sw.headRead);
		AfxMessageBox(_T("Insufficient memory to allocate all trace buffers."));
		return false;
	}

	// now get the data

//	register float aVal;
	int jj;
	double dx = (xmax - xmin) / (nx - 1.);
	BYTE *grays = (BYTE *)alloc1(numsamp, 1);

	for (int ix = 0; ix < nx; ix++) {
		sw.headRead.trArr[ix].tracl = ix + 1;

		for (int j = 0; j < numsamp; j++) {
			jj = (!isFaceUp) ? numsamp - 1 - j : j;
			//register RGBQUAD a = ximage->GetPixelColor(ix, jj); // plots are upside down
			grays[j] = 255- ximage->GetPixelGray(ix, jj);  // for some reason, GetPixelGray(0 returns reversed color
		}
		pAmpColorDyn->getAmpFromGray(grays, numsamp, &sw.headRead.trArr[ix].data[0]);
		sw.headRead.trArr[ix].setTraceDelayF(tmin);

		register float x = (float)(xmin + ix * dx);
		if (xmode == 0)
			sw.headRead.trArr[ix].setOffset(x);
		else if (xmode==1)
			sw.headRead.trArr[ix].setDistance(x);
	}
	free1(grays);
	sw.writeDataAll();

	// update the ascii header, 3200 length only
	CString tStr;
	tStr.Format(_T("XX6BITMAP , Converted segy from bitmap."));
	
	char *aChar = getCharFromCString(tStr, 3200);
	CFile file;
	if (!file.Open(fSgyOut, CFile::modeReadWrite | CFile::typeBinary)) {
		return 0;
	}
	file.Write(aChar, 3200L);
	free1(aChar);
	file.Close();
	return true;
}

// note: dig2 and headRead must already have the same numner of traces for this function to work.
// xmode==0 : offset
// xmode==1 : distance
// xmode==2 : trace
// Why I use xvel[] as float? Because that is the format used in segy
#include "pfafft.h"
bool SegyWrapper::saveDig2Trace(CDigitize::DIGITIZE *dig2, CString fSgyOut, int numsamp, double tmin, double dt) {

	if (dig2->totCurve < 2) return false;
	int nx = dig2->totCurve;

	SegyWrapper sw(fSgyOut);
	bool isSuc = initNewSegyHead(&sw.headRead, nx, tmin, dt, numsamp);
	if (!isSuc) {
		// XML section failed to read. So just clean up and return
		// clean up before return
		sw.freeSegyArray(&sw.headRead);
		return false;
	}

	// allocate data array

	for (int ix = 0; ix < nx; ix++) {
		sw.headRead.trArr[ix].data = ealloc1float(numsamp);
	}
	if (ERROR == sw.headRead.trArr[nx - 1].data) {
		sw.freeSegyArray(&sw.headRead);
		AfxMessageBox(_T("Insufficient memory to allocate all trace buffers."));
		return false;
	}


	//int nout = sw.headRead.getns();
	float *yin = (float *)ealloc1float(NP_MAX);
	float *xin = (float *)ealloc1float(NP_MAX);
	float *xout = (float *)ealloc1float(numsamp);

	int j;
	float dyOut = (float)sw.headRead.getdt();
	for (int ic = 0; ic < dig2->totCurve; ic++) {
		int nin = dig2->nPoint[ic];
		if (nin < 2 || nin>NP_MAX) continue;
		for (int j = 0; j < nin; j++) {
			xin[j] = dig2->t[j][ic];
			yin[j] = dig2->x[j][ic] - dig2->curveVal[ic];
		}

		xout[0] = sw.headRead.trArr[ic].getTraceDelay();
		for (j = 1; j < numsamp; j++) {
			xout[j] = xout[j - 1] + dyOut;
		}

		//void intlin(int nin, float xin[], float yin[], float yinl, float yinr,
		//	int nout, float xout[], float yout[]);
		intlin(nin, xin, yin, 0.f, 0.f, numsamp, xout, &sw.headRead.trArr[ic].data[0]);
		
		if (dig2->xMode == CDigitize::offset)
			sw.headRead.trArr[ic].setOffset(dig2->curveVal[ic]);
		else if (dig2->xMode == CDigitize::distance)
			sw.headRead.trArr[ic].setDistance(dig2->curveVal[ic]);
		else
			sw.headRead.trArr[ic].tracl = (int)dig2->curveVal[ic];
	}

	free1float(xin);
	free1float(yin);
	free1float(xout);

	sw.writeDataAll();

	// update the ascii header, 3200 length only
	CString tStr;
	tStr.Format(_T("XX8DIG2SEGY , Digitized traces Converted seg."));
	char *aChar = getCharFromCString(tStr, 3200);
	CFile file;
	if (!file.Open(fSgyOut, CFile::modeReadWrite | CFile::typeBinary)) {
		return 0;
	}
	file.Write(aChar, 3200L);
	free1(aChar);
	file.Close();
	return true;

}

// static functions follows

// processingGradient will only work in trace mode.
// headDx and headDt must already have proper headers copied from headRead, which is checked for.
bool SegyWrapper::processingGradient(HEAD_READ *headIn, HEAD_READ *headDx, HEAD_READ *headDt)
{
	int ix, it, it1, it2;
	int nx = headIn->getnx();
	int nt = headIn->getns();
	if (nx < 2 || nt<2) return false;

	if (headDx->getnx() < nx || headDx->getns() < nt ||
		headDt->getnx() < nx || headDt->getns() < nt) return false;
	//float odt = 1.f / (float)(2. * headIn->dt());

	// now compute for headDt*
	for (it = 0; it < nt; it++) {
		it1 = (it == 0) ? 0 : it - 1;
		it2 = it == nt - 1 ? nt - 1 : it + 1;
		for (ix = 1; ix < nx-1; ix++) {
			headDt->trArr[ix].data[it] = headIn->trArr[ix-1].data[it2] - headIn->trArr[ix-1].data[it1]
				+ headIn->trArr[ix].data[it2] - headIn->trArr[ix].data[it1]
				+ headIn->trArr[ix+1].data[it2] - headIn->trArr[ix+1].data[it1]
				;
			headDt->trArr[ix].data[it] *= 0.333333333f;
		}

		headDt->trArr[0].data[it] = headIn->trArr[0].data[it2] - headIn->trArr[0].data[it1]
			+ headIn->trArr[1].data[it2] - headIn->trArr[1].data[it1];
		headDt->trArr[0].data[it] *= 0.5f;

		headDt->trArr[nx - 1].data[it] = headIn->trArr[nx - 2].data[it2] - headIn->trArr[nx - 2].data[it1]
			+ headIn->trArr[nx - 1].data[it2] - headIn->trArr[nx - 1].data[it1];
		headDt->trArr[nx - 1].data[it] *= 0.5f;
	}


	// now compute for headDx*
	int ix1, ix2;
	for (ix = 0; ix < nx; ix++) {
		ix1 = (ix == 0) ? 0 : ix - 1;
		ix2 = ix == nx - 1 ? nx - 1 : ix + 1;
		for (it = 1; it < nt-1; it++) {
			headDx->trArr[ix].data[it] = headIn->trArr[ix2].data[it-1] - headIn->trArr[ix1].data[it-1]
				+ headIn->trArr[ix2].data[it] - headIn->trArr[ix1].data[it]
				+ headIn->trArr[ix2].data[it+1] - headIn->trArr[ix1].data[it+1]
				;
			headDx->trArr[ix].data[it] *= 0.333333333f;
		}

		headDx->trArr[ix].data[0] = headIn->trArr[ix2].data[0] - headIn->trArr[ix1].data[0]
			+ headIn->trArr[ix2].data[1] - headIn->trArr[ix1].data[1];
		headDx->trArr[ix].data[0] *= 0.5f;

		headDx->trArr[ix].data[nt-1] = headIn->trArr[ix2].data[nt-2] - headIn->trArr[ix1].data[nt-2]
			+ headIn->trArr[ix2].data[nt-1] - headIn->trArr[ix1].data[nt-1];
		headDx->trArr[ix].data[nt-1] *= 0.5f;
	}


	return true;
}


