#pragma once
#include "segy.h"
#include "util.h"
#include "base64.h"
#include "xallocator.h"
// CSegyMeta

class CSegyMeta
{
public:
	//int x, y;
	CSegyMeta();
	~CSegyMeta();

	// note: I deliberatly made the following functions to be static
	//    because they will not use any class variables. All variables will 
	//    be supplied at the calling time, and only one: HEAD *head .
	static double getAvgInterval(double x[], int n);
	static int segyHeadInit(CString t, HEAD *head);
	inline static void segyHeadFree(HEAD *head)
	{
		if (head->pDataArr != NULL) {
			xfree(head->pDataArr);
			head->pDataArr = NULL;
		}
		if (head->tminArr != NULL) {
			xfree(head->tminArr);
			head->tminArr = NULL;
		}

		TRY{
			if (head->traces != NULL) {
				xfree(head->traces);
				head->traces = NULL;
			}
			if (head->cdp != NULL) {
				xfree(head->cdp);
				head->cdp = NULL;
			}
			if (head->cdpt != NULL) {
				xfree(head->cdpt);
				head->cdpt = NULL;
			}
			if (head->ffid != NULL) {
				xfree(head->ffid);
				head->ffid = NULL;
			}
			if (head->dist != NULL) {
				xfree(head->dist);
				head->dist = NULL;
			}
			if (head->offsets != NULL) {
				xfree(head->offsets);
				head->offsets = NULL;
			}
			if (head->sx != NULL) {
				xfree(head->sx);
				head->sx = NULL;
			}
			if (head->sy != NULL) {
				xfree(head->sy);
				head->sy = NULL;
			}
		}
			CATCH_ALL(e) {
		}
		END_CATCH_ALL;


		head->numTraces = 0;
	}



	inline static int getCountNonMonoArr(float arr[], int n)
	{
		int count = 0;
		for (int j = 0; j<n - 1; j++) {
			if (arr[j] > arr[j + 1])
				count++;
		} // j
		return count;
	}

	static int getNumTraceInSegy(CString f);
	static int getNumTraceInSegy(CFile *file);
	static BOOL getSegyMetaHead(CString f, HEAD *head);
	static CString segyFormatToString(short iFormat);

	static bool Serialize(CArchive &ar, HEAD *head);
	static CString getSegyMetaCacheName(CString fSegy, bool isWriteNeeded);
	static bool getSegyMetaCache(CString fEncoded, HEAD *head);
	static bool setSegyMetaCache(CString fEncoded, HEAD *head);

	static void CSegyMeta::sortDistance(HEAD *pHead, bool offsetMode = false);

	static double getlatLongFromSegyMod(HEAD *pHead, float dist,
		float &latitude, float &longitude);

	static float getXShotInSegy2(CString f);
	static BOOL getXshotListFromTxOut(CString txName, CStringArray *sArr);
	
	// after this, the segy data is freed, but the trace header and section headers will remain.
	inline static void freeSegyDataArrayOnly(segydynamic *trArr, int nxmax) {
		if (trArr != NULL) {
			for (int jj = 0; jj < nxmax; jj++) {
				if (trArr[jj].data != NULL) {
					free1float(trArr[jj].data);
					trArr[jj].data = NULL;
				}
			}
		} // if
	}

	// NOTE: the calling function must manually set trArr = NULL, even though this function does it
	// because they are different numbers
	inline static void freeSegyArray(segydynamic *trArr, int nxmax) {
		// clean up before return
		if (trArr != NULL) {
			freeSegyDataArrayOnly(trArr, nxmax);
			free1(trArr);
			trArr = NULL; 
		} // if
	}


	inline static BOOL swapIbmIeee(CString inFile) {
		if (!isFileExistNotDirectory(inFile)) return FALSE;
		bhed bh;
		BOOL isUnix = false;
		CFile file;
		if (!file.Open(inFile, CFile::modeReadWrite | CFile::typeBinary | CFile::shareDenyNone)) {
			return FALSE;
		}
		file.Seek(3200, CFile::begin);
		if (file.Read(&bh, 400L) < 400) {
			file.Close();
			return FALSE;
		}
		if (isUnix = IsUnix(bh.format))
			SwapTapeHeader(&bh);
		if (bh.format != 1 && bh.format != 5) {
			file.Close();
			return FALSE;
		}

		if (bh.format == 1)
			bh.format = 5;
		else
			bh.format = 1;
		if (isUnix) SwapTapeHeader(&bh); // put back to Unix or PC format

		file.Seek(3200, CFile::begin);
		file.Write(&bh, 400L);
		file.Close();
		return TRUE;
	}

	// note: *file must be already opened with readWrite access rights!!!
	inline static void setSegyTotalTrace(CFile *file, int numtotal) {
		if (file == NULL) return;
		if (numtotal > 0) {
			bhed bh;
			file->Seek(3200L, CFile::begin);
			file->Read(&bh, 400L);
			if (IsUnix(bh.format))
				SwapTapeHeader(&bh);
			bh.setnx(numtotal);
			if (IsUnix(bh.format))
				SwapTapeHeader(&bh);
			file->Seek(3200L, CFile::begin);
			file->Write(&bh, 400L);
		}
	}
private:
	// if return 1, it is initialized, otherwise return 0
	inline static bool segyHeadInitArrays(HEAD *head, int numTraces)
	{
		if (head->numTraces == numTraces && !head->traces == false)
			return 1;
		// no need to touch the arrays since their sizes are already perfect

		// start to initialize the segy
		segyHeadFree(head);
		head->numTraces = numTraces;
		head->traces = (int *)xmalloc(numTraces* sizeof(int));
		head->pDataArr = (ULONGLONG *)xmalloc(sizeof(ULONGLONG)*numTraces);
		head->tminArr = (float *)xmalloc(numTraces* sizeof(float));
		head->cdp = (int *)xmalloc(numTraces* sizeof(int));
		head->cdpt = (int *)xmalloc(numTraces * sizeof(int));
		head->ffid = (int *)xmalloc(numTraces* sizeof(int));
		head->dist = (float *)xmalloc(numTraces* sizeof(float));
		head->offsets = (float *)xmalloc(numTraces* sizeof(float));
		head->sx = (float *)xmalloc(numTraces* sizeof(float));
		head->sy = (float *)xmalloc(numTraces* sizeof(float));
		return 1;
	}

};



