#include "stdafx.h"
#include "math.h"
#include "float.h"
#include "ealloc_d.h"
#include "rayHelper.h"
#include "VelMod.h"
#include "segyMeta.h"

CRayHelper::CRayHelper() 
{
}

CRayHelper::~CRayHelper() 
{
}

//void CRayHelper::freeRaypathsAll(CRayHelper::RAYPATHS *raypaths2)
//{
//	if (raypaths2 && raypaths2->getTotCurve() > 0) {
//		if (raypaths2->npts != NULL) free1int(raypaths2->npts);
//		if (raypaths2->irrcol != NULL) free1int(raypaths2->irrcol);
//		if (raypaths2->xr != NULL) free2float(raypaths2->xr);
//		if (raypaths2->zr != NULL) free2float(raypaths2->zr);
//		//if (raypaths2->xrClipCached != NULL) free2double(raypaths2->xrClipCached);
//		//if (raypaths2->zrClipCached != NULL) free2double(raypaths2->zrClipCached);
//	}
//
//	raypaths2->npts = NULL;
//	raypaths2->irrcol = NULL;
//	raypaths2->xr = NULL;
//	raypaths2->zr = NULL;
//	raypaths2->setTotCurve(0);  // this is important
//	//raypaths2->xrClipCached = NULL;
//	//raypaths2->zrClipCached = NULL;
//
//}
//BOOL CRayHelper::initRaypathsArray2(CRayHelper::RAYPATHS *raypaths2, int npoint, int ncurve)
//{
//	if (raypaths2->totCurve > 0) freeRaypathsAll(raypaths2); // this is important for avoiding memory leaks.
//
//	if (ncurve > 10000 || npoint > MIL) return FALSE;  // too much memory is needed curves
//
//	// alocate enough space to work with
//	ncurve += 30;
//	npoint += 100;
//	raypaths2->npts = (int *)ealloc1int(npoint);
//	raypaths2->irrcol = (int *)ealloc1int(ncurve);
//	raypaths2->xr = (float **)ealloc2float(npoint, ncurve);
//	raypaths2->zr = (float **)ealloc2float(npoint, ncurve);
//	raypaths2->npts_max = npoint;
//	raypaths2->totCurve = ncurve;
//	//raypaths2->xrClipCached = (double **)ealloc2double(npoint*2, ncurve);
//	//raypaths2->zrClipCached = (double **)ealloc2double(npoint*2, ncurve);
//	return TRUE;
//}


