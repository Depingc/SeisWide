#pragma once

#define MOD_LAYER_MAX 50
#define MOD_POINTS_MAX 999
#define MOD_REGRESSION_MAX 5
#define MOD_LOAD_MAX 20
#define TX_POINTS_MAX 1999  // I once got 1100 points per curve for a raypath; if there exist more points per curve than this num, it will not show
#define TX_CURVES_MAX 899 // if many shots to display together, we have lots curves
#define TX_SHOTS_MAX 100
#define NUM_WATER_LAYER 1
#define MAXLINE_ZELT 999     /* The max length of each text line input, for Zelt format only */
#include "segy.h"
#include "coords.h"
#include "colorHelper.h"

class CVelMod
{
private:

public:

	// Note a dilemma: the VELMOD struct defines 2D arrays with indeces in a revease
	// way relative to a proper convention: e.g. xm[point][layer] , which does not
	// make it possible to simply copy memory blocks using &xm[layer][0] et al.
	// In the future one may reverse this

	// NOTE 2:  we do not want a default VELMOD velMod struct, since CSeisWideDoc class already has one such default struct created.
	//          we can have such a member variable here, but it would cause confusion
	struct DEPLOY {
		CString metaTitle;
		double deployDist;
		double deployDepth;
		double deployLatLong[2];  // [0] is lat, [1] is lon
		double startDist;
		double startOffset;
		double startLatLong[2];
		double endDist;
		double endOffset;
		double endLatLong[2];
		inline void swapStartEnd() {
			double a;
			a = startDist;
			startDist = endDist;
			endDist = a;


			a = startOffset;
			startOffset = endOffset;
			endOffset = a;


			a = startLatLong[0];
			startLatLong[0] = endLatLong[0];
			endLatLong[0] = a;

			a = startLatLong[1];
			startLatLong[1] = endLatLong[1];
			endLatLong[1] = a;
		}

		inline const int count() {
			for (int i = 0; i < TX_SHOTS_MAX; i++) {
				if (deployDist == ERRORVAL) return i;
			}
			return 0;
		}

		DEPLOY() {
			deployLatLong[0] = 
				deployLatLong[1] =
				startLatLong[0] = 
				startLatLong[1] =
				endLatLong[0] = 
				endLatLong[1] = ERRORVAL;
			deployDist = startDist = endDist = startOffset = endOffset = ERRORVAL;
			deployDepth = 0.;
		}
	};

	struct PSEUDOWELL {
		CString title;
		double wellDist;  // this is made float since it is saved in .SEI file
		double wellLatLong[2];
		CString wellText;  // this is the pseudo-well details, for output only, not used in modeling
		PSEUDOWELL() {
			title = wellText = _T("");
			wellDist = ERRORVAL;
			wellLatLong[0] = wellLatLong[1] = ERRORVAL;
		}
	};

	struct VELMOD{
		CString velSecName;
		CTime modifiedLoaded; // normally equals to the file modified time. But if it is 0, the VELMOD should be saved, otherwise it should be loaded from file. 
		int ncont;  // if -1, it is an invalid model and need initializing. If 0, already initialized with proper arrays
		int ncontMaxConstrained; // always smaller than ncont
		BOOL isVelBelowValid;

		// now this struct has been made with dynamic arrays, while the array names and dimensions are still the same.
		int *nzed, **nvel;
		float **xm, **zm, ***xvel,***vf;
		float **tm;  // model control points in TWTT domain, calculated field, not stored in file

		// optional 3 extra parameters for sedimentary layers
		double regVZero = 1.69;
		double regVInf = 5.33;
		double regAlpha = 0.42745;
		double regVmin = 1.53;
		double regVmax2 = 4.3;  // not exposed to UI.
		int nRegressionLayer;  // 1-based. User will see this number so it should be 1-based.
		// if >0, it means the layer for regression! 
		// 0 for no regression; 1 means for water layer which cannot be regressed as well!
		// the final decision for performing regression will also depend on velocity values

		// note: velMod.xmin and velMod.xmax can be different from cd.coord.xmin and cd.coord.xmax!
		// if model is smaller the display window, xminDisplay is diff from parax.min.
		// otherwise, xminDisplay,xmaxDisplay should be the same as  cd.coord.xmin and cd.coord.xmax
		// So everytime we display a model, we have re-calc the xminDisplay,xmaxDisplay, **depth,**vTop,**vBot, **depthTwtt
		// double xmin, xmax;  // added for easier modeling. replaced by inline functions for coevenience of programing
		double zmin, zmax;  // added for easier modeling. Not saved to .sei file. Cannot be replaced by inline functions
		float *anisotropy;
		double poisson[MOD_LAYER_MAX];
		double xmMin[MOD_LAYER_MAX];
		double xmMax[MOD_LAYER_MAX];


		// #######################################
		// #######################################
		// note: floating reflectors are extra lines loaded only when doing raytracing
		//  therefore, they are assigned before raytracing, and are not assigned when you are using velMod2 et al.
		//  One may free these arrays in the destruction of CSeisWideDoc. They are auto free-ed before allocation
		CString flFloatName;
		CString flFloatNameOld; //name of file for floating reflector in digitize format. 
			// if it is different from flFloatName, it will try to load it
		CFileStatus flFloatReflStatus;
		int flTotCurve, *flNpArray; // num of points/curves in the floating reflectors
		float **flXArray, **flTArray;    // data for the floating reflectors.
		//  end of lines for floating reflectors
		// #######################################
		// #######################################

		// #######################################
		// #######################################
		// meta info for saving to .vxml format only
		CString metaCruise;
		CString metaProcessor;
		CString metaCompany;
		CString metaDate;
		DEPLOY deploys[TX_SHOTS_MAX];
		PSEUDOWELL wells[TX_SHOTS_MAX];
		double **latitude;
		double **longitude;
		// #######################################
		// #######################################


		// #######################################
		// #######################################
		// color gridding params, not saved to model file
		int grdNx; //for vel interpolation.
		bool isGrdInitialised; // DELETE? model depth[][],depthTwtt[][] is not meshed even though the arrays are initialized
		double grdXmin;  // added for easier modeling. Not saved to .sei file. 
		// double grdXmax;  // no need to put to VELMOD struct, since it can be calculated:
			// grdXmax = grdXmin + (grdNx-1)*grdDx
		double grdDx; // it can be adjusted during interpolation since the user specified dx can be too small for big displays
		double **grdDepth,**vTop,**vBot; //for vel interpolation/color gridding. not saved
		double **grdTwtt; // added for storing vel interpolation/gridding results in TWTT domain, calculated field, not stored in file 
		BOOL isNoNodeNav; // for display purpose only, not related to modeling

		int nFlatten;
		float *xmFlatten;
		float *zmFlatten;
		float *tmFlatten;
		float zShotDatum;
		float tShotDatum;
		// #######################################
		// #######################################

		// #######################################
		// #######################################
		// for gravity modeling, not related to velocity modeling
		bool isDenPrompt; // for prompting dialogbox for Vel-Den conversion
		double denMax;
		double velRef[9], denRef[9]; // adding for interactive gravity modeling, not saved: convert from V to Density
		// #######################################
		// #######################################
		inline const bool isVelModValid() {
			return  ncont>0 &&
				ncont < MOD_LAYER_MAX &&
				nzed[0]>0 &&
				nzed[0] <= MOD_POINTS_MAX;
		}
		inline const double getXmin() {
			return xm[0][0];
		}
		inline const double getXmax() {
			int t = nzed[0];
			return t>0 ? xm[t-1][0] : 0.;
		}