BOOL CRayHelper::loadRayPathFromFile(CRayHelper::RAYPATHS *raypaths2)
{
//	return TRUE;

	CString f = raypaths2->rayFileName;
	if (raypaths2->rayFileModifiedTime == getFileModifiedTime(f) && raypaths2->getTotCurve() > 0) return TRUE; // checked cached

	CFile file;
	// here I do not use  | CFile::shareDenyNone otherwise it may read too many times
	if (!file.Open(f, CFile::modeRead | CFile::typeBinary)) {
		return FALSE;
	}
	int nptsRead;
	register UINT nBytes;

	int npMax2 = 0;
	int ncMax2 = 0;
	int temp;
	ULONGLONG fileLen = file.GetLength();
	while (file.Read(&nptsRead, sizeof(int)) == sizeof(int)) {
		if (nptsRead <= 0) {
			// this means the raypath file is corupt, or old style
			file.Close();
			return FALSE;
		}
		if (npMax2 < nptsRead) npMax2 = nptsRead;

		file.Read(&temp, sizeof(int)); // this is irrcol
		nBytes = nptsRead * sizeof(float);
		register ULONGLONG pos = file.Seek(nBytes*2, CFile::current);
		if (pos <= fileLen)
			ncMax2++;

		if (pos >= fileLen)
			break;  // this is important to avoid exception
	} // while

	if (!raypaths2->initRaypathsArray(npMax2, ncMax2)) {
		file.Close();
		return FALSE;
	}

	int count = 0;
	file.SeekToBegin();
	while (file.Read(&nptsRead, sizeof(int)) == sizeof(int)) {
		raypaths2->npts[count] = nptsRead;
		if (nptsRead < 1) break; // this is useful to avoid exception
		if (file.Read(&raypaths2->irrcol[count], sizeof(int)) != sizeof(int))
			break;
		nBytes = nptsRead * sizeof(float);
		if (file.Read(&raypaths2->xr[count][0], nBytes) != nBytes)
			break;
		if (file.Read(&raypaths2->zr[count][0], nBytes) != nBytes)
			break;
		count++;

		// the last reading was not complete and so we ignore it
		if (count >= ncMax2) 
			break; // no more data can be read in
		//if (count >= 6080) {
		//	int iiittttt = 1;
		//}
	} // while
	file.Close();
	raypaths2->rayFileModifiedTime = getFileModifiedTime(f);
	raypaths2->setTotCurve(count);
	return raypaths2->getTotCurve() > 0;
}
BOOL CRayHelper::saveRayPath(CRayHelper::RAYPATHS *raypaths)
{
	// we need to re-create file:  rPath.out.twtt   
	CString f = raypaths->rayFileName;
	CFile file2;
	if (!file2.Open(f, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
		file2.Close();
		return FALSE;
	}

	for (int count = 0; count < raypaths->getTotCurve(); count++) {
		register int npts = raypaths->npts[count];
		register int irrcol = raypaths->irrcol[count];
		register UINT nBytes = npts * sizeof(float);
		file2.Write(&npts, sizeof(int));
		file2.Write(&irrcol, sizeof(int));
		file2.Write(raypaths->xr[count], nBytes);
		file2.Write(raypaths->zr[count], nBytes);
	} // for count
	file2.Close();
	return TRUE;
}

BOOL CRayHelper::loadRayPathFromDepth2TWTT(
	CRayHelper::RAYPATHS *raypaths,
	CRayHelper::RAYPATHS *raypathsTwt, 
	float dxVDesired, float dz, CVelMod::VELMOD *velm, BOOL isShowDetails)
{
	if (!raypaths || !raypathsTwt) return FALSE;
	if (raypaths->rayFileName.IsEmpty() || raypathsTwt->rayFileName.IsEmpty()) return FALSE;
	if (!isFileExistNotDirectory(raypaths->rayFileName)) return FALSE;
	if (velm->ncont < 2) return FALSE;

	int nzv = (int)ceil(velm->zmax / dz) + 1;
	if (nzv >= SU_NFLTS) return FALSE;  // this prevent array overflow


	// we need to re-create twtt version only in 2 cases: 
	// 1. fDepth is newer, 2. RAYPATHS struct in twtt domain is invalid
	if (isFileNewer(raypathsTwt->rayFileName, raypaths->rayFileName) && loadRayPathFromFile(raypathsTwt))
		return TRUE;


	// now the twtt version should be reloaded, because it is newer or it is not loaded yet.
	if (!isFileNewer(raypathsTwt->rayFileName, raypaths->rayFileName)) {
		if (!loadRayPathFromFile(raypaths)) {
			// cannot load even the original RAYPATHS in depth, give up
			return FALSE;
		}
	}


	// we need to re-create file:  rPath.out.twtt   
	CFile file2;
	if (!file2.Open(raypathsTwt->rayFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
		file2.Close();
		return FALSE;
	}

	CVelMod vm;
	CVelMod::VELMOD_X modX;

	
//	double *xmm = (double *)ealloc1double(raypaths->npMax + 1);
	double xmm[TX_POINTS_MAX];
	xmm[0] = velm->getXmin();
	int count = 0;

	do {
			register double xmMin = velm->getXmax();
			for (int icont = 0; icont < velm->ncont; icont++) {
				for (int j = 1; j < velm->nzed[icont]; j++) {
					if (velm->xm[j][icont] > xmm[count] && velm->xm[j][icont] < xmMin)
						xmMin = velm->xm[j][icont];
				}
			}
			if (xmMin > xmm[count]) {
				xmm[count + 1] = xmMin;
				count++;
				if (count >= TX_POINTS_MAX)
					break;
			}
			else
				break;
	} while (true);

	int nxv = count;
	float dzThreshold = isShowDetails ? (float)(9.*dz) : (float)(0.2*dz);
	float *tr = (float *)ealloc1float(raypaths->npMax + 1);
	double **trv = (double **)alloc2double(nzv, nxv);  // vtr[ix][iz] is for caching the Z-T conversion table for quicker action, iz=[0, dtz ...]

	velm->m_isConvert = true;
	for (int ix = 0; ix < nxv; ix++) {
		modX.dist = xmm[ix];
		vm.initModX(velm, &modX, false, true, dz);
		// vm.getZModFromZ(&velMod, &modX);
		vm.getTzFromZModGrad(&modX, nzv, dz, &trv[ix][0]);
	}

	double *twtt = (double *)ealloc1double((nzv + 2));

	velm->m_isConvert = false;
	for (count = 0; count < raypaths->getTotCurve(); count++) {
			register int npts = raypaths->npts[count];
			//if (npts > TX_POINTS_MAX) npts = TX_POINTS_MAX;
			register int irrcol = raypaths->irrcol[count];
			file2.Write(&npts, sizeof(int));
			file2.Write(&irrcol, sizeof(int));

			// now convert zr[] to twtt domain
			for (int j = 0; j < npts; j++) {
				//#############The following causes memory leak. Help###################
				register float x = raypaths->xr[count][j];
				register float z = raypaths->zr[count][j];

				tr[j] = -1.f;

				//########### consider remove this segment
				register bool isLowPoint =
					j>0 && j < npts - 1 &&
					(fabs(z - raypaths->zr[count][j - 1]) < dzThreshold ||
						fabs(z - raypaths->zr[count][j + 1]) < dzThreshold ||
						(z > raypaths->zr[count][j - 1] && z > raypaths->zr[count][j + 1]));
				if (!isLowPoint) {
					register int ix1 = -1;
					for (int ix = 0; ix < nxv-1; ix++) {
						if (x >= xmm[ix] && x <= xmm[ix + 1]) {
							ix1 = ix;
							break;
						}
					}
					if (ix1 < 0) continue;

					// now inpololate between 
					register double aiz = z / dz;  // tr[] is in time (sec)
					register int iz = (int)aiz + 1;
					if (iz > 1 
						&& ix1 < nxv - 1 
						&& fabs(trv[ix1][iz - 1] - trv[ix1 + 1][iz - 1]) < 2.*dz) {
						tr[j] = (float)interpolLinear((double)trv[ix1][iz - 2],
							(double)trv[ix1][iz - 1], (double)(aiz - iz));
							register double b = (float)interpolLinear((double)trv[ix1 + 1][iz - 2],
								(double)trv[ix1 + 1][iz - 2], (aiz - iz));
							register double tt = (x - xmm[ix1]) / (xmm[ix1 + 1] - xmm[ix1]);
							tr[j] = (float)interpolLinear((double)tr[j], (double)b, tt);
					}
				}
				//########### end consider 

				if (tr[j]<0.) {
					// if here, it is the lowest point and we need to re-calculate the TWTT for this point
					modX.dist = x;
					vm.initModX(velm, &modX, false, true, dz);
					tr[j] = (float)vm.getTimeAtZ(&modX, z, dz, twtt, nzv+2);
				}
			} // for j
			file2.Write(raypaths->xr[count], npts*sizeof(float));
			file2.Write(tr, npts*sizeof(float));
	} // for count
	free1double(twtt);


	free1float(tr);
//	free1double(xmm);
	free2double(trv);
	file2.Close();


	// raypaths2 struct may be incomplete in some parameters. So let us reload it from file to avoid coding complexities.
	return loadRayPathFromFile(raypathsTwt);
}


//BOOL CRayHelper::loadRayPathFromDepth2TWTTOld(
//		CRayHelper::RAYPATHS *raypaths,
//		CRayHelper::RAYPATHS *raypathsTwt,
//		float dxVDesired, float dz, CVelMod::VELMOD *velm, BOOL isShowDetails)
//{
//		if (!raypaths || !raypathsTwt) return FALSE;
//		if (raypaths->rayFileName.IsEmpty() || raypathsTwt->rayFileName.IsEmpty()) return FALSE;
//		if (!isFileExistNotDirectory(raypaths->rayFileName)) return FALSE;
//
//		// we need to re-create twtt version only in 2 cases: 
//		// 1. fDepth is newer, 2. RAYPATHS struct in twtt domain is invalid
//		if (isFileNewer(raypathsTwt->rayFileName, raypaths->rayFileName) &&
//			loadRayPathFromFile(raypathsTwt))
//			return TRUE;
//
//
//		// now the twtt version should be reloaded, because it is newer or it is not loaded yet.
//		if (!isFileNewer(raypathsTwt->rayFileName, raypaths->rayFileName)) {
//			if (!loadRayPathFromFile(raypaths)) {
//				// cannot load even the original RAYPATHS in depth, give up
//				return FALSE;
//			}
//
//			// we need to re-create file:  rPath.out.twtt   
//			CFile file2;
//			if (!file2.Open(raypathsTwt->rayFileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
//				file2.Close();
//				return FALSE;
//			}
//
//			CVelMod vm;
//			CVelMod::VELMOD_X modX;
//
//			double dxv = dxVDesired;
//			int nxv = 200;  		//myRound((cd.coord.xmax - cd.coord.xmin) / dxv);
//
//			double dtz = dz;  // this is important 
//			float dzThreshold = isShowDetails ? (float)(9.*dtz) : (float)(0.2*dtz);
//
//			dxv = (velm->xmax() - velm->xmin()) / (double)(nxv - 1);
//			int nzv = (int)myRound(velm->zmax / dtz) + 1;
//			double **trv = (double **)alloc2double(nzv, nxv);  // vtr[ix][iz] is for caching the Z-T conversion table for quicker action, iz=[0, dtz ...]
//			for (int ix = 0; ix < nxv; ix++) {
//				modX.dist = velm->xmin() + ix*dxv;
//				vm.initModX(velm, &modX, false, true, false, dz);
//				// vm.getZModFromZ(&velMod, &modX);
//				vm.getTzFromZModGrad(&modX, nzv, dtz, &trv[ix][0]);
//			}
//
//			double *twtt = (double *)ealloc1double((nzv + 2));
//			for (int count = 0; count < raypaths->getTotCurve(); count++) {
//				register int npts = raypaths->npts[count];
//				register int irrcol = raypaths->irrcol[count];
//				file2.Write(&npts, sizeof(int));
//				file2.Write(&irrcol, sizeof(int));
//
//				// now convert zr[] to twtt domain
//				float *tr = (float *)ealloc1float(npts);
//				for (int j = 0; j < npts; j++) {
//					register float az = raypaths->zr[count][j];
//					register bool isLowPoint =
//						j>0 && j < npts - 1 &&
//						(fabs(az - raypaths->zr[count][j - 1]) < dzThreshold ||
//						fabs(az - raypaths->zr[count][j + 1]) < dzThreshold ||
//						(az > raypaths->zr[count][j - 1] && az > raypaths->zr[count][j + 1]));
//
//					if (!isLowPoint) {
//						register double aix = raypaths->xr[count][j] / dxv;
//						register double aiz = az / dtz;  // tr[] is in time (sec)
//						register int ix = (int)aix;
//						if (ix < 0) ix = 0;
//						if (ix > nxv - 1) ix = nxv - 1;
//						register int iz = (int)aiz + 1;
//						if (iz > 1) {
//							register double a = trv[ix][iz - 2] + (trv[ix][iz - 1] - trv[ix][iz - 2]) * (aiz - iz);
//							if (ix < nxv - 1) {
//								register double b = trv[ix + 1][iz - 2] + (trv[ix + 1][iz - 1] - trv[ix + 1][iz - 2]) * (aiz - iz);
//								a = a + (b - a)*(aix - ix);
//							}
//							tr[j] = (float)a;  // tr[] is now TWTT
//						}
//						else
//							tr[j] = 0.f;
//					}
//					else {
//						// if here, it is the lowest point and we need to re-calculate the TWTT for this point
//						modX.dist = raypaths->xr[count][j];
//						vm.initModX(velm, &modX, false, true, false, dz);
//						tr[j] = (float)vm.getTimeAtZ(&modX, raypaths->zr[count][j], dtz, twtt, nzv+2);
//					}
//				} // for j
//				file2.Write(raypaths->xr[count], npts*sizeof(float));
//				file2.Write(tr, npts*sizeof(float));
//				free1float(tr);
//			} // for count
//			free1double(twtt);
//			free2double(trv);
//			file2.Close();
//		} // if
//
//		// raypaths2 struct may be incomplete in some parameters. So let us reload it from file to avoid coding complexities.
//		return loadRayPathFromFile(raypathsTwt);
//}

// this is for 
BOOL CRayHelper::loadRayPathFromDepth2TWTT_detail(
	CRayHelper::RAYPATHS *raypaths,
	CRayHelper::RAYPATHS *raypaths3, int npoint, int ncurve, 
	float dxVDesired, float dz, CVelMod::VELMOD *velm)
{
	CString fDepth = raypaths3->rayFileName;
	CString fTwtt = raypaths3->rayFileName;
	if (!raypaths3 || !isFileExistNotDirectory(fDepth)) return FALSE;

	// we need to re-create twtt version only in 2 cases: 1. fDepth is newer, 2. RAYPATHS struct in twtt domain is invalid
	if (isFileNewer(fTwtt, fDepth) && loadRayPathFromFile(raypaths3))
		return TRUE;

	CVelMod vm;
	// now the twtt version should be reloaded, because it is newer or it is not loaded yet.
	if (!isFileNewer(fTwtt, fDepth)) {
		if (!loadRayPathFromFile(raypaths)) {
			// cannot load even the original RAYPATHS in depth, give up
			return FALSE;
		}

		// we need to re-create file:  rPath.out.twtt   
		CFile file2;
		if (!file2.Open(fTwtt, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary)) {
			file2.Close();
			return FALSE;
		}

		CVelMod::VELMOD_X modX;
		int nxv = 50;  		//myRound((cd.coord.xmax - cd.coord.xmin) / dxv);
		double dxv = (velm->getXmax() - velm->getXmin()) / (float)(nxv - 1);
		double odxv = 1. / dxv;
		int nzv = (int)ceil(velm->zmax / dz) + 1;
		float odz = 1.f / dz;
		double **trv = (double **)alloc2double(nzv, nxv);  // vtr[ix][iz] is for caching the Z-T conversion table for quicker action, iz=[0, vel.dz ...]
		velm->m_isConvert = true;
		for (int ix = 0; ix < nxv; ix++) {
			modX.dist = (velm->getXmin() + ix*dxv);
			vm.initModX(velm, &modX, false, true, dz);
			vm.getTzFromZModGrad(&modX, nzv, dz, &trv[ix][0]);
		}

		int npts_max = 0;
		for (int count = 0; count < raypaths->getTotCurve(); count++) {
			npts_max = max(npts_max, raypaths->npts[count]);
		}

		float *zr = (float *)ealloc1float(npts_max);
		for (int count = 0; count < raypaths->getTotCurve(); count++) {
			register int npts = raypaths->npts[count];
			register int irrcol = raypaths->irrcol[count];
			file2.Write(&npts, sizeof(int));
			file2.Write(&irrcol, sizeof(int));

			// now convert zr[] to twtt domain
			for (int j = 0; j < npts; j++) {
				register float aix = (float)(raypaths->xr[count][j] * odxv);
				register int ix = (int)aix;
				if (ix < 0) ix = 0;
				if (ix > nxv - 1) ix = nxv - 1;
				register float aiz = raypaths->zr[count][j] * odz;  // zr[] is in depth (km)
				register int iz = (int)aiz + 1;
				if (iz > 1) {
					register double a = trv[ix][iz - 2] + (trv[ix][iz - 1] - trv[ix][iz - 2]) * (aiz - iz);
					if (ix < nxv - 1) {
						register double b = trv[ix + 1][iz - 2] + (trv[ix + 1][iz - 1] - trv[ix + 1][iz - 2]) * (aiz - iz);
						a = a + (b - a)*(aix - ix);
					}
					zr[j] = (float)a;  // zr[] is now TWTT
				}
				else
					zr[j] = 0.f;
			} // for j
			file2.Write(raypaths->xr[count], npts*sizeof(float));
			file2.Write(zr, npts*sizeof(float));
		} // for count
		free1float(zr);
		free2double(trv);
		file2.Close();
	} // if


	// raypaths2 struct may be incomplete in some parameters. So let us reload it from file to avoid coding complexities.
	return loadRayPathFromFile(raypaths3);
}


//
//int CRayHelper::isPointsLeftofShotOld(CRayHelper::TRAVELTIME *tx2)
//{
//	//returns; -1=left; 0=plus; 1=right; Can be directly moved to other files
//	// note: 1. an offset can be calculated by: xoffset=xshot-x; xshot+x; x-xshot for -1, 0, 1 cases
//	//       2. this value cannot be assigned globally since the file is calculated by fortran 
//	//          and loaded passively in C++ , and fortran program does not supply this value
//	if (tx2->tot[0] < 1 && tx2->tot[1] < 1) return 0;
//
//	bool isLeftSideOnly = true;
//	bool isRightSideOnly = true;
//	// double xshot = tx2->xShot;
//	for (int j = 0; j < 2; j++)
//		for (register int k = 0; k < tx2->tot[j]; k++) {
//			register int n = tx2->num[k][j];
//			for (register int i = 0; i < n; i++) {
//				register float xpos = tx2->x[i][k][j];
//				if (xpos < tx2->xShotRay*0.9) {
//					isRightSideOnly = false;
//					break;
//				}
//				else if (xpos > tx2->xShotRay*1.1) {
//					isLeftSideOnly = false;
//					break;
//				}
//
//				// note: xpos==xshot can be true, so we want to ignore this here
//				// Why we use *0.9  and  *1.1 here? It is because the actual numbers may not be very exact
//				// e.g. When xshot=35, left side shots can be calculated to occur at 35.02km
//			}
//		}
//
//
//	int out = 0;
//	if (!isRightSideOnly && isLeftSideOnly)
//		out = -1;
//	else if (!isLeftSideOnly && isRightSideOnly)
//		out = 1;
//
//	return out;
//}

BOOL CRayHelper::isMinusXShot(CRayHelper::TRAVELTIME *tx2)
{
	if (tx2->tot[0] < 1 && tx2->tot[1] < 1) return 0;

	bool isLessThanXs = true;
	bool isGreaterThanXs = true;
	int i, c;
	float xShot1 = tx2->xShotRay*0.99f;
	float xShot2 = tx2->xShotRay*1.01f;
	for (int k = 0; k < 2; k++) {
		if (tx2->tot[k] < 1) continue;
		
		for (c = 0; c < tx2->tot[k]; c++) {
			if (tx2->Num[k][c] < 2) continue;
			for (i = 0; i < tx2->Num[k][c]; i++) {
				if (tx2->X[k][i][c] < xShot1) {
					isGreaterThanXs = false;
					break;
				}
				// note: xpos==xshot can be true, so we want to ignore this here
				// Why we use *0.9  and  *1.1 here? It is because the actual numbers may not be very exact
				// e.g. When xshot=35, left side shots can be calculated to occur at 35.02km
			}
		}

		for (c = 0; c < tx2->tot[k]; c++) {
			if (tx2->Num[k][c] < 2) continue;
			for (i = 0; i < tx2->Num[k][c]; i++) {
				if (tx2->X[k][i][c] > xShot2) {
					isLessThanXs = false;
					break;
				}
			}
		}
	}

	//BOOL out;
	//if (isLessThanXs != isGreaterThanXs)
	//	out = FALSE;
	//else
	//	out = TRUE;

	return isLessThanXs && isGreaterThanXs;
	// note: if both are false, it is 2-sided, so return false
	// 
}


// Load xshot,zshot information from ray parameter file draw.rayName
// it will output results into arrays:  tx->shots[k][0], tx->shots[k][1]
// where k indexes to all xshot and zshot  listing
int CRayHelper::loadRayPara(CString rayName, CRayHelper::TRAVELTIME *tx)
{
	int i, k, m;
	CString cs, csDig;
	FILE *ifh = NULL;
	TCHAR line[255];
	int numShotOut = 0;
	k = tx->nShot = 0;
	if (isFileExistNotDirectory(rayName)) {
		// now load the contents of this ray parameter file
		if (_tfopen_s(&ifh, rayName, _T("r")) != 0) {
			tx->nShot = 0;
			return 0;
		}
		while (_fgetts(line, 255, ifh) != NULL) {
			cs.Format(_T("%s"), line);
			if (cs.GetLength() <= 0) continue;

			if (cs.Find(_T("&pltpar")) >= 0) {
				// we store colour codes here only, for plotting in the T-X curves.
				// the colour codes have to match the ones defined in the ray parameter file
				// since the raypath plotting uses this colour codes as done by Zelt's fortran program
				for (m = 0; m < 100; m++) {
					if ((i = cs.Find(_T("colour"))) >= 0) {
						while ((i = cs.Find(_T("colour"))) >= 0) {
							cs = cs.Mid(i + 5);
							cs = cs.Mid(cs.Find('=') + 1);
							k = 0;
							for (i = 0; i < 100; i++) {
								int iWordEnd = cs.FindOneOf(_T(", "));
								if (iWordEnd < 0) break;
								csDig = cs.Left(iWordEnd);
								csDig.Trim();
								if (csDig.GetLength() == 0) {
									cs = cs.Mid(1);
									continue;
								}
								tx->colour[k] = _tstoi(csDig);
								k++;
								tx->ncolour = k;
								cs = cs.Mid(iWordEnd + 1);
							}
						} // while
						break;
					} // if
					if (_fgetts(line, 255, ifh) != NULL) {
						cs.Format(_T("%s"), line);
					}
				} // for k
			} // if

			if (cs.Find(_T("&end")) >= 0) {
				// we strip all codes in the array except the final comments
				tx->rayCodes->RemoveAll();
				continue;
			}
			else {
				// we store the readings into array
				tx->rayCodes->Add(cs);
			}

			// now find out the xshot,zshot pairs
			i = cs.Find(_T("xshot"));
			if (i >= 0) {
				cs = cs.Mid(i + 5);
				cs = cs.Mid(cs.Find('=') + 1);
				numShotOut = 0;
				for (i = 0; i < 100; i++) {
					int iWordEnd = cs.FindOneOf(_T(", "));
					if (iWordEnd < 0) break;
					csDig = cs.Left(iWordEnd);

					csDig.Trim();
					if (csDig.GetLength() == 0) {
						cs = cs.Mid(1);
						continue;
					}
					tx->xzShot[0][numShotOut] = (float)_tstof(csDig);
					numShotOut++;
					cs = cs.Mid(iWordEnd + 1);
				}
			} // if i

			i = cs.Find(_T("zshot"));
			if (i >= 0) {
				cs = cs.Mid(i + 5);
				cs = cs.Mid(cs.Find('=') + 1);
				// cs = cs.Right('=');
				numShotOut = 0;
				for (i = 0; i < 100; i++) {
					int iWordEnd = cs.FindOneOf(_T(", "));
					if (iWordEnd < 0) break;
					csDig = cs.Left(iWordEnd);
					csDig.Trim();
					if (csDig.GetLength() == 0) {
						cs = cs.Mid(1);
						continue;
					}
					tx->xzShot[1][numShotOut] = (float)_tstof(csDig);
					numShotOut++;
					cs = cs.Mid(iWordEnd + 1);
				}
			} // if i


			i = cs.Find(_T(" ray"));
			if (i >= 0) {
				cs = cs.Mid(i + 5);
				cs = cs.Mid(cs.Find('=') + 1);
				// cs = cs.Right('=');
				k = 0;
				for (i = 0; i < 100; i++) {
					int iWordEnd = cs.FindOneOf(_T(", "));
					if (iWordEnd < 0) break;
					csDig = cs.Left(iWordEnd);
					csDig.Trim();
					if (csDig.GetLength() == 0) {
						cs = cs.Mid(1);
						continue;
					}

					tx->rayCodeIn[k] = (float)_tstof(csDig);
					k++;
					cs = cs.Mid(iWordEnd + 1);
				}
			} // if i

			i = cs.Find(_T("ivray"));
			if (i >= 0) {
				cs = cs.Mid(i + 5);
				cs = cs.Mid(cs.Find('=') + 1);
				// cs = cs.Right('=');
				k = 0;
				for (i = 0; i < 100; i++) {
					int iWordEnd = cs.FindOneOf(_T(", "));
					if (iWordEnd < 0) break;
					csDig = cs.Left(iWordEnd);
					csDig.Trim();
					if (csDig.GetLength() == 0) {
						cs = cs.Mid(1);
						continue;
					}

					tx->ivray[k] = _tstoi(csDig);
					k++;
					cs = cs.Mid(iWordEnd + 1);
				}
			} // if i
			if (k >= TX_SHOTS_MAX) break;
		} // while
		fclose(ifh);
	} // if isFileExist
	else {
		// CString a;
		// a.Format(_T("Ray parameter file invalid: %s", draw.rayName);
		// this->myCaption(a);
	}
	tx->nShot = numShotOut;
	return numShotOut;
}


// first it will try to load xShotRay from  rayName
// if it failed, it will try to load it from from segy file
void CRayHelper::setXShotAll(double xShotIn, CString rayName, CRayHelper::TRAVELTIME *tx, int iShot)
{
	if (isFileExistNotDirectory(rayName)) {
		// load xshot,zshot information from ray parameter file draw.rayName
		CRayHelper::loadRayPara(rayName, tx); // this will load all xShot values into *tx struct
	} // if !isXS

	if (tx->nShot == 0) {
		tx->xShotRay = (float)xShotIn;
		tx->iShot = 0;
		tx->nShot = 1;
		tx->xzShot[0][0] = tx->xShotRay;
	}
	else if (tx->iShot > 0 && tx->iShot<tx->nShot) {
		tx->xShotRay = tx->xzShot[0][tx->iShot];
	}
}



BOOL CRayHelper::myLoadTravelTime(CString txName, CRayHelper::TRAVELTIME *tx, float xShotDesired)
{
	//when return FALSE, then the tx curves for tx->xShotInSegy cannot be loaded.
	// then you need to consider changing the segy data
	// tx->xShotInSegy should be accurate up to 3 decimal points!

	// calling this routine will reload the data regardless of cachig
	CTime aTime = getFileModifiedTime(txName);
	if ((tx->tot[0] > 0 || tx->tot[1] > 0) && tx->txTimeLoaded == aTime && isFloatEqual3(tx->xShotRay, xShotDesired)) {
		return TRUE;
		// same data already inside tx struct. No need to load again
	}

	// now need to load the data
	CString tStr;
	CString tExt = GetFileExtension(txName);
	if (tExt != _T("out")) return FALSE;   // txName should be like tx->out

	long j, n, nOld, iShot, curSide;
	float t1, t2, t3;
	BOOL textErr = FALSE;

	CStringArray sArr;
	if (!CSegyMeta::getXshotListFromTxOut(txName, &sArr) || sArr.GetSize() < 1) {
		sArr.RemoveAll();
		return FALSE;  // since no TT curves loaded, tx.xShotRay will not be changed. tx.xShotRay is used in digitizing as well.
	}
	
	float xShotFirst = (float)StringToFloat(sArr.ElementAt(0));
	bool isDesiredXshotFound = false;
	for (j = 0; j < sArr.GetSize(); j++) {
		t1 = (float)StringToFloat(sArr.ElementAt(j));
		if (isFloatEqual(xShotDesired, t1)) {
			isDesiredXshotFound = true;
			break;
		}
	}
	sArr.RemoveAll();
	if (!isDesiredXshotFound) {
		// since the desired xshot not found, we just return whataver existing in tx struct
		if ((tx->tot[0] > 0 || tx->tot[1] > 0) && tx->txTimeLoaded == aTime)
			return TRUE;
		else
			tx->xShotRay = xShotFirst;
	}
	else
		tx->xShotRay = xShotDesired;
	//#########################################
	//#########################################
	FILE *stream;
	TCHAR line[MAXLINE_ZELT];


	if (_tfopen_s(&stream, txName, _T("r")) != 0) {
		return FALSE;
	}

	while (_fgetts(line, MAXLINE_ZELT, stream) != NULL) {
		if (_tcslen(line) < 2) continue;
		if (line[0] == _T('#')) continue; // we do not need comment lines
		_stscanf_s(line, _T("%g %g %g %d"), &t1, &t2, &t3, &n);
		if (n < 0) {
			fclose(stream);
			return FALSE;
		}

		if (
			isFloatEqual(tx->xShotRay, t1)
			&& isFloatEqual(0.f, t3) 
			&& n == 0) {
			// this indicates the xshot of the matchable TT curves
			iShot = (long)t2;
			break;
		}
	}
	// noe the position in stream is correct.

	// let's reset the arrays first
	j = 0;
	int i = 0;
	nOld = -1;

	// the difference between iShot  and curSide: 
	//		iShot is Zelt's tx.out format, either -1  or 1
	//		curSize follows array index format, either 0 or 1
	if (iShot == -1) curSide = 0;
	else if (iShot == 1) curSide = 1;
	else {
		fclose(stream);
		return FALSE;
	}

	for (j = 0; j < TX_CURVES_MAX; j++) {
		tx->Num[0][j] = 0;
		tx->Num[1][j] = 0;
	}
	tx->tot[0] = tx->tot[1] = 0;

	// rewind(stream);
	i = 0;
	while (_fgetts(line, MAXLINE_ZELT, stream) != NULL) {
		if (_tcslen(line) < 2) continue;
		if (line[0] == '#') continue; // we do not need comment lines
		tStr.Format(_T("%s"), line);
		if (tStr.Find(_T("NaN")) >= 0) continue;
		_stscanf_s(line, _T("%g %g %g %d"), &t1, &t2, &t3, &n);

		if (n == -1 || (!isFloatEqual(t1, tx->xShotRay) && t3 == 0 && n == 0)) {
			//last line of the file.
			tx->tot[curSide] ++;
			break;
		}
		if (t2 >= 0.f && t3 > 0.f) {
			if (n == nOld) {
				//  or the same curve
				i = tx->Num[curSide][tx->tot[curSide]]; // note that i will change frequently locally
				tx->X[curSide][i][tx->tot[curSide]] = t1;
				tx->T[curSide][i][tx->tot[curSide]] = t2;
				tx->RayCodePerCurve[curSide][tx->tot[curSide]] = n;
				if (tx->Num[curSide][tx->tot[curSide]] < TX_POINTS_MAX)
					tx->Num[curSide][tx->tot[curSide]]++;  // only one point exists for that curve
			}
			else {
				if (tx->tot[curSide] == 0 && tx->Num[curSide][0] == 0) {
					// starting the first curve
					tx->tot[curSide] = 0; // indicating the current curve number
					tx->X[curSide][0][0] = t1;
					tx->T[curSide][0][0] = t2;
					tx->RayCodePerCurve[curSide][0] = n;
					tx->Num[curSide][0] = 1;
				}
				else {
					// a new curve is found
					if (tx->tot[curSide] >= TX_CURVES_MAX) {
						fclose(stream);
						AfxMessageBox(_T("Maximum number of travel-time curves reached."), MB_ICONQUESTION);
						return FALSE;
					}

					tx->tot[curSide]++;  // indicating the current curve number
					tx->X[curSide][0][tx->tot[curSide]] = t1;
					tx->T[curSide][0][tx->tot[curSide]] = t2;
					tx->RayCodePerCurve[curSide][tx->tot[curSide]] = n;
					tx->Num[curSide][tx->tot[curSide]] = 1;
				}
			} // else
			nOld = n;
		} // if t2
		else	{
			if (t2 != 0.f && n == 0) {
				if (tx->Num[curSide][tx->tot[curSide]] > 0) {
					// this means all data from the previous curSide is already read in
					tx->tot[curSide] ++;
				}

				if (t2 == -1.f) curSide = 0;
				else if (t2 == 1.f) curSide = 1;
				nOld = -999;
			}
			else {
				break;
			}
		} // else
	} // while
	fclose(stream);

	// if here, it terminates un-expectedly
	//tx->num[tx->tot[curSide]][curSide] ++;
	//tx->tot[curSide] ++;

	tx->txTimeLoaded = aTime;  // this enables caching
	return TRUE;
}



double CRayHelper::getXPosAll_deprec(CString timeSecName, CString workingPathSlash) {
	double out = ERRORVAL;

	if (isFileExistNotDirectory(timeSecName))  {
		// if here, the current segy display has a requirement to display only its own xshot
		out = CSegyMeta::getXShotInSegy2(timeSecName);
	}


	if (CCoords::isXShotDefined((float)out))  {
		return out;
	}

	if (isFileExistNotDirectory(timeSecName))  {
		// the segy exists but no valid xshot detected, we should not display any TX curves
		return out;
	}

	// Now the segy does not exist, we just need to take the first available xshot from tx.out 
	// CString txPath = workingPathSlash;
	if (!isDirectoryExist(workingPathSlash))
		workingPathSlash = _T("tx.out");
	else
		workingPathSlash += _T("tx.out");
	CString tStr = getLineFirst(workingPathSlash).Trim();
	int j = tStr.Find(SPACE);
	if (j > 0) {
		CString tStr2 = tStr.Left(j);
		if (IsNumeric(tStr2)) {
			out = StringToFloat(tStr2);
		}
	}

	// here, if we still failed to get a valid xShot from a segy, we do not want get from rayName file otherwise it is confusing to the user

	return out;
}


bool CRayHelper::setVelocityXmLimits(CVelMod::VELMOD * velm, RAYPATHS *raypaths, int icont, int ic, bool isTime)
{
	if (ic<0 || ic>raypaths->m_totCurve - 1) return false;
	if (icont<0 || icont>velm->ncont - 1) return false;
	int j, k;
	float X, Y, ay, by;
	float xmin = -ERRORVAL;
	float xmax = ERRORVAL;
	for (j = 0; j < velm->nzed[icont]-1; j++) {
		if (isTime) {
			ay = velm->tm[j][icont];
			by = velm->tm[j + 1][icont];
		}
		else {
			ay = velm->zm[j][icont];
			by = velm->zm[j + 1][icont];
		}
		for (k = 0; k < raypaths->npts[ic] - 1; k++) {
			if (LineIntersection(velm->xm[j][icont], ay, velm->xm[j + 1][icont], by,
				raypaths->xr[ic][k], raypaths->zr[ic][k], raypaths->xr[ic][k + 1], raypaths->zr[ic][k + 1], &X, &Y)) {
				xmin = min(X, xmin);
				xmax = max(X, xmax);
			}
		}
	}

	velm->xmMin[icont] = velm->xmMin[icont] == ERRORVAL ? xmin : min(xmin, velm->xmMin[icont]);
	velm->xmMax[icont] = velm->xmMax[icont] == ERRORVAL ? xmax : max(xmax, velm->xmMax[icont]);
	return true;
}

bool CRayHelper::setVelocityXmLimits(CVelMod::VELMOD * velm, RAYPATHS *raypaths, bool isTime)
{
	if (velm->ncont <= 0 || raypaths->m_totCurve <= 0) return false;
	for (int icont = 0; icont < velm->ncont; icont++) {
		for (int ic = 0; ic < raypaths->m_totCurve; ic++) {
			setVelocityXmLimits(velm, raypaths, icont, ic, isTime);
		}
	}
	return true;
}