		inline const double getXmin(int icont) {
			double out = xm[0][0];
			
			if (fabs(this->xmMin[icont] - ERRORVAL) > 0.1) {
				// we want to use it
				if (this->xmMin[icont] > out && this->xmMin[icont] < this->getXmax())
					out = this->xmMin[icont];
			}
			return out;
		}

		inline const double getXmax(int icont) {
			double out = getXmax();

			if (fabs(this->xmMax[icont] - ERRORVAL) > 0.1) {
				// we want to use it
				if (this->xmMax[icont] < out && this->xmMax[icont] > this->getXmin())
					out = this->xmMax[icont];
			}
			return out;
		}

		inline const double getVavg(int ilayer) {
			if (ilayer < 0 || ilayer >= ncont - 1) return 0.;  // ilayer is invalid
			int i;
			double v;

			double vmin = 9999.f;
			double vmax = -9999.f;
			for (i = 0; i < nvel[ilayer][0] - 1; i++) {
				v = vf[i][ilayer][0];
				if (vmin > v) vmin = v;
				if (vmax < v) vmax = v;
			}
			double vTop = (vmin + vmax) * 0.5;

			vmin = 9999.f;
			vmax = -9999.f;
			for (i = 0; i < nvel[ilayer][1] - 1; i++) {
				v = vf[i][ilayer][1];
				if (vmin > v) vmin = v;
				if (vmax < v) vmax = v;
			}
			double vBot = (vmin + vmax) * 0.5;
			return (vTop + vBot) * 0.5;
		}

		inline const bool isModelVisible(CCoords::SEI_COORD *pCoord, double xShift) {
			if (!isFileExistNotDirectory(this->velSecName)) return false;
			if (pCoord->drawmode == 1) return false; // if purely in depth without modelinging, we should return false
			if (ncont < 0 || !nzed) return false; // not initialized
			if (nzed[0] < 1) return false; // no points exist
			if (xShift + getXmin() >= pCoord->xmax || xShift + getXmax() <= pCoord->xmin) return(FALSE);
			return(TRUE);
		}

		// this isSedimentRegression() will consider all X points instead of one X in VELMMOD_X::isSedimentRegression()
		inline const bool isSedimentRegression(int ilayer) {
			if (ilayer<0 || ilayer >= nRegressionLayer || ilayer+1==NUM_WATER_LAYER ) return false;
			float vTop = -1.f;
			int i;

			for (int k = 0; k < 2; k++) {
				for (i = 0; i < nvel[ilayer][k] - 1; i++) {
					register double v = vf[i][ilayer][k];
					if (v > regVmin && v <= regVmax2) return true;
				}
			}
			return false;
		}

		// this will decide if this model really has regression applicable
		inline const bool isSedimentRegression() {
			if (nRegressionLayer <= 0) return false;
			for (int i = 0; i < nRegressionLayer; i++) {
				if (isSedimentRegression(i))
					return true;
			}
			return false;
		}

		// if greyed, the layer is not controled by the data in theory
		inline const bool isLayerGreyed(int ilayer) {
			return ilayer + 1 > ncontMaxConstrained
				|| (ilayer + 1 == ncontMaxConstrained && !isVelBelowValid);
		}

		// it is very tricky here
		inline void setSedimentRegression(int ilayer, BOOL isChecked) {
			if (isChecked) {
				if (nRegressionLayer >= ilayer + 1) return; // perfect
				nRegressionLayer = ilayer + 1;
			}
			else {
				// now the current layer should not be regression-ed
				if (nRegressionLayer < ilayer + 1) return; // perfect
				if (nRegressionLayer == ilayer+1)
					nRegressionLayer--;

				// now, what about if the water layer is selected, and 
				// nRegressionLayer > 0 ? It is ok.

			}
		}

		//  Z is sub-water depth
		inline const double getRegVelAtZ(double Z) {
			if (Z < 0.000001) return regVZero;
			//VELMOD_X modX;
			//modX.ncont = this->ncont;
			//modX.nRegressionLayer = this->nRegressionLayer;
			//modX.regVZero = this->regVZero;
			//modX.regVInf = this->regVInf;
			//modX.regAlpha = this->regAlpha;
			//modX.regVmin = this->regVmin;
			//modX.regVmax = this->regVmax;
			//return modX.getRegVelAtZ(Z);

			double rVZero = fabs(regVZero) == 0. ? 0. : 1. / regVZero;
			double rVInf = fabs(regVInf) == 0. ? 0. : 1. / regVInf;
			double v1 = rVInf + (rVZero - rVInf) * exp(-regAlpha * Z);
			if (v1 == 0.) v1 = regVZero;
			else
				v1 = 1. / v1;
			if (v1 < regVmin) v1 = regVmin;
			else if (v1 > regVmax2) v1 = regVmax2;

			return v1;
		}
		inline const float getVelIncNice(float v) {
			float out = 0.1f;
			if (v < 3.f) out = 0.02f;
			else out = 0.05f;
			return out;
		}

		// this will get the current horizon depth even if the model is already flattened
		inline const float getCurrHorizonDepthAtDist(float x, int icont) {
			if (icont < 1) return 0.f;

				int n = nzed[icont];
				if (n < 1) return 0.f;
				if (x <= xm[0][icont]) return zm[0][icont];
				else if (x >= xm[n - 1][icont]) return zm[n - 1][icont];

				for (int j = 1; j < n; j++) {
					if (x >= xm[j - 1][icont] && x <= xm[j][icont]) {
						register float dx = x - xm[j - 1][icont];
						if (isFloatEqual(dx, 0.f)) return zm[j - 1][icont]; // this saves CPU

						if (xm[j][icont] == xm[j - 1][icont]) return zm[j][icont]; // very rare

						register float t = dx / (xm[j][icont] - xm[j - 1][icont]);
						return interpolLinear(zm[j - 1][icont], zm[j][icont], t);
					}
				}
				return zm[0][icont];
		}

		// this is only linear interpolation, not accurate
		inline const float getCurrHorizonTimeAtDist(float x, int icont) {
			
			if (icont < 1) return 0.f;
			int n = nzed[icont];
			if (n < 1) return 0.f;
			if (x <= xm[0][icont]) return tm[0][icont];
			else if (x >= xm[n - 1][icont]) return tm[n - 1][icont];

			for (int j = 1; j < n; j++) {
				if (x >= xm[j - 1][icont] && x <= xm[j][icont]) {
					register float dx = x - xm[j - 1][icont];
					if (isFloatEqual(dx, 0.f)) return tm[j - 1][icont]; // this saves CPU

					if (xm[j][icont] == xm[j - 1][icont]) return tm[j][icont]; // this is very rare

					register float t = dx / (xm[j][icont] - xm[j - 1][icont]);
					return interpolLinear(tm[j - 1][icont], tm[j][icont], t);
				}
			}
			return tm[0][icont];
		}

		inline const float getCurrHorizonAtDist(float x, int icont, bool isTime) {
			if (isTime)
				return getCurrHorizonTimeAtDist(x, icont);
			else
				return getCurrHorizonDepthAtDist(x, icont);
		}


		// when a complex model is plotted in time, some horizons interct with others. To solve this,
		// I output 3 arrays describing all horizons properly interpolated without any intercecting.
		// Will return the fixed number of points per horizon for all horizons
		inline const int getXmAll(double xx[]) {
			int i, j;
			xx[0] = xm[0][0];
			int count = 1;

			while (true) {
				xx[count] = 99999.;
				for (i = 1; i < ncont; i++) {
					for (j = 0; j < nzed[i]; j++) {
						if (xm[j][i] > xx[count - 1]+0.00001 && xm[j][i] < xx[count])
							xx[count] = xm[j][i];
					}
				}
				if (xx[count] > 99998.) break;
				count++; // accept this value
			} // while

			return count;
		}


		// ncontFlatten is 1-based, not 0-based!
		inline void setHorizonModel2Flat(int ncontFlatten, float xShot) {
			if (ncontFlatten <= 0) return;
			int iFlat = ncontFlatten - 1;
			if (nFlatten > 0) return;
			nFlatten = nzed[iFlat];
			if (nFlatten < 1) return;

			zShotDatum = getCurrHorizonDepthAtDist(xShot, iFlat);
			tShotDatum = getCurrHorizonTimeAtDist(xShot, iFlat);
			xmFlatten = (float *)ealloc1float(nFlatten);
			zmFlatten = (float *)ealloc1float(nFlatten);
			tmFlatten = (float *)ealloc1float(nFlatten);
			for (int j = 0; j < nFlatten; j++) {
				xmFlatten[j] = xm[j][iFlat];
				zmFlatten[j] = zm[j][iFlat];
				tmFlatten[j] = tm[j][iFlat];
			}

			// first, insert all points at xmFlatten[]
			for (int icont = 0; icont < ncont; icont++) {
				addPoint(xmFlatten, nFlatten, icont);
			}

			for (int icont = 0; icont < ncont; icont++) {
				for (int j = 0; j < nzed[icont]; j++) {
					// register float x = xm[j][icont];
					register float z = interpolLinear(xmFlatten, zmFlatten, nFlatten, xm[j][icont]);
					zm[j][icont] -= z - zShotDatum;
					register float t = interpolLinear(xmFlatten, tmFlatten, nFlatten, xm[j][icont]);
					tm[j][icont] -= t - tShotDatum;
				}
			}
		}

		// this will undo the flatten process
		inline void setHorizonFlat2Model(int ncontFlatten, float xShot, bool isTime) {
			if (ncontFlatten <= 0) return;
			int iFlat = ncontFlatten - 1;
			if (nFlatten <= 0) return; // no need to do anything

			for (int icont = 0; icont < ncont; icont++) {
				for (int j = 0; j < nzed[icont]; j++) {
					register float x = xm[j][icont];
					register float z = interpolLinear(xmFlatten, zmFlatten, nFlatten, x);
					zm[j][icont] += z - zShotDatum;
					register float t = interpolLinear(xmFlatten, tmFlatten, nFlatten, x);
					tm[j][icont] += t - tShotDatum;
				}
			}

			// now remove the many extra points: just remove any points 
			for (int icont = 0; icont < ncont; icont++) {
				if (isTime)
					delRedundentPntTime(icont);
				else
					delRedundentPntDepth(icont);
			}


			// now free the arrays
			resetNonflattenValues();
		}

		// this init will not change ncontFlatten 
		inline void resetNonflattenValues() {
			nFlatten = 0;
			zShotDatum = tShotDatum = 0.f;
			if (xmFlatten) free1float(xmFlatten); xmFlatten = NULL;
			if (zmFlatten) free1float(zmFlatten); zmFlatten = NULL;
			if (tmFlatten) free1float(tmFlatten); tmFlatten = NULL;
		}


		inline void freeFlArrays()
		{
			// this is for extra information inside the VELMOD struct
			if (!flNpArray) free1int(flNpArray);
			if (!flXArray) free2float(flXArray);
			if (!flTArray) free2float(flTArray);

			flNpArray = NULL;
			flXArray = NULL;
			flTArray = NULL;
		}

		// will return the index for the added point
		// if -1, it is not added
		inline int addPoint(float x, int icont) {
			if (icont < 0 || ncont<1 || icont>ncont - 1) return -1;
			if (x <= getXmin() || x >= getXmax()) return -1;
			if (nzed[icont] >= MOD_POINTS_MAX) {
				return -1;
			}

			int j, k;

			// if the point already exists within 1 meter, we do not insert
			for (j = 0; j < nzed[icont]; j++) {
				if (fabs(x - xm[j][icont]) < 0.001) {
					return -1;
				}
			}
			
			// now the new point
			for (j = 0; j < nzed[icont]-1; j++) {
				if (isInsideFloat(x, xm[j][icont], xm[j + 1][icont])) {
					register double dx = xm[j + 1][icont] - xm[j][icont];
					if (fabs(dx) < 0.001) return -1; // insering not valid
					for (k = nzed[icont]; k > j + 1; k--) {
						xm[k][icont] = xm[k - 1][icont];
						zm[k][icont] = zm[k - 1][icont];
						tm[k][icont] = tm[k - 1][icont];
						longitude[k][icont] = longitude[k - 1][icont];
						latitude[k][icont] = latitude[k - 1][icont];
					}
					register double t = (x - xm[j][icont]) / dx;
					xm[j + 1][icont] = x;
					// note that point [j+1] has been moved to [j+2]
					zm[j + 1][icont] = interpolLinear(zm[j][icont], zm[j + 2][icont], (float)t);
					tm[j + 1][icont] = interpolLinear(tm[j][icont], tm[j + 2][icont], (float)t);
					longitude[j + 1][icont] = ERRORVAL;
					latitude[j + 1][icont] = ERRORVAL;
					nzed[icont] ++;
					return j+1;
				} // if
			} // for j

			return -1;
		}

		inline bool addPoint(float xIns[], int nIns, int icont) {
			if (icont < 0 || ncont<1 || icont>ncont - 1) return false;
			for (int jIns = 0; jIns < nIns; jIns++) {
				addPoint(xIns[jIns], icont);
			} // for jIns

			return true;
		}

		inline bool delPoint(int icont, int jDel) {
			if (icont < 0 || ncont<1 || icont>ncont - 1) return false;
			if (jDel <= 0 || jDel >= nzed[icont] - 1) return false;
			// first and last points cannot be deleted

			for (int k = jDel; k < nzed[icont] - 1; k++) {
				xm[k][icont] = xm[k + 1][icont];
				zm[k][icont] = zm[k + 1][icont];
				tm[k][icont] = tm[k + 1][icont];
				longitude[k][icont] = longitude[k + 1][icont];
				latitude[k][icont] = latitude[k + 1][icont];
			}
			nzed[icont]--;
			return true;
		}

		inline void delRedundentPntDepth(int icont) {
			if (icont < 0 || ncont<1 || icont>ncont - 1) return;
			for (int j = 0; j < nzed[icont]-2; j++) {
				register double dx = xm[j + 2][icont] - xm[j][icont];
				if (fabs(dx) < 0.001) continue; 
				register double t = (xm[j + 1][icont] - xm[j][icont]) / dx;
				register double yMid = interpolLinear(zm[j][icont], zm[j + 2][icont], (float)t);
				if (fabs(yMid - zm[j+1][icont]) < 0.0009) {
					// now remove this point. Will keep points when spaced at 0.001
					delPoint(icont, j + 1);
					j--; // since one point is deleted, we will repeat this process at the same index
				}
			}
		}

		inline void delRedundentPntTime(int icont) {
			if (icont < 0 || ncont<1 || icont>ncont - 1) return;
			for (int j = 0; j < nzed[icont] - 2; j++) {
				register double dx = xm[j + 2][icont] - xm[j][icont];
				if (fabs(dx) < 0.0009) continue;
				register double t = (xm[j + 1][icont] - xm[j][icont]) / dx;
				register double yMid = interpolLinear(tm[j][icont], tm[j + 2][icont], (float)t);
				if (fabs(yMid - tm[j + 1][icont]) < 0.001) {
					// now remove this point. Will keep points when spaced at 0.001
					delPoint(icont, j + 1);
					j--; // since one point is deleted, we will repeat this process at the same index
				}
			}
		}

		inline const double getTimeMax() {
			int j;
			double tmax = -MIL;
			double t;
			for (int i = 0; i < ncont; i++) {
				for (j = 0; j<nzed[i]; j++) {
					t = tm[j][i];
					if (t > tmax) tmax = t;
				}
			}
			return tmax;
		}

		inline const double getDepthWaterDepre(double x) {
			double depthWater = 0.;
			int icont = NUM_WATER_LAYER;  // only the first layer is water
			if (x < getXmin()) x = getXmin();
			if (x > getXmax()) x = getXmax();
			for (int j = 0; j < nzed[icont] - 1; j++) {
				if (x >= xm[j][icont] && x <= xm[j + 1][icont]) {
					register double dx = xm[j + 1][icont] - xm[j][icont];
					if (dx == 0.)
						depthWater = zm[j][icont];
					else
						depthWater = zm[j][icont] + (zm[j + 1][icont] - zm[j][icont])*(x - xm[j][icont]) / dx;
					break;
				}
			}
			return depthWater;
		}
		inline const double getTwtWaterDepre(double x) {
			double twtWater = 0.;
			int icont = NUM_WATER_LAYER;  // only the first layer is water
			if (x < getXmin()) x = getXmin();
			if (x > getXmax()) x = getXmax();
			for (int j = 0; j < nzed[icont] - 1; j++) {
				if (x >= xm[j][icont] && x <= xm[j + 1][icont]) {
					register double dx = xm[j + 1][icont] - xm[j][icont];
					if (dx == 0.)
						twtWater = tm[j][icont];
					else
						twtWater = tm[j][icont] + (tm[j + 1][icont] - tm[j][icont])*(x - xm[j][icont]) / dx;
					break;
				}
			} 
			return twtWater;
		}

		inline void velModReset() {
			// this will reset everything to zero, assuming the arrays are already assigned.
			// this function can be useful if you want to re-use an established CVelMod struct
			// some numbers need to reset to zero
			for (int icont = 0; icont < MOD_LAYER_MAX; icont++) {
				nzed[icont] = 0;
				nvel[icont][0] = 0;
				nvel[icont][1] = 0;
				anisotropy[icont] = 0.f;
				poisson[icont] = 0.;
				xmMin[icont] = xmMax[icont] = ERRORVAL;
				for (int j = 0; j < MOD_POINTS_MAX; j++) {
					latitude[j][icont] = ERRORVAL;
					longitude[j][icont] = ERRORVAL;
				}
			}
			ncont = 0; // indicating the arrays already initialized

			for (int i = 0; i < TX_SHOTS_MAX; i++) {
				this->deploys[i].deployDist = ERRORVAL;
			}

			for (int i = 0; i < TX_SHOTS_MAX; i++) {
				this->wells[i].wellDist = ERRORVAL;
			}
		}
		inline void velModSetArrays() {
			// if you load the struct with another model, do not call this routine. But instead call velModReset()
			if (ncont >= 0 && xm != NULL) {
				// we already have arrays properly allocated, so simple
				ncont = 0;
				return;
			}


			grdNx = 500;
			nzed = ealloc1int(MOD_LAYER_MAX);
			nvel = ealloc2int(2, MOD_POINTS_MAX);
			xm = ealloc2float(MOD_LAYER_MAX, MOD_POINTS_MAX);
			zm = ealloc2float(MOD_LAYER_MAX, MOD_POINTS_MAX);
			xvel = ealloc3float(2, MOD_LAYER_MAX, MOD_POINTS_MAX);
			vf = ealloc3float(2, MOD_LAYER_MAX, MOD_POINTS_MAX);
			grdDepth = ealloc2double(grdNx, MOD_LAYER_MAX); //for vel interpolation.
			vTop = ealloc2double(grdNx, MOD_LAYER_MAX); //for vel interpolation.
			vBot = ealloc2double(grdNx, MOD_LAYER_MAX); //for vel interpolation.
			grdTwtt = ealloc2double(grdNx, MOD_LAYER_MAX);// added for storing vel interpolation results in TWTT domain, calculated field, not stored in file 
			tm = ealloc2float(MOD_LAYER_MAX, MOD_POINTS_MAX);  // model control points in TWTT domain, calculated field, not stored in file
			anisotropy = ealloc1float(MOD_LAYER_MAX);  // model control points in TWTT domain, calculated field, not stored in file

			// meta arrays
			latitude = ealloc2double(MOD_LAYER_MAX, MOD_POINTS_MAX);
			longitude = ealloc2double(MOD_LAYER_MAX, MOD_POINTS_MAX);

			// some numbers need to reset to zero
			velModReset();
		}

		inline void velModFreeArrays() {
			if (xm == 0) return;

			ncont = -1;
			grdNx = 0;
			free1int(nzed);
			free2int(nvel);
			free2float(xm);
			free2float(zm);
			free3float(xvel);
			free3float(vf);
			free2double(grdDepth);
			free2double(vTop);
			free2double(vBot);
			free2double(grdTwtt);
			free2float(tm);
			free1float(anisotropy);
			free2double(latitude);
			free2double(longitude);

			nzed = NULL;
			nvel = NULL;
			xm = NULL;
			zm = NULL;
			xvel = NULL;
			vf = NULL;
			grdDepth = NULL;
			vTop = NULL;
			grdTwtt = NULL;
			tm = NULL;
			anisotropy = NULL;
			resetNonflattenValues();
		}

		inline bool cloneTo(CVelMod::VELMOD* velmTo, CString fTo) {
			velmTo->modifiedLoaded = modifiedLoaded;
			velmTo->ncont = ncont;
			velmTo->velSecName = fTo;
			velmTo->ncontMaxConstrained = ncontMaxConstrained;
			velmTo->nFlatten = nFlatten;
			velmTo->isVelBelowValid = isVelBelowValid;		
			velmTo->nRegressionLayer = nRegressionLayer;
			velmTo->regVZero = regVZero;
			velmTo->regVInf = regVInf;
			velmTo->regAlpha = regAlpha;
			velmTo->zmin = zmin;
			velmTo->zmax = zmax;
			velmTo->metaCruise = metaCruise;
			velmTo->metaProcessor = metaProcessor;
			velmTo->metaCompany = metaCompany;
			velmTo->metaDate = metaDate;

			int ip, k;

			// deploy
			for (k = 0; k < deploys->count(); k++) {
				velmTo->deploys[k].deployDist = deploys[k].deployDist;
				velmTo->deploys[k].deployDepth = deploys[k].deployDepth;
				velmTo->deploys[k].deployLatLong[0] = deploys[k].deployLatLong[0];
				velmTo->deploys[k].deployLatLong[1] = deploys[k].deployLatLong[1];
				velmTo->deploys[k].endDist = deploys[k].endDist;
				velmTo->deploys[k].endLatLong[0] = deploys[k].endLatLong[0];
				velmTo->deploys[k].endLatLong[1] = deploys[k].endLatLong[1];
				velmTo->deploys[k].metaTitle = deploys[k].metaTitle;
				velmTo->deploys[k].startDist = deploys[k].startDist;
			}

			// wells
			for (k = 0; k < TX_SHOTS_MAX; k++) {
				if (wells[k].wellDist == ERRORVAL) break;
				velmTo->wells[k].wellDist = wells[k].wellDist;
				velmTo->wells[k].title = wells[k].title;
				velmTo->wells[k].wellText = wells[k].wellText;
				velmTo->wells[k].wellLatLong[0] = wells[k].wellLatLong[0];
				velmTo->wells[k].wellLatLong[1] = wells[k].wellLatLong[1];
			}

			for (int icont = 0; icont < ncont; icont++) {
				velmTo->nzed[icont] = nzed[icont];
				velmTo->xmMin[icont] = xmMin[icont];
				velmTo->xmMax[icont] = xmMax[icont];
				for (ip = 0; ip < nzed[icont]; ip++) {
					velmTo->xm[ip][icont] = xm[ip][icont];
					velmTo->zm[ip][icont] = zm[ip][icont];
					velmTo->tm[ip][icont] = tm[ip][icont];
					velmTo->latitude[ip][icont] = latitude[ip][icont];
					velmTo->longitude[ip][icont] = longitude[ip][icont];
				}
				for (k = 0; k < 2; k++) {
					velmTo->nvel[icont][k] = nvel[icont][k];
					for (ip = 0; ip < nvel[icont][k]; ip++) {
						velmTo->xvel[ip][icont][k] = xvel[ip][icont][k];
						velmTo->vf[ip][icont][k] = vf[ip][icont][k];
					}
				}
			} // for icont
			return false;
		}

		// it is possible that the current first pseudo-well does not have valid xpos
		inline const double getXWellDef() {
			double out = ERRORVAL;
			int i;
			for (i = 0; i < TX_SHOTS_MAX; i++) {
				if (wells[i].wellDist == ERRORVAL) break;
				out = wells[i].wellDist; // only need to pick 1 valid value
				break;
			}
			if (out == ERRORVAL) {
				for (i = 0; i < TX_SHOTS_MAX; i++) {
					if (deploys[i].deployDist == ERRORVAL) break;
					out = deploys[i].deployDist; // only need to pick 1 valid value
					break;
				}
			}

			if (out < getXmin())
				out = getXmin() + 8.;
			else if (out > getXmax())
				out = getXmax() + 8.;
			return out;
		}

		bool m_isConvert; // an internal cursor indicating if the time-depth conversion is already completed
		VELMOD() : nRegressionLayer(0),
			grdNx(0), ncont(-1), ncontMaxConstrained(0), isVelBelowValid(true),
			isGrdInitialised(false), isDenPrompt(true), denMax(3.33),
			isNoNodeNav(FALSE), m_isConvert(true),
			nFlatten(0), xmFlatten(NULL), zmFlatten(NULL), tmFlatten(NULL),
			zShotDatum(0.f), tShotDatum(0.f),
			nzed(NULL), nvel(NULL), xm(NULL), zm(NULL), tm(NULL), xvel(NULL), vf(NULL), 
			anisotropy(NULL), 
			grdDepth(NULL), grdTwtt(NULL), vTop(NULL), vBot(NULL), 
			flTotCurve(0), flNpArray(NULL), flXArray(NULL), flTArray(NULL) {
			velModSetArrays();
		};

		~VELMOD() {
			velModFreeArrays();
		}
	};

	// TX_SHOTS_MAX
	struct VELMODS {
		// note: if I set 99 models as max, it will fail to initiate at all
		double xShiftArr[MOD_LOAD_MAX];
		double xposArr[MOD_LOAD_MAX];
		VELMOD velm[MOD_LOAD_MAX];
		int num;
		VELMODS() : num(0) {}
	};

	// note: VELMOD_X struct only stores info ar each layer boundary at a fixed X.
	struct VELMOD_X{
		int ncont; // total num of boundaries
		double depthMod[MOD_LAYER_MAX];
		double timeMod[MOD_LAYER_MAX];
		double vTop[MOD_LAYER_MAX], vBot[MOD_LAYER_MAX];
		double dist; // the vel values are all for dist km
		double regVZero = 1.69;
		double regVInf = 5.33;
		double regAlpha = 0.42745;
		double regVmin = 1.53;
		double regVmax2 = 4.3;  // not exposed to UI. Only for deciding if the layer can be regressed
		int  nRegressionLayer;

		// can be improved later
		inline double getWaterDepth() {
			if (NUM_WATER_LAYER < 0) return 0.;
			return depthMod[NUM_WATER_LAYER];
		}
		inline double getWaterTwtt() {
			if (NUM_WATER_LAYER < 0) return 0.;
			return timeMod[NUM_WATER_LAYER];
		}
		inline double getWaterVelBot() {
			if (NUM_WATER_LAYER < 1) return 1.5; // general case
			return vBot[NUM_WATER_LAYER-1];
		}
		inline void cloneTo(VELMOD_X *modXto) {
			modXto->ncont = ncont;
			if (ncont < 1) return;
			modXto->dist = dist;
			modXto->regVZero = regVZero;
			modXto->regVInf = regVInf;
			modXto->regAlpha = regAlpha;
			modXto->regVmin = regVmin;
			modXto->regVmax2 = regVmax2;
			modXto->nRegressionLayer = nRegressionLayer;
			memcpy(modXto->depthMod, depthMod, sizeof(double)*ncont);
			memcpy(modXto->timeMod, timeMod, sizeof(double)*ncont);
			memcpy(modXto->vTop, vTop, sizeof(double)*(ncont-1));
			memcpy(modXto->vBot, vBot, sizeof(double)*(ncont - 1));
		}

		inline void interpol(VELMOD_X *modX1, VELMOD_X *modX2, double range) {
			//VELMOD_X modXOut;
			this->dist = range;
			double dx = (modX2->dist - modX1->dist);
			if (dx == 0.) {
				modX1->cloneTo(this);
				return;
			}
			double tFac = (range - modX1->dist) / dx;
			this->depthMod[0] = modX1->depthMod[0];
			this->timeMod[0] = modX1->timeMod[0];
			this->vTop[0] = interpolLinear(modX1->vTop[0], modX2->vTop[0], tFac);
			this->vBot[0] = interpolLinear(modX1->vBot[0], modX2->vBot[0], tFac);
			int count = 1;
			int i, i2;
			for (i = 1; i < modX1->ncont; i++) {
				i2 = i; if (i2 > modX2->ncont-2) i2 = modX2->ncont-2;
				register double za = interpolLinear(modX1->depthMod[i], modX2->depthMod[i2], tFac);
				register double ta = interpolLinear(modX1->timeMod[i], modX2->timeMod[i2], tFac);
				register double vaTop = interpolLinear(modX1->vTop[i], modX2->vTop[i2], tFac);
				register double vaBot = interpolLinear(modX1->vBot[i], modX2->vBot[i2], tFac);

				i2 = i+1; if (i2 > modX2->ncont - 2) i2 = modX2->ncont - 2;
				register double zb = interpolLinear(modX1->depthMod[i], modX2->depthMod[i2], tFac);
				register double tb = interpolLinear(modX1->timeMod[i], modX2->timeMod[i2], tFac);
				register double vbTop = interpolLinear(modX1->vTop[i], modX2->vTop[i2], tFac);
				register double vbBot = interpolLinear(modX1->vBot[i], modX2->vBot[i2], tFac);

				if (za < zb) {
					this->depthMod[count] = za;
					this->timeMod[count] = ta;
					this->vTop[count] = vaTop;
					this->vBot[count] = vaBot;
					count++;
					this->depthMod[count] = zb;
					this->timeMod[count] = tb;
					this->vTop[count] = vbTop;
					this->vBot[count] = vbBot;
					count++;
				}
				else if (za == zb) {
					this->depthMod[count] = za;
					this->timeMod[count] = ta;
					this->vTop[count] = vaTop;
					this->vBot[count] = vaBot;
					count++;
				}
				else {
					this->depthMod[count] = zb;
					this->timeMod[count] = tb;
					this->vTop[count] = vbTop;
					this->vBot[count] = vbBot;
					count++;
					this->depthMod[count] = za;
					this->timeMod[count] = ta;
					this->vTop[count] = vaTop;
					this->vBot[count] = vaBot;
					count++;
				}
			} // for i
			this->ncont = count;
			return;
		}


		inline const bool isSedimentRegression(int ilayer) {
			return ilayer < nRegressionLayer
				&& vTop[ilayer] > regVmin
				&& vBot[ilayer] <= regVmax2;
		}

		// this will decide if this model really has regression applicable
		inline const bool isSedimentRegression() {
			if (nRegressionLayer < 1) return false;
			for (int i = 0; i < nRegressionLayer; i++) {
				if (isSedimentRegression(i))
					return true;
			}
			return false;
		}
		//  Z is sub-water depth
		inline const double getRegVelAtZ(double Z) {
			if (Z < 0.000001) return regVZero;
			double rVZero = fabs(regVZero) == 0. ? 0. : 1. / regVZero;
			double rVInf = fabs(regVInf) == 0. ? 0. : 1. / regVInf;
			double v1 = rVInf + (rVZero - rVInf) * exp(-regAlpha * Z);
			if (v1 == 0.) v1 = regVZero;
			else
				v1 = 1. / v1;
			if (v1 < regVmin) v1 = regVmin;
			else if (v1 > regVmax2) v1 = regVmax2;

			return v1;
		}

		// note: T is sub-water TWT only; will return sub-water depth. Checked Oct 25, 2016
		inline const double regTime2DepthSlow(double T, double dz)
		{
			if (T < 0.000001) return 0.;
			double beta = log(regVInf / regVZero - 1.);
			double ovInf = 2. / regVInf;
			double oAlpha = 1. / regAlpha;
			double eBeta = exp(beta);
			double twt = 0.;
			double twtOld = 0.;
			double H = 0.;
			do {
				twtOld = twt;
				H += dz;
				twt = ovInf * ((eBeta - exp(beta - regAlpha*H)) * oAlpha + H);
			} while (twt < T);

			double deltat = twt - twtOld;
			return deltat == 0. ?
				H :
				H - dz + dz * (T - twtOld) / deltat;
		}

		// this is a test for a faster calculation
		inline const double regTime2Depth(double T, double dz)
		{
			if (T < 0.000001) return 0.;
			double beta = log(regVInf / regVZero - 1.);
			double ovInf = 2. / regVInf;
			double oAlpha = 1. / regAlpha;
			double eBeta = exp(beta);
			double twt = 0.;  // the starting twt
			double twtOld = 0.;  // the starting twt
			double Z = 0.;  // the starting depth

			// first, we test on dz*100
			double dzBig = 0.1; // make it a big number to save CPU time
			do {
				twtOld = twt;
				Z += dzBig;
				twt = ovInf * ((eBeta - exp(beta - regAlpha*Z)) * oAlpha + Z);
			} while (twt < T);


			// now we use dz to fine tune
			Z -= dzBig;  // the starting depth
			twt = twtOld;  // the starting twt
			do {
				twtOld = twt;
				Z += dz;
				twt = ovInf * ((eBeta - exp(beta - regAlpha*Z)) * oAlpha + Z);
			} while (twt < T);

			double deltat = twt - twtOld;
			double out = deltat == 0. ?
				Z : Z - dz + dz * (T - twtOld) / deltat;
			return out;
		}

		inline const double regDepth2Time(double H)
		{
			double beta = log(regVInf / regVZero - 1.);
			double twt = (exp(beta) - exp(beta - regAlpha*H)) / regAlpha + H;
			twt *= 2. / regVInf;
			return twt;
		}

		inline const BOOL regDepth2Time(double twt[], int n, double dz)
		{
			if (regVZero == 0. || dz <= 0. || n < 2) return FALSE;
			double beta = log(regVInf / regVZero - 1.);
			double ovInf = 2. / regVInf;
			for (int j = 0; j < n; j++) {
				register double H = j*dz;
				twt[j] = (exp(beta) - exp(beta - regAlpha*H)) / regAlpha + H;
				twt[j] *= ovInf;
			}
			return TRUE;
		}


		VELMOD_X() : nRegressionLayer(0), ncont(0) {};
	};


	CVelMod(void);
	~CVelMod(void);

	BOOL loadInitVel(CVelMod::VELMOD * velm, CString f, double dz, CString& strErr);
	void loadInitVel(VELMODS *velms, CString str, double dz);

	static BOOL myLoadFloatReflectors(CVelMod::VELMOD * velm);
	static BOOL mySaveFloatReflectorsIn(CVelMod::VELMOD * velm, CString f);
	static BOOL mySaveRInFile(CString fIn, CString rin, BOOL isSimplified);
	static void mySaveTXInFile(CString fIn, CString txin);
	void initModX(CVelMod::VELMOD * velm, CVelMod::VELMOD_X *modX, bool isTime, 
		bool isAnisotropyApply, double dz = 0.004);
	bool initModX(CString velSecNamesEx, CVelMod::VELMOD_X *modX, bool isTime,
		bool isAnisotropyApply, double dz = 0.004);
	bool initModX(VELMOD *velm, VELMODS *velms, CVelMod::VELMOD_X *modXOut1, CVelMod::VELMOD_X *modXOut2,
		double dist, bool isTime, bool isAnisotropyApply, double dz = 0.004);

	bool initModXOld(VELMOD *velm, VELMODS *velms, CVelMod::VELMOD_X *modXOut1, CVelMod::VELMOD_X *modXOut2,
		double dist, bool isTime, bool isAnisotropyApply, double dz = 0.004);
	void interpol_ModX_Notused(CVelMod::VELMOD_X *modX1, CVelMod::VELMOD_X *modX2, CVelMod::VELMOD_X *modXOut, double dist);
	// if it is in time domain, this will calc timeMod[] not depthMod[]
	// if in depth domain, this will calc depthMod[] not timeMod[]

	// this will do the complete conversion between time and depth domains, 
	// but it will be a little slower compared to the other func that does not do conversion

	BOOL MyCalcVelLimitsOld(CVelMod::VELMOD * velm); // this will not calc max time of model boundaries, since the VELMOD struct does not have it.
	BOOL formatCalcVelLimits(CVelMod::VELMOD * velm); 
		

	bool velModCloneArrays(CVelMod::VELMOD *velMod, CVelMod::VELMOD *velModTo);
	void setNewMod(CVelMod::VELMOD *velm, int ncont, double xmin, double xmax, double ztmax, double dzt, bool isTime);
	int formatVelocityArray(float dvUniform, int ncont, int nmax, double depth[], double vTop[], double vBot[]);
	
	// working in depth domain
	// must pre-suply twttWork[] big enough to hold all trace values
	// it is because this routine may be buried very deep, and we do not want to re-allocate too often
	// otherwise the Allocator class may have memory issue
	inline const double CVelMod::getTimeAtZ(CVelMod::VELMOD_X *modX, double z, double dz, double twttWork[], int nWork) {
		// OUTPUT 2-way time at one depth
		if (dz < MINAMP) dz = 1.;
		int iz = (int)(z / dz);
		if (iz < 0) return 0.f;
		// z is inside  [iz*dz, (iz+1)*dz]

		if (iz > nWork) 
			return 0.f;

		getTzFromZModGrad(modX, iz + 2, dz, twttWork);
		// note: this function will produce iz+2 values in twtt[0:iz+1]

		double t = twttWork[iz] + (twttWork[iz + 1] - twttWork[iz]) / dz * (z - iz*dz);
		return t;
	}
	inline const double CVelMod::getTimeAtZ(CVelMod::VELMOD_X *modX, double z, double dz) {
		if (z<MINAMP || dz < MINAMP) return 0.;
		int nWork = (int)(z / dz + 1.5);
		double *twttWork = alloc1double(nWork);
		double out = getTimeAtZ(modX, z, dz, twttWork, nWork);
		free1double(twttWork);
		return out;
	}


	inline const double CVelMod::getVFromVz(CVelMod::VELMOD_X *modX, double z) {
		for (int icont = 0; icont<modX->ncont - 1; icont++) {
			if (modX->depthMod[icont] <= modX->depthMod[icont + 1]) {
				if (z >= modX->depthMod[icont] && z <= modX->depthMod[icont + 1]) {
					register double dz = (modX->depthMod[icont + 1] - modX->depthMod[icont]);
					return (dz > 0.00001) ?
						modX->vTop[icont] + (modX->vBot[icont] - modX->vTop[icont]) / dz * (z - modX->depthMod[icont])
						:
						(modX->vTop[icont] + modX->vBot[icont]) * 0.5;
				}
			}
			else {
				if (z <= modX->depthMod[icont] && z >= modX->depthMod[icont + 1]) {
					return (modX->vTop[icont] + modX->vBot[icont]) * 0.5;
				}
			}
		} // for icont

		return 2.;
	}

	const double myDepth2Time2Way(CVelMod::VELMOD *velm, double x, double  z, double dz, double twttWork[], int nWork);
	BOOL myModel_Depth2TWTT(CVelMod::VELMOD *velm, double dz);

	// time domain condition
	double my2WayTime2Depth2(CVelMod::VELMOD *velm, double x, double  t2, double dz, double depthWork[], int nWork);
	BOOL myModel_TWTT2Depth(CVelMod::VELMOD *velm, double dt);

	bool getTzFromZMod(CVelMod::VELMOD_X *modX, int nz, double fz, double dz, float Tz[]);  // just a wrapper func
	bool getZtFromTMod(CVelMod::VELMOD_X *modX, int nt, double dt, float Zt[]);

	static void myModelShiftX(CVelMod::VELMOD *velm, float dx);
	static bool velNamesExParse(CString str, double xShiftArr[], double xposArr[], CStringArray *arrVelNamesEx);
	static CString velNamesExReformat(CString str); // after this reformat, the string is exactly the correct format
	// static CString velNamesExToString(double xShiftArr[], double xposArr[], CStringArray *arrVelNamesEx);
	static CString velNamesExToString(double xShiftArr[], double xposArr[], CStringArray *arrVelNamesEx, int iExclude = -1);

	static BOOL parseString2Regression2(CVelMod::VELMOD *velm, CString str = _T(""));
	static BOOL getStringRegression(CVelMod::VELMOD *velm, CString& str);
	inline const double CVelMod::velModUpdAnisotropy(double v, double anisotropy) {
		if (fabs(anisotropy) >= 1.) return v;
		return (1. - anisotropy)*v;
	}

	const double getDepthFromVel(CVelMod::VELMOD *velm, double dist, double vel, int icont, double percent, double zinc = 0.0);
	const float getAvgVelAtDistRange(CVelMod::VELMOD *velm, double x1, double x2, int icont);


	bool velModVelDelPoint(CVelMod::VELMOD *velm, int icont, int jDel, bool isTop);
	bool velModVelInsPoint(CVelMod::VELMOD *velm, int icont, int jIns, bool isTop, float x, float v);

	int isPointAbove(CVelMod::VELMOD *velm, int icont, SEI_PAIR pNew, BOOL isTimeDomain);

	int getClosestLayerNum(CVelMod::VELMOD *velm, SEI_PAIR pNew, BOOL isTimeDomain);
	int getPointIndexBeforeOnBoundary(VELMOD *velm, double xNew, int icont);
	int getPointIndexClosestOnBoundary(VELMOD *velm, SEI_PAIR pNew, int icont, BOOL isTimeDomain);
	int getPointIndexBeforeOnLayer(VELMOD *velm, double xNew, int icont, bool isTopBound, double *vInterpol);
	int getPointIndexClosestOnLayer(VELMOD *velm, double xNew, int icont, bool isTopBound, double *vInterpol);
	bool velModInsPoint(CVelMod::VELMOD *velm, int icont, SEI_PAIR pNew, BOOL isTimeDomain);
	bool velModInsPoint(CVelMod::VELMOD *velm, int icont, int jIns, SEI_PAIR pNew, BOOL isTimeDomain);
	bool velModInterceptPoint(CVelMod::VELMOD *velm, int icont, int jMove, SEI_PAIR pNew, BOOL isTimeDomain);
	bool velModInterceptPointMode9(CVelMod::VELMOD *velm, int icont, int jMove, SEI_PAIR pNew, BOOL isTimeDomain);

	int myModelBoundaryDup(CVelMod::VELMOD *velm, int icontFocus, double dz);
	bool myModelsMerge(CVelMod::VELMOD *velMod, double shiftKm, CVelMod::VELMOD *velMod2, double dz, double shiftKm2, double vTolerance);


	void ClipModelXmin(CVelMod::VELMOD *velm, float cmMin, BOOL isTime);
	void ClipModelXmax(CVelMod::VELMOD *velm, float cmMax, BOOL isTime);
	bool velModVelocityPerturb(VELMOD *velm, double deltavPercent, int layer1, int layer2, bool isMinus);

	// bool velModVelocityPerturb(VELMOD *velm, CString strLines);
	bool velModVelocityPerturb(VELMOD *velm, CString strLines, bool isMinus);

	BOOL saveModelAs(CVelMod::VELMOD *velm, CString f, double dz, BOOL isTimeDomain, int restoreLevelMax = 10);
	BOOL saveModelBinDirect(CVelMod::VELMOD *velm, CString f);
	BOOL myModelBackup2(CVelMod::VELMOD *velm, int restoreLevelMax, double dz, BOOL isTimeDomain);

	bool loadModelZelt4(VELMOD *velm, CString zeltName);
	bool loadModelZelt3(VELMOD *velm, CString zeltName);
	bool loadModelZelt2(VELMOD *velm, CString zeltName);
	static bool LoadReidsmodel(VELMOD *velm, CString reidName);

	bool myVelModInterpolDisplay(CVelMod::VELMOD *velm, double xmin, double xmax, double xShift, double dxVDesired);
	bool saveModelZelt4(VELMOD *velm, CString newName);
	bool saveModelZelt3(VELMOD *velm, CString newName);
	bool saveModelZelt2(VELMOD *velm, CString newName);

	const bool getVelocityRmsMod(CVelMod::VELMOD_X *modX, double dz, double dist, double *twttMod, float *VrmsTopMod, float *VrmsBotMod);
	const bool getVelocityRmsTrc(CVelMod::VELMOD_X *modX, double dz, int nzmaxModelAll, float *twttOut, float *vrmsOut);



	bool getVelocityIntFromRms(int ntrms, float *tau, float *vrms, int nt, float dt, float ft, float *vint);
	int getVelocityIntFromRms(int ntrms, float *tau, float *vrms, float *vint2);

	static CString getPseudoWellText(CVelMod::VELMOD *velm, double dist, double dz, bool isTime);

	// XML related
	// BOOL loadModelXML_old(VELMOD *velm, CString inName, double dz);
	// bool saveModelXML(VELMOD *velm, CString newName);
	bool saveModelXML_V2(VELMOD *velm, CString newName);
	bool saveModelXML_V3(VELMOD *velm, CString newName, double dz, BOOL isTime);

	static void appendPlacemarkOld(MSXML::IXMLDOMDocument2Ptr pXMLDoc, MSXML::IXMLDOMElementPtr pElem, 
		VELMOD *velm);
	static void appendPlacemarkLine(MSXML::IXMLDOMDocument2Ptr pXMLDoc, MSXML::IXMLDOMElementPtr pElem, 
		VELMOD *velm, CString aStyle);
	static bool saveModelsNav2Txt(CString inFileNames[], int nf, CString newName);
	static bool saveModelsNav2TxtWally(CString inFileNames[], int nf, CString newName);
	static bool saveModel2Kml(CString inFileNames[], int nf, CString newName);

	static int saveModel2Kml(CString inFileNames[], int nf, CString newName, CStringArray* arrTxt, double vmin, double vmax);
	static int saveModel2KmlMoho(CString inFileNames[], int nf, CString newName, CStringArray* arrTxt, double vmin);

	// BOOL IsValid(MSXML::IXMLDOMNodePtr pParent);
	BOOL loadModelXML(VELMOD *velm, CString inName, double dz, CString& strErr);
	BOOL loadModelXML_V2(VELMOD *velm, MSXML::IXMLDOMDocument2Ptr pXMLDoc, double dz, CString& strErr);
	BOOL loadModelXML_V3(VELMOD *velm, MSXML::IXMLDOMDocument2Ptr pXMLDoc, double dz, CString& strErr);



	static BOOL GetVelGrid(float fx, int nx, float dx, float fz, int nz, float dz, float *v, CVelMod::VELMOD *velm);
	static CString checkModelError(CVelMod::VELMOD *velm);

	// not used at all
	static int formatVelocityArray2(float distInModel, double depth[], double vTop[], double vBot[], CVelMod::VELMOD *velm);
	
	
	// static BOOL isModelVisible(CVelMod::VELMOD *velm, CCoords::SEI_COORD *pCoord, double xShift);
	static BOOL DrawVelocityColormapDepth(CDC* pDC, CVelMod::VELMOD *velm, CCoords::SEI_COORD *pCoord, CColorHelper::VELCOLOR *velColor, 
		BOOL isNoNodes, double xShift, int iLayerFocus, BOOL isNoColorMap, int nodesMode);

	static void DrawVelocityLabelDepth2(CDC* pDC, CCoords::SEI_COORD *pcd, 
		CVelMod::VELMOD *velm, double xShift, double xWell, COLORREF cr,
		float fLabelDistance3, bool isMainModelShowing, 
		int iLayerFocus, int drawnodes, double dz);
	static void DrawVelocityNodeValueTwtt(CDC* pDC, CCoords::SEI_COORD* pcd, CVelMod::VELMOD* velm, double xShift, float fLabelDistance3, int iLayerFocus, double dz);
	static void DrawVelocityLabelTwtt2(CDC* pDC, CCoords::SEI_COORD *pcd,
		CVelMod::VELMOD *velm, double xShift, double xWell, COLORREF cr,
		float fLabelDistance3, bool isMainModelShowing, BOOL isShowDetails,
		int iLayerFocus, int drawnodes, double dz);
	static void DrawSonobuoyPos(CDC* pDC, CCoords::SEI_COORD *pcd,
		CVelMod::VELMOD *velm, double xShift, int bigNodesSize, COLORREF color, BOOL isShowDetails);

	//static void DrawVelocitySWaveAnisotropy(CDC* pDC, CCoords::SEI_COORD *pcd,
	//	CVelMod::VELMOD *velm, double xShift, COLORREF cr, CString rayName,
	//	float fLabelDistance3, bool isMainModelShowing,
	//	int drawnodes, double dz);

	static void DrawModelDashCurve(CDC* pDC, CCoords::SEI_COORD *pcd, 
		CVelMod::VELMOD *velm, int numWaterLayers, double h);

	inline static double calcPoisson(double vpovs) {
		double a = 2.*(1. - vpovs*vpovs);
		if (fabs(a) < MINAMP) return 0.;
		return (2. - vpovs*vpovs) / a;
	}
	inline static double calcVpovs(double poisson) {
		double a = 1. - 2.*poisson;
		if (fabs(a) < MINAMP) return 0.;
		return sqrt((2. - 2.*poisson) / a);
	}
	inline static double getVKai(double v2ov1) {
		// double oV1 = 1. / v1;
		//double v2ov1 = v2 * ov1;
		return v2ov1==1. ? 1. : log(v2ov1) / (v2ov1 - 1.);
	}

	const bool getTzFromZModGrad(CVelMod::VELMOD_X *modX, int nz, double dz, double Tz[]); // do the real work


private:
	// only used internally, so be private
	BOOL myModel_TWTT2Depth(CVelMod::VELMOD *velm, int icontWorking, double dt);
	const bool getTmodFromZmodGrad(CVelMod::VELMOD_X *modX, double dz = 0.004);  // do the real work,

	// this one needs a better function using analytical formula!!!
	const bool getZtFromTmodGrad2(CVelMod::VELMOD_X *modX, int nt, double dt, double depth[]);  // do the real work

	const bool getZmodFromTmodGrad(CVelMod::VELMOD_X *modX, double dt = 0.004);  // do the real work,
	const bool getZmodFromTmodGrad2(CVelMod::VELMOD_X *modX, int ilayerWorking2, double dt);  // do the real work,

	const bool getZModFromZ(CVelMod::VELMOD *velm, CVelMod::VELMOD_X *modX, bool isAnisotropyApply = true); //  make it private?
	const bool getTZModFromT2(CVelMod::VELMOD *velm, CVelMod::VELMOD_X *modX, double dt, bool isAnisotropyApply);  // will not do any T-Z conversion, make it private?

	const float getVelocityMin(CVelMod::VELMOD * velm, int icont);
	const float getVelocityMax(CVelMod::VELMOD * velm, int icont);

	// private values
	double m_dz;
	double m_dt;

};
