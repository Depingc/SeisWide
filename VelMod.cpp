#include "stdafx.h"
#include "VelMod.h"
#include "ealloc_d.h"
#include "util.h"
#include "math.h"
//static CString strRegressionOverwrite = _T("1.69  5.33  0.42745");
static int ILAYER_WATER = 1;
static _variant_t strBlank = _T("");

CVelMod::CVelMod(void)
{
	this->m_dz = this->m_dt = 0.004;
}

CVelMod::~CVelMod(void)
{
}
//
//void CVelMod::velModSetArrays(CVelMod::VELMOD *velMod2) {
//	// sometimes we need an extra VELMOD struct to store temporary values
//	// NOTE: if you have   VELMOD modNew;  make sure you add  modNew.nx = 0;   before calling this to avoid an exception!
//	if (velMod2 == NULL) return;
//	if (velMod2->ncont >= 0 && velMod2->xm != NULL) {
//		// we already have arrays properly allocated, so simple
//		velMod2->ncont = 0;
//		return;
//	}
//
//
//	velMod2->grdNx = 500;
//	velMod2->nzed = ealloc1int(MOD_LAYER_MAX);
//	velMod2->nvel = ealloc2int(2, MOD_POINTS_MAX);
//	velMod2->xm = ealloc2float(MOD_LAYER_MAX, MOD_POINTS_MAX);
//	velMod2->zm = ealloc2float(MOD_LAYER_MAX, MOD_POINTS_MAX);
//	velMod2->xvel = ealloc3float(2, MOD_LAYER_MAX, MOD_POINTS_MAX);
//	velMod2->vf = ealloc3float(2, MOD_LAYER_MAX, MOD_POINTS_MAX);
//	velMod2->grdDepth = ealloc2double(velMod2->grdNx, MOD_LAYER_MAX); //for vel interpolation.
//	velMod2->vTop = ealloc2double(velMod2->grdNx, MOD_LAYER_MAX); //for vel interpolation.
//	velMod2->vBot = ealloc2double(velMod2->grdNx, MOD_LAYER_MAX); //for vel interpolation.
//	velMod2->grdTwtt = ealloc2double(velMod2->grdNx, MOD_LAYER_MAX);// added for storing vel interpolation results in TWTT domain, calculated field, not stored in file 
//	velMod2->tm = ealloc2float(MOD_LAYER_MAX, MOD_POINTS_MAX);  // model control points in TWTT domain, calculated field, not stored in file
//	velMod2->anisotropy = ealloc1float(MOD_LAYER_MAX);  // model control points in TWTT domain, calculated field, not stored in file
//	velMod2->poisson = ealloc1double(MOD_LAYER_MAX);
//	
//	// meta arrays
//	velMod2->latitude = ealloc2double(MOD_LAYER_MAX, MOD_POINTS_MAX);
//	velMod2->longitude = ealloc2double(MOD_LAYER_MAX, MOD_POINTS_MAX);
//
//	// some numbers need to reset to zero
//	for (int icont = 0; icont < MOD_LAYER_MAX; icont++) {
//		velMod2->nzed[icont] = 0;
//		velMod2->nvel[icont][0] = 0;
//		velMod2->nvel[icont][1] = 0;
//		velMod2->anisotropy[icont] = 0.f;
//		velMod2->poisson[icont] = 0.f;
//		for (int j = 0; j < MOD_POINTS_MAX; j++) {
//			velMod2->latitude[j][icont] = ERRORVAL;
//			velMod2->longitude[j][icont] = ERRORVAL;
//		}
//	}
//	velMod2->ncont = 0; // indicating the arrays already initialized
//
//}
//
//void CVelMod::velModFreeArrays(CVelMod::VELMOD *velm)
//{
//	// sometimes we need an extra VELMOD struct to store temporary values
//	if (velm == NULL || velm->xm == 0) return;
//
//	velm->ncont = -1;
//	velm->grdNx = 0;
//	free1int(velm->nzed);
//	free2int(velm->nvel);
//	free2float(velm->xm);
//	free2float(velm->zm);
//	free3float(velm->xvel);
//	free3float(velm->vf);
//	free2double(velm->grdDepth);
//	free2double(velm->vTop);
//	free2double(velm->vBot);
//	free2double(velm->grdTwtt);
//	free2float(velm->tm);
//	free1float(velm->anisotropy);
//	free1double(velm->poisson);
//	free2double(velm->latitude);
//	free2double(velm->longitude);
//
//	velm->nzed = NULL;
//	velm->nvel = NULL;
//	velm->xm = NULL;
//	velm->zm = NULL;
//	velm->xvel = NULL;
//	velm->vf = NULL;
//	velm->grdDepth = NULL;
//	velm->vTop = NULL;
//	velm->grdTwtt = NULL;
//	velm->tm = NULL;
//	velm->anisotropy = NULL;
//	velm->poisson = NULL;
//
//	velm->resetNonflattenValues();
//}


// this function should be removed!!!
// not used at all
int CVelMod::formatVelocityArray2(float distInModel, double depth[], double vTop[], double vBot[], CVelMod::VELMOD *velm)
{
	float dvUniform = 0.05f;
	double x1, x2, x3, x4, v1, v2, v3, v4;
	int j, icont, ncont;

	for (icont = 0; icont < velm->ncont; icont++)
	{
		for (j = 1; j < velm->nzed[icont]; j++)
		{
			if (distInModel >= velm->xm[j - 1][icont] && distInModel <= velm->xm[j][icont])
			{
				if (fabs(velm->xm[j][icont] - velm->xm[j - 1][icont]) < 0.00001f)
					depth[icont] = velm->zm[j][icont];
				else
					depth[icont] = velm->zm[j - 1][icont] + (distInModel - velm->xm[j - 1][icont])*
					(velm->zm[j][icont] - velm->zm[j - 1][icont]) / (velm->xm[j][icont] - velm->xm[j - 1][icont]);
				break;
			}
		}
	}

	//Now locate the four corners that define the velocity of for all points at distInModel.
	//*****x1*****************x2******* with distInModel at depth[icont]
	//**********distInModel************
	//***************************
	//**x3*********************x4 with distInModel at depth[icont+1]
	for (icont = 0; icont < velm->ncont - 1; icont++)
	{
		for (j = 1; j < velm->nvel[icont][0]; j++)
		{
			if (distInModel >= velm->xvel[j - 1][icont][0] && distInModel <= velm->xvel[j][icont][0])
			{
				x1 = velm->xvel[j - 1][icont][0];
				x2 = velm->xvel[j][icont][0];
				v1 = velm->vf[j - 1][icont][0];
				v2 = velm->vf[j][icont][0];
				break;
			}
		}
		for (j = 1; j < velm->nvel[icont][1]; j++)
		{
			if (distInModel >= velm->xvel[j - 1][icont][1] && distInModel <= velm->xvel[j][icont][1])
			{
				x3 = velm->xvel[j - 1][icont][1];
				x4 = velm->xvel[j][icont][1];
				v3 = velm->vf[j - 1][icont][1];
				v4 = velm->vf[j][icont][1];
				break;
			}
		}
		vTop[icont] = v1 + (v2 - v1)*(distInModel - x1) / (x2 - x1);
		vBot[icont] = v3 + (v4 - v3)*(distInModel - x3) / (x4 - x3);
	}

	CVelMod vm;
	ncont = vm.formatVelocityArray(dvUniform, velm->ncont, 4000, (double *)&depth[0], (double *)&vTop[0], (double *)&vBot[0]);

	return ncont;
}


bool CVelMod::velModCloneArrays(CVelMod::VELMOD *velMod, CVelMod::VELMOD *velModTo) {
	// sometimes we need an extra VELMOD struct to store temporary values
	// 	VELMOD velModTo;
	//  setVelModArrays(&velModTo);
	//  cloneVelModArrays(&velMod, &velModTo);
	//  freeVelModArrays(&velModTo);

	if (velMod == NULL || velModTo == NULL) return false;

	velModTo->ncont = velMod->ncont;
	for (int icont = 0; icont < velMod->ncont; icont++) {
		velModTo->nzed[icont] = velMod->nzed[icont];
		for (int i = 0; i < velMod->nzed[icont]; i++) {
			velModTo->xm[i][icont] = velMod->xm[i][icont];
			velModTo->zm[i][icont] = velMod->zm[i][icont];
			velModTo->tm[i][icont] = velMod->tm[i][icont];
		}
	} // for icont

	velModTo->ncont = velMod->ncont;
	for (int icont = 0; icont < velMod->ncont - 1; icont++) {
		velModTo->anisotropy[icont] = velMod->anisotropy[icont];
		for (int k = 0; k < 2; k++) {
			velModTo->nvel[icont][k] = velMod->nvel[icont][k];
			for (int i = 0; i < velMod->nvel[icont][k]; i++) {
				velModTo->xvel[i][icont][k] = velMod->xvel[i][icont][k];
				velModTo->vf[i][icont][k] = velMod->vf[i][icont][k];
			}
		}
	}

	velModTo->deploys[0].deployDist = velMod->deploys[0].deployDist;
	velModTo->deploys[0].startDist = velMod->deploys[0].startDist;
	velModTo->deploys[0].endDist = velMod->deploys[0].endDist;

	velModTo->deploys[0].startOffset = velMod->deploys[0].startOffset;
	velModTo->deploys[0].endOffset = velMod->deploys[0].endOffset;
	//velModTo->xmin = velMod->xmin;
	//velModTo->xmax = velMod->xmax;

	return true;
}

void CVelMod::setNewMod(CVelMod::VELMOD *velm, int ncont, double xmin, double xmax, double ztmax, double dzt, bool isTime) {
	//velModSetArrays(velm);
	velm->ncont = ncont;
	for (int icont=0; icont<velm->ncont; icont++) {
			velm->xm[0][icont] = (float)xmin;
			velm->xm[1][icont] = (float)xmax;
			if (isTime)
				velm->tm[1][icont] = velm->tm[0][icont] = (float)(ztmax/(double)(velm->ncont-1)*icont);
			else
				velm->zm[1][icont] = velm->zm[0][icont] = (float)(ztmax / (double)(velm->ncont - 1)*icont);
			velm->nzed[icont] = 2;
	}

	for (int icont=0; icont<velm->ncont-1; icont++) {
			velm->nvel[icont][0] = 2;
			velm->nvel[icont][1] = 2;
			velm->xvel[0][icont][0] = (float)xmin;
			velm->xvel[1][icont][0] = (float)xmax;
			velm->xvel[0][icont][1] = (float)xmin;
			velm->xvel[1][icont][1] = (float)xmax;

			velm->vf[0][icont][0] = 1.5f + icont*1.3f;
			velm->vf[1][icont][0] = 1.5f + icont*1.3f;
			velm->vf[0][icont][1] = 1.5f + icont*1.3f+0.1f;
			velm->vf[1][icont][1] = 1.5f + icont*1.3f+0.1f;
			velm->anisotropy[icont] = 0.f;
	}

	// set a default deployment
	velm->deploys[0].deployDist = xmin;
	velm->deploys[0].startDist = xmin;
	velm->deploys[0].endDist = xmax;

	velm->deploys[0].startOffset = ERRORVAL;
	velm->deploys[0].endOffset = ERRORVAL;

	if (isTime) {
		// need to convert to depth domain
		myModel_TWTT2Depth(velm, dzt);
	}
	else
		myModel_Depth2TWTT(velm, dzt);

	return;
}


const float CVelMod::getVelocityMin(CVelMod::VELMOD * velm, int icont)
{
	int j;
	float vmin = MIL;
	float v;
	for (j = 0; j < velm->nvel[icont][0]; j++)
	{
		v = velm->vf[j][icont][0];
		if (v < vmin) vmin = v;
	}

	return vmin;
}

const float CVelMod::getVelocityMax(CVelMod::VELMOD * velm, int icont)
{
	int j;
	float vmax = -MIL;
	float v;
	for (j = 0; j<velm->nvel[icont][1]; j++)
	{
		v = velm->vf[j][icont][1];
		if (v > vmax) vmax = v;
	}

	return vmax;
}

BOOL CVelMod::formatCalcVelLimits(CVelMod::VELMOD * velm)
{
	int j, icont;

	if (velm->ncont > MOD_LAYER_MAX)
		velm->ncont = MOD_LAYER_MAX;

	// sometimes we have 0 points for a boundary due to unreasonable read-in
	// it happens a lot!  Dec 2006
	for (icont = 0; icont < velm->ncont; icont++) {
		if (velm->nzed[icont] < 1) {
			velm->ncont = icont - 1;
			break;
		}

		// occasionally the num of points are too much
		if (velm->nzed[icont] > MOD_POINTS_MAX) {
			velm->nzed[icont] = MOD_POINTS_MAX; // prevents overflow???
		}
	}

	for (j = 0; j < velm->nzed[0]; j++) {
		velm->zm[j][0] = 0.f;
	}


	//Search for minimum and max model distance.
	// final fix up for some
	//velm->xmax = -MIL;
	//velm->xmin = MIL;

	//find the max distance.
	//for (j = 0; j < velm->nzed[0]; j++) {
	//	if (velm->xmax < velm->xm[j][0]) velm->xmax = (float)velm->xm[j][0];
	//}

	////find the min distance.
	//for (j = 0; j<velm->nzed[0]; j++) {
	//	if (velm->xmin > velm->xm[j][0]) velm->xmin = (float)velm->xm[j][0];
	//}

	//Search for depth limits.
	velm->zmin = MIL;
	for (j = 0; j<velm->nzed[0]; j++) {
		if (velm->zmin > velm->zm[j][0])
			velm->zmin = velm->zm[j][0];
	}

	velm->zmax = -1.;
	for (icont = 0; icont < velm->ncont; icont++) {
		for (j = 0; j < velm->nzed[icont]; j++) {
			if (velm->zmax < velm->zm[j][icont])
				velm->zmax = velm->zm[j][icont];
		}
	}
	//for (j = 0; j < velm->nzed[velm->ncont - 1]; j++) {
	//	// a fix up: the last boundary must be at the maximum depth; there can be odds here!
	//	velm->zm[j][velm->ncont - 1] = (float)velm->zmax;
	//}



	for (icont = 0; icont < velm->ncont; icont++) {
		if (velm->nzed[icont] == 1) {
			velm->nzed[icont] = 2;
			velm->xm[1][icont] = (float)velm->getXmax();
			velm->zm[1][icont] = (float)velm->zm[0][icont];
			velm->xm[0][icont] = (float)velm->getXmin();
		}
		else if (velm->nzed[icont] == 0) {
			// it has happens sometimes: no points for a boundary. So we just stop here for loading the model
			velm->ncont = icont + 1;
			break;
		}
	}


	float x;
	float x2;

	for (icont = 0; icont < velm->ncont; icont++) {
		for (j=0; j<velm->nzed[icont]-1; j++) {
			x = velm->xm[j][icont];
			x2 = velm->xm[j+1][icont];
			if (x<velm->getXmin() && x2>velm->getXmin()) {
				velm->zm[j][icont] = (float)(velm->zm[j][icont] + (velm->zm[j+1][icont] - velm->zm[j][icont]) * (velm->getXmin() - x) / (x2 - x));
				x = velm->xm[j][icont] = (float)velm->getXmin();
			}
			if (x<velm->getXmax() && x2>velm->getXmax()) {
				register SEI_PAIR pNew;
				pNew.x = velm->getXmax();
				pNew.y = velm->zm[j][icont] + (velm->zm[j+1][icont] - velm->zm[j][icont]) * (velm->getXmax() - x) / (x2 - x);
				velModInsPoint(velm, icont, j, pNew, false);
				//velm->zm[j][icont] = (float)(velm->zm[j][icont] + (velm->zm[j+1][icont] - velm->zm[j][icont]) * (velm->xmax - x) / (x2 - x));
				//velm->xm[j][icont] = (float)velm->xmax;
			}
		} // for j

		for (j=0; j<velm->nzed[icont]-1; j++) {
			x = velm->xm[j][icont];
			x2 = velm->xm[j+1][icont];
			if (x<velm->getXmin() && x2<=velm->getXmin()) {
				// the deletion of outside points should be done the last step.
				// otherwise if there is only 2 points and it can mistakenly delete the 2nd point 
				if (velm->delPoint(icont, j))
					j--;  // the next loop will have 1 added, so we use -1 here
			}
		}

		for (j=0; j<velm->nzed[icont]; j++) {
			// Note: this cannot be merged into the above loop!
			// handle if a point is to the right of the model limit
			if (velm->xm[j][icont] > velm->getXmax()) {
				velm->delPoint(icont, j);
				j--;
			}
		} // for j

		// finally, handle if a boundary is narrower than model limits
		if (velm->xm[0][icont] > velm->getXmin())
			velm->xm[0][icont] = (float)velm->getXmin();
		j = velm->nzed[icont];
		if (velm->xm[j-1][icont] < velm->getXmax())
			velm->xm[j-1][icont] = (float)velm->getXmax();

		// fix up invalid values 
		if (velm->xmMin[icont] >= velm->getXmax()) velm->xmMin[icont] = ERRORVAL;
		if (velm->xmMax[icont] <= velm->getXmin()) velm->xmMax[icont] = ERRORVAL;

	} // for icont

	for (int k=0; k<2; k++)
		for (icont = 0; icont < velm->ncont - 1; icont++) {
			// fix up for some models that only have 1 point per layer
			if (velm->nvel[icont][k] == 1) {
				velm->nvel[icont][k] = 2;
				velm->xvel[1][icont][k] = (float)velm->getXmax();
				velm->vf[1][icont][k] = velm->vf[0][icont][k];
				velm->xvel[0][icont][k] = (float)velm->getXmin();
			}

			// finally, handle if a layer is narrower than model limits
			//if (velm->xvel[0][icont][k] > velm->xmin)
			//	velm->xvel[0][icont][k] = velm->xmin;
			//j = velm->nvel[icont][k];
			//if (velm->xvel[j-1][icont][k] < velm->xmax)
			//	velm->xvel[j-1][icont][k] = velm->xmax;


			// fix up for some models that have invalid xmin/xmax
			velm->xvel[0][icont][k] = (float)velm->getXmin();
			register int count = velm->nvel[icont][k] - 1;
			if (count<1 || count>MOD_POINTS_MAX - 1) {
				// this has happened: this layer is invalid. So...
				velm->ncont = icont + 1;
				break;
			}
			velm->xvel[count][icont][k] = (float)velm->getXmax();
		}

		return TRUE;
}


BOOL CVelMod::MyCalcVelLimitsOld(CVelMod::VELMOD * velm)
{
	int j, icont;
	for (j = 0; j < velm->nzed[0]; j++) {
		velm->zm[j][0] = 0.f;
	}

	// sometimes we have 0 points for a boundary due to unreasonable read-in
	// it happens a lot!  Dec 2006
	for (icont = 0; icont < velm->ncont; icont++) {
		if (velm->nzed[icont] < 1) {
			velm->ncont = icont - 1;
			break;
		}
	}

	//Search for minimum and max model distance.
	// final fix up for some
	//velm->xmax = -999999.;
	//velm->xmin = 9999999.;

	//find the max distance.
	//for (j = 0; j < velm->nzed[0]; j++) {
	//	if (velm->xmax() < velm->xm[j][0]) velm->xmax = (float)velm->xm[j][0];
	//}

	//find the min distance.
	//for (j = 0; j<velm->nzed[0]; j++) {
	//	if (velm->xmin() > velm->xm[j][0]) velm->xmin = (float)velm->xm[j][0];
	//}


	//for (icont=0; icont<velm->ncont; icont++) {
	//	velm->xmin=velm->xm[0][icont];
	//	velm->xmax=velm->xm[velm->nzed[icont]-1][icont];
	//	break;
	//}


	//Search for depth limits.
	velm->zmin = 99999.;
	for (j = 0; j<velm->nzed[0]; j++) {
		if (velm->zmin > velm->zm[j][0])
			velm->zmin = velm->zm[j][0];
	}

	velm->zmax = -1.;
	for (icont = 0; icont < velm->ncont; icont++) {
		for (j = 0; j < velm->nzed[icont]; j++) {
			if (velm->zmax < velm->zm[j][icont])
				velm->zmax = velm->zm[j][icont];
		}
	}
	for (j = 0; j < velm->nzed[velm->ncont - 1]; j++) {
		// a fix up: the last boundary must be at the maximum depth; there can be odds here!
		velm->zm[j][velm->ncont - 1] = (float)velm->zmax;
	}



	for (icont = 0; icont < velm->ncont; icont++) {
		if (velm->nzed[icont] == 1) {
			velm->nzed[icont] = 2;
			velm->xm[1][icont] = (float)velm->getXmax();
			velm->zm[1][icont] = (float)velm->zm[0][icont];
			velm->xm[0][icont] = (float)velm->getXmin();
		}
		else if (velm->nzed[icont] == 0) {
			// it has happens sometimes: no points for a boundary. So we just stop here for loading the model
			velm->ncont = icont + 1;
			break;

		}
		else {
			int jj = velm->nzed[icont];
			velm->xm[0][icont] = (float)velm->getXmin();
			velm->xm[jj - 1][icont] = (float)velm->getXmax();
		}

	} // for icont

	for (icont = 0; icont < velm->ncont - 1; icont++) {
		// fix up for some models that only have 1 point per layer
		if (velm->nvel[icont][0] == 1) {
			velm->nvel[icont][0] = 2;
			velm->xvel[1][icont][0] = (float)velm->getXmax();
			velm->vf[1][icont][0] = velm->vf[0][icont][0];
			velm->xvel[0][icont][0] = (float)velm->getXmin();
		}
		if (velm->nvel[icont][1] == 1) {
			velm->nvel[icont][1] = 2;
			velm->xvel[1][icont][1] = (float)velm->getXmax();
			velm->vf[1][icont][1] = velm->vf[0][icont][1];
			velm->xvel[0][icont][1] = (float)velm->getXmin();
		}

		// fix up for some models that have invalid xmin/xmax
		velm->xvel[0][icont][0] = (float)velm->getXmin();
		velm->xvel[0][icont][1] = (float)velm->getXmin();
		int k = velm->nvel[icont][0] - 1;
		if (k<1 || k>MOD_LAYER_MAX - 1) {
			// this has happened: this layer is invalid. So...
			velm->ncont = icont + 1;
			break;
		}
		velm->xvel[k][icont][0] = (float)velm->getXmax();
		k = velm->nvel[icont][1] - 1;
		velm->xvel[k][icont][1] = (float)velm->getXmax();
	}


	return TRUE;
}



// this also load model as well
BOOL CVelMod::loadInitVel(CVelMod::VELMOD* velm, CString f, double dz, CString& strErr)
{
	//initialize vel file.
	strErr.Format(BLANK);
	if (velm->modifiedLoaded == 0 && velm->ncont > 0)
		return FALSE; // this case has a valid model to be saved. So we cannot load the model from file; otherwise the un-saved info will be lost.

	int i, j, icont;
	int nlayer; // this was previously inside the struct VELMOD, but I disabled it.
	// CString aVelName = velm->velSecName;
	if (!isFileExistNotDirectory(f)) {
		// since the file does not exist, let's try again
		f = SetFileExtension(f, _T("vxml"));
		if (!isFileExistNotDirectory(f)) {
			velm->ncont = 0;
			return FALSE;
		}
	}

	// when the model is flattened at a certain horizon, we cannot cache it since it
	// is not a real model!!!
	CTime aTime = getFileModifiedTime(f);
	// note: it can occasionally happen that 2 different models have the same time 
	if (velm->ncont > 0 && velm->modifiedLoaded == aTime && velm->velSecName==f)
		return TRUE;  // if no change, we do not want to reload since the current model may be newer but just not saved yet!
	else {
		velm->modifiedLoaded = aTime;
	}
	velm->velSecName = f;
	velm->ncontMaxConstrained = 0;
	velm->nFlatten = 0;
	velm->isVelBelowValid = 0;

	if (GetFileExtension(f) == _T("vxml"))
		return loadModelXML(velm, f, dz, strErr);
	else if (GetFileExtension(f) == _T("in")) {
		// determine if this is really a Zelt's v.in format
		// if not, it can cause freezing
		bool isFormatCorrect = true;
		CString tStr = getLineFirst(f);
		CStringArray *arr2 = new CStringArray();
		SplitStringArr(tStr, SPACE, arr2, false);
		size_t count2 = arr2->GetCount();
		if (count2 > 0) {
			CString cstr2 = arr2->ElementAt(0);
			if (StringToInt(cstr2) != 1) {
				isFormatCorrect = false;
			}
		}
		arr2->RemoveAll();
		delete arr2;
		if (!isFormatCorrect) {
			return FALSE;
		}




		// determine how many decimals
		i = tStr.Find('.', 0);
		if (i < 0) return FALSE;
		j = tStr.Find(' ', i);
		if (j < 0) {
			j = tStr.Find('\r', i);
		}

		int numDecimal = 0;
		if (j > i) {
			// found a decimal.
			numDecimal = j - i - 1; // numDecimal is the number of decimals as calculated from file

			//if (delta == 4 && is4Decimal != 0) 
			//	is4Decimal = 1;
			//else if (delta == 3 && is4Decimal != 1) 
			//	is4Decimal = 0;
			//else if (delta == 2 && is4Decimal != 2)
			//	is4Decimal = -1;
		}

		BOOL isSuc = FALSE;
		if (numDecimal == 2)
			isSuc = this->loadModelZelt2(velm, f);
		else if (numDecimal == 3)
			isSuc = this->loadModelZelt3(velm, f);
		else if (numDecimal == 4)
			isSuc = this->loadModelZelt4(velm, f);

		if (isSuc) {
			myModel_Depth2TWTT(velm, dz);
			this->formatCalcVelLimits(velm); // should be after depth2TWTT ?
		}

		velm->ncontMaxConstrained = velm->ncont;
		return isSuc;
	}



	CFile filev(f, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone);
	if (filev.Read(&velm->ncont, 4) != 4 || velm->ncont <= 0 || velm->ncont > MOD_LAYER_MAX) {
		filev.Close();
		velm->ncont = 0;  // this indicates invalid model
		return FALSE;
	}

	filev.Read(&velm->nzed[0], (long)(velm->ncont * 4));
	for (icont = 0; icont < velm->ncont; icont++) {
		if (velm->nzed[icont] < 0 || velm->nzed[icont] >= MOD_POINTS_MAX) {
			filev.Close();
			velm->ncont = 0;  // this indicates invalid model
			return FALSE;
		}
	}
	for (icont = 0; icont < velm->ncont; icont++)
		for (j = 0; j < velm->nzed[icont]; j++) filev.Read(&velm->xm[j][icont], 4);
	for (icont = 0; icont < velm->ncont; icont++)
		for (j = 0; j < velm->nzed[icont]; j++) filev.Read(&velm->zm[j][icont], 4);

	filev.Read(&nlayer, 4);  // nlayer shall be discarded
	for (i = 0; i < 2; i++)
		for (icont = 0; icont < velm->ncont - 1; icont++)
			filev.Read(&velm->nvel[icont][i], 4);


	for (int k=0; k<2; k++) {
		for (icont = 0; icont < velm->ncont - 1; icont++)
			for (j = 0; j < velm->nvel[icont][k]; j++) filev.Read(&velm->xvel[j][icont][k], 4);
		for (icont = 0; icont < velm->ncont - 1; icont++)
			for (j = 0; j < velm->nvel[icont][k]; j++) filev.Read(&velm->vf[j][icont][k], 4);
		//for (icont = 0; icont < velm->ncont - 1; icont++)
		//	for (j = 0; j < velm->nvel[icont][1]; j++) filev.Read(&velm->xvel[j][icont][1], 4);
		//for (icont = 0; icont < velm->ncont - 1; icont++)
		//	for (j = 0; j < velm->nvel[icont][1]; j++) filev.Read(&velm->vf[j][icont][1], 4);
	}

	// added Nov.1, 2006 since I found a model that has incorrect nlayer.
	// note that this correction has to be done after the whole model has been read in
	// otherwise the model is incorrect

	// additions: anisotropy added Oct., 2009 for model
	for (icont = 0; icont < velm->ncont - 1; icont++) {
		if (filev.Read(&velm->anisotropy[icont], 4) != 4) {
			// if here, no anisotropy data exist in the file, we just set them to zero and get out of the loop
			for (icont = 0; icont < velm->ncont - 1; icont++)
				velm->anisotropy[icont] = 0.f;

			break;
		}
	}

	// additions: sedimentary regression results, added Apr 23., 2014
	if (filev.Read(&velm->nRegressionLayer, 4) == 4 && velm->nRegressionLayer >= 0) {
		filev.Read(&velm->regVZero, 8);
		filev.Read(&velm->regVInf, 8);
		filev.Read(&velm->regAlpha, 8);
	}
	else
		velm->nRegressionLayer = 0;



	filev.Close();
	//Finish reading velocity data

	myModel_Depth2TWTT(velm, dz);
	this->formatCalcVelLimits(velm); // should be after depth2TWTT ?
	velm->ncontMaxConstrained = velm->ncont;

	return(TRUE);
}

BOOL CVelMod::mySaveFloatReflectorsIn(CVelMod::VELMOD * velm, CString f)
{
	// NOTE: f == _T("f.in") shall be in the current directory which is writable, required for Fortran program to write to.
	if (!isFileExistNotDirectory(velm->flFloatName)) return FALSE;
	int j, icont;
	FILE *stream;
	if (_tfopen_s(&stream, f, _T("w")) != 0) {
		AfxMessageBox(_T("New files could not open in the working directory."));
		return FALSE;
	}
	// fseek( stream, 0L, SEEK_SET );
	for (icont = 0; icont < velm->flTotCurve; icont++) {
		_ftprintf_s(stream, _T("%2d\n"), velm->flNpArray[icont]);
		_ftprintf_s(stream, _T("%2d "), icont + 1);
		for (j = 0; j < velm->flNpArray[icont]; j++)
			_ftprintf_s(stream, _T("%10.4f"), velm->flXArray[j][icont]);
		_ftprintf_s(stream, _T("\n   "));
		for (j = 0; j < velm->flNpArray[icont]; j++)
			_ftprintf_s(stream, _T("%10.4f"), velm->flTArray[j][icont]);
		_ftprintf_s(stream, _T("\n   "));
		for (j = 0; j < velm->flNpArray[icont]; j++)
			_ftprintf_s(stream, _T("%10d"), 0);
		_ftprintf_s(stream, _T("\n"));
	}
	fclose(stream);
	// showFileInNotePad(tempStr);
	return TRUE;

}


// Save r.in to file, from fIn to rin. Full path expected, 
// otherwise, the file will be written as is, which is ok if the current dir is already proper
// will need to revise this function for a better tune up later
BOOL CVelMod::mySaveRInFile(CString fIn, CString rin, BOOL isSimplified)
{
	fIn.MakeLower();
	rin.MakeLower();
	if (fIn != rin && 
		!CopyFile(fIn, rin, FALSE))  {
		if (!isFileExistNotDirectory(rin))
			return FALSE;
	}
	return TRUE;
}


void CVelMod::mySaveTXInFile(CString fIn, CString txin)
{
	CString tStr = GetFileExtension(fIn);
	if (!fIn.IsEmpty() && tStr.MakeLower() == _T("in")) {
		CStringArray arr;
		CStringArray arr3;
		float t1, t2, t3;
		int k;
		if (fIn.Find(_T("*"), 0) >= 0) {
			// with wildcards, we need to concatenate into a single tx.in file
			TCHAR line[MAXLINE_ZELT];
			CStringArray *arr5 = getAllPathArray(fIn);	// CString Array to hold Directory Structures
			for (int io = 0; io < arr5->GetSize(); io++) {
				register FILE *ifh1;  /* Input file handle */
				if (_tfopen_s(&ifh1, arr5->ElementAt(io), _T("r")) != 0) continue;
				while (_fgetts(line, MAXLINE_ZELT, ifh1) != NULL) {
					if (_tcslen(line) < 2) continue;
					if (_stscanf_s(line, _T("%f%f%f%d"), &t1, &t2, &t3, &k) == 0) {
						continue;
					}
					if (t1 != 0.f && t2 != 0.f && t3 != 0.f && k != -1)
						arr.Add(line);
				} // while fgets
				fclose(ifh1);
			} // for io=0
			arr5->RemoveAll();  arr5 = NULL;
			delete arr5;
			arr.Add(_T("     0.000     0.000     0.000        -1\n"));
		}
		else if (isFileExistNotDirectory(fIn)) {
			getLinesFromFile(&arr, fIn, false, 0);
		}

		if (arr.GetCount() > 0) {
			for (int i = 0; i < arr.GetCount(); i++) {
				tStr = arr.ElementAt(i);
				if (tStr[0] == '#') continue; // we do not need comment lines
				if (_stscanf_s(tStr, _T("%f%f%f%d"), &t1, &t2, &t3, &k) == 0) {
					continue;
				}

				// the fortran program only reads in %10.3f  format, so we have to reformat all lines
				tStr.Format(_T("%10.3f%10.3f%10.3f%10d"), t1, t2, t3, k);
				arr3.Add(tStr);
			} // for i
			saveStringArrayToFile(&arr3, txin);
		}
		arr3.RemoveAll();
		arr.RemoveAll();
	} // if draw.tx
	return;
}



BOOL CVelMod::myLoadFloatReflectors(CVelMod::VELMOD * velm)
{
	//	if (velMod.floatName.IsEmpty()) return FALSE;
	if (!isFileExistNotDirectory(velm->flFloatName)) return FALSE;

	// if here, the float file must be loaded.

	if (velm->flFloatName.Compare(velm->flFloatNameOld)==0) {
		//check to see if some other processes have changed the file since this document last initilized it.
		CFileStatus status;
		if(!CFile::GetStatus( velm->flFloatName, status )) return FALSE;
		CTimeSpan timeSpan1=status.m_mtime-velm->flFloatReflStatus.m_mtime;
		if(timeSpan1.GetTotalSeconds() == 0) return TRUE;
	}


	BOOL firstTim=TRUE, textErr=FALSE ;
	TCHAR line[MAXLINE_ZELT];
	FILE *ifh1;  /* Input file handle */
	int i,j,k,k0, ncurve,npoint;
	float x1,t1;


	if(_tfopen_s(&ifh1, velm->flFloatName, _T("r") ) != 0)  {
		fclose(ifh1);
		AfxMessageBox(_T("File ")+velm->flFloatName+_T(" cannot be opened"));
		velm->flFloatName = _T("");
		return FALSE;
	}
	i = ncurve = npoint = 0;
	k0=-9999;

	//first decide how big the arrays should be.
	while (_fgetts(line, MAXLINE_ZELT,ifh1)!=NULL) {
		if (_tcslen(line) < 2) continue;
		if (line[0] == _T('#')) continue; // we do not need comment lines
		if (_stscanf_s(line,_T("%f%f%d"),&x1, &t1, &k)==0) {
			textErr=TRUE; 
			continue;
		}
		if(firstTim) {
			firstTim = FALSE;
			i ++;
			k0 = k;
		}
		else if(k!=k0 && k0!=-99999) { //now a new curve is encountered and has already been read in.
			if(npoint<i) npoint = i;
			ncurve++;
			i = 1; //must be 1 instead of 0 since already read in.
			k0 = k;
		}
		else {
			i ++;
			k0 = k;
		}
	}
	if(npoint<i) npoint = i;
	ncurve++;

	rewind(ifh1);
	CString tStr;
	if(textErr) tStr  = _T("Input file read with error.");
	//if(ncurve>100 || npoint>100) {
	//	tStr.Format(_T("%s %5d curves & each curve has a max of %5d points. You may use the Select, Move, Delete, Add, and Zoom buttons for digitizing."), tStr, ncurve, npoint);
	//	myCaption(tStr);
	//}

	velm->freeFlArrays();  // before allocating arrays for digiting, we need to free them anyway
	velm->flNpArray = (int *)ealloc1int(ncurve+1);
	velm->flXArray = (float **)ealloc2float(ncurve,npoint+1);
	velm->flTArray = (float **)ealloc2float(ncurve,npoint+1);

	firstTim = TRUE;
	i=0; j=0; k0=-99999;
	while (_fgetts(line, MAXLINE_ZELT,ifh1)!=NULL) {
		if (_tcslen(line) < 2) continue;
		if (line[0] == _T('#')) continue;
		if( _stscanf_s(line, _T("%f%f%d"),&velm->flXArray[j][i], &velm->flTArray[j][i], &k)==0 ) continue;
//		if (k == 0) continue; // it happens sometimes the first curve has k==0
		if(firstTim) {
			firstTim = FALSE;
			j ++;
			k0 = k;
		}
		else if(k!=k0) { //now a new curve is encountered and has already been read in.
			velm->flNpArray[i] = j;
			i ++;
			velm->flXArray[0][i] = velm->flXArray[j][i-1];
			velm->flTArray[0][i] = velm->flTArray[j][i-1];

			k0 = k;
			j = 1;
		}
		else j ++;
	}
	//now write the last set.
	velm->flNpArray[i] = j;
	velm->flTotCurve = i+1;
	fclose(ifh1);
	velm->flFloatNameOld = velm->flFloatName; // indicates the data file is loaded already.

	//	extern CFile cfile;
	BOOL initFileStatus = CFile::GetStatus( velm->flFloatName, velm->flFloatReflStatus ); // static function
	if(!initFileStatus)	return FALSE;

	return TRUE; // succesful.
}


BOOL CVelMod::parseString2Regression2(CVelMod::VELMOD *velm, CString str)
{
	if (str.Trim().IsEmpty())
		//str = strRegressionOverwrite;
		return FALSE;

	float t1, t2, t4;

	int num = _stscanf_s(str, _T("%f %f %f"), &t4, &t1, &t2);
	if (num == 3) {
		velm->regVZero = t4;
		velm->regVInf = t1;
		velm->regAlpha = t2;
		return TRUE;
	}

	return FALSE;
}


BOOL CVelMod::getStringRegression(CVelMod::VELMOD *velm, CString& str)
{
	if (velm->nRegressionLayer > 0) {
		str.Format(_T("%g    %g    %g"), velm->regVZero, velm->regVInf, velm->regAlpha);
		return TRUE;
	}

	return FALSE;  // str not changed, and therefore return FALSE  /*strRegressionOverwrite*/;
}




bool CVelMod::getTzFromZMod(CVelMod::VELMOD_X *modX, int nz, double fz, double dz, float Tz[]) {
	double *twttdouble = (double *)ealloc1double(nz);
	bool isSuc = getTzFromZModGrad(modX, nz, dz, twttdouble);
	for (int i = 0; i < nz; i++) {
		Tz[i] = (float)twttdouble[i];
	}
	free1double(twttdouble);
	return isSuc;
}

bool CVelMod::getZtFromTMod(CVelMod::VELMOD_X *modX, int nt, double dt, float Zt[]) {
	double *depthDouble = (double *)ealloc1double(nt);
	bool isSuc = getZtFromTmodGrad2(modX, nt, dt, depthDouble);
	for (int j = 0; j < nt; j++)
		Zt[j] = (float)depthDouble[j];
	free1double(depthDouble);
	return isSuc;
}

/*
Initialize a VELMOD_X struct from a model (VELMOD struct).
This will initialize the arrays in time or depth domains sepecified by isTime input variable
and will do the conversion from time to depth or from depth to time if needed, only once
*/
void CVelMod::initModX(CVelMod::VELMOD * velm, CVelMod::VELMOD_X *modX, 
	bool isTime, 
	bool isAnisotropyApply, double dz) {
	this->m_dz = dz;
	CString tStr;

	if (!velm->isVelModValid()) {
		if (!loadInitVel(velm, velm->velSecName, dz, tStr)) {
			modX->ncont = 0;
			return;
		}
	}
	modX->ncont = velm->ncont;
	modX->nRegressionLayer = velm->nRegressionLayer;
	modX->regVZero = velm->regVZero;
	modX->regVInf = velm->regVInf;
	modX->regAlpha = velm->regAlpha;
	modX->regVmin = velm->regVmin;
	modX->regVmax2 = velm->regVmax2;

	if (modX->dist < velm->getXmin()) modX->dist = velm->getXmin();
	if (modX->dist > velm->getXmax()) modX->dist = velm->getXmax();

	if (isTime) {
		// velm->m_isConvert  is used and modified here to be false 
		this->getTZModFromT2(velm, modX, dz, isAnisotropyApply);
	}
	else {
		this->getZModFromZ(velm, modX, isAnisotropyApply);
		// velm->m_isConvert is used here but not modified here
		if (velm->m_isConvert) {
			this->getTmodFromZmodGrad(modX, dz);
		}
	}
}

bool CVelMod::initModX(CString velSecNamesEx, CVelMod::VELMOD_X *modX,
	bool isTime,
	bool isAnisotropyApply, double dz) {
	if (velSecNamesEx.GetLength() <= 0) return false;

	CVelMod::VELMOD velm;
	//this->velModSetArrays(&velm);
	double xposArr[MOD_POINTS_MAX];
	double xShiftArr[MOD_POINTS_MAX];
	CStringArray *arrVelNamesEx = new CStringArray();
	CVelMod::velNamesExParse(velSecNamesEx, xShiftArr, xposArr, arrVelNamesEx);

	CString tStr;
	bool isFound = false;
	for (int i = 0; i < arrVelNamesEx->GetCount(); i++) {
		// this is a vel model
		if (i >= MOD_POINTS_MAX) break; // avoid overflow
		tStr = arrVelNamesEx->ElementAt(i);
		if (this->loadInitVel(&velm, tStr, dz, tStr)) {
			if (modX->dist >= velm.getXmin() + xShiftArr[i] && modX->dist <= velm.getXmax() + xShiftArr[i]) {
				initModX(&velm, modX, isTime, isAnisotropyApply, dz);
				isFound = true;
			}
		}
		//this->velModFreeArrays(&velm);
		if (isFound) break;
	}

	arrVelNamesEx->RemoveAll();
	delete arrVelNamesEx;
	return isFound;
}

void CVelMod::interpol_ModX_Notused(CVelMod::VELMOD_X *modX1, CVelMod::VELMOD_X *modX2, CVelMod::VELMOD_X *modXOut, double dist) {
	modX1->cloneTo(modXOut);
	modXOut->dist = dist;

	int i, i1 = 1, i2;
	int i0 = 1;  // seafloor
	double t = (dist - modX1->dist)/ (modX2->dist - modX1->dist);
	double delta = interpolLinear(modX1->depthMod[i0], modX2->depthMod[i0], t) - modX1->depthMod[i0];
	for (i = i0; i<modX1->ncont; i++)
		modXOut->depthMod[i1] += delta;

	// now process the basement
	for (i = 1; i < modX1->ncont; i++) {
		if (modX1->vTop[i] >= 4.0) {
			i1 = i;
			break;
		}
	}
	for (i = 1; i < modX2->ncont; i++) {
		if (modX2->vTop[i] >= 4.0) {
			i2 = i;
			break;
		}
	}
	delta = interpolLinear(modX1->depthMod[i1], modX2->depthMod[i2], t) - modX1->depthMod[i1];
	for (i = i0 + 1; i <= i1; i++) {
		register double dyThis = (modX1->depthMod[i] - modX1->depthMod[i0]) / (modX1->depthMod[i1] - modX1->depthMod[i0]);
		modXOut->depthMod[i1] += delta * dyThis;
	}

}

// this routine can handle multiple models, but it will not interpolate when modX->dist is not covered by all models.
// instead, it will find the closest location to get the vertical profile.
// Priority: if models overlap, the first matched model will be chosen!
bool CVelMod::initModXOld(VELMOD *velm, VELMODS *velms, CVelMod::VELMOD_X *modXOut1, CVelMod::VELMOD_X *modXOut2,
	double dist, bool isTime, bool isAnisotropyApply, double dz) {
	modXOut1->ncont = modXOut2->ncont = 0;
	if (velms->num <= 0) return false; // if only 1 model, we cannot interpolate

	double distUnshifted;

	// first, try to find the correct model(s)
	int i;
	for (i = 0; i < velms->num; i++) {
		if (velms->velm[i].ncont < 1) continue; // invalid model to skip
		distUnshifted = dist - velms->xShiftArr[i];
		if (distUnshifted >= velms->velm[i].getXmin() && distUnshifted <= velms->velm[i].getXmax()) {
			modXOut1->dist = distUnshifted;
			initModX(&velms->velm[i], modXOut1, isTime, isAnisotropyApply, dz);
			modXOut1->dist += velms->xShiftArr[i]; // inside a model, the xShift is not counted, but when output, we need the xShift, otherwise we lose control
			return true;
		}
	}

	// now we search for 2 adjacent models to interpolate.
	// first, find the model to the left of dist
	double dist_ref = -MIL;
	int i1 = -1;
	for (i = 0; i < velms->num; i++) {
		if (velms->velm[i].ncont < 1) continue; // invalid model to skip
		register double dist_this = velms->velm[i].getXmax() + velms->xShiftArr[i];
		if (dist_this <= dist && velms->velm[i].getXmax() + velms->xShiftArr[i] >= dist_ref) {
			dist_ref = dist_this;
			i1 = i;
		}
	}
	if (velm->ncont > 0 && velm->getXmax() <= dist && velm->getXmax() >= dist_ref) {
		modXOut1->dist = velm->getXmax();
		initModX(velm, modXOut1, isTime, isAnisotropyApply, dz);
	}
	else {
		if (i1 >= 0) {
			modXOut1->dist = velms->velm[i1].getXmax();
			initModX(&velms->velm[i1], modXOut1, isTime, isAnisotropyApply, dz);
			modXOut1->dist += velms->xShiftArr[i1]; // inside a model, the xShift is not counted, but when output, we need the xShift, otherwise we lose control
		}
	}


	dist_ref = MIL;
	int i2 = -1;
	for (i = 0; i < velms->num; i++) {
		if (velms->velm[i].ncont < 1) continue; // invalid model to skip
		distUnshifted = dist - velms->xShiftArr[i];
		register double dist_this = velms->velm[i].getXmin() + velms->xShiftArr[i];
		if (dist_this >= dist && dist_this <= dist_ref) {
			dist_ref = dist_this;
			i2 = i;
		}
	}
	if (velm->ncont > 0 && velm->getXmin() >= dist && velm->getXmin() <= dist_ref) {
		modXOut2->dist = velm->getXmin();
		initModX(velm, modXOut2, isTime, isAnisotropyApply, dz);
	}
	else {
		if (i2 >= 0) {
			modXOut2->dist = velms->velm[i2].getXmin();
			initModX(&velms->velm[i2], modXOut2, isTime, isAnisotropyApply, dz);
			modXOut2->dist += velms->xShiftArr[i2];
		}
	}

	return true;
}

// this newer version of initModX() takes VELMODS at higher priority than VELMOD, whereas in initModXOld() takes the opposite!
bool CVelMod::initModX(VELMOD *velm, VELMODS *velms, CVelMod::VELMOD_X *modXOut1, CVelMod::VELMOD_X *modXOut2,
	double dist, bool isTime, bool isAnisotropyApply, double dz) {
	modXOut1->ncont = modXOut2->ncont = 0;
	if (velms->num <= 0) {
		// if only 1 model, we cannot interpolate and just use it
		velm->m_isConvert = true;
		modXOut1->dist = dist; // the main model has no shift
		initModX(velm, modXOut1, false, true, dz);
		velm->m_isConvert = false; // only need to convert once
		modXOut1->cloneTo(modXOut2);
		return true;
	}

	double distUnshifted;

	// first, try to find the correct model(s)
	int i;
	for (i = 0; i < velms->num; i++) {
		if (velms->velm[i].ncont < 1) continue; // invalid model to skip
		distUnshifted = dist - velms->xShiftArr[i];
		if (distUnshifted >= velms->velm[i].getXmin() && distUnshifted <= velms->velm[i].getXmax()) {
			modXOut1->dist = distUnshifted;
			initModX(&velms->velm[i], modXOut1, isTime, isAnisotropyApply, dz);
			modXOut1->dist = dist; // inside a model, the xShift is not counted, but when output, we need the xShift, otherwise we lose control
			modXOut1->cloneTo(modXOut2);
			return true;
		}
	}

	// now test if the main model covers dist location already
	if (dist >= velm->getXmin() && dist <= velm->getXmax()) {
		modXOut1->dist = dist;
		initModX(velm, modXOut1, isTime, isAnisotropyApply, dz);
		modXOut1->cloneTo(modXOut2);
		return true;
	}

	// Now no models cover the point at dist. We search for 2 adjacent models to interpolate.
	// first, find the model to the left of dist
	double distLeft = -MIL;
	int i1 = -1;
	for (i = 0; i < velms->num; i++) {
		if (velms->velm[i].ncont < 1) continue; // invalid model to skip
		register double aDistMax = velms->velm[i].getXmax() + velms->xShiftArr[i];
		if (aDistMax <= dist && aDistMax >= distLeft) {
			distLeft = aDistMax;
			i1 = i;
		}
	}
	if (velm->ncont > 0 && velm->getXmax() <= dist && velm->getXmax() >= distLeft) {
		// the right side of the main model is still closer to dist
		modXOut1->dist = velm->getXmax();
		initModX(velm, modXOut1, isTime, isAnisotropyApply, dz);
	}
	else if (i1 >= 0) {
		modXOut1->dist = velms->velm[i1].getXmax();
		initModX(&velms->velm[i1], modXOut1, isTime, isAnisotropyApply, dz);
		modXOut1->dist += velms->xShiftArr[i1]; // inside a model, the xShift is not counted, but when output, we need the xShift, otherwise we lose control
	}
	else {
		// cannot find it
		modXOut1->dist = velm->getXmax();
		initModX(velm, modXOut1, isTime, isAnisotropyApply, dz);
	}


	// the second model to the right of dist
	double distRight = MIL;
	int i2 = -1;
	for (i = 0; i < velms->num; i++) {
		if (velms->velm[i].ncont < 1) continue; // invalid model to skip
		register double aDistMin = velms->velm[i].getXmin() + velms->xShiftArr[i];
		if (aDistMin >= dist && aDistMin <= distRight) {
			distRight = aDistMin;
			i2 = i;
		}
	}
	if (velm->ncont > 0 && velm->getXmin() >= dist && velm->getXmin() <= distRight) {
		// the xmin side of the main model is still closer to dist
		modXOut2->dist = velm->getXmin();
		initModX(velm, modXOut2, isTime, isAnisotropyApply, dz);
	}
	else if (i2 >= 0) {
		modXOut2->dist = velms->velm[i2].getXmin();
		initModX(&velms->velm[i2], modXOut2, isTime, isAnisotropyApply, dz);
		modXOut2->dist += velms->xShiftArr[i2];
	}
	else {
		// cannot find it
		modXOut2->dist = velm->getXmin();
		initModX(velm, modXOut2, isTime, isAnisotropyApply, dz);
	}

	return true;
}
BOOL CVelMod::myModel_Depth2TWTT(CVelMod::VELMOD *velm, double dz)
{
	// this function will only convert  velm->zm[][] to velm->tm[][]. Safe to do it multiple times.
	// if (!isTimeDomain()) return FALSE; // This function should be able to be called when it's in depth domain

	// to avoid miss-fire, we need to make sure the model is properly loaded before saving and overwriting to disk
	if (!velm->m_isConvert) return true;
	if (velm->ncont<1 && velm->ncont > MOD_LAYER_MAX) return FALSE;
	if (velm->nzed[0]<1 && velm->nzed[0] > MOD_POINTS_MAX) return FALSE;
	if (velm->nvel[0][0]<1 && velm->nvel[0][0] > MOD_POINTS_MAX) return FALSE;


	TRY
	{
		int icont, j;

		// in case it Velocity contours will be drawn in TWTT domain, we need to convert velMod->tm[] to  velMod->zm[] first
		VELMOD_X modX;
		for (icont = 0; icont < velm->ncont; icont++) {
			for (j = 0; j < velm->nzed[icont]; j++) {
				modX.dist = velm->xm[j][icont];
				initModX(velm, &modX, false, true, dz);
				velm->tm[j][icont] = (float)modX.timeMod[icont];
				//}
			}
		}


		// when converting a complex model, especially basement,
		// some points may be intercecting other boundaries
		for (icont = 1; icont < velm->ncont-1; icont++) {
			for (j = 1; j < velm->nzed[icont]-1; j++) {
				// register float t = velm->tm[j][icont];
				register float x = velm->xm[j][icont];
				register float t2 = velm->getCurrHorizonTimeAtDist(x, icont + 1);
				if (velm->tm[j][icont] > t2) {
					// this point is invalid, so interpolate
					register float dx = velm->xm[j + 1][icont] - velm->xm[j - 1][icont];
					if (dx == 0.f) continue;
					register float f = (x - velm->xm[j - 1][icont]) / dx;
					velm->tm[j][icont] =
						interpolLinear(velm->tm[j - 1][icont], velm->tm[j+1][icont], f);
					j++;
				}
			} // for j
		} // for icont

		return TRUE;
	}
	CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL
}



double CVelMod::my2WayTime2Depth2(CVelMod::VELMOD *velm, double x, double t2, double dz, double depthWork[], int nWork)
{
	//a routine to return a depth in km given a distance x and a 2-way time t in sec.
	// this routine also converts from depth to time domain as well
	// long icont;

	if (dz < 0.00001) dz = 0.004;  //make sure it doesn't overflow.
	if (x < velm->getXmin()) x = (float)velm->getXmin();
	if (x > velm->getXmax()) x = (float)velm->getXmax();

	VELMOD_X modX;
	modX.dist = x;
	velm->m_isConvert = true;
	this->initModX(velm, &modX, true, true, true);

	int ntmax = (int)(modX.timeMod[modX.ncont - 1] / dz + 1);
	double out = 0.;
	if (ntmax > nWork) {
		ntmax = nWork;  // the work size of array is smaller than maximum. Sometimes this is desired and it can save CPU time.
	}
	this->getZtFromTmodGrad2(&modX, ntmax, dz, depthWork);

	for (int j = 0; j < ntmax - 1; j++) {
		register double t = j*dz;
		if (t2 >= t && t2 <= t + dz) {
			out = depthWork[j] + (depthWork[j + 1] - depthWork[j]) / dz * (t2 - t);
			break;
		}
	}

	return out;
}

const double CVelMod::myDepth2Time2Way(CVelMod::VELMOD *velm, double x, double  z, double dz, double twttWork[], int nWork)
{
	// note: we need to convert from depth to time
	//	long icont;

	if (dz < 0.00001) dz = 0.004;  //make sure it doesn't overflow.
	this->m_dz = dz;

	long nzmax = (long)ceil(velm->zmax / dz) + 1;  //note: y is time in sec. nzmax is max num of pnts in depth.
	if (nzmax > nWork) 
		return 0.;

	if (x < velm->getXmin()) x = (float)velm->getXmin();
	if (x > velm->getXmax()) x = (float)velm->getXmax();

	VELMOD_X modX;
	modX.dist = x;
	velm->m_isConvert = true;
	this->initModX(velm, &modX, false, true, true);

	double out = getTimeAtZ(&modX, z, dz, twttWork, nWork);
	return out;
}



/*
This function will construct depthMod[], vTop[], vBot[] but not timeMod[].
It works in the depth domain only.
*/
const bool CVelMod::getZModFromZ(CVelMod::VELMOD *velm, CVelMod::VELMOD_X *modX, bool isAnisotropyApply)
{
	//***Note***: anisotropy will be considered in output of vTop[] and vBot[]
	if (velm->ncont < 1) return false;

	// the model may not have calculated the velm->xmin yet, so we do not use it
	float xmin = velm->xm[0][0];
	int nz = velm->nzed[0];
	if (nz < 1) return false;
	float xmax = velm->xm[nz - 1][0];
	if (modX->dist < xmin) modX->dist = xmin;
	if (modX->dist > xmax) modX->dist = xmax;

	long icont, j;
	double x1, x2, x3, x4, v1, v2, v3, v4;

	for (icont = 0; icont < velm->ncont; icont++) {
		for (j = 1; j < velm->nzed[icont]; j++) {
			if (modX->dist >= velm->xm[j - 1][icont] && modX->dist <= velm->xm[j][icont]) {
				register double dx = velm->xm[j][icont] - velm->xm[j - 1][icont];
				if (fabs(dx) < 0.00001f)
					modX->depthMod[icont] = velm->zm[j - 1][icont];
				else
					modX->depthMod[icont] = velm->zm[j - 1][icont] + (modX->dist - velm->xm[j - 1][icont])*
					(velm->zm[j][icont] - velm->zm[j - 1][icont]) / dx;
				break;
			}
		}
	}

	//Now locate the four corners that define the velocity of for all points at range.
	//*****x1***********x2******* with range at depth[icont]
	//**********range************
	//***************************
	//**x3*********************x4 with range at depth[icont+1]
	
//	double depthWater = velm->getDepthWater(modX->dist);
	double depthWater = modX->depthMod[NUM_WATER_LAYER];
	for (icont = 0; icont<velm->ncont - 1; icont++) {
		// icont is now actually layer numbering
		modX->vBot[icont] = -1.;

		if (velm->isSedimentRegression(icont)) {
			// consider regression
			register double H = modX->depthMod[icont] - depthWater;
			modX->vTop[icont] = velm->getRegVelAtZ(H);
			H = modX->depthMod[icont + 1] - depthWater;
			modX->vBot[icont] = velm->getRegVelAtZ(H);
		}

		if (modX->vBot[icont] < 0.) {
			for (j = 1; j < velm->nvel[icont][0]; j++) {
				if (modX->dist >= velm->xvel[j - 1][icont][0] && modX->dist <= velm->xvel[j][icont][0]) {
					x1 = velm->xvel[j - 1][icont][0];
					x2 = velm->xvel[j][icont][0];
					v1 = velm->vf[j - 1][icont][0];
					v2 = velm->vf[j][icont][0];
					break;
				}
			}
			for (j = 1; j < velm->nvel[icont][1]; j++) {
				if (modX->dist >= velm->xvel[j - 1][icont][1] && modX->dist <= velm->xvel[j][icont][1]) {
					x3 = velm->xvel[j - 1][icont][1];
					x4 = velm->xvel[j][icont][1];
					v3 = velm->vf[j - 1][icont][1];
					v4 = velm->vf[j][icont][1];
					break;
				}
			}
			modX->vTop[icont] = v1 + (v2 - v1)*(modX->dist - x1) / (x2 - x1);
			modX->vBot[icont] = v3 + (v4 - v3)*(modX->dist - x3) / (x4 - x3);
		}

		if (isAnisotropyApply) {
			modX->vTop[icont] = velModUpdAnisotropy(modX->vTop[icont], velm->anisotropy[icont]);
			modX->vBot[icont] = velModUpdAnisotropy(modX->vBot[icont], velm->anisotropy[icont]);
		}

	} // for icont


	return true;
}



// 
// Get depth for a point at xpos for a given velocity in a given layer
// percent  is the percentage of the current depth inside the model percent = (z-z1)/(z2-z1)
//          used only when the layer has a constant velocity
//  zinc: smallest depth increment when the calculated depth becomes the same as the top or bottom boundary
//			default: 0.0km
const double CVelMod::getDepthFromVel(CVelMod::VELMOD *velm, double dist, double vel, int icont, double percent, double zinc)
{
	if (icont < 0 || icont >= velm->ncont - 1) return -1.;

	CVelMod vm;

	double x1, x2, z1, z2, v1, v2, vtop, vbot;
	int j;

	// first get the min and max depth at dist in this layer
	int num = velm->nzed[icont];
	for (j = 1; j < num; j++) {
		z1 = -1.;
		if (dist <= velm->xm[0][icont]) {
			dist = velm->xm[0][icont];
			z1 = velm->zm[0][icont];
			break;
		}
		else if (dist >= velm->xm[num - 1][icont]) {
			dist = velm->xm[num - 1][icont];
			z1 = velm->zm[num - 1][icont];
			break;
		}
		else if (dist >= velm->xm[j - 1][icont] && dist <= velm->xm[j][icont])
		{
			z1 = velm->zm[j - 1][icont] + (dist - velm->xm[j - 1][icont])*
				(velm->zm[j][icont] - velm->zm[j - 1][icont]) / (velm->xm[j][icont] - velm->xm[j - 1][icont]);
			break;
		}
	} // for j

	num = velm->nzed[icont + 1];
	for (j = 1; j < num; j++) {
		z2 = -1.;
		if (dist <= velm->xm[0][icont + 1]) {
			dist = velm->xm[0][icont + 1];
			z2 = velm->zm[0][icont + 1];
			break;
		}
		else if (dist >= velm->xm[num - 1][icont + 1]) {
			dist = velm->xm[num - 1][icont + 1];
			z2 = velm->zm[num - 1][icont + 1];
			break;
		}
		else if (dist >= velm->xm[j - 1][icont + 1] && dist <= velm->xm[j][icont + 1])
		{
			z2 = velm->zm[j - 1][icont + 1] + (dist - velm->xm[j - 1][icont + 1])*
				(velm->zm[j][icont + 1] - velm->zm[j - 1][icont + 1]) / (velm->xm[j][icont + 1] - velm->xm[j - 1][icont + 1]);
			break;
		}
	} // for j
	if (z1 < 0. || z2 < 0.) {
		return -1.;
	}


	v1 = -1.;
	v2 = -1.;

	// secondly, get the velocity at the top and bottom of this layer at dist 
	for (j = 1; j < velm->nvel[icont][0]; j++) {
		if (dist >= velm->xvel[j - 1][icont][0] && dist <= velm->xvel[j][icont][0]) {
			x1 = velm->xvel[j - 1][icont][0];
			x2 = velm->xvel[j][icont][0];
			v1 = velm->vf[j - 1][icont][0];
			v2 = velm->vf[j][icont][0];
			break;
		}
	}
	if (v1 < 0. || v2 < 0.) {
		// prob????
		int ffff = 1;
	}
	v1 = vm.velModUpdAnisotropy((float)v1, velm->anisotropy[icont]);
	v2 = vm.velModUpdAnisotropy((float)v2, velm->anisotropy[icont]);
	vtop = (fabs(x2 - x1) > 0.00001) ? v1 + (v2 - v1)*(dist - x1) / (x2 - x1) : v1;

	if (vel <= vtop) return z1 + zinc;

	v1 = -1.;
	v2 = -1.;
	for (j = 1; j < velm->nvel[icont][1]; j++) {
		if (dist >= velm->xvel[j - 1][icont][1] && dist <= velm->xvel[j][icont][1]) {
			x1 = velm->xvel[j - 1][icont][1];
			x2 = velm->xvel[j][icont][1];
			v1 = velm->vf[j - 1][icont][1];
			v2 = velm->vf[j][icont][1];
			break;
		}
	}
	if (v1 < 0. || v2 < 0.) {
		// prob????
		int ffff = 1;
	}
	v1 = vm.velModUpdAnisotropy((float)v1, velm->anisotropy[icont]);
	v2 = vm.velModUpdAnisotropy((float)v2, velm->anisotropy[icont]);
	vbot = (fabs(x2 - x1) > 0.00001) ? v1 + (v2 - v1)*(dist - x1) / (x2 - x1) : v1;
	if (vel >= vbot) {
		return (z2 - zinc) > z1 ? z2 - zinc : z2;
	}

	double result;
	if (fabs(vbot - vtop) > 0.00001) {
		// the velocity has a gradient
		result = z1 + (z2 - z1)*(vel - vtop) / (vbot - vtop);
	}
	else {
		// we are in a constant velocity layer
		result = z1 + (z2 - z1)*percent;
	}


	return result;
}

/*
*/
const bool CVelMod::getTZModFromT2(CVelMod::VELMOD *velm, CVelMod::VELMOD_X *modX, double dt, bool isAnisotropyApply)
{
	if (velm->ncont < 1) return false;

	// the model may not have calculated the velm->xmin yet, so we do not use it
	float xmin = velm->xm[0][0];
	int nz = velm->nzed[0];
	if (nz < 1) return false;
	float xmax = velm->xm[nz-1][0];
	if (modX->dist < xmin) modX->dist = xmin;
	if (modX->dist > xmax) modX->dist = xmax;

	if (velm->m_isConvert) {
		myModel_TWTT2Depth(velm, dt);  // right now, it converts too many times, please help reduce it???
		velm->m_isConvert = false;
	}

	// now we can safe use any depth values 
	if (!getZModFromZ(velm, modX, isAnisotropyApply)) return false;
	return getTmodFromZmodGrad(modX, dt);
}

const float CVelMod::getAvgVelAtDistRange(CVelMod::VELMOD *velm, double x1, double x2, int icont)
{
	//average velocity?
	//create the polygons for each layer.
	//*****x1***********x2******* with range at depth[icont]
	//**********range************
	//***************************
	//**x3*********************x4 with range at depth[icont+1]
	int j;
	//double x1,x2,z1,z2,z3,z4;
	double v1 = -1.0, v2 = -1.0, v3 = -1.0, v4 = -1.0;
	double dx;

	int num = velm->nvel[icont][0];
	if (x1 <= velm->xvel[0][icont][0]) {
		v1 = velm->vf[0][icont][0];
	}
	else if (x1 >= velm->xvel[num - 1][icont][0]) {
		v1 = velm->vf[num - 1][icont][0];
	}
	else {
		for (j = 0; j < num - 1; j++) {
			if (x1 >= velm->xvel[j][icont][0] && x1 <= velm->xvel[j + 1][icont][0]) {
				dx = velm->xvel[j + 1][icont][0] - velm->xvel[j][icont][0];
				v1 = (fabs(dx) < MINAMP) ?
					velm->vf[j][icont][0]
				:
				velm->vf[j][icont][0] + (velm->vf[j + 1][icont][0] - velm->vf[j][icont][0])*
					(x1 - velm->xvel[j][icont][0]) / dx;
				break;
			}
			else continue;
		}
	}

	if (x2 >= velm->xvel[num - 1][icont][0]) {
		v2 = velm->vf[num - 1][icont][0];
	}
	else if (x2 >= velm->xvel[num - 1][icont][0]) {
		v2 = velm->vf[num - 1][icont][0];
	}
	else {
		for (j = 0; j < num - 1; j++) {
			if (x2 >= velm->xvel[j][icont][0] && x2 <= velm->xvel[j + 1][icont][0])
			{
				dx = velm->xvel[j + 1][icont][0] - velm->xvel[j][icont][0];
				v2 = (fabs(dx) < MINAMP) ?
					velm->vf[j][icont][0]
				:
				velm->vf[j][icont][0] + (velm->vf[j + 1][icont][0] - velm->vf[j][icont][0])*
					(x2 - velm->xvel[j][icont][0]) / dx;
				break;
			}
			else continue;
		}
	}


	num = velm->nvel[icont][1];
	if (x1 <= velm->xvel[0][icont][1]) {
		v3 = velm->vf[0][icont][1];
	}
	else if (x1 >= velm->xvel[num - 1][icont][1]) {
		v3 = velm->vf[num - 1][icont][1];
	}
	else {
		for (j = 0; j < velm->nvel[icont][1] - 1; j++) {
			if (x1 >= velm->xvel[j][icont][1] && x1 <= velm->xvel[j + 1][icont][1]) {
				dx = velm->xvel[j + 1][icont][1] - velm->xvel[j][icont][1];
				v3 = (fabs(dx) < MINAMP) ?
					velm->vf[j][icont][1]
				:
				velm->vf[j][icont][1] + (velm->vf[j + 1][icont][1] - velm->vf[j][icont][1])*
					(x1 - velm->xvel[j][icont][1]) / dx;
				break;
			}
			else continue;
		}
	}


	if (x2 <= velm->xvel[0][icont][1]) {
		v4 = velm->vf[0][icont][1];
	}
	else if (x2 >= velm->xvel[num - 1][icont][1]) {
		v4 = velm->vf[num - 1][icont][1];
	}
	else {
		for (j = 0; j < velm->nvel[icont][1] - 1; j++) {
			if (x2 >= velm->xvel[j][icont][1] && x2 <= velm->xvel[j + 1][icont][1]) {
				dx = velm->xvel[j + 1][icont][1] - velm->xvel[j][icont][1];
				v4 = (fabs(dx) < MINAMP) ?
					velm->vf[j][icont][1]
				:
				velm->vf[j][icont][1] + (velm->vf[j + 1][icont][1] - velm->vf[j][icont][1])*
					(x2 - velm->xvel[j][icont][1]) / dx;
				break;
			}
			else continue;
		}
	}
	if (v1 < 0.0 || v2 < 0.0 || v3 < 0.0 || v4 < 0.0) return 0.0;

	return (float)((v1 + v2 + v3 + v4) / 4.0);
}








// delete a point in the velocity mode
bool CVelMod::velModVelDelPoint(CVelMod::VELMOD *velm, int icont, int jDel, bool isTop)
{
	int nlayer = velm->ncont - 1;  // num of layers is 1 less than num of boundaries
	int k = isTop ? 0 : 1;
	if (nlayer < 0 ||
		icont < 0 || icont >= nlayer
		|| jDel <= 0 || jDel >= velm->nvel[icont][k] - 1) return false;

	velm->nvel[icont][k]--;
	for (int j = jDel; j < velm->nvel[icont][k]; j++) {
		velm->xvel[j][icont][k] = velm->xvel[j + 1][icont][k];
		velm->vf[j][icont][k] = velm->vf[j + 1][icont][k];
	}

	return true;
}

bool CVelMod::velModVelInsPoint(CVelMod::VELMOD *velm, int icont, int jIns, bool isTop, float x, float v)
{
	int nlayer = velm->ncont - 1;  // num of layers is 1 less than num of boundaries
	if (icont < 0 || jIns < 0 || nlayer<0 || icont>nlayer) return false;

	int k = isTop ? 0 : 1;
	int n = velm->nvel[icont][k];
	if (n >= MOD_POINTS_MAX) {
		AfxMessageBox(_T("Maximum points reached for selected layer"));
		return false;
	}
	if (jIns>0 && fabs(x - velm->xvel[jIns - 1][icont][k]) < 0.00001) return false;
	if (jIns < n - 1 && fabs(x - velm->xvel[jIns + 1][icont][k]) < 0.00001) return false;
	if (jIns > n) jIns = n;

	velm->nvel[icont][k]++;
	for (int j = velm->nvel[icont][k] - 2; j >= jIns; j--) {
		velm->xvel[j + 1][icont][k] = velm->xvel[j][icont][k];
		velm->vf[j + 1][icont][k] = velm->vf[j][icont][k];
	}
	velm->xvel[jIns][icont][k] = x;
	velm->vf[jIns][icont][k] = v;
	return true;
}




/*
return=1:  true
0:  false
-1:  unable to determine
*/
int CVelMod::isPointAbove(CVelMod::VELMOD *velm, int icont, SEI_PAIR pNew, BOOL isTimeDomain)
{
	if (icont < 0) return -1;
	int n = velm->nzed[icont];
	if (n < 2) return -1;
	if (pNew.x < velm->xm[0][icont] || pNew.x > velm->xm[n - 1][icont]) return -1;

	for (int j = 0; j < n - 1; j++) {
		if (pNew.x >= velm->xm[j][icont] && pNew.x <= velm->xm[j + 1][icont]) {
			if (isTimeDomain)
				return IsPointAbove(velm->xm[j][icont], velm->tm[j][icont], velm->xm[j + 1][icont], velm->tm[j + 1][icont], (float)pNew.x, (float)pNew.y);
			else
				return IsPointAbove(velm->xm[j][icont], velm->zm[j][icont], velm->xm[j + 1][icont], velm->zm[j + 1][icont], (float)pNew.x, (float)pNew.y);
		}
	}

	return -1;
}


// default layer number to return is 0;
int CVelMod::getClosestLayerNum(CVelMod::VELMOD *velm, SEI_PAIR pNew, BOOL isTimeDomain)
{
	int icontOut = 0;
	if (velm->ncont < 1) return icontOut;

	double distMin = MIL;
	for (int i = 0; i < velm->ncont; i++) {
		int n = velm->nzed[i];
		if (n < 1) continue;
		//if (pNew.x < velm->xm[0][icont] || pNew.x > velm->xm[n-1][icont]) return -1;

		for (int j = 0; j < n - 1; j++) {
			if (pNew.x >= velm->xm[j][i] && pNew.x <= velm->xm[j + 1][i]) {
				register double z = isTimeDomain ? velm->tm[j][i] : velm->zm[j][i];
				register double z1 = isTimeDomain ? velm->tm[j + 1][i] : velm->zm[j + 1][i];
				register double a = FindShortestDistanceToSegment((double)velm->xm[j][i], z, (double)velm->xm[j + 1][i], z1, pNew.x, pNew.y);
				if (distMin > a) {
					// found a closer boundary to use
					distMin = a;
					icontOut = i;
				}
				break;  // finished processing for this loop
			}
		} // for j
	} // for i

	return icontOut;
}


// return -1 if failed
// we assume the model must be monotonically increase with X!
int CVelMod::getPointIndexBeforeOnBoundary(VELMOD *velm, double xNew, int icont)
{
	int jSel = -1;
	if (icont < 0) return jSel;

	for (int j = 1; j < velm->nzed[icont]; j++) {
		if (xNew >= velm->xm[j - 1][icont] && xNew <= velm->xm[j][icont]) {
			jSel = j - 1;
			break;
		}
	}



	return jSel;
}

// return -1 if failed
// we assume the model must be monotonically increase with X!
int CVelMod::getPointIndexClosestOnBoundary(VELMOD *velm, SEI_PAIR pNew, int icont, BOOL isTimeDomain)
{
	int jSel = -1;
	if (icont < 0) return jSel;

	if (abs(this->getClosestLayerNum(velm, pNew, isTimeDomain) - icont) > max(1, 0.25*velm->ncont)) return -1;
	// this is needed since sometimes the user clicked on a different layer and we do not want to react to that.
	// but if I use only allow the same icont to be selected before allowing to continue,
	// it is sometimes too restrictive for mouse operation

	double distOld = MIL;
	for (int j = 0; j < velm->nzed[icont]; j++) {
		register double distNew = fabs(velm->xm[j][icont] - pNew.x);
		register double dist = min(distNew, distOld);
		if (dist == distNew)  {
			jSel = j;
			distOld = distNew;
		}
	}



	return jSel;
}

// return -1 if failed, *vInterpol  will be output too
// we assume the model must be monotonically increase with X!
int CVelMod::getPointIndexBeforeOnLayer(VELMOD *velm, double xNew, int icont, bool isTopBound, double *vInterpol)
{
	int jSel = -1;
	*vInterpol = -1.;
	if (icont < 0) return jSel;
	int k = (isTopBound) ? 0 : 1;

	for (int j = 1; j < velm->nvel[icont][k]; j++) {
		if (xNew >= velm->xvel[j - 1][icont][k] && xNew <= velm->xvel[j][icont][k]) {
			register double vdelta = velm->xvel[j][icont][k] - velm->xvel[j - 1][icont][k];
			if (fabs(vdelta) < 0.00001)
				*vInterpol = velm->xvel[j - 1][icont][k];
			else
				*vInterpol = velm->vf[j - 1][icont][k] +
				(velm->vf[j][icont][k] - velm->vf[j - 1][icont][k])*(xNew - velm->xvel[j - 1][icont][k]) / vdelta;
			jSel = j - 1;
			break;
		}
	}

	return jSel;
}

// return -1 if failed, *vInterpol  will be output too
// we assume the model must be monotonically increase with X!
int CVelMod::getPointIndexClosestOnLayer(VELMOD *velm, double xNew, int icont, bool isTopBound, double *vInterpol)
{
	int jSel = -1;
	*vInterpol = -1.;
	if (icont < 0) return jSel;
	int k = (isTopBound) ? 0 : 1;

	double distOld = MIL;
	int n = velm->nvel[icont][k];
	for (int j = 0; j < n; j++) {
		register double distNew = fabs(velm->xvel[j][icont][k] - xNew);
		register double dist = min(distNew, distOld);
		if (dist == distNew)  {
			jSel = j;
			distOld = distNew;
		}
	}
	if (jSel < 0) return jSel;
	if (jSel == 0 || jSel == n) {
		*vInterpol = velm->vf[jSel][icont][k];
		return jSel;
	}

	int j1, j2;  // defining 2 indeses to interpolate
	if (xNew <= velm->xvel[jSel][icont][k]) {
		j1 = jSel - 1;
		j2 = jSel;
	}
	else {
		j1 = jSel;
		j2 = jSel + 1;
	}

	double vdelta = velm->xvel[j2][icont][k] - velm->xvel[j1][icont][k];
	if (fabs(vdelta) < 0.00001)
		*vInterpol = velm->xvel[j1][icont][k];
	else
		*vInterpol = velm->vf[j1][icont][k] +
		(velm->vf[j2][icont][k] - velm->vf[j1][icont][k])*(xNew - velm->xvel[j1][icont][k]) / vdelta;

	return jSel;
}

bool CVelMod::velModInsPoint(CVelMod::VELMOD *velm, int icont, SEI_PAIR pNew, BOOL isTimeDomain)
{
	if (icont < 0 || velm->ncont<1 || icont>velm->ncont - 1) return false;
	int jInserted = velm->addPoint((float)pNew.x, icont);
	if (jInserted<1 || fabs(pNew.x - velm->xm[jInserted - 1][icont]) < 0.00001) return false;
	int n = velm->nzed[icont];
	if (jInserted < n - 1 && fabs(pNew.x - velm->xm[jInserted + 1][icont]) < 0.00001) return false;

	// will need to test if the new point makes boundaries crossing each other
	VELMOD_X modX;
	modX.dist = pNew.x;
	velm->m_isConvert = true;
	this->initModX(velm, &modX, isTimeDomain==TRUE, true, m_dt);
	if (isTimeDomain) {
		if (icont>0 && pNew.y < modX.timeMod[icont - 1]) 
			pNew.y = modX.timeMod[icont - 1];
		if (icont<velm->ncont-1 && pNew.y > modX.timeMod[icont+1]) 
			pNew.y = modX.timeMod[icont + 1];
	}
	else {
		if (icont>0 && pNew.y < modX.depthMod[icont - 1]) 
			pNew.y = modX.depthMod[icont - 1];
		if (icont<velm->ncont - 1 && pNew.y > modX.depthMod[icont + 1]) 
			pNew.y = modX.depthMod[icont + 1];
	}

	if (isTimeDomain)
		velm->tm[jInserted][icont] = (float)pNew.y;
	else
		velm->zm[jInserted][icont] = (float)pNew.y;

	return true;
}

bool CVelMod::velModInsPoint(CVelMod::VELMOD *velm, int icont, int jIns, SEI_PAIR pNew, BOOL isTimeDomain)
{
	if (icont < 0 || jIns < 0 || velm->ncont<1 || icont>velm->ncont - 1) return false;
	int n = velm->nzed[icont];
	if (n >= MOD_POINTS_MAX) {
		AfxMessageBox(_T("Maximum points reached for selected boundary"));
		return false;
	}
	if (jIns>0 && fabs(pNew.x - velm->xm[jIns - 1][icont]) < 0.00001) return false;
	if (jIns < n - 1 && fabs(pNew.x - velm->xm[jIns + 1][icont]) < 0.00001) return false;
	if (jIns > n) jIns = n;

	// will need to test if the new point makes boundaries crossing each other
	VELMOD_X modX;
	modX.dist = pNew.x;
	velm->m_isConvert = true;
	this->initModX(velm, &modX, isTimeDomain == TRUE, true, m_dt);
	if (isTimeDomain) {
		if (icont>0 && pNew.y < modX.timeMod[icont - 1])
			pNew.y = modX.timeMod[icont - 1];
		if (icont<velm->ncont - 1 && pNew.y > modX.timeMod[icont + 1])
			pNew.y = modX.timeMod[icont + 1];
	}
	else {
		if (icont>0 && pNew.y < modX.depthMod[icont - 1])
			pNew.y = modX.depthMod[icont - 1];
		if (icont<velm->ncont - 1 && pNew.y > modX.depthMod[icont + 1])
			pNew.y = modX.depthMod[icont + 1];
	}

	// now it is safe to add point
	velm->nzed[icont]++;
	for (int k = velm->nzed[icont] - 2; k >= jIns; k--) {
		velm->xm[k + 1][icont] = velm->xm[k][icont];
		velm->zm[k + 1][icont] = velm->zm[k][icont];
		velm->tm[k + 1][icont] = velm->tm[k][icont];
	}
	velm->xm[jIns][icont] = (float)pNew.x;
	velm->zm[jIns][icont] = (float)pNew.y;
	velm->tm[jIns][icont] = (float)pNew.y;

	// because all the newly added points do not have any nav info
	velm->longitude[jIns][icont] = ERRORVAL;
	velm->latitude[jIns][icont] = ERRORVAL;


	return true;
}


/*
Given a *velMod, the boundary icont has a point defined by
(velMod->xm[jMove][icont], velMod->xm[jMove][icont])
which just got moved to a new position at pNew.
We want to make sure the boundaries above and below icont to not crossing icont at all.
RULES: 1. Boundary icont shall have only 1 point that crosses other boundaries.
2. This function does not need to modify anything on boundary icont since it will be handled in the calling program
*/
bool CVelMod::velModInterceptPoint(CVelMod::VELMOD *velm, int icont, int jMove, SEI_PAIR pNew, BOOL isTimeDomain)
{
	CVelMod vm;

	if (!velm->isVelModValid() || icont < 0 || icont > velm->ncont - 1) {
		return false;
	}

	double m1, c1, m2, c2;
	//float intersection_X, intersection_Y;
	float x, y;
	bool isTrue;
	int i, j; // indeces for boundary number and point number
	int jInd;
	int nzed = velm->nzed[icont];
	if (nzed < 2) return true;  // we just give up auto adjusting since it is unusual
	int isMovingUpInt = vm.isPointAbove(velm, icont, pNew, isTimeDomain);
	if (isMovingUpInt < 0) return false;
	velm->xm[jMove][icont] = (float)pNew.x;
	velm->zm[jMove][icont] = (float)pNew.y;

	SEI_QUAD l1;
	SEI_PAIR intersection;
	l1.x1 = velm->xm[jMove - 1][icont];
	l1.y1 = velm->zm[jMove - 1][icont];
	l1.x2 = pNew.x;
	l1.y2 = pNew.y;

	// handles all overlying boundaries
	if (isMovingUpInt > 0) {
		for (i = 0; i < icont; i++) {
			jInd = -1;
			for (j = 0; j < velm->nzed[i]; j++) {
				if (pNew.y < velm->zm[j][i]) {
					jInd = j;
					break;
				}
			}
			if (jInd < 0) continue;  // no points intersecting, so skip this boundary

			// now there are at least one point at boundary i that's need to adjust shallower.
			if (jMove == 0) {
				if (pNew.y < velm->zm[0][i]) {
					// simply move is enough
					velm->zm[0][i] = (float)pNew.y;
					for (int k = 1; k < velm->nzed[i]; k++) {
						if (IsPointAbove(pNew, velm->xm[jMove + 1][icont], velm->zm[jMove + 1][icont], velm->xm[k][i], velm->zm[k][i]) != 0)
							break;
						velm->delPoint(i, k);
						k--;
					}
				}
			}
			else if (jMove == nzed - 1) {
				// pNew is at the last point of icont boundary
				int n = velm->nzed[i];
				if (pNew.y < velm->zm[n - 1][i]) {
					// simply move is enough
					velm->zm[n - 1][i] = (float)pNew.y;
					for (int k = n - 2; k > 0; k--) {
						if (IsPointAbove(velm->xm[jMove - 1][icont], velm->zm[jMove - 1][icont], (float)pNew.x, (float)pNew.y, velm->xm[k][i], velm->zm[k][i]) != 0)
							break;
						velm->delPoint(i, k);
						n = velm->nzed[i];
						k--;
					}
				}
			}
			else {
				//register float x1 = velm->xm[jMove - 1][icont];
				//register float y1 = velm->zm[jMove - 1][icont];
				register float x2 = velm->xm[jMove + 1][icont];
				register float y2 = velm->zm[jMove + 1][icont];
				for (j = 0; j < velm->nzed[i] - 1; j++) {
					if (velm->xm[j + 1][i] < l1.x1 + 0.1f) continue;
					if (velm->xm[j][i] > x2 - 0.1f) continue;
					//isTrue = LineSegmentIntersection(x1, y1, (float)pNew.x, (float)pNew.y,
					//	velm->xm[j][i], velm->zm[j][i], velm->xm[j + 1][i], velm->zm[j + 1][i],
					//	&m1, &c1, &m2, &c2, &intersection_X, &intersection_Y);
					register SEI_QUAD l2;
					l2.x1 = velm->xm[j][i];
					l2.y1 = velm->zm[j][i];
					l2.x2 = velm->xm[j + 1][i];
					l2.y2 = velm->zm[j + 1][i];
					isTrue = LineSegmentIntersection(l1, l2, &intersection, &m1, &c1, &m2, &c2);
					if (isTrue) {
						// now we have intersection.
						// now all points from and after j+1 are to be deleted except after a certain point. How to calculate this second point?
						for (int k = j + 1; k < velm->nzed[i]; k++) {
							x = velm->xm[k][i];
							y = velm->zm[k][i];
							if (x >= x2) break;
							register SEI_PAIR p; p.x = x; p.y = y;
							if ((x < pNew.x && IsPointAbove(l1, p) == 0)
								||
								(x >= pNew.x && IsPointAbove(pNew, x2, y2, p) == 0)) {
								velm->delPoint(i, k);
									k--;
							}
							else break;
						}

						// a new point is needed to insert at j+1
						velModInsPoint(velm, i, j + 1, pNew, isTimeDomain);
						break;  // for simplicity, only do 1 operation at a time for a boundary

					} // if isTrue
				} // for j
			}
		} // for i
	}
	else {
		// handles all underlying boundaries
		for (i = icont + 1; i < velm->ncont; i++) {
			int n = velm->nzed[i];
			jInd = -1;
			for (j = 0; j<n; j++) {
				if (pNew.y > velm->zm[j][i]) {
					jInd = j;
					break;
				}
			}
			if (jInd < 0) continue;  // no points intersecting, so skip this boundary

			// now there are at least one point at boundary i that's need to adjust shallower.
			if (jMove == 0) {
				if (pNew.y > velm->zm[0][i]) {
					// simply move is enough
					velm->zm[0][i] = (float)pNew.y;
					for (int k = 1; k<n; k++) {
						if (IsPointAbove(pNew, velm->xm[jMove + 1][icont], velm->zm[jMove + 1][icont], velm->xm[k][i], velm->zm[k][i]) != 1)
							break;
						velm->delPoint(i, k);
						k--;
					}
				}
			}
			else if (jMove == nzed - 1) {
				// pNew is at the last point of icont boundary
				if (pNew.y > velm->zm[n - 1][i]) {
					// simply move is enough
					velm->zm[n - 1][i] = (float)pNew.y;
					for (int k = n - 2; k > 0; k--) {
						if (IsPointAbove(velm->xm[jMove - 1][icont], velm->zm[jMove - 1][icont], (float)pNew.x, (float)pNew.y, velm->xm[k][i], velm->zm[k][i]) != 1)
							break;
						velm->delPoint(i, k);
						n = velm->nzed[i];
						k--;
					}
				}
			}
			else {
				//register float x1 = velm->xm[jMove - 1][icont];
				//register float y1 = velm->zm[jMove - 1][icont];
				register float x2 = velm->xm[jMove + 1][icont];
				register float y2 = velm->zm[jMove + 1][icont];
				for (j = 0; j < velm->nzed[i] - 1; j++) {
					if (velm->xm[j + 1][i] < l1.x1 + 0.1f) continue;
					if (velm->xm[j][i] > x2 - 0.1f) continue;
					//isTrue = LineSegmentIntersection(x1, y1, (float)pNew.x, (float)pNew.y,
					//	velm->xm[j][i], velm->zm[j][i], velm->xm[j + 1][i], velm->zm[j + 1][i],
					//	&m1, &c1, &m2, &c2, &intersection_X, &intersection_Y);
					register SEI_QUAD l2;
					l2.x1 = velm->xm[j][i];
					l2.y1 = velm->zm[j][i];
					l2.x2 = velm->xm[j + 1][i];
					l2.y2 = velm->zm[j + 1][i];
					isTrue = LineSegmentIntersection(l1, l2, &intersection, &m1, &c1, &m2, &c2);
					if (isTrue) {
						// now we have intersection.
						// now all points from and after j+1 are to be deleted except after a certain point. How to calculate this second point?
						for (int k = j + 1; k < velm->nzed[i]; k++) {
							x = velm->xm[k][i];
							y = velm->zm[k][i];
							SEI_PAIR p; p.x = x; p.y = y;
							if (x >= x2) break;
							if ((x < pNew.x && IsPointAbove(l1, p) == 1)
								||
								(x >= pNew.x && IsPointAbove(pNew, x2, y2, p) == 1)) {
									velm->delPoint(i, k);
									k--;
							}
							else break;
						}

						// a new point is needed to insert at j+1
						velModInsPoint(velm, i, j + 1, pNew, isTimeDomain);
						break;  // for simplicity, only do 1 operation at a time for a boundary

					} // if isTrue
				} // for j
			}
		} // for i
	}


	return true;
}


bool CVelMod::velModInterceptPointMode9(CVelMod::VELMOD *velm, int icont, int jMove, SEI_PAIR pNew, BOOL isTimeDomain)
{
	if (!velm->isVelModValid() || icont < 0 || icont > velm->ncont - 1) {
		return false;
	}

	CVelMod vm;

	double m1, c1, m2, c2;
	//float intersection_X, intersection_Y;
	float x, y;
	bool isTrue;
	int i, j; // indeces for boundary number and point number
	int jInd;
	int nzed = velm->nzed[icont];
	if (nzed < 2) return true;  // we just give up auto adjusting since it is unusual
	int isMovingUpInt = vm.isPointAbove(velm, icont, pNew, isTimeDomain);
	if (isMovingUpInt < 0) return false;

	velm->xm[jMove][icont] = (float)pNew.x;
	velm->tm[jMove][icont] = (float)pNew.y;

	SEI_QUAD l1;
	SEI_PAIR intersection;
	l1.x1 = velm->xm[jMove - 1][icont];
	l1.y1 = velm->tm[jMove - 1][icont];
	l1.x2 = pNew.x;
	l1.y2 = pNew.y;

	// handles all overlying boundaries
	if (isMovingUpInt > 0) {
		for (i = 0; i < icont; i++) {
			jInd = -1;
			for (j = 0; j < velm->nzed[i]; j++) {
				if (pNew.y < velm->tm[j][i]) {
					jInd = j;
					break;
				}
			}
			if (jInd < 0) continue;  // no points intersecting, so skip this boundary

			// now there are at least one point at boundary i that's need to adjust shallower.
			if (jMove == 0) {
				if (pNew.y < velm->tm[0][i]) {
					// simply move is enough
					velm->tm[0][i] = (float)pNew.y;
					for (int k = 1; k < velm->nzed[i]; k++) {
						if (IsPointAbove(pNew, velm->xm[jMove + 1][icont], velm->tm[jMove + 1][icont], velm->xm[k][i], velm->tm[k][i]) != 0)
							break;
						velm->delPoint(i, k);
						k--;
					} // for
				} // if pNew
			}
			else if (jMove == nzed - 1) {
				// pNew is at the last point of icont boundary
				int n = velm->nzed[i];  // usage of n is discouraged since it is confusing
				if (pNew.y < velm->tm[n - 1][i]) {
					// simply move is enough
					velm->tm[n - 1][i] = (float)pNew.y;
					for (int k = n - 2; k > 0; k--) {
						if (IsPointAbove(velm->xm[jMove - 1][icont], velm->tm[jMove - 1][icont], (float)pNew.x, (float)pNew.y, velm->xm[k][i], velm->tm[k][i]) != 0)
							break;
						velm->delPoint(i, k);
						n = velm->nzed[i];
						k--;
					}
				}
			}
			else {
				//register float x1 = velm->xm[jMove - 1][icont];
				//register float y1 = velm->tm[jMove - 1][icont];
				register float x2 = velm->xm[jMove + 1][icont];
				register float y2 = velm->tm[jMove + 1][icont];
				for (j = 0; j < velm->nzed[i] - 1; j++) {
					if (velm->xm[j + 1][i] < l1.x1 + 0.1f) continue;
					if (velm->xm[j][i] > x2 - 0.1f) continue;
					//isTrue = LineSegmentIntersection(x1, y1, (float)pNew.x, (float)pNew.y,
					//	velm->xm[j][i], velm->tm[j][i], velm->xm[j + 1][i], velm->tm[j + 1][i],
					//	&m1, &c1, &m2, &c2, &intersection_X, &intersection_Y);
					register SEI_QUAD l2;
					l2.x1 = velm->xm[j][i];
					l2.y1 = velm->tm[j][i];
					l2.x2 = velm->xm[j + 1][i];
					l2.y2 = velm->tm[j + 1][i];
					isTrue = LineSegmentIntersection(l1, l2, &intersection, &m1, &c1, &m2, &c2);
					if (isTrue) {
						// now we have intersection.
						// now all points from and after j+1 are to be deleted except after a certain point. How to calculate this second point?
						for (int k = j + 1; k < velm->nzed[i]; k++) {
							x = velm->xm[k][i];
							y = velm->tm[k][i];
							register SEI_PAIR p; p.x = x; p.y = y;
							if (x >= x2) break;
							if ((x >= l1.x1 && x <= (float)pNew.x && IsPointAbove(l1, p) == 0)
								||
								(x >= (float)pNew.x && x <= x2 && IsPointAbove(pNew, x2, y2, p) == 0)) {
								velm->delPoint(i, k);
									k--;
							}
							else break;
						}

						// a new point is needed to insert at j+1
						this->velModInsPoint(velm, i, j + 1, pNew, isTimeDomain);
						break;  // for simplicity, only do 1 operation at a time for a boundary

					} // if isTrue
				} // for j
			}
		} // for i
	}
	else {
		// handles all underlying boundaries
		for (i = icont + 1; i < velm->ncont; i++) {
			jInd = -1;
			for (j = 0; j<velm->nzed[i]; j++) {
				if (pNew.y > velm->tm[j][i]) {
					jInd = j;
					break;
				}
			}
			if (jInd < 0) continue;  // no points intersecting, so skip this boundary

			// now there are at least one point at boundary i that's need to adjust shallower.
			if (jMove == 0) {
				if (pNew.y > velm->tm[0][i]) {
					// simply move is enough
					velm->tm[0][i] = (float)pNew.y;
					for (int k = 1; k<velm->nzed[i]; k++) {
						if (IsPointAbove(pNew, velm->xm[jMove + 1][icont], velm->tm[jMove + 1][icont], velm->xm[k][i], velm->tm[k][i]) != 1)
							break;
						velm->delPoint(i, k);
						k--;
					}
				}
			}
			else if (jMove == nzed - 1) {
				// pNew is at the last point of icont boundary
				int n = velm->nzed[i];
				if (pNew.y > velm->tm[n - 1][i]) {
					// simply move is enough
					int n = velm->nzed[i];
					velm->tm[n - 1][i] = (float)pNew.y;
					for (int k = n - 2; k > 0; k--) {
						if (IsPointAbove(velm->xm[jMove - 1][icont], velm->tm[jMove - 1][icont], (float)pNew.x, (float)pNew.y, velm->xm[k][i], velm->tm[k][i]) != 1)
							break;
						velm->delPoint(i, k);
						n = velm->nzed[i];
						k--;
					}
				}
			}
			else {
				//register float x1 = velm->xm[jMove - 1][icont];
				//register float y1 = velm->tm[jMove - 1][icont];
				register float x2 = velm->xm[jMove + 1][icont];
				register float y2 = velm->tm[jMove + 1][icont];
				for (j = 0; j < velm->nzed[i] - 1; j++) {
					if (velm->xm[j + 1][i] < l1.x1 + 0.1f) continue;
					if (velm->xm[j][i] > x2 - 0.1f) continue;
					register SEI_QUAD l2;
					l2.x1 = velm->xm[j][i];
					l2.y1 = velm->tm[j][i];
					l2.x2 = velm->xm[j + 1][i];
					l2.y2 = velm->tm[j + 1][i];
					//isTrue = LineSegmentIntersection(x1, y1, (float)pNew.x, (float)pNew.y,
					//	velm->xm[j][i], velm->tm[j][i], velm->xm[j + 1][i], velm->tm[j + 1][i],
					//	&m1, &c1, &m2, &c2, &intersection_X, &intersection_Y);
					isTrue = LineSegmentIntersection(l1, l2, &intersection, &m1, &c1, &m2, &c2);
					if (isTrue) {
						// now we have intersection.
						// now all points from and after j+1 are to be deleted except after a certain point. How to calculate this second point?
						for (int k = j + 1; k < velm->nzed[i]; k++) {
							x = velm->xm[k][i];
							y = velm->tm[k][i];
							if (x >= x2) break;
							register SEI_PAIR p; p.x = x; p.y = y;
							if ((x < pNew.x && IsPointAbove(l1, p) == 1)
								||
								(x >= pNew.x && IsPointAbove(pNew, x2, y2, p) == 1)) {
								velm->delPoint(i, k);
									k--;
							}
							else break;
						}

						// a new point is needed to insert at j+1
						this->velModInsPoint(velm, i, j + 1, pNew, isTimeDomain);
						break;  // for simplicity, only do 1 operation at a time for a boundary

					} // if isTrue
				} // for j
			}
		} // for i
	}


	return true;
}


// save the model as .BIN file directly without any conversion or XML issues
BOOL CVelMod::saveModelBinDirect(CVelMod::VELMOD *velm, CString f)
{
	// NOTE: Recursive usage: the model backup function also uses this routine, but its files
	//       will be stored inside a temp dir and does not have
	//       extensions like .vxml or .bin. So do not get confused 
	if (velm->ncont<1 || velm->ncont > MOD_LAYER_MAX) return FALSE;
	if (velm->nzed[0]<1 || velm->nzed[0] > MOD_POINTS_MAX) return FALSE;
	if (velm->nvel[0][0]<1 || velm->nvel[0][0] > MOD_POINTS_MAX) return FALSE;

	// is f is empty, this function is only doing backups!!!
	if (GetFileExtension(f).CompareNoCase(_T("bin")) != 0)
		f = SetFileExtension(f, _T("bin"));

	//if (!isInsideTempDir(f))
	//	f = SetFileExtension(f, _T("bin"));

	TRY
	{


	int icont, j, k;

	CFile filev(f, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	filev.Write(&velm->ncont, 4);

	filev.Write(&velm->nzed[0], (long)(velm->ncont * 4));
	for (icont = 0; icont < velm->ncont; icont++)
		for (j = 0; j < velm->nzed[icont]; j++) filev.Write(&velm->xm[j][icont], 4);
	for (icont = 0; icont < velm->ncont; icont++)
		for (j = 0; j < velm->nzed[icont]; j++) filev.Write(&velm->zm[j][icont], 4);

	int nlayer = velm->ncont - 1;
	filev.Write(&nlayer, 4);
	for (k = 0; k < 2; k++)
		for (icont = 0; icont < velm->ncont - 1; icont++)
			filev.Write(&velm->nvel[icont][k], 4);

	for (k = 0; k < 2; k++) {
		for (icont = 0; icont < velm->ncont - 1; icont++)
			for (j = 0; j < velm->nvel[icont][k]; j++) filev.Write(&velm->xvel[j][icont][k], 4);
		for (icont = 0; icont < velm->ncont - 1; icont++)
			for (j = 0; j < velm->nvel[icont][k]; j++) filev.Write(&velm->vf[j][icont][k], 4);
	}

	// additions: anisotropy added Oct., 2009 for model
	for (icont = 0; icont < velm->ncont - 1; icont++)
		filev.Write(&velm->anisotropy[icont], 4);

	if (velm->nRegressionLayer > 0) {
		// regression para exist
		filev.Write(&velm->nRegressionLayer, 4);
		filev.Write(&velm->regVZero, 8);
		filev.Write(&velm->regVInf, 8);
		filev.Write(&velm->regAlpha, 8);
	}

	filev.Close();

	// note: do not set the velm->velSecTimeLoaded to the current modified time
	// since the data has been saved but not loaded yet.
	// Why loading again? because initilizing a velMod file will compute some useful info as well
	// CTime aTimeLoaded = getFileModifiedTime(f);
//	velm->modifiedLoaded = zeroTime;
	velm->modifiedLoaded = getFileModifiedTime(f);

	return TRUE;
	}
		CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL

}


BOOL CVelMod::saveModelAs(CVelMod::VELMOD *velm, CString f, double dz, BOOL isTimeDomain, int restoreLevelMax)
{
	// no global variables are used from this routine, so you need to convert from TWTT to depth beforehand!
	// dz is used only for time to depth conversion
	// to avoid miss-fire, we need to make sure the model is properly loaded before saving and overwriting to disk

	// NOTE: Recursive usage: the model backup function also uses this routine, but its files
	//       will be stored inside a temp dir and does not have
	//       extensions like .vxml or .bin. So do not get confused 
	if (velm->ncont<1 || velm->ncont > MOD_LAYER_MAX) return FALSE;
	if (velm->nzed[0]<1 || velm->nzed[0] > MOD_POINTS_MAX) return FALSE;
	if (velm->nvel[0][0]<1 || velm->nvel[0][0] > MOD_POINTS_MAX) return FALSE;

	// is f is empty, this function is only doing backups!!!
	if (!f.IsEmpty()) {
		if (isTimeDomain) {
			myModel_TWTT2Depth(velm, dz);
		}

		if (restoreLevelMax > 0)
			myModelBackup2(velm, restoreLevelMax, dz, isTimeDomain);

		if (GetFileExtension(f).IsEmpty())
			f = SetFileExtension(f, _T("vxml"));

		velm->velSecName = f; // this is important
		if (GetFileExtension(f).CompareNoCase(_T("vxml")) == 0)
			return this->saveModelXML_V3(velm, f, dz, isTimeDomain);
	}

	return saveModelBinDirect(velm, f);

	//TRY
	//{

	//	// now, since it is not in vxml extension, we want to make sure it is .bin extension
	//	if (!isInsideTempDir(f))
	//		f = SetFileExtension(f, _T("bin"));

	//	int icont, j, k;

	//	CFile filev(f, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
	//	filev.Write(&velm->ncont, 4);

	//	filev.Write(&velm->nzed[0], (long)(velm->ncont * 4));
	//	for (icont = 0; icont < velm->ncont; icont++)
	//		for (j = 0; j < velm->nzed[icont]; j++) filev.Write(&velm->xm[j][icont], 4);
	//	for (icont = 0; icont < velm->ncont; icont++)
	//		for (j = 0; j < velm->nzed[icont]; j++) filev.Write(&velm->zm[j][icont], 4);

	//	int nlayer = velm->ncont - 1;
	//	filev.Write(&nlayer, 4);
	//	for (k = 0; k < 2; k++)
	//		for (icont = 0; icont < velm->ncont - 1; icont++)
	//			filev.Write(&velm->nvel[icont][k], 4);

	//	for (k = 0; k < 2; k++) {
	//		for (icont = 0; icont < velm->ncont - 1; icont++)
	//			for (j = 0; j < velm->nvel[icont][k]; j++) filev.Write(&velm->xvel[j][icont][k], 4);
	//		for (icont = 0; icont < velm->ncont - 1; icont++)
	//			for (j = 0; j < velm->nvel[icont][k]; j++) filev.Write(&velm->vf[j][icont][k], 4);
	//	}

	//	// additions: anisotropy added Oct., 2009 for model
	//	for (icont = 0; icont < velm->ncont - 1; icont++)
	//		filev.Write(&velm->anisotropy[icont], 4);

	//	if (velm->nRegressionLayer > 0) {
	//		// regression para exist
	//		filev.Write(&velm->nRegressionLayer, 4);
	//		filev.Write(&velm->regVZero, 8);
	//		filev.Write(&velm->regVInf, 8);
	//		filev.Write(&velm->regAlpha, 8);
	//	}

	//	filev.Close();

	//	// note: do not set the velm->velSecTimeLoaded to the current modified time
	//	// since the data has been saved but not loaded yet.
	//	// Why loading again? because initilizing a velMod file will compute some useful info as well
	//	// CTime aTimeLoaded = getFileModifiedTime(f);
	//	velm->modifiedLoaded = zeroTime;
	//	return TRUE;
	//}
	//CATCH_ALL(e)
	//{
	//	return FALSE;
	//}
	//END_CATCH_ALL


}




//
//BOOL CVelMod::myModel_TWTT2Depth(CVelMod::VELMOD *velm, double dt)
//{
//	// this function will only convert  velm->tm[][] to velm->zm[][]. Safe to do it multiple times.
//	// if (!isTimeDomain()) return FALSE; // This function should be able to be called when it's in depth domain
//
//	// to avoid miss-fire, we need to make sure the model is properly loaded before saving and overwriting to disk
//	if (velm->ncont<1 || velm->ncont > MOD_LAYER_MAX) return FALSE;
//	if (velm->nzed[0]<1 || velm->nzed[0] > MOD_POINTS_MAX) return FALSE;
//	if (velm->nvel[0][0]<1 || velm->nvel[0][0] > MOD_POINTS_MAX) return FALSE;
//
//	CVelMod vm;
//
//
//	TRY
//	{
//		int icont, j;
//
//		// in case it Velocity contours will be drawn in TWTT domain, we need to convert velMod->tm[] to  velMod->zm[] first
//		double timeMod[MOD_LAYER_MAX],vTop[MOD_LAYER_MAX],vBot[MOD_LAYER_MAX];
//		double depthMod[MOD_LAYER_MAX];
//		double x;
//		for (icont=0; icont<velm->ncont; icont++) {
//			for (j=0; j<velm->nzed[icont]; j++) {
//				x = velm->xm[j][icont];
//				vm.getTZCurveAtDist(velm, x, &timeMod[0], &vTop[0], &vBot[0]);
//				getDepthModFromTwtt(velm->ncont, &timeMod[0], &vTop[0], &vBot[0], dt, x, &depthMod[0]); 
//				velm->zm[j][icont] = (float)depthMod[icont];
//			}
//		}
//
//		return TRUE;
//	}
//	CATCH_ALL(e)
//	{
//		return FALSE;
//	}
//	END_CATCH_ALL
//}

//
//BOOL CVelMod::myModel_TWTT2DepthOld(CVelMod::VELMOD *velm, double dt)
//{
//	// this function will only convert  velm->tm[][] to velm->zm[][]. Safe to do it multiple times.
//	// if (!isTimeDomain()) return FALSE; // This function should be able to be called when it's in depth domain
//
//	// to avoid miss-fire, we need to make sure the model is properly loaded before saving and overwriting to disk
//	if (velm->ncont<1 || velm->ncont > MOD_LAYER_MAX) return FALSE;
//	if (velm->nzed[0]<1 || velm->nzed[0] > MOD_POINTS_MAX) return FALSE;
//	if (velm->nvel[0][0]<1 || velm->nvel[0][0] > MOD_POINTS_MAX) return FALSE;
//
//	CVelMod vm;
//	VELMOD_X modX;
//
//	TRY
//	{
//	int icont, j;
//	velm->m_isConvert = true;
//	for (icont = 0; icont < velm->ncont; icont++) {
//			for (j = 0; j < velm->nzed[icont]; j++) {
//				modX.dist = velm->xm[j][icont];
//				if (icont == 3) {
//					int sdfsd = 1;
//				}
//				this->initModX(velm, &modX, true, true, dt);
//				velm->zm[j][icont] = (float)modX.depthMod[icont];
//			}
//		}
//
//		//free(zTrc);
//		return TRUE;
//	}
//	CATCH_ALL(e)
//	{
//		//free(zTrc);
//		return FALSE;
//	}
//	END_CATCH_ALL
//		//free(zTrc);
//
//		return TRUE;
//}


BOOL CVelMod::myModel_TWTT2Depth(CVelMod::VELMOD *velm, double dt)
{
	// this function will only convert  velm->tm[][] to velm->zm[][]. Safe to do it multiple times.
	// if (!isTimeDomain()) return FALSE; // This function should be able to be called when it's in depth domain

	// to avoid miss-fire, we need to make sure the model is properly loaded before saving and overwriting to disk
	if (velm->ncont<1 || velm->ncont > MOD_LAYER_MAX) return FALSE;
	if (velm->nzed[0]<1 || velm->nzed[0] > MOD_POINTS_MAX) return FALSE;
	if (velm->nvel[0][0]<1 || velm->nvel[0][0] > MOD_POINTS_MAX) return FALSE;

	CVelMod vm;
	VELMOD_X modX;

	TRY
	{
		int icont;
	for (icont = 0; icont < velm->ncont; icont++) {
		myModel_TWTT2Depth(velm, icont, dt);
	}

	return TRUE;
	}
		CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL

		return TRUE;
}

BOOL CVelMod::myModel_TWTT2Depth(CVelMod::VELMOD *velm, int icontWorking, double dt)
{
	// this function will only convert  velm->tm[][] to velm->zm[][] for horizon icontWorking. Safe to do it multiple times.
	// assuming all horizons above icontWorking were already converted, so that one can call this iteratively
	if (velm->ncont<1 || velm->ncont > MOD_LAYER_MAX) return FALSE;
	if (velm->nzed[0]<1 || velm->nzed[0] > MOD_POINTS_MAX) return FALSE;
	if (velm->nvel[0][0]<1 || velm->nvel[0][0] > MOD_POINTS_MAX) return FALSE;
	if (icontWorking<0 || velm->ncont -1 < icontWorking) return FALSE;

	CVelMod vm;
	VELMOD_X modX;

	int j;
	if (icontWorking == 0) {
		for (j = 0; j < velm->nzed[0]; j++) {
			velm->zm[j][icontWorking] = 0.f;
		}
		return TRUE;
	}

	velm->m_isConvert = true;
	for (j = 0; j < velm->nzed[icontWorking]; j++) {
		modX.dist = velm->xm[j][icontWorking];
		this->initModX(velm, &modX, false, true, dt);  // this will assume working in depth domain
				// and I will only use the output values for layers above icontWorking
		//double z1 = modX.depthMod[icontWorking-1]; // depth at the top horizon of this layer
		//double t1 = modX.timeMod[icontWorking - 1]; // twt at the top horizon of this layer
		modX.timeMod[icontWorking] = velm->tm[j][icontWorking];

		// now calculate velm->zm[j][icontWorking] == modX.depthMod[icontWorking] 
		getZmodFromTmodGrad2(&modX, icontWorking, dt);
		velm->zm[j][icontWorking] = (float)modX.depthMod[icontWorking];
	}

	return TRUE;
}

BOOL CVelMod::myModelBackup2(CVelMod::VELMOD *velm, int restoreLevelMax, double dz, BOOL isTimeDomain)
{
	// this is to save model for undo. It'll save up to 9 versions.
	CString tStr;
//	CString tBin = CString(_T("~%dbin~"));
	CString outFile;
	int fBackupIndex = 0;
	for (int i = 0; i < restoreLevelMax * 2; i++) {
		tStr.Format(_T("%d.bin"), i); 
		outFile = getFileNameTemp(velm->velSecName, tStr);
		if (!isFileExistNotDirectory(outFile)) {
			fBackupIndex = i;
			break;
		}
	}
	// now fBackupIndex is the maximum backup index that is not used. We should use it

	if (fBackupIndex >= restoreLevelMax) {
		// since we got too many backup files, we need to move the backup files backward by 1
		CString s1, s2;
		for (int i = 1; i < restoreLevelMax; i++) {
			tStr.Format(_T("%d.bin"), i - 1);  
			s1 = getFileNameTemp(velm->velSecName, tStr);
			tStr.Format(_T("%d.bin"), i);  
			s2 = getFileNameTemp(velm->velSecName, tStr);
			if (!MoveFile(s2, s1)) return FALSE;
		}
		if (!s2.IsEmpty()) outFile = s2;
	} // if 

	saveModelBinDirect(velm, outFile);
	return TRUE;
}



bool CVelMod::myVelModInterpolDisplay(CVelMod::VELMOD *velm, double xmin, double xmax, double xShift, double dxVDesired)
{
	//this routine calculates the interpolated value for X, Z, V for a given model that is already loaded by MyInitVel.
	//this is called only by DrawVel(), DrawCont(), not by OnDraw()
	// the model may be bigger, but this routine only 

	//this rouine set values for 
	// in velMod struct.

	int ix, j, icont;
	double x1, x2, x3, x4, v1, v2, v3, v4, range;

	if (velm->ncont < 1) return false;



	velm->grdXmin = velm->getXmin() + xShift;
	if (velm->grdXmin < xmin)
		velm->grdXmin = xmin;

	double grdXmax = velm->getXmax() + xShift;
	if (grdXmax > xmax)
		grdXmax = xmax;
	if (velm->grdXmin > xmax || grdXmax < xmin) return false; // nothing to display for this model



	velm->grdDx = dxVDesired;
	velm->grdNx = (int)((grdXmax - velm->grdXmin) / velm->grdDx + 1.5);
	// note: at the beginning,  velm->nx<1 can be true !  updated Jan 2008
	if (velm->grdNx < 1 || velm->grdNx>500) {
		velm->grdDx = (grdXmax - velm->grdXmin) / 499.;
		velm->grdNx = (int)((grdXmax - velm->grdXmin) / velm->grdDx + 1.5);
	}
	if (velm->grdDx < MINAMP) {
		velm->grdDx = 0.001;
		velm->grdNx = (int)((grdXmax - velm->grdXmin) / velm->grdDx + 1.5);
	}

	// First interpolate the velocity model according to newly-defined regular mesh.
	for (ix = 0; ix < velm->grdNx; ix++) {
		range = velm->grdXmin + velm->grdDx*(double)ix;
		register double rangeUnshifted = range - xShift;
		for (icont = 0; icont < velm->ncont; icont++) {
			for (j = 1; j < velm->nzed[icont]; j++) {
				if (rangeUnshifted >= velm->xm[j - 1][icont] && rangeUnshifted <= velm->xm[j][icont]) {
					register double dx = velm->xm[j][icont] - velm->xm[j - 1][icont];
					velm->grdDepth[icont][ix] = (fabs(dx)<0.00001)  ?  velm->zm[j][icont]  :  
						velm->zm[j - 1][icont] + (rangeUnshifted - velm->xm[j - 1][icont])*
						(velm->zm[j][icont] - velm->zm[j - 1][icont]) / dx;
					break;
				}
			}
		}

		//Now locate the four corners that define the velocity of for all points at range.
		//*****x1***********x2******* with range at depth[icont]
		//**********range************
		//***************************
		//**x3*********************x4 with range at depth[icont+1]
		for (icont = 0; icont < velm->ncont - 1; icont++) {
			for (j = 1; j < velm->nvel[icont][0]; j++) {
				if (rangeUnshifted >= velm->xvel[j - 1][icont][0] && rangeUnshifted <= velm->xvel[j][icont][0]) {
					x1 = velm->xvel[j - 1][icont][0];
					x2 = velm->xvel[j][icont][0];
					v1 = velm->vf[j - 1][icont][0];
					v2 = velm->vf[j][icont][0];
					break;
				}
			}
			for (j = 1; j < velm->nvel[icont][1]; j++) {
				if (rangeUnshifted >= velm->xvel[j - 1][icont][1] && rangeUnshifted <= velm->xvel[j][icont][1]) {
					x3 = velm->xvel[j - 1][icont][1];
					x4 = velm->xvel[j][icont][1];
					v3 = velm->vf[j - 1][icont][1];
					v4 = velm->vf[j][icont][1];
					break;
				}
			}
			velm->vTop[icont][ix] = v1 + (v2 - v1)*(rangeUnshifted - x1) / (x2 - x1);
			velm->vBot[icont][ix] = v3 + (v4 - v3)*(rangeUnshifted - x3) / (x4 - x3);
		}
	}  //Finish preparing for interpolation of new mesh.

	velm->isGrdInitialised = true;
	return true;
}




// save Zelt's ascii format with 2 decimals
bool CVelMod::saveModelZelt2(CVelMod::VELMOD *velm, CString newName)
{
	int j, j1, j2, icnt, icont;
	FILE *stream;
	if (_tfopen_s(&stream, newName, _T("w")) != 0)
	{
		AfxMessageBox(_T("The new velocity file was not opened"));
		return false;
	}
	// fseek( stream, 0L, SEEK_SET );
	//TCHAR *form = _T("%9.2f");
	CString form = _T("%7.2f");

	//VELMOD velMod = *velm;
	for (icont = 0; icont < velm->ncont; icont++) {
		j1 = 1;
		j2 = 10;
		while (j1 <= velm->nzed[icont]) {
			if (j2 >= velm->nzed[icont]) {
				icnt = 0;
				j2 = velm->nzed[icont];
			}
			else
				icnt = 1;

			if (icont == 0 && j1 == 1)
				_ftprintf_s(stream, _T("%2d "), icont + 1);
			else
				_ftprintf_s(stream, _T("\n%2d "), icont + 1);

			if (icont < velm->ncont - 1) {
				for (j = j1; j <= j2; j++)
					_ftprintf_s(stream, form, velm->xm[j - 1][icont]);
				// _ftprintf_s(stream, _T("%8.4f", velm->xm[j-1][icont]);
				_ftprintf_s(stream, _T("\n%2d "), icnt);
				for (j = j1; j <= j2; j++)
					_ftprintf_s(stream, form, velm->zm[j - 1][icont]);
				_ftprintf_s(stream, _T("\n%10d"), 0);
				for (j = j1; j < j2; j++)
					_ftprintf_s(stream, _T("%7d"), 0);
			}
			else { //last boundary can only have one point, required by zelt's program.
				_ftprintf_s(stream, form, velm->xm[velm->nzed[icont] - 1][icont]);
				_ftprintf_s(stream, _T("\n%2d "), 0);
				_ftprintf_s(stream, form, velm->zm[velm->nzed[icont] - 1][icont]);
				_ftprintf_s(stream, _T("\n%10d"), 0); // this is required ending, fixed Dec 2006
			}
			j1 += 10;
			j2 += 10;
		}

		if (icont == velm->ncont - 1) break;
		j1 = 1;
		j2 = 10;
		while (j1 <= velm->nvel[icont][0]) {
			if (j2 >= velm->nvel[icont][0]) {
				icnt = 0;
				j2 = velm->nvel[icont][0];
			}
			else
				icnt = 1;

			_ftprintf_s(stream, _T("\n%2d "), icont + 1);
			for (j = j1; j <= j2; j++)
				_ftprintf_s(stream, form, velm->xvel[j - 1][icont][0]);
			_ftprintf_s(stream, _T("\n%2d "), icnt);
			for (j = j1; j <= j2; j++)
				_ftprintf_s(stream, form, velm->vf[j - 1][icont][0]);
			_ftprintf_s(stream, _T("\n%10d"), 0);
			for (j = j1; j < j2; j++)
				_ftprintf_s(stream, _T("%7d"), 0);

			j1 += 10;
			j2 += 10;
		}

		j1 = 1;
		j2 = 10;
		while (j1 <= velm->nvel[icont][1]) {
			if (j2 >= velm->nvel[icont][1]) {
				icnt = 0;
				j2 = velm->nvel[icont][1];
			}
			else
				icnt = 1;

			_ftprintf_s(stream, _T("\n%2d "), icont + 1);
			for (j = j1; j <= j2; j++)
				_ftprintf_s(stream, form, velm->xvel[j - 1][icont][1]);
			_ftprintf_s(stream, _T("\n%2d "), icnt);
			for (j = j1; j <= j2; j++)
				_ftprintf_s(stream, form, velm->vf[j - 1][icont][1]);
			_ftprintf_s(stream, _T("\n%10d"), 0);
			for (j = j1; j < j2; j++)
				_ftprintf_s(stream, _T("%7d"), 0);
			j1 += 10;
			j2 += 10;
		}
	}
	fclose(stream);

	return true;
}




// save Zelt's ascii format with 4 decimals
bool CVelMod::saveModelZelt4(CVelMod::VELMOD *velm, CString newName)
{
	//VELMOD velMod = *velm;
	int j, j1, j2, icnt, icont;
	FILE *stream;
	if (_tfopen_s(&stream, newName, _T("w")) != 0)
	{
		AfxMessageBox(_T("The new velocity file was not opened"));
		return false;
	}
	// fseek( stream, 0L, SEEK_SET );
	CString form = _T("%9.4f");
	for (icont = 0; icont < velm->ncont; icont++)
	{
		j1 = 1;
		j2 = 10;
		while (j1 <= velm->nzed[icont])
		{
			if (j2 >= velm->nzed[icont])
			{
				icnt = 0;
				j2 = velm->nzed[icont];
			}
			else
				icnt = 1;

			if (icont == 0 && j1 == 1)
				_ftprintf_s(stream, _T("%2d "), icont + 1);
			else
				_ftprintf_s(stream, _T("\n%2d "), icont + 1);

			if (icont < velm->ncont - 1)
			{
				for (j = j1; j <= j2; j++)
					_ftprintf_s(stream, form, velm->xm[j - 1][icont]);
				// _ftprintf_s(stream, _T("%8.4f", velm->xm[j-1][icont]);
				_ftprintf_s(stream, _T("\n%2d "), icnt);
				for (j = j1; j <= j2; j++)
					_ftprintf_s(stream, form, velm->zm[j - 1][icont]);
				_ftprintf_s(stream, _T("\n%12d"), 0);
				for (j = j1; j < j2; j++)
					_ftprintf_s(stream, _T("%9d"), 0);
			}
			else
			{ //last boundary can only have one point, required by zelt's program.
				_ftprintf_s(stream, form, velm->xm[velm->nzed[icont] - 1][icont]);
				_ftprintf_s(stream, _T("\n%2d "), 0);
				_ftprintf_s(stream, form, velm->zm[velm->nzed[icont] - 1][icont]);
				//	_ftprintf_s(stream, _T("\n%13d ", 0); // this is required ending, fixed Dec 2006
			}
			j1 += 10;
			j2 += 10;
		}

		if (icont == velm->ncont - 1) break;
		j1 = 1;
		j2 = 10;
		while (j1 <= velm->nvel[icont][0])
		{
			if (j2 >= velm->nvel[icont][0])
			{
				icnt = 0;
				j2 = velm->nvel[icont][0];
			}
			else
				icnt = 1;

			_ftprintf_s(stream, _T("\n%2d "), icont + 1);
			for (j = j1; j <= j2; j++)
				_ftprintf_s(stream, form, velm->xvel[j - 1][icont][0]);
			_ftprintf_s(stream, _T("\n%2d "), icnt);
			for (j = j1; j <= j2; j++)
				_ftprintf_s(stream, form, velm->vf[j - 1][icont][0]);
			_ftprintf_s(stream, _T("\n%12d"), 0);
			for (j = j1; j < j2; j++)
				_ftprintf_s(stream, _T(" %8d"), 0);

			j1 += 10;
			j2 += 10;
		}

		j1 = 1;
		j2 = 10;
		while (j1 <= velm->nvel[icont][1])
		{
			if (j2 >= velm->nvel[icont][1])
			{
				icnt = 0;
				j2 = velm->nvel[icont][1];
			}
			else
				icnt = 1;

			_ftprintf_s(stream, _T("\n%2d "), icont + 1);
			for (j = j1; j <= j2; j++)
				_ftprintf_s(stream, form, velm->xvel[j - 1][icont][1]);
			_ftprintf_s(stream, _T("\n%2d "), icnt);
			for (j = j1; j <= j2; j++)
				_ftprintf_s(stream, form, velm->vf[j - 1][icont][1]);
			_ftprintf_s(stream, _T("\n%12d"), 0);
			for (j = j1; j < j2; j++)
				_ftprintf_s(stream, _T(" %8d"), 0);
			j1 += 10;
			j2 += 10;
		}
	}
	fclose(stream);

	return true;
}






// save Zelt's ascii format in XML
//bool CVelMod::saveModelXML(CVelMod::VELMOD *velm, CString newName)
//{
//	return saveModelXML_V3(velm, newName);
//
//}


bool getModelDeploysV3(MSXML::IXMLDOMDocument2Ptr pXMLDoc, CVelMod::DEPLOY deploys[]) {
	//return true;
	/*
	<deploy title="">
		<drop x="0" z="0.3" lat="-99999" lon="-99999"></drop>
		<start lat="-99999" lon="-99999"></start>
		<end lat="-99999" lon="-99999"></end>
	</deploy>
	<deploy title="">
		<dist>0</dist>
		<deployDepth>0</deployDepth>
		<deployLat>-99999</deployLat>
		<deployLon>-99999</deployLon>
		<startLat>73.8061218261719</startLat>
		<startLon>-140.318634033203</startLon>
		<endLat>74.0981979370117</endLat>
		<endLon>-140.433654785156</endLon>
	</deploy>
	*/

	MSXML::IXMLDOMNodeListPtr pElems = pXMLDoc->getElementsByTagName(_T("deploy"));
	if (pElems == NULL) return false;
	long len = pElems->length;
	if (len < 1) return false;

	for (int i = 0; i < len; i++) {
		MSXML::IXMLDOMNodePtr pElem = pElems->Getitem(i);
		if (pElem == NULL) break;

		deploys[i].metaTitle = getAttributeDef(pElem->attributes, _T("title"), strBlank);
		MSXML::IXMLDOMNodePtr p = myGetSingleChildByName(pElem, _T("dist"));
		if (p == NULL) break;
		deploys[i].deployDist = _variant_t(p->Gettext());

		p = myGetSingleChildByName(pElem, _T("deployDepth"));
		if (p) deploys[i].deployDepth = _variant_t(p->Gettext());

		p = myGetSingleChildByName(pElem, _T("deployLat"));
		if (p) deploys[i].deployLatLong[0] = _variant_t(p->Gettext());
		p = myGetSingleChildByName(pElem, _T("deployLon"));
		if (p) deploys[i].deployLatLong[1] = _variant_t(p->Gettext());

		p = myGetSingleChildByName(pElem, _T("startLat"));
		if (p) deploys[i].startLatLong[0] = _variant_t(p->Gettext());
		p = myGetSingleChildByName(pElem, _T("startLon"));
		if (p) deploys[i].startLatLong[1] = _variant_t(p->Gettext());

		p = myGetSingleChildByName(pElem, _T("endLat"));
		if (p) deploys[i].endLatLong[0] = _variant_t(p->Gettext());
		p = myGetSingleChildByName(pElem, _T("endLon"));
		if (p) deploys[i].endLatLong[1] = _variant_t(p->Gettext());
	}


	return true;
}

bool getModelDeploysV5(MSXML::IXMLDOMDocument2Ptr pXMLDoc
	, CVelMod::DEPLOY deploys[]
	, CVelMod::PSEUDOWELL wells[]) {
	
	MSXML::IXMLDOMNodeListPtr pElems = pXMLDoc->getElementsByTagName(_T("deploy"));
	if (pElems == NULL) return false;
	long len = pElems->length;
	if (len < 1) return false;

	for (int i = 0; i < len; i++) {
		MSXML::IXMLDOMNodePtr pElem = pElems->Getitem(i);
		if (pElem == NULL) break;

		deploys[i].metaTitle = getAttributeDef(pElem->attributes, _T("title"), strBlank);


		MSXML::IXMLDOMNodePtr p = myGetSingleChildByName(pElem, _T("drop"));
		if (p == NULL) break;
		deploys[i].deployDist = getAttributeDef(p->attributes, _T("x"), ERRORVAL);
		deploys[i].deployDepth = getAttributeDef(p->attributes, _T("z"), 0);
		deploys[i].deployLatLong[0] = getAttributeDef(p->attributes, _T("lat"), ERRORVAL);
		deploys[i].deployLatLong[1] = getAttributeDef(p->attributes, _T("lon"), ERRORVAL);

		p = myGetSingleChildByName(pElem, _T("start"));
		if (p == NULL) break;
		deploys[i].startDist = getAttributeDef(p->attributes, _T("x"), ERRORVAL);
		deploys[i].startOffset = getAttributeDef(p->attributes, _T("offset"), ERRORVAL);
		deploys[i].startLatLong[0] = getAttributeDef(p->attributes, _T("lat"), ERRORVAL);
		deploys[i].startLatLong[1] = getAttributeDef(p->attributes, _T("lon"), ERRORVAL);

		p = myGetSingleChildByName(pElem, _T("end"));
		if (p == NULL) break;
		deploys[i].endDist = getAttributeDef(p->attributes, _T("x"), ERRORVAL);
		deploys[i].endOffset = getAttributeDef(p->attributes, _T("offset"), ERRORVAL);
		deploys[i].endLatLong[0] = getAttributeDef(p->attributes, _T("lat"), ERRORVAL);
		deploys[i].endLatLong[1] = getAttributeDef(p->attributes, _T("lon"), ERRORVAL);

		p = myGetSingleChildByName(pElem, _T("pseudoWell"));
		if (p == NULL) {
			// it is ok not to have a pseudo-well
			wells[i].wellDist = ERRORVAL;
			wells[i].wellLatLong[0] = ERRORVAL;
			wells[i].wellLatLong[1] = ERRORVAL;
		}
		else {
			wells[i].wellDist = getAttributeDef(p->attributes, _T("x"), ERRORVAL);
			wells[i].wellLatLong[0] = getAttributeDef(p->attributes, _T("lat"), ERRORVAL);
			wells[i].wellLatLong[1] = getAttributeDef(p->attributes, _T("lon"), ERRORVAL);
		}
	}




	pElems = pXMLDoc->getElementsByTagName(_T("pseudoWell"));
	if (pElems == NULL) return false;
	len = pElems->length;
	if (len < 1) return false;

	for (int i = 0; i < len; i++) {
		MSXML::IXMLDOMNodePtr pElem = pElems->Getitem(i);
		if (pElem == NULL) break;

		wells[i].title = getAttributeDef(pElem->attributes, _T("title"), strBlank);
		wells[i].wellDist = getAttributeDef(pElem->attributes, _T("x"), ERRORVAL);
		wells[i].wellLatLong[0] = getAttributeDef(pElem->attributes, _T("lat"), ERRORVAL);
		wells[i].wellLatLong[1] = getAttributeDef(pElem->attributes, _T("lon"), ERRORVAL);
	}


	return true;
}


/*
// this function will set only 1 *deploy. You may need to loop thru like this
for (int i = 0; i < TX_SHOTS_MAX; i++) {
	if (velm->deploys[i].deployDist == ERRORVAL) break;
	setModelDeploy(pXMLDoc, &velm->deploys[i]);
}
*/
void setModelDeploy5(MSXML::IXMLDOMDocument2Ptr pXMLDoc, CVelMod::DEPLOY *deploy, CString deploylText) {
	MSXML::IXMLDOMElementPtr pElem = pXMLDoc->createElement(_T("deploy"));

	pElem->setAttribute(_T("title"), _variant_t(deploy->metaTitle));

	MSXML::IXMLDOMElementPtr p = pXMLDoc->createElement(_T("drop"));
	p->setAttribute(_T("x"), _variant_t(myFloat8(deploy->deployDist)));
	p->setAttribute(_T("z"), _variant_t(myFloat8(deploy->deployDepth)));
	p->setAttribute(_T("lon"), _variant_t(myFloat8(deploy->deployLatLong[1])));
	p->setAttribute(_T("lat"), _variant_t(myFloat8(deploy->deployLatLong[0])));
	CString t = CARRAGE + deploylText + CARRAGE;  //  
	p->Puttext(_bstr_t(t));
	pElem->appendChild(p);

	p = pXMLDoc->createElement(_T("start"));
	p->setAttribute(_T("x"), _variant_t(myFloat8(deploy->startDist)));
	p->setAttribute(_T("offset"), _variant_t(myFloat8(deploy->startOffset)));
	p->setAttribute(_T("lon"), _variant_t(myFloat8(deploy->startLatLong[1])));
	p->setAttribute(_T("lat"), _variant_t(myFloat8(deploy->startLatLong[0])));
	pElem->appendChild(p);

	p = pXMLDoc->createElement(_T("end"));
	p->setAttribute(_T("x"), _variant_t(myFloat8(deploy->endDist)));
	p->setAttribute(_T("offset"), _variant_t(myFloat8(deploy->endOffset)));
	p->setAttribute(_T("lon"), _variant_t(myFloat8(deploy->endLatLong[1])));
	p->setAttribute(_T("lat"), _variant_t(myFloat8(deploy->endLatLong[0])));
	pElem->appendChild(p);

	MSXML::IXMLDOMElementPtr pXMLRootElem = pXMLDoc->GetdocumentElement();
	pXMLRootElem->appendChild(pElem);
}

void setModelWell5(MSXML::IXMLDOMDocument2Ptr pXMLDoc, CVelMod::PSEUDOWELL *well) {

	MSXML::IXMLDOMElementPtr pElem = pXMLDoc->createElement(_T("pseudoWell"));

	pElem->setAttribute(_T("title"), _variant_t(well->title));
	pElem->setAttribute(_T("x"), _variant_t(well->wellDist));
	pElem->setAttribute(_T("lon"), _variant_t(well->wellLatLong[1]));
	pElem->setAttribute(_T("lat"), _variant_t(well->wellLatLong[0]));
	CString t = CARRAGE + well->wellText + CARRAGE;
	pElem->Puttext(_bstr_t(t));
	MSXML::IXMLDOMElementPtr pXMLRootElem = pXMLDoc->GetdocumentElement();
	pXMLRootElem->appendChild(pElem);
}

void setModelDeployOld(MSXML::IXMLDOMDocument2Ptr pXMLDoc, CVelMod::DEPLOY *deploy) {
	MSXML::IXMLDOMElementPtr pElem = pXMLDoc->createElement(_T("deploy"));

	pElem->setAttribute(_T("title"), _variant_t(deploy->metaTitle));

	MSXML::IXMLDOMElementPtr p = pXMLDoc->createElement(_T("dist"));
	p->Puttext(_bstr_t(deploy->deployDist));
	pElem->appendChild(p);

	p = pXMLDoc->createElement(_T("deployDepth"));
	p->Puttext(_bstr_t(deploy->deployDepth));
	pElem->appendChild(p);


	p = pXMLDoc->createElement(_T("deployLon"));
	p->Puttext(_bstr_t(deploy->deployLatLong[1]));
	pElem->appendChild(p);
	p = pXMLDoc->createElement(_T("deployLat"));
	p->Puttext(_bstr_t(deploy->deployLatLong[0]));
	pElem->appendChild(p);

	p = pXMLDoc->createElement(_T("startLon"));
	p->Puttext(_bstr_t(deploy->startLatLong[1]));
	pElem->appendChild(p);
	p = pXMLDoc->createElement(_T("startLat"));
	p->Puttext(_bstr_t(deploy->startLatLong[0]));
	pElem->appendChild(p);

	p = pXMLDoc->createElement(_T("endLon"));
	p->Puttext(_bstr_t(deploy->endLatLong[1]));
	pElem->appendChild(p);
	p = pXMLDoc->createElement(_T("endLat"));
	p->Puttext(_bstr_t(deploy->endLatLong[0]));
	pElem->appendChild(p);

	MSXML::IXMLDOMElementPtr pXMLRootElem = pXMLDoc->GetdocumentElement();
	pXMLRootElem->appendChild(pElem);
}



bool CVelMod::saveModelsNav2Txt(CString inFileNames[], int nf, CString newName)
{
	newName = SetFileExtension(newName, _T("txt"));
	CStringArray sa;
	sa.Add(_bstr_t("#start end long & lat for many vxml models"));

	CString tStr, tStr2;
	tStr.Format(_T("#PWELL_DIST is pseudo_Well's model distance(km), while PWELL_OFFSET is its computed offset from the drop location. Model is 2D while seismic shooting line may not be perfectly straight."));
	sa.Add(tStr);
	tStr.Format(_T("#DROP_LON    DROP_LAT   START_LON    START_LAT  END_LON      END_LAT    PWELL_LON    PWELL_LAT PWELL_DIST(PWELL_OFFSET) TITLE"));
	sa.Add(tStr);
	CVelMod vm;
	for (int n = 0; n < nf; n++) {
		CVelMod::VELMOD velm;
		if (isFileExistNotDirectory(inFileNames[n]) &&
			vm.loadInitVel(&velm, inFileNames[n], 0.004, tStr)) {
			// tStr.Format(_T("%1.1f"), x);

			register double x0 = velm.deploys[0].deployDist;
			register double x1 = velm.deploys[0].startDist;
			register double x2 = velm.deploys[0].endDist;
			if (velm.deploys[0].startDist == ERRORVAL) {
				continue;
			}
			for (int m = 0; m < TX_SHOTS_MAX; m++) {
				register double wellDist = velm.wells[m].wellDist; 
				register double wellLon;
				register double wellLat;
				register double distCalc;
				if (isFloatEqual(wellDist, (double)ERRORVAL)) {
					if (m>0) break;
					wellDist = x0;
					wellLon = velm.deploys[0].deployLatLong[1];
					wellLat = velm.deploys[0].deployLatLong[0];
					distCalc = 0.; // offset is zero if deploy position is used for pseudowell.
				}
				else {
					wellLon = velm.wells[0].wellLatLong[1];
					wellLat = velm.wells[0].wellLatLong[0];
					distCalc = calcDist(wellLon, wellLat,
						velm.deploys[0].deployLatLong[1],
						velm.deploys[0].deployLatLong[0]);
				}

				//if (m > 0) {
				//	tStr2.Format(_T("%s(Deploy%d)"), velm.deploys[0].metaTitle, m + 1);
				//}
				//else
				tStr2 = velm.deploys[0].metaTitle;
				
				if (fabs(x1 - x0) >= fabs(x2 - x0)) {
					CString tStr3;
					tStr3.Format(_T("(Drop %g, Start %g, end %g km)"), x0, x1, x2);
					tStr2 += tStr3;
				}
				tStr.Format(
					_T("%4.5f   %4.5f   %4.5f   %4.5f   %4.5f   %4.5f   %4.5f   %4.5f   %4.5f  (%4.5f)     %s"),
					velm.deploys[0].deployLatLong[1],
					velm.deploys[0].deployLatLong[0],
					velm.deploys[0].startLatLong[1],
					velm.deploys[0].startLatLong[0],
					velm.deploys[0].endLatLong[1],
					velm.deploys[0].endLatLong[0],
					wellLon,
					wellLat,
					wellDist,
					distCalc,
					tStr2
				);
				sa.Add(tStr);
			} // for m
		}
		//vm.velModFreeArrays(&velm);
	} // for n
	bool isTrue = saveStringArrayToFile(&sa, newName);
	sa.RemoveAll();
	return isTrue;
}

bool CVelMod::saveModelsNav2TxtWally(CString inFileNames[], int nf, CString newName)
{
	newName = SetFileExtension(newName, _T("txt"));
	CStringArray sa;

	CString tStr, tStr2;
	tStr.Format(_T("Sonobuoy,Longitude,Latitute,Status"));
	sa.Add(tStr);
	CVelMod vm;
	for (int n = 0; n < nf; n++) {
		CVelMod::VELMOD velm;
		if (isFileExistNotDirectory(inFileNames[n]) &&
			vm.loadInitVel(&velm, inFileNames[n], 0.004, tStr)) {
			// tStr.Format(_T("%1.1f"), x);

			for (int m = 0; m < TX_SHOTS_MAX; m++) {
				if (velm.deploys[m].startDist == ERRORVAL) {
					break;
				}
				//double wellDist = velm.wells[m].wellDist != ERRORVAL ? velm.wells[m].wellDist : velm.wells[0].wellDist;
				//double wellLon = velm.wells[m].wellDist != ERRORVAL ? velm.wells[m].wellLatLong[1] : velm.wells[0].wellLatLong[1];
				//double wellLat = velm.wells[m].wellDist != ERRORVAL ? velm.wells[m].wellLatLong[0] : velm.wells[0].wellLatLong[0];

				//// catch a situ where no pseudo-wells existing
				//if (isFloatEqual(wellDist, (double)ERRORVAL)) {
				//	wellDist = x0;
				//	wellLon = velm.deploys[0].deployLatLong[1];
				//	wellLat = velm.deploys[0].deployLatLong[0];
				//}


				if (m > 0) {
					tStr2.Format(_T("%s(Deploy%d)"), velm.deploys[m].metaTitle, m + 1);
				}
				else
					tStr2 = velm.deploys[m].metaTitle;

				tStr.Format(_T("%s,%4.5f,%4.5f,Drop"), tStr2, velm.deploys[m].deployLatLong[1], velm.deploys[m].deployLatLong[0]);
				sa.Add(tStr);

				tStr.Format(_T("%s,%4.5f,%4.5f,Start"), tStr2, velm.deploys[m].startLatLong[1], velm.deploys[m].startLatLong[0]);
				sa.Add(tStr);

				tStr.Format(_T("%s,%4.5f,%4.5f,End"), tStr2, velm.deploys[m].endLatLong[1], velm.deploys[m].endLatLong[0]);
				sa.Add(tStr);
			} // for m
		}
		//vm.velModFreeArrays(&velm);
	} // for n
	bool isTrue = saveStringArrayToFile(&sa, newName);
	sa.RemoveAll();
	return isTrue;
}

void CVelMod::appendPlacemarkOld(MSXML::IXMLDOMDocument2Ptr pXMLDoc, MSXML::IXMLDOMElementPtr pElem, VELMOD *velm)
{
	MSXML::IXMLDOMElementPtr p = pXMLDoc->createElement(_T("Placemark"));
	p->setAttribute(_T("id"), _variant_t(velm->deploys[0].metaTitle));
	pElem->appendChild(p);

	MSXML::IXMLDOMElementPtr p2, p3;

	p2 = pXMLDoc->createElement(_T("name"));
	p2->Puttext(_bstr_t(velm->deploys[0].metaTitle));
	p->appendChild(p2);

	p2 = pXMLDoc->createElement(_T("description"));
	p2->Puttext(_bstr_t(velm->deploys[0].metaTitle));
	p->appendChild(p2);

	p2 = pXMLDoc->createElement(_T("Point"));
	p->appendChild(p2);

	p3 = pXMLDoc->createElement(_T("altitudeMode"));
	p3->Puttext(_bstr_t("relativeToGround"));
	p2->appendChild(p3);

	
	p3 = pXMLDoc->createElement(_T("coordinates"));
	p3->Puttext(
		_bstr_t(velm->deploys[0].deployLatLong[1]) +
		_bstr_t(",") +
		_bstr_t(velm->deploys[0].deployLatLong[0]) +
		_bstr_t(",") +
		_bstr_t(velm->deploys[0].deployDepth)
		);
	p2->appendChild(p3);

	p2 = pXMLDoc->createElement(_T("LineString"));
	p->appendChild(p2);

	p3 = pXMLDoc->createElement(_T("coordinates"));
	p3->Puttext(
		_bstr_t("\n\t\t\t\t") +
		_bstr_t(velm->deploys[0].startLatLong[1]) +
		_bstr_t(",") +
		_bstr_t(velm->deploys[0].startLatLong[0]) +
		_bstr_t("\n\t\t\t\t") +
		_bstr_t(velm->deploys[0].endLatLong[1]) +
		_bstr_t(",") +
		_bstr_t(velm->deploys[0].endLatLong[0]) +
		_bstr_t("\n\t\t\t\t")
		);
	p2->appendChild(p3);
	// end </LineString>
	// end </Placemark>
}

void CVelMod::appendPlacemarkLine(MSXML::IXMLDOMDocument2Ptr pXMLDoc, MSXML::IXMLDOMElementPtr pElem, 
	VELMOD *velm, CString aStyle)
{
	MSXML::IXMLDOMElementPtr p0, p2, p3;

	//appendPlacemarkPoint(pXMLDoc, pElem, 
	//	velm->deploys[0].deployLatLong[1], 
	//	velm->deploys[0].deployLatLong[0],
	//	velm->deploys[0].deployDepth, 
	//	velm->deploys[0].metaTitle);

	p0 = pXMLDoc->createElement(_T("Placemark"));
//	p0->setAttribute(_T("id"), _variant_t(velm->deploys[0].metaTitle));
	pElem->appendChild(p0);

	int count = 0;
	for (int m = 0; m < TX_SHOTS_MAX; m++) {
		if (velm->deploys[m].startDist == ERRORVAL) {
			break;
		}
		count++;
	}

	p2 = pXMLDoc->createElement(_T("name"));
	CString tStr = velm->deploys[0].metaTitle;
	if (count > 1) tStr += _T("_") + velm->deploys[count-1].metaTitle;
	p2->Puttext(_bstr_t(tStr));
	p0->appendChild(p2);

	p2 = pXMLDoc->createElement(_T("description"));
	p2->Puttext(_bstr_t("Start-End"));
	p0->appendChild(p2);

	p2 = pXMLDoc->createElement(_T("styleUrl"));
	p2->Puttext(_bstr_t(aStyle));
	p0->appendChild(p2);


	for (int m = 0; m < count; m++) {
		// double wellDist = velm->wells[m].wellDist != ERRORVAL ? velm->wells[m].wellDist : velm->wells[0].wellDist;
		p2 = pXMLDoc->createElement(_T("LineString"));
		p0->appendChild(p2);

		p3 = pXMLDoc->createElement(_T("coordinates"));

		p3->Puttext(
			_bstr_t("\n\t\t\t\t") +
			_bstr_t(velm->deploys[m].startLatLong[1]) +
			_bstr_t(",") +
			_bstr_t(velm->deploys[m].startLatLong[0]) +
			_bstr_t("\n\t\t\t\t") +
			_bstr_t(velm->deploys[m].endLatLong[1]) +
			_bstr_t(",") +
			_bstr_t(velm->deploys[m].endLatLong[0]) +
			_bstr_t("\n\t\t\t\t")
		);
		p2->appendChild(p3);
	}
	// end </LineString>
	// end </Placemark>
}


bool CVelMod::saveModel2Kml(CString inFileNames[], int nf, CString newName)
{
	newName = SetFileExtension(newName, _T("kml"));
	MSXML::IXMLDOMDocument2Ptr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		AfxMessageBox(_T("Failed to create the XML class instance"));
		return false;
	}

	if (pXMLDoc->loadXML(_T("<kml></kml>")) == VARIANT_FALSE) {
		ShowError(pXMLDoc);
		return false;
	}

	//Get the root element just created	
	MSXML::IXMLDOMElementPtr pXMLRootElem = pXMLDoc->GetdocumentElement();
	MSXML::IXMLDOMProcessingInstructionPtr pXMLProcessingNode =
		pXMLDoc->createProcessingInstruction("xml", " version='1.0' encoding='UTF-8'");

	pXMLRootElem->setAttribute(_T("xmlns"), _variant_t("http://www.opengis.net/kml/2.2"));
	MSXML::IXMLDOMElementPtr pElem = pXMLDoc->createElement(_T("Document"));
	pXMLRootElem->appendChild(pElem);
	MSXML::IXMLDOMElementPtr p = pXMLDoc->createElement(_T("name"));
	p->Puttext(_bstr_t("KmlFile for Seiswide's vxml"));
	pElem->appendChild(p);
	p = pXMLDoc->createElement(_T("description"));
	p->Puttext(_bstr_t("This file contains the navigation information..."));
	pElem->appendChild(p);

	CString aStyle = _T("_sb");
	appendPlacemarkStyle(pXMLDoc, pElem, aStyle, 1.1);
	aStyle = _T("#m") + aStyle;

	CVelMod vm;
	CString tStr;

	MSXML::IXMLDOMElementPtr pFolder;
	pFolder = pXMLDoc->createElement(_T("Folder"));
	p = pXMLDoc->createElement(_T("name"));
	p->Puttext(_bstr_t("StartEnd"));
	pFolder->appendChild(p);
	for (int n = 0; n < nf; n++) {
		CVelMod::VELMOD velm;
		if (isFileExistNotDirectory(inFileNames[n]) &&
			vm.loadInitVel(&velm, inFileNames[n], 0.004, tStr)) {
			CVelMod::appendPlacemarkLine(pXMLDoc, pFolder, &velm, aStyle);
		}
		//vm.velModFreeArrays(&velm);
	} // for n
	pElem->appendChild(pFolder);

	pFolder = pXMLDoc->createElement(_T("Folder"));
	p = pXMLDoc->createElement(_T("name"));
	p->Puttext(_bstr_t("Drop"));
	pFolder->appendChild(p);
	for (int n = 0; n < nf; n++) {
		CVelMod::VELMOD velm;
		if (isFileExistNotDirectory(inFileNames[n]) &&
			vm.loadInitVel(&velm, inFileNames[n], 0.004, tStr)) {
			for (int m = 0; m < TX_SHOTS_MAX; m++) {
				if (velm.deploys[m].startDist == ERRORVAL) {
					break;
				}
				appendPlacemarkPoint(pXMLDoc, pFolder,
					velm.deploys[m].deployLatLong[1],
					velm.deploys[m].deployLatLong[0],
					velm.deploys[m].deployDepth,
					velm.deploys[m].metaTitle, aStyle);
			}
		}
		//vm.velModFreeArrays(&velm);
	} // for n
	pElem->appendChild(pFolder);


	// Format the XML. This requires a style sheet
	MSXML::IXMLDOMDocument2Ptr loadXML;
	hr = loadXML.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		ShowError(loadXML);
		return false;
	}

	//We need to load the style sheet which will be used to indent the XMl properly.
	CString xslFile = getSeisWideParaPath() + _T("\\StyleSheet.xsl");
	if (loadXML->load(variant_t(xslFile)) == VARIANT_FALSE) {
		ShowError(loadXML);
		return false;
	}

	// now save to file
	//Create the final document which will be indented properly
	MSXML::IXMLDOMDocument2Ptr pXMLFormattedDoc;
	hr = pXMLFormattedDoc.CreateInstance(__uuidof(DOMDocument60));

	CComPtr<IDispatch> pDispatch;
	hr = pXMLFormattedDoc->QueryInterface(IID_IDispatch, (void**)&pDispatch);
	if (FAILED(hr)) {
		return false;
	}

	_variant_t	vtOutObject;
	vtOutObject.vt = VT_DISPATCH;
	vtOutObject.pdispVal = pDispatch;
	vtOutObject.pdispVal->AddRef();

	//Apply the transformation to format the final document	
	hr = pXMLDoc->transformNodeToObject(loadXML, vtOutObject);

	//By default it is writing the encoding = UTF-16. Let us change the encoding to UTF-8
	MSXML::IXMLDOMNodePtr pXMLFirstChild = pXMLFormattedDoc->GetfirstChild();				// <?xml version="1.0" encoding="UTF-8"?>
	MSXML::IXMLDOMNamedNodeMapPtr pXMLAttributeMap = pXMLFirstChild->Getattributes();		// A map of the a attributes (vesrsion, encoding) values (1.0, UTF-8) pair
	MSXML::IXMLDOMNodePtr pXMLEncodNode = pXMLAttributeMap->getNamedItem(_T("encoding"));
	pXMLEncodNode->PutnodeValue(_T("UTF-8"));												//encoding = UTF-8

	hr = pXMLFormattedDoc->save(newName.AllocSysString());
	if (FAILED(hr)) {
		ShowError(pXMLFormattedDoc);
	}

	//	showFileInNotePad(newName);
	return true;
}


// this function will only use the models that have velocities inside [vmin, vmax]
// arrTxt must already be created
int CVelMod::saveModel2Kml(CString inFileNames[], int nf, CString newName, CStringArray* arrTxt, double vmin, double vmax)
{
	newName = SetFileExtension(newName, _T("kml"));
	MSXML::IXMLDOMDocument2Ptr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		AfxMessageBox(_T("Failed to create the XML class instance"));
		return 0;
	}

	if (pXMLDoc->loadXML(_T("<kml></kml>")) == VARIANT_FALSE) {
		ShowError(pXMLDoc);
		return 0;
	}

	//Get the root element just created	
	MSXML::IXMLDOMElementPtr pXMLRootElem = pXMLDoc->GetdocumentElement();
	MSXML::IXMLDOMProcessingInstructionPtr pXMLProcessingNode =
		pXMLDoc->createProcessingInstruction("xml", " version='1.0' encoding='UTF-8'");

	pXMLRootElem->setAttribute(_T("xmlns"), _variant_t("http://www.opengis.net/kml/2.2"));
	MSXML::IXMLDOMElementPtr pElem = pXMLDoc->createElement(_T("Document"));
	pXMLRootElem->appendChild(pElem);
	MSXML::IXMLDOMElementPtr p = pXMLDoc->createElement(_T("name"));
	p->Puttext(_bstr_t("KmlFile for Seiswide's vxml"));
	pElem->appendChild(p);
	p = pXMLDoc->createElement(_T("description"));
	p->Puttext(_bstr_t("This file contains the navigation information..."));
	pElem->appendChild(p);

	CString aStyle = _T("_sb");
	appendPlacemarkStyle(pXMLDoc, pElem, aStyle, 1.1);
	aStyle = _T("#m") + aStyle;

	CVelMod vm;
	CString tStr;

	MSXML::IXMLDOMElementPtr pFolder;
	pFolder = pXMLDoc->createElement(_T("Folder"));
	p = pXMLDoc->createElement(_T("name"));
	p->Puttext(_bstr_t("StartEnd"));
	pFolder->appendChild(p);
	CVelMod::VELMOD velm;
	double v;
	int ilayer;

	pFolder = pXMLDoc->createElement(_T("Folder"));
	p = pXMLDoc->createElement(_T("name"));
	p->Puttext(_bstr_t("Drop"));
	pFolder->appendChild(p);

	tStr.Format(_T("#LON          LAT          Zsf        Zbsf         STATION   VTopRange"));
	arrTxt->Add(tStr);
	int count = 0;
	VELMOD_X modX;
	for (int n = 0; n < nf; n++) {
		if (isFileExistNotDirectory(inFileNames[n]) &&
			vm.loadInitVel(&velm, inFileNames[n], 0.004, tStr)) {
			register CString modelNameBase = GetBaseFileName(inFileNames[n]);
			int mSel = 0;
			for (int m = 0; m < TX_SHOTS_MAX; m++) {
				if (velm.deploys[m].startDist == ERRORVAL) {
					break;
				}
				modX.dist = velm.deploys[m].deployDist;
				if (modX.dist < velm.getXmin() || modX.dist > velm.getXmax()) continue;
				vm.initModX(&velm, &modX, false, true);
				
				mSel = m;
				register bool isFound = false;
				for (ilayer = 0; ilayer < velm.ncont - 1; ilayer++) {
					// we want to make sure this layer's velocity is controled
					if (ilayer > velm.ncontMaxConstrained - 1) break;
					if (ilayer == velm.ncontMaxConstrained - 1 && !velm.isVelBelowValid) break;

					// now proceed
					//v = (modX.vTop[ilayer] + modX.vBot[ilayer]) * 0.5;
					v = modX.vTop[ilayer];
					if (isInsideDouble(v, vmin, vmax)) {
						appendPlacemarkPoint(pXMLDoc, pFolder,
							velm.deploys[mSel].deployLatLong[1],
							velm.deploys[mSel].deployLatLong[0],
							velm.deploys[mSel].deployDepth,
							velm.deploys[mSel].metaTitle, aStyle);
						tStr.Format(_T("%10.4f %10.4f  %10.4f  %10.4f km   %s : %g-%g km/s"),
							velm.deploys[mSel].deployLatLong[1],
							velm.deploys[mSel].deployLatLong[0],
							modX.getWaterDepth(),
							modX.depthMod[ilayer] - modX.getWaterDepth(),
							modelNameBase,
							modX.vTop[ilayer],
							modX.vBot[ilayer]);
						arrTxt->Add(tStr);
						count++;
						isFound = true;
						break;
					}
				} // for ilayer
				if (!isFound) {
					// we output an empty space in text format, which is good excel processing
					tStr.Format(_T("%10.4f %10.4f  %10.4f  %10.4f km   %s : %g-%g km/s"),
						velm.deploys[mSel].deployLatLong[1],
						velm.deploys[mSel].deployLatLong[0],
						modX.getWaterDepth(),
						ERRORVAL,
						modelNameBase,
						vmin,
						vmax);
					arrTxt->Add(tStr);
				}
			} // for m
			velm.velModReset();
		}
	} // for n
	pElem->appendChild(pFolder);



	// Format the XML. This requires a style sheet
	MSXML::IXMLDOMDocument2Ptr loadXML;
	hr = loadXML.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		ShowError(loadXML);
		return 0;
	}

	//We need to load the style sheet which will be used to indent the XMl properly.
	CString xslFile = getSeisWideParaPath() + _T("\\StyleSheet.xsl");
	if (loadXML->load(variant_t(xslFile)) == VARIANT_FALSE) {
		ShowError(loadXML);
		return 0;
	}

	// now save to file
	//Create the final document which will be indented properly
	MSXML::IXMLDOMDocument2Ptr pXMLFormattedDoc;
	hr = pXMLFormattedDoc.CreateInstance(__uuidof(DOMDocument60));

	CComPtr<IDispatch> pDispatch;
	hr = pXMLFormattedDoc->QueryInterface(IID_IDispatch, (void**)&pDispatch);
	if (FAILED(hr)) {
		return 0;
	}

	_variant_t	vtOutObject;
	vtOutObject.vt = VT_DISPATCH;
	vtOutObject.pdispVal = pDispatch;
	vtOutObject.pdispVal->AddRef();

	//Apply the transformation to format the final document	
	hr = pXMLDoc->transformNodeToObject(loadXML, vtOutObject);

	//By default it is writing the encoding = UTF-16. Let us change the encoding to UTF-8
	MSXML::IXMLDOMNodePtr pXMLFirstChild = pXMLFormattedDoc->GetfirstChild();				// <?xml version="1.0" encoding="UTF-8"?>
	MSXML::IXMLDOMNamedNodeMapPtr pXMLAttributeMap = pXMLFirstChild->Getattributes();		// A map of the a attributes (vesrsion, encoding) values (1.0, UTF-8) pair
	MSXML::IXMLDOMNodePtr pXMLEncodNode = pXMLAttributeMap->getNamedItem(_T("encoding"));
	pXMLEncodNode->PutnodeValue(_T("UTF-8"));												//encoding = UTF-8

	hr = pXMLFormattedDoc->save(newName.AllocSysString());
	if (FAILED(hr)) {
		ShowError(pXMLFormattedDoc);
	}

	//	showFileInNotePad(newName);
	return count;
}

int CVelMod::saveModel2KmlMoho(CString inFileNames[], int nf, CString newName, CStringArray* arrTxt, double vmin)
{
	newName = SetFileExtension(newName, _T("kml"));
	MSXML::IXMLDOMDocument2Ptr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		AfxMessageBox(_T("Failed to create the XML class instance"));
		return 0;
	}

	if (pXMLDoc->loadXML(_T("<kml></kml>")) == VARIANT_FALSE) {
		ShowError(pXMLDoc);
		return 0;
	}

	//Get the root element just created	
	MSXML::IXMLDOMElementPtr pXMLRootElem = pXMLDoc->GetdocumentElement();
	MSXML::IXMLDOMProcessingInstructionPtr pXMLProcessingNode =
		pXMLDoc->createProcessingInstruction("xml", " version='1.0' encoding='UTF-8'");

	pXMLRootElem->setAttribute(_T("xmlns"), _variant_t("http://www.opengis.net/kml/2.2"));
	MSXML::IXMLDOMElementPtr pElem = pXMLDoc->createElement(_T("Document"));
	pXMLRootElem->appendChild(pElem);
	MSXML::IXMLDOMElementPtr p = pXMLDoc->createElement(_T("name"));
	p->Puttext(_bstr_t("Moho Constrained"));
	pElem->appendChild(p);
	p = pXMLDoc->createElement(_T("description"));
	p->Puttext(_bstr_t("This file contains the navigation information..."));
	pElem->appendChild(p);

	CString aStyle = _T("_sb");
	appendPlacemarkStyle(pXMLDoc, pElem, aStyle, 1.1);
	aStyle = _T("#m") + aStyle;

	CVelMod vm;
	CString tStr;

	MSXML::IXMLDOMElementPtr pFolder;
	pFolder = pXMLDoc->createElement(_T("Folder"));
	p = pXMLDoc->createElement(_T("name"));
	p->Puttext(_bstr_t("StartEnd"));
	pFolder->appendChild(p);
	CVelMod::VELMOD velm;
	double v;
	int ilayer;

	pFolder = pXMLDoc->createElement(_T("Folder"));
	p = pXMLDoc->createElement(_T("name"));
	p->Puttext(_bstr_t("Moho"));
	pFolder->appendChild(p);
	tStr.Format(_T("#LON          LAT          Zsf        Zbsf         STATION   VBelowMoho"));
	arrTxt->Add(tStr);
	int count = 0;
	VELMOD_X modX;
	for (int n = 0; n < nf; n++) {
		if (isFileExistNotDirectory(inFileNames[n]) &&
			vm.loadInitVel(&velm, inFileNames[n], 0.004, tStr)) {
			register CString modelNameBase = GetBaseFileName(inFileNames[n]);
			int mSel = 0;
			for (int m = 0; m < TX_SHOTS_MAX; m++) {
				if (velm.deploys[m].startDist == ERRORVAL) {
					break;
				}
				modX.dist = velm.deploys[m].deployDist;
				if (modX.dist < velm.getXmin() || modX.dist > velm.getXmax()) continue;
				vm.initModX(&velm, &modX, false, true);
				mSel = m;

				register bool isFound = false;
				for (ilayer = 0; ilayer < velm.ncont - 1; ilayer++) {
					// we want to make sure this layer's velocity is controled
					// if (ilayer < velm.ncontMaxConstrained - 1) continue;
					if (ilayer > velm.ncontMaxConstrained - 1) break;

					// now proceed
					v = modX.vTop[ilayer];
					if (v > vmin) {
						appendPlacemarkPoint(pXMLDoc, pFolder,
							velm.deploys[mSel].deployLatLong[1],
							velm.deploys[mSel].deployLatLong[0],
							velm.deploys[mSel].deployDepth,
							velm.deploys[mSel].metaTitle, aStyle);
						tStr.Format(_T("%10.4f %10.4f  %10.4f  %10.4f km   %s : %g km/s"),
							velm.deploys[mSel].deployLatLong[1],
							velm.deploys[mSel].deployLatLong[0],
							modX.getWaterDepth(),
							modX.depthMod[ilayer] - modX.getWaterDepth(),
							modelNameBase,
							modX.vTop[ilayer]);
						arrTxt->Add(tStr);
						count++;
						isFound = true;
						break;
					}
				} // for ilayer

				if (!isFound) {
					tStr.Format(_T("%10.4f %10.4f  %10.4f  %10.4f km   %s : %g km/s"),
						velm.deploys[mSel].deployLatLong[1],
						velm.deploys[mSel].deployLatLong[0],
						modX.getWaterDepth(),
						ERRORVAL,
						modelNameBase,
						vmin);
					arrTxt->Add(tStr);
				}
			} // for m
			velm.velModReset();
		}
	} // for n
	pElem->appendChild(pFolder);



	// Format the XML. This requires a style sheet
	MSXML::IXMLDOMDocument2Ptr loadXML;
	hr = loadXML.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		ShowError(loadXML);
		return 0;
	}

	//We need to load the style sheet which will be used to indent the XMl properly.
	CString xslFile = getSeisWideParaPath() + _T("\\StyleSheet.xsl");
	if (loadXML->load(variant_t(xslFile)) == VARIANT_FALSE) {
		ShowError(loadXML);
		return 0;
	}

	// now save to file
	//Create the final document which will be indented properly
	MSXML::IXMLDOMDocument2Ptr pXMLFormattedDoc;
	hr = pXMLFormattedDoc.CreateInstance(__uuidof(DOMDocument60));

	CComPtr<IDispatch> pDispatch;
	hr = pXMLFormattedDoc->QueryInterface(IID_IDispatch, (void**)&pDispatch);
	if (FAILED(hr)) {
		return 0;
	}

	_variant_t	vtOutObject;
	vtOutObject.vt = VT_DISPATCH;
	vtOutObject.pdispVal = pDispatch;
	vtOutObject.pdispVal->AddRef();

	//Apply the transformation to format the final document	
	hr = pXMLDoc->transformNodeToObject(loadXML, vtOutObject);

	//By default it is writing the encoding = UTF-16. Let us change the encoding to UTF-8
	MSXML::IXMLDOMNodePtr pXMLFirstChild = pXMLFormattedDoc->GetfirstChild();				// <?xml version="1.0" encoding="UTF-8"?>
	MSXML::IXMLDOMNamedNodeMapPtr pXMLAttributeMap = pXMLFirstChild->Getattributes();		// A map of the a attributes (vesrsion, encoding) values (1.0, UTF-8) pair
	MSXML::IXMLDOMNodePtr pXMLEncodNode = pXMLAttributeMap->getNamedItem(_T("encoding"));
	pXMLEncodNode->PutnodeValue(_T("UTF-8"));												//encoding = UTF-8

	hr = pXMLFormattedDoc->save(newName.AllocSysString());
	if (FAILED(hr)) {
		ShowError(pXMLFormattedDoc);
	}

	//	showFileInNotePad(newName);
	return count;
}

bool CVelMod::saveModelXML_V3(CVelMod::VELMOD *velm, CString newName, double dz, BOOL isTime)
{
	newName = SetFileExtension(newName, _T("vxml"));
	MSXML::IXMLDOMDocument2Ptr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		AfxMessageBox(_T("Failed to create the XML class instance"));
		return false;
	}
	if (pXMLDoc->loadXML(_T("<velMod></velMod>")) == VARIANT_FALSE) {
		ShowError(pXMLDoc);
		return false;
	}

	const CString cstrComment = _T("Draft XML format Version 3.1 used by SeisWide:\n\t1. Attributes of <velMod> and <layer> elements are optional, although xmin, xmax, zmin and zmax may be used if supplied. numHorizonValid indicates horizons constrained by data. Multiple <deploy> for the model can exist with >2 deployments displayed near the end of file. <pseudoWell> for each deployment incidates an approximate location near the center of raytracing for target layers.\n\t2. Layer number attrbute may be arbitrary integer.\n\t3. Each horizon should have >= 2 points starting from xmin to xmax. Each horizon can be optionally constrained by [xmMin, xmMax], outside which the structure is not controlled by data.\n\t4. Optional attributes of <point> include t(two-way time), lat(latitude), and lon(longitude). Only when z(depth) does not exist, t is required.\n\t5. All the latitude/longitude values may be empty when not available.\n\t6. If the velocities below the max valid horizon are not validated (e.g. Moho without Pn), <topVelocity> can have validated=\"0\" attribute.\n7. For any horizon, to avoid auto sorting, just manually insert a point 0.001 apart in X.");
	MSXML::IXMLDOMNodePtr aNodePtr = pXMLDoc->createComment(_bstr_t(cstrComment));
	pXMLDoc->appendChild(aNodePtr);

	//Get the root element just created	
	MSXML::IXMLDOMElementPtr pXMLRootElem = pXMLDoc->GetdocumentElement();

	MSXML::IXMLDOMProcessingInstructionPtr pXMLProcessingNode =
		pXMLDoc->createProcessingInstruction("xml", " version='1.0' encoding='UTF-8'");
	pXMLRootElem->setAttribute(_T("numHorizon"), _variant_t(velm->ncont));
	pXMLRootElem->setAttribute(_T("numHorizonValid"), _variant_t(velm->ncontMaxConstrained));

	if (velm->nRegressionLayer>0) {
		// note: in XML, regressionLayer starts from 1, 2, 3 ... instead of 0, 1, 2 ...
		if (velm->nRegressionLayer < 2) velm->nRegressionLayer = 0; // velm->nRegressionLayer==1 means water layer which cannot be regressed
		pXMLRootElem->setAttribute(_T("regressionLayer"), _variant_t(velm->nRegressionLayer));
		pXMLRootElem->setAttribute(_T("vzero"), _variant_t(velm->regVZero));
		pXMLRootElem->setAttribute(_T("vinf"), _variant_t(velm->regVInf));
		pXMLRootElem->setAttribute(_T("alpha"), _variant_t(velm->regAlpha));
	}

	//pXMLRootElem->setAttribute(_T("xmin"), _variant_t(velm->xmin));
	//pXMLRootElem->setAttribute(_T("xmax"), _variant_t(velm->xmax));
	pXMLRootElem->setAttribute(_T("zmin"), _variant_t(velm->zmin));
	pXMLRootElem->setAttribute(_T("zmax"), _variant_t(velm->zmax));

	// meta info
	pXMLRootElem->setAttribute(_T("cruise"), _variant_t(velm->metaCruise));
	pXMLRootElem->setAttribute(_T("processor"), _variant_t(velm->metaProcessor));
	pXMLRootElem->setAttribute(_T("company"), _variant_t(velm->metaCompany));

	if (velm->metaDate.IsEmpty()) {
		CTime curTime = CTime::GetCurrentTime();
		velm->metaDate = dateTimeToCString(curTime);
	}
	pXMLRootElem->setAttribute(_T("date"), _variant_t(velm->metaDate));
	
	// we only display the first 2 deployments at the beginning
	for (int i = 0; i < 2; i++) {
		if (velm->deploys[i].deployDist == ERRORVAL) break;
		CString aDeployText = CVelMod::getPseudoWellText(velm, velm->deploys[i].deployDist, dz, isTime == TRUE);
		setModelDeploy5(pXMLDoc, &velm->deploys[i], aDeployText);
	}
	for (int i = 0; i < TX_SHOTS_MAX; i++) {
		if (velm->wells[i].wellDist == ERRORVAL) break;
		if (isFloatEqual3((float)(velm->wells[i].wellDist), (float)velm->deploys[0].deployDist)) continue;
		velm->wells[i].wellText = getPseudoWellText(velm, velm->wells[i].wellDist, dz, isTime==TRUE);
		setModelWell5(pXMLDoc, &velm->wells[i]);
	}

	_variant_t vtObject;
	vtObject.vt = VT_DISPATCH;
	vtObject.pdispVal = pXMLRootElem;
	vtObject.pdispVal->AddRef();
	pXMLDoc->insertBefore(pXMLProcessingNode, vtObject);

	int icont, j;
	for (icont = 0; icont < velm->ncont; icont++) {
		MSXML::IXMLDOMElementPtr pBoundary = pXMLDoc->createElement(_T("layer"));	//Create first child element
		pBoundary->setAttribute(_T("number"), _variant_t(icont + 1));

		if (icont > velm->ncontMaxConstrained - 1) {
			// this layer would be not validated
			pBoundary->setAttribute(_T("validated"), _variant_t(0));
		}


		if (icont < velm->ncont - 1) {
			pBoundary->setAttribute(_T("anisotropy"), _variant_t(velm->anisotropy[icont]));
			pBoundary->setAttribute(_T("poisson"), _variant_t(velm->poisson[icont]));
		}

		pBoundary = pXMLRootElem->appendChild(pBoundary);
		MSXML::IXMLDOMElementPtr pCont = pXMLDoc->createElement(_T("horizon"));
		//if (velm->xmMin[icont] == ERRORVAL) {
		//	velm->xmMin[icont] = velm->getXmin();
		//}
		//if (velm->xmMax[icont] == ERRORVAL) {
		//	velm->xmMax[icont] = velm->getXmax();
		//}
		
		// want to simplify the texts
		if (isFloatEqual(velm->xmMin[icont], velm->getXmin())) {
			velm->xmMin[icont] = ERRORVAL;
			velm->xmMax[icont] = ERRORVAL;
		}

		pCont->setAttribute(_T("xmin"), _variant_t(velm->xmMin[icont]));
		pCont->setAttribute(_T("xmax"), _variant_t(velm->xmMax[icont]));
		pCont = pBoundary->appendChild(pCont);

		for (j = 0; j < velm->nzed[icont]; j++) {
			MSXML::IXMLDOMElementPtr pPoint = pXMLDoc->createElement(_T("point"));
			pPoint = pCont->appendChild(pPoint);
			pPoint->setAttribute(_T("x"), _variant_t(velm->xm[j][icont]));
			pPoint->setAttribute(_T("z"), _variant_t(velm->zm[j][icont]));
			pPoint->setAttribute(_T("t"), _variant_t(velm->tm[j][icont]));
			if (!velm->isNoNodeNav) {
				if (velm->longitude[j][icont] != ERRORVAL)
					pPoint->setAttribute(_T("lon"), _variant_t(myFloat8(velm->longitude[j][icont])));
				if (velm->latitude[j][icont] != ERRORVAL)
					pPoint->setAttribute(_T("lat"), _variant_t(myFloat8(velm->latitude[j][icont])));
			}
		}

		// 
		if (velm->isSedimentRegression(icont)) {
			// now create a comment
			const CString tStr = _T("The velocity of this layer is overwritten by regression formula and varies with sub-seafloor depth. Therefore, <topVelocity> and <bottomVelocity> are for illustration purpose only.");
			MSXML::IXMLDOMNodePtr aNodePtr =
				pXMLDoc->createComment(_bstr_t(tStr));
			pBoundary->appendChild(aNodePtr);
		}



		MSXML::IXMLDOMElementPtr pTop = pXMLDoc->createElement(_T("topVelocity"));
		pTop = pBoundary->appendChild(pTop);
		MSXML::IXMLDOMElementPtr pBottom = pXMLDoc->createElement(_T("bottomVelocity"));
		pBottom = pBoundary->appendChild(pBottom);

		if (icont == velm->ncontMaxConstrained - 1) {
			if (velm->isVelBelowValid) {
				pTop->setAttribute(_T("validated"), _variant_t(1));
			}
			else {
				pTop->setAttribute(_T("validated"), _variant_t(0));
			}
			pBottom->setAttribute(_T("validated"), _variant_t(0));
		}
		for (j = 0; j < velm->nvel[icont][0]; j++) {
			MSXML::IXMLDOMElementPtr pPoint = pXMLDoc->createElement(_T("point"));
			pPoint = pTop->appendChild(pPoint);
			pPoint->setAttribute(_T("x"), _variant_t(velm->xvel[j][icont][0]));
			pPoint->setAttribute(_T("v"), _variant_t(velm->vf[j][icont][0]));
		}
		for (j = 0; j < velm->nvel[icont][1]; j++) {
			MSXML::IXMLDOMElementPtr pPoint = pXMLDoc->createElement(_T("point"));
			pPoint = pBottom->appendChild(pPoint);
			pPoint->setAttribute(_T("x"), _variant_t(velm->xvel[j][icont][1]));
			pPoint->setAttribute(_T("v"), _variant_t(velm->vf[j][icont][1]));
		}
		
		if (icont == velm->ncontMaxConstrained - 1) {
			// now create a comment
			const CString tStr = _T("All following layers are not controled by data, but needed to stay for modeling purposes, and actually they may well be correct.");
			MSXML::IXMLDOMNodePtr aNodePtr =
				pXMLDoc->createComment(_bstr_t(tStr));
			pXMLRootElem->appendChild(aNodePtr);
		}

	}

	// only if there are more than 2 deploys, they will appear at the end of this file
	for (int i = 2; i < TX_SHOTS_MAX; i++) {
		if (velm->deploys[i].deployDist == ERRORVAL) break;
		CString aDeployText = CVelMod::getPseudoWellText(velm, velm->deploys[i].deployDist, dz, isTime == TRUE);
		setModelDeploy5(pXMLDoc, &velm->deploys[i], aDeployText);
	}

	// Format the XML. This requires a style sheet
	MSXML::IXMLDOMDocument2Ptr loadXML;
	hr = loadXML.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		ShowError(loadXML);
		return false;
	}

	//We need to load the style sheet which will be used to indent the XMl properly.
	CString xslFile = getSeisWideParaPath() + _T("\\StyleSheet.xsl");
	if (loadXML->load(variant_t(xslFile)) == VARIANT_FALSE) {
		ShowError(loadXML);
		return false;
	}

	// now save to file
	//Create the final document which will be indented properly
	MSXML::IXMLDOMDocument2Ptr pXMLFormattedDoc;
	hr = pXMLFormattedDoc.CreateInstance(__uuidof(DOMDocument60));

	CComPtr<IDispatch> pDispatch;
	hr = pXMLFormattedDoc->QueryInterface(IID_IDispatch, (void**)&pDispatch);
	if (FAILED(hr)) {
		return false;
	}

	_variant_t	vtOutObject;
	vtOutObject.vt = VT_DISPATCH;
	vtOutObject.pdispVal = pDispatch;
	vtOutObject.pdispVal->AddRef();

	//Apply the transformation to format the final document	
	hr = pXMLDoc->transformNodeToObject(loadXML, vtOutObject);

	//By default it is writing the encoding = UTF-16. Let us change the encoding to UTF-8
	MSXML::IXMLDOMNodePtr pXMLFirstChild = pXMLFormattedDoc->GetfirstChild();				// <?xml version="1.0" encoding="UTF-8"?>
	MSXML::IXMLDOMNamedNodeMapPtr pXMLAttributeMap = pXMLFirstChild->Getattributes();		// A map of the a attributes (vesrsion, encoding) values (1.0, UTF-8) pair
	MSXML::IXMLDOMNodePtr pXMLEncodNode = pXMLAttributeMap->getNamedItem(_T("encoding"));
	pXMLEncodNode->PutnodeValue(_T("UTF-8"));												//encoding = UTF-8

	hr = pXMLFormattedDoc->save(newName.AllocSysString());
	if (FAILED(hr)) {
		ShowError(pXMLFormattedDoc);
	}
	else
		velm->velSecName = newName;  // accept this file
	//	showFileInNotePad(newName);

	velm->modifiedLoaded = getFileModifiedTime(newName);
	return true;
}

bool CVelMod::saveModelXML_V2(CVelMod::VELMOD *velm, CString newName)
{
	static _variant_t strBlank = _T("");
	newName = SetFileExtension(newName, _T("vxml"));
	MSXML::IXMLDOMDocument2Ptr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		AfxMessageBox(_T("Failed to create the XML class instance"));
		return false;
	}
	if (pXMLDoc->loadXML(_T("<velMod></velMod>")) == VARIANT_FALSE) {
		ShowError(pXMLDoc);
		return false;
	}

	const CString cstrComment = _T("Draft XML format Version 2 used by SeisWide:\n\t1. Attributes of <velMod> and <layer> elements are optional, although xmin, xmax, zmin and zmax may be used if supplied. Start/end points refer to xmin/xmax of model and may not represent the cruise directions. numHorizonValid indicates horizons constrained by data. pseudoWellLat/pseudoWellLon incidate an approximate location near the center of raytracing for target layers.\n\t2. Layer number attrbute may be arbitrary integer.\n\t3. Each horizon should have >= 2 points starting from xmin to xmax.\n\t4. Optional attributes of <point> include t(two-way time), lat(latitude), and lon(longitude). Only when z(depth) does not exist, t is required.\n\t5. All the latitude/longitude values may be empty when no reflection data are available.\n\t6. If the velocities below the max valid horizon are not validated (e.g. Moho without Pn), <topVelocity> can have validated=\"0\" attribute.\n\t7. If the model is for >1 deployments, one can add more attributes of <velMod>, e.g. deployDist2/deployLat2/deployLon2/startLat2/startLon2/endLat2/endLon2 et al., and then may use startLat1/startLon1/endLat1/endLon1 for the first deployment.");
	MSXML::IXMLDOMNodePtr aNodePtr = pXMLDoc->createComment(_bstr_t(cstrComment));
	pXMLDoc->appendChild(aNodePtr);

	//Get the root element just created	
	MSXML::IXMLDOMElementPtr pXMLRootElem = pXMLDoc->GetdocumentElement();

	MSXML::IXMLDOMProcessingInstructionPtr pXMLProcessingNode =
		pXMLDoc->createProcessingInstruction("xml", " version='1.0' encoding='UTF-8'");
	pXMLRootElem->setAttribute(_T("numHorizon"), _variant_t(velm->ncont));
	pXMLRootElem->setAttribute(_T("numHorizonValid"), _variant_t(velm->ncontMaxConstrained));

	if (velm->nRegressionLayer> 0) {
		// note: in XML, regressionLayer starts from 1, 2, 3 ... instead of 0, 1, 2 ...
		pXMLRootElem->setAttribute(_T("regressionLayer"), _variant_t(velm->nRegressionLayer));
		pXMLRootElem->setAttribute(_T("vzero"), _variant_t(velm->regVZero));
		pXMLRootElem->setAttribute(_T("vinf"), _variant_t(velm->regVInf));
		pXMLRootElem->setAttribute(_T("alpha"), _variant_t(velm->regAlpha));
	}

	//pXMLRootElem->setAttribute(_T("xmin"), _variant_t(velm->xmin));
	//pXMLRootElem->setAttribute(_T("xmax"), _variant_t(velm->xmax));
	pXMLRootElem->setAttribute(_T("zmin"), _variant_t(velm->zmin));
	pXMLRootElem->setAttribute(_T("zmax"), _variant_t(velm->zmax));

	// meta info
	pXMLRootElem->setAttribute(_T("title"), _variant_t(velm->deploys[0].metaTitle));
	pXMLRootElem->setAttribute(_T("cruise"), _variant_t(velm->metaCruise));
	pXMLRootElem->setAttribute(_T("processor"), _variant_t(velm->metaProcessor));
	pXMLRootElem->setAttribute(_T("company"), _variant_t(velm->metaCompany));

	if (velm->metaDate.IsEmpty()) {
		CTime curTime = CTime::GetCurrentTime();
		velm->metaDate = dateTimeToCString(curTime);
	}
	pXMLRootElem->setAttribute(_T("date"), _variant_t(velm->metaDate));

	pXMLRootElem->setAttribute(_T("deployDist"), _variant_t(velm->deploys[0].deployDist));
	pXMLRootElem->setAttribute(_T("deployLat"), _variant_t(myFloat8(velm->deploys[0].deployLatLong[0])));
	pXMLRootElem->setAttribute(_T("deployLon"), _variant_t(myFloat8(velm->deploys[0].deployLatLong[1])));
	pXMLRootElem->setAttribute(_T("startLat"), _variant_t(myFloat8(velm->deploys[0].startLatLong[0])));
	pXMLRootElem->setAttribute(_T("startLon"), _variant_t(myFloat8(velm->deploys[0].startLatLong[1])));
	pXMLRootElem->setAttribute(_T("endLat"), _variant_t(myFloat8(velm->deploys[0].endLatLong[0])));
	pXMLRootElem->setAttribute(_T("endLon"), _variant_t(myFloat8(velm->deploys[0].endLatLong[1])));


	//pXMLRootElem->setAttribute(_T("pseudoWellLat"), _variant_t(myFloat8(velm->wellLatLong[0])));
	//pXMLRootElem->setAttribute(_T("pseudoWellLon"), _variant_t(myFloat8(velm->wellLatLong[1])));
	//pXMLRootElem->setAttribute(_T("pseudoWellDist"), _variant_t(myFloat8(velm->wellDist)));

	_variant_t vtObject;
	vtObject.vt = VT_DISPATCH;
	vtObject.pdispVal = pXMLRootElem;
	vtObject.pdispVal->AddRef();
	pXMLDoc->insertBefore(pXMLProcessingNode, vtObject);


	// pXMLRootElem->appendChild(aNodePtr);

	int icont, j;
	for (icont = 0; icont < velm->ncont; icont++) {
		MSXML::IXMLDOMElementPtr pBoundary = pXMLDoc->createElement(_T("layer"));	//Create first child element
		pBoundary->setAttribute(_T("number"), _variant_t(icont + 1));

		if (icont > velm->ncontMaxConstrained - 1) {
			// this layer would be not validated
			pBoundary->setAttribute(_T("validated"), _variant_t(0));
		}


		if (icont < velm->ncont - 1)
			pBoundary->setAttribute(_T("anisotropy"), _variant_t(velm->anisotropy[icont]));

		pBoundary = pXMLRootElem->appendChild(pBoundary);
		MSXML::IXMLDOMElementPtr pCont = pXMLDoc->createElement(_T("horizon"));
		pCont = pBoundary->appendChild(pCont);

		for (j = 0; j < velm->nzed[icont]; j++) {
			MSXML::IXMLDOMElementPtr pPoint = pXMLDoc->createElement(_T("point"));
			pPoint = pCont->appendChild(pPoint);
			pPoint->setAttribute(_T("x"), _variant_t(velm->xm[j][icont]));
			pPoint->setAttribute(_T("z"), _variant_t(velm->zm[j][icont]));
			pPoint->setAttribute(_T("t"), _variant_t(velm->tm[j][icont]));
			if (velm->latitude[j][icont] != ERRORVAL)
				pPoint->setAttribute(_T("lat"), _variant_t(myFloat8(velm->latitude[j][icont])));
			if (velm->longitude[j][icont] != ERRORVAL)
				pPoint->setAttribute(_T("lon"), _variant_t(myFloat8(velm->longitude[j][icont])));
		}

		MSXML::IXMLDOMElementPtr pTop = pXMLDoc->createElement(_T("topVelocity"));
		pTop = pBoundary->appendChild(pTop);
		MSXML::IXMLDOMElementPtr pBottom = pXMLDoc->createElement(_T("bottomVelocity"));
		pBottom = pBoundary->appendChild(pBottom);

		if (!velm->isVelBelowValid && icont == velm->ncontMaxConstrained - 1) {
			pTop->setAttribute(_T("validated"), _variant_t(0));
			pBottom->setAttribute(_T("validated"), _variant_t(0));
		}
		for (j = 0; j < velm->nvel[icont][0]; j++) {
			MSXML::IXMLDOMElementPtr pPoint = pXMLDoc->createElement(_T("point"));
			pPoint = pTop->appendChild(pPoint);
			pPoint->setAttribute(_T("x"), _variant_t(velm->xvel[j][icont][0]));
			pPoint->setAttribute(_T("v"), _variant_t(velm->vf[j][icont][0]));
		}
		for (j = 0; j < velm->nvel[icont][1]; j++) {
			MSXML::IXMLDOMElementPtr pPoint = pXMLDoc->createElement(_T("point"));
			pPoint = pBottom->appendChild(pPoint);
			pPoint->setAttribute(_T("x"), _variant_t(velm->xvel[j][icont][1]));
			pPoint->setAttribute(_T("v"), _variant_t(velm->vf[j][icont][1]));
		}

		if (icont == velm->ncontMaxConstrained - 1) {
			// now create a comment
			const CString tStr = _T("All following layers are not controled by data, but needed to stay for modeling purposes, and actually they may well be correct.");
			MSXML::IXMLDOMNodePtr aNodePtr =
				pXMLDoc->createComment(_bstr_t(tStr));
			pXMLRootElem->appendChild(aNodePtr);
		}

	}


	// Format the XML. This requires a style sheet
	MSXML::IXMLDOMDocument2Ptr loadXML;
	hr = loadXML.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		ShowError(loadXML);
		return false;
	}

	//We need to load the style sheet which will be used to indent the XMl properly.
	CString xslFile = getSeisWideParaPath() + _T("\\StyleSheet.xsl");
	if (loadXML->load(variant_t(xslFile)) == VARIANT_FALSE) {
		ShowError(loadXML);
		return false;
	}

	// now save to file
	//Create the final document which will be indented properly
	MSXML::IXMLDOMDocument2Ptr pXMLFormattedDoc;
	hr = pXMLFormattedDoc.CreateInstance(__uuidof(DOMDocument60));

	CComPtr<IDispatch> pDispatch;
	hr = pXMLFormattedDoc->QueryInterface(IID_IDispatch, (void**)&pDispatch);
	if (FAILED(hr)) {
		return false;
	}

	_variant_t	vtOutObject;
	vtOutObject.vt = VT_DISPATCH;
	vtOutObject.pdispVal = pDispatch;
	vtOutObject.pdispVal->AddRef();

	//Apply the transformation to format the final document	
	hr = pXMLDoc->transformNodeToObject(loadXML, vtOutObject);

	//By default it is writing the encoding = UTF-16. Let us change the encoding to UTF-8
	MSXML::IXMLDOMNodePtr pXMLFirstChild = pXMLFormattedDoc->GetfirstChild();				// <?xml version="1.0" encoding="UTF-8"?>
	MSXML::IXMLDOMNamedNodeMapPtr pXMLAttributeMap = pXMLFirstChild->Getattributes();		// A map of the a attributes (vesrsion, encoding) values (1.0, UTF-8) pair
	MSXML::IXMLDOMNodePtr pXMLEncodNode = pXMLAttributeMap->getNamedItem(_T("encoding"));
	pXMLEncodNode->PutnodeValue(_T("UTF-8"));												//encoding = UTF-8

	hr = pXMLFormattedDoc->save(newName.AllocSysString());
	if (FAILED(hr)) {
		ShowError(pXMLFormattedDoc);
	}
	else
		velm->velSecName = newName;  // accept this file
	//	showFileInNotePad(newName);


	return true;
}

// save Zelt's ascii format with 3 decimals
bool CVelMod::saveModelZelt3(CVelMod::VELMOD *velm, CString newName)
{
	//VELMOD velMod = *velm;
	int j, j1, j2, icnt, icont;
	FILE *stream;
	if (_tfopen_s(&stream, newName, _T("w")) != 0)
	{
		AfxMessageBox(_T("The new velocity file was not opened"));
		return false;
	}
	// fseek( stream, 0L, SEEK_SET );
	TCHAR *form = _T("%8.3f");
	for (icont = 0; icont < velm->ncont; icont++)
	{
		j1 = 1;
		j2 = 10;
		while (j1 <= velm->nzed[icont])
		{
			if (j2 >= velm->nzed[icont])
			{
				icnt = 0;
				j2 = velm->nzed[icont];
			}
			else
				icnt = 1;

			if (icont == 0 && j1 == 1)
				_ftprintf_s(stream, _T("%2d "), icont + 1);
			else
				_ftprintf_s(stream, _T("\n%2d "), icont + 1);

			if (icont < velm->ncont - 1)
			{
				for (j = j1; j <= j2; j++)
					_ftprintf_s(stream, form, velm->xm[j - 1][icont]);
				// _ftprintf_s(stream, _T("%8.4f", velm->xm[j-1][icont]);
				_ftprintf_s(stream, _T("\n%2d "), icnt);
				for (j = j1; j <= j2; j++)
					_ftprintf_s(stream, form, velm->zm[j - 1][icont]);
				_ftprintf_s(stream, _T("\n%11d"), 0);
				for (j = j1; j < j2; j++)
					_ftprintf_s(stream, _T("%8d"), 0);
			}
			else
			{ //last boundary can only have one point, required by zelt's program.
				_ftprintf_s(stream, form, velm->xm[velm->nzed[icont] - 1][icont]);
				_ftprintf_s(stream, _T("\n%2d "), 0);
				_ftprintf_s(stream, form, velm->zm[velm->nzed[icont] - 1][icont]);
				//	_ftprintf_s(stream, _T("\n%13d ", 0); // this is required ending, fixed Dec 2006
			}
			j1 += 10;
			j2 += 10;
		}

		if (icont == velm->ncont - 1) break;
		j1 = 1;
		j2 = 10;
		while (j1 <= velm->nvel[icont][0])
		{
			if (j2 >= velm->nvel[icont][0])
			{
				icnt = 0;
				j2 = velm->nvel[icont][0];
			}
			else
				icnt = 1;

			_ftprintf_s(stream, _T("\n%2d "), icont + 1);
			for (j = j1; j <= j2; j++)
				_ftprintf_s(stream, form, velm->xvel[j - 1][icont][0]);
			_ftprintf_s(stream, _T("\n%2d "), icnt);
			for (j = j1; j <= j2; j++)
				_ftprintf_s(stream, form, velm->vf[j - 1][icont][0]);
			_ftprintf_s(stream, _T("\n%11d"), 0);
			for (j = j1; j < j2; j++)
				_ftprintf_s(stream, _T(" %7d"), 0);

			j1 += 10;
			j2 += 10;
		}

		j1 = 1;
		j2 = 10;
		while (j1 <= velm->nvel[icont][1])
		{
			if (j2 >= velm->nvel[icont][1])
			{
				icnt = 0;
				j2 = velm->nvel[icont][1];
			}
			else
				icnt = 1;

			_ftprintf_s(stream, _T("\n%2d "), icont + 1);
			for (j = j1; j <= j2; j++)
				_ftprintf_s(stream, form, velm->xvel[j - 1][icont][1]);
			_ftprintf_s(stream, _T("\n%2d "), icnt);
			for (j = j1; j <= j2; j++)
				_ftprintf_s(stream, form, velm->vf[j - 1][icont][1]);
			_ftprintf_s(stream, _T("\n%11d"), 0);
			for (j = j1; j < j2; j++)
				_ftprintf_s(stream, _T(" %7d"), 0);
			j1 += 10;
			j2 += 10;
		}
	}
	fclose(stream);

	return true;
}


// since the *velm is specified by float, not double, so we just pass float dx to here!
void CVelMod::myModelShiftX(CVelMod::VELMOD *velm, float dx)
{
	int icont, j;
	for (icont = 0; icont < velm->ncont; icont++)
		for (j = 0; j < velm->nzed[icont]; j++)  {
			velm->xm[j][icont] += dx;
			velm->longitude[j][icont] = ERRORVAL;
			velm->latitude[j][icont] = ERRORVAL;
		}
	for (icont = 0; icont < velm->ncont - 1; icont++)
		for (j = 0; j < velm->nvel[icont][0]; j++)
			velm->xvel[j][icont][0] += dx;

	for (icont = 0; icont < velm->ncont - 1; icont++)
		for (j = 0; j<velm->nvel[icont][1]; j++)
			velm->xvel[j][icont][1] += dx;

	//velm->xmin += dx;
	//velm->xmax += dx;
}

// returns the boundary number that was duplicated. both ncont and nlayer will be incremented by 1
// the depth domain will be used first, and then converted to time domain *****CHeck******************
int CVelMod::myModelBoundaryDup(CVelMod::VELMOD *velm, int icontFocus, double dz)
{
	//	if (draw.nodes != 1) return icontFocus;
	float dist[MOD_POINTS_MAX * 2];
	float yNew[MOD_POINTS_MAX * 2];

	int j;
	long count;
	float x;
	//	bool isLastBoundary = icontFocus==velm->ncont-1;


	if (icontFocus == velm->ncont - 1) {
		// last boundary, we have to add a new one first otherwise the points have problem
		velm->ncont++;
		velm->nzed[icontFocus + 1] = velm->nzed[icontFocus];
		for (j = 0; j < velm->nzed[icontFocus]; j++) {
			velm->xm[j][icontFocus + 1] = velm->xm[j][icontFocus];
			velm->zm[j][icontFocus + 1] = velm->zm[j][icontFocus] + 1.f;
		}
		velm->nvel[icontFocus][0] = velm->nvel[icontFocus - 1][0];
		velm->nvel[icontFocus][1] = velm->nvel[icontFocus - 1][1];
		for (j = 0; j < velm->nvel[icontFocus][0]; j++) {
			velm->xvel[j][icontFocus][0] = velm->xvel[j][icontFocus - 1][0];
			velm->vf[j][icontFocus][0] = velm->vf[j][icontFocus - 1][0];
		}
		for (j = 0; j < velm->nvel[icontFocus][1]; j++) {
			velm->xvel[j][icontFocus][1] = velm->xvel[j][icontFocus - 1][1];
			velm->vf[j][icontFocus][1] = velm->vf[j][icontFocus - 1][1];
		}
		velm->anisotropy[icontFocus] = velm->anisotropy[icontFocus - 1];
	}
	else {
		int icontNew = icontFocus + 1;
		for (j = 0; j < velm->nzed[icontFocus]; j++) {
			dist[j] = velm->xm[j][icontFocus];
		}
		count = velm->nzed[icontFocus];
		for (j = 0; j < velm->nzed[icontNew]; j++) {
			x = velm->xm[j][icontNew];
			if (!isValueDefined(x, count, &dist[0])) {
				dist[count] = x;
				count++;
			}
		} // for j
		sortSei(dist, count);
		if (count > MOD_POINTS_MAX) {
			// too many points, chop
			dist[MOD_POINTS_MAX - 1] = dist[count - 1];
			count = MOD_POINTS_MAX;
		}

		// pre-calc the depth array for the new duplicated boundary
		VELMOD_X modX2;
		velm->m_isConvert = false;
		for (j = 0; j < count; j++) {
			modX2.dist = dist[j];
			this->initModX(velm, &modX2, false, true, dz);
			register float deltaz = (float)(modX2.depthMod[icontNew] - modX2.depthMod[icontFocus])*0.5f;
			if (deltaz < 0.f) deltaz = 0.001f;
			yNew[j] = (float)modX2.depthMod[icontFocus] + deltaz;
		}

		// now duplicate one boundary
		velm->ncont++;
		for (int icont = velm->ncont - 1; icont > icontNew; icont--) {
			velm->nzed[icont] = velm->nzed[icont - 1];
			for (j = 0; j < velm->nzed[icont]; j++) {
				velm->xm[j][icont] = velm->xm[j][icont - 1];
				velm->zm[j][icont] = velm->zm[j][icont - 1];
			}
		}

		for (j = 0; j < count; j++) {
			velm->xm[j][icontNew] = dist[j];
			velm->zm[j][icontNew] = yNew[j];
		}
		velm->nzed[icontNew] = count;

		// now assign the velocities for the duplicates
		for (int ilayer = velm->ncont - 2; ilayer > icontFocus; ilayer--) {
			velm->nvel[ilayer][0] = velm->nvel[ilayer - 1][0];
			velm->nvel[ilayer][1] = velm->nvel[ilayer - 1][1];
			for (j = 0; j < velm->nvel[ilayer][0]; j++) {
				velm->xvel[j][ilayer][0] = velm->xvel[j][ilayer - 1][0];
				velm->vf[j][ilayer][0] = velm->vf[j][ilayer - 1][0];
			}
			for (j = 0; j < velm->nvel[ilayer][1]; j++) {
				velm->xvel[j][ilayer][1] = velm->xvel[j][ilayer - 1][1];
				velm->vf[j][ilayer][1] = velm->vf[j][ilayer - 1][1];
			}
			velm->anisotropy[ilayer] = velm->anisotropy[ilayer - 1];
		}

		icontFocus++;
	}





	// recalc the TWTT for velMod
	//CVelMod vm;
	velm->m_isConvert = true;
	this->myModel_Depth2TWTT(velm, dz);

	return icontFocus;
}



bool CVelMod::myModelsMerge(CVelMod::VELMOD *velMod, double shiftKm, CVelMod::VELMOD *velMod2, double dz, double shiftKm2, double vTolerance)
{
	//double vTolerance = 0.05;


	CVelMod vm;
	if (!velMod->isVelModValid() || !velMod2->isVelModValid())
		return false;
	if (velMod2->getXmax() < velMod->getXmax())
		return false;  // the second model has no extra information
	if (dz <= 0 || dz > 10.)
		return false;  // this prevents some unknown situations to cause exceptions

	int icont, i, j;  // common indeces



	// first incorporate the effect of shifting
	if (shiftKm != 0.0) {
		for (icont = 0; icont < velMod->ncont; icont++)
			for (j = 0; j < velMod->nzed[icont]; j++)
				velMod->xm[j][icont] += (float)shiftKm;
		for (i = 0; i < 2; i++)
			for (icont = 0; icont < velMod->ncont - 1; icont++)
				for (j = 0; j < velMod->nvel[icont][i]; j++)
					velMod->xvel[j][icont][i] += (float)shiftKm;
		//velMod->xmin += (float)shiftKm;
		//velMod->xmax += (float)shiftKm;
	} // if

	if (shiftKm2 != 0.0) {
		for (icont = 0; icont < velMod2->ncont; icont++)
			for (j = 0; j < velMod2->nzed[icont]; j++)
				velMod2->xm[j][icont] += (float)shiftKm2;
		for (i = 0; i < 2; i++)
			for (icont = 0; icont < velMod2->ncont - 1; icont++)
				for (j = 0; j < velMod2->nvel[icont][i]; j++)
					velMod2->xvel[j][icont][i] += (float)shiftKm2;
		//velMod2->xmin() += (float)shiftKm2;
		//velMod2->xmax() += (float)shiftKm2;
	} // if

	if (velMod2->getXmin() < velMod->getXmax()) {
		// we need to remove overlapping here
		// this is very rare
		for (icont = 0; icont < velMod2->ncont; icont++)
			for (j = 0; j < velMod2->nzed[icont]; j++)
				if (velMod2->xm[j][icont] < velMod->getXmax())
					velMod2->xm[j][icont] = (float)velMod->getXmax();
		for (i = 0; i < 2; i++)
			for (icont = 0; icont < velMod2->ncont - 1; icont++)
				for (j = 0; j < velMod2->nvel[icont][i]; j++)
					if (velMod2->xvel[j][icont][i] < velMod->getXmax())
						velMod2->xvel[j][icont][i] = (float)velMod->getXmax();
		//velMod2->xmin = velMod->xmax;
		// for simplicity, we do not remove those duplicate points. it's no harm to have them
	}

	if (velMod2->getXmax() - velMod2->getXmin() < 0.001) return false;

	// make both models to have the same number of layers
	if (velMod2->ncont < velMod->ncont) {
		for (icont = velMod2->ncont; icont<velMod->ncont; icont++)
			vm.myModelBoundaryDup(velMod2, velMod2->ncont - 1, dz); // note: now icont points to the newly inserted boundary
		// noote: after this call, velMod2->ncont is incremented already by 1
	}
	else if (velMod2->ncont > velMod->ncont) {
		for (icont = velMod->ncont; icont < velMod2->ncont; icont++)
			vm.myModelBoundaryDup(velMod, velMod->ncont - 1, dz); // note: now icont points to the newly inserted boundary
		// noote: after this call, velMod->ncont is incremented already by 1
	}




	float dist[MOD_POINTS_MAX], depthAtDist[MOD_POINTS_MAX];
	VELMOD_X modX;
	VELMOD_X modX2;
	modX.dist = velMod->getXmax();
	velMod->m_isConvert = false;
	this->initModX(velMod, &modX, false, true, dz);

	velMod2->m_isConvert = false;
	modX2.dist = velMod2->getXmin();
	this->initModX(velMod2, &modX2, false, true, dz);

	for (icont = 1; icont < MOD_LAYER_MAX; icont++) {
		if (icont >= velMod->ncont) break; // velMod->ncont can change inside this loop!
		if (modX.vBot[icont - 1] < 1.6) continue; // seafloor should be kept as is
		//if (velMod->nvel[icont][0] <2 || velMod->nvel[icont][0] > MOD_POINTS_MAX) {
		//	// no more layers should be used
		//	velMod->ncont = icont+1;
		//}
		if (modX.vTop[icont - 1] >= 7.7 && modX2.vTop[icont - 1] >= 7.7) {
			continue; // deeper layers can be treated as the same layer
		}

		if (myFloat(modX.vBot[icont - 1], 1) == myFloat(modX2.vBot[icont - 1], 1) &&
			myFloat(modX.vTop[icont], 1) == myFloat(modX2.vTop[icont], 1))
			continue; // no change if both models have the same velocity around boundary icont

		if (fabs(modX.vBot[icont - 1] - modX2.vBot[icont - 1]) / modX.vBot[icont - 1] < vTolerance &&
			fabs(modX.vTop[icont - 1] - modX2.vTop[icont - 1]) / modX.vTop[icont - 1] < vTolerance) continue;


		if (modX.vBot[icont - 1] < modX2.vBot[icont - 1] * (1. - vTolerance) && modX.vTop[icont - 1] < modX2.vTop[icont - 1] * (1. - vTolerance)) {

			// we need to insert a layer in the model on the right
			for (j = 0; j < velMod2->nzed[icont - 1]; j++)
				dist[j] = velMod2->xm[j][icont - 1];
			long n = velMod2->nzed[icont - 1];
			for (j = 1; j < velMod2->nzed[icont] - 1; j++) {
				dist[n] = velMod2->xm[j][icont];
				n++;
			}
			// n is now the total number of points for vDist[]
			sortSei(dist, n);
			for (j = 0; j < n; j++) {
				depthAtDist[j] = (float)vm.getDepthFromVel(velMod2, (double)dist[j], modX.vBot[icont - 1], icont - 1, 0.5, 0.01);
			} // for j

			vm.myModelBoundaryDup(velMod2, icont - 1, dz); // note: now icont points to the newly inserted boundary
			velMod2->nzed[icont] = n;
			for (j = 0; j < n; j++) {
				velMod2->xm[j][icont] = dist[j];
				velMod2->zm[j][icont] = depthAtDist[j];
			}

			// the layer above the new boundary is icont-1, and the one below is icont.
			// we need to assign velocities for both of them
			for (j = 0; j < velMod2->nvel[icont - 1][0]; j++) {
				velMod2->vf[j][icont - 1][0] = (float)modX.vTop[icont - 1];
			}
			for (j = 0; j < velMod2->nvel[icont - 1][1]; j++) {
				velMod2->vf[j][icont - 1][1] = (float)modX.vBot[icont - 1];
			}
			vm.getZModFromZ(velMod2, &modX2, true);
			// we have to redo the depth2[], vTop2, bBot2 arrays since the model is changed
		}
		else if (modX.vBot[icont - 1] * (1. - vTolerance) > modX2.vBot[icont - 1] && modX.vTop[icont - 1] * (1. - vTolerance) > modX2.vTop[icont - 1]) {
			// we need to insert a layer in the model on the left
			for (j = 0; j < velMod->nzed[icont - 1]; j++)
				dist[j] = velMod->xm[j][icont - 1];
			long n = velMod->nzed[icont - 1];
			for (j = 1; j < velMod->nzed[icont] - 1; j++) {
				dist[n] = velMod->xm[j][icont];
				n++;
			}
			// n is now the total number of points for vDist[]
			sortSei(dist, n);
			for (j = 0; j < n; j++) {
				depthAtDist[j] = (float)vm.getDepthFromVel(velMod, (double)dist[j], modX2.vBot[icont - 1], icont - 1, 0.5, 0.01);
			}

			vm.myModelBoundaryDup(velMod, icont - 1, dz); // note: now icont points to the newly inserted boundary
			velMod->nzed[icont] = n;
			for (j = 0; j < n; j++) {
				velMod->xm[j][icont] = dist[j];
				velMod->zm[j][icont] = depthAtDist[j];
			}
			// the layer above the new boundary is icont-1, and the one below is icont.
			// we need to assign velocities for both of them
			for (j = 0; j < velMod->nvel[icont - 1][0]; j++) {
				velMod->vf[j][icont - 1][0] = (float)modX2.vTop[icont - 1];
			}
			for (j = 0; j < velMod->nvel[icont - 1][1]; j++) {
				velMod->vf[j][icont - 1][1] = (float)modX2.vBot[icont - 1];
			}
			vm.getZModFromZ(velMod, &modX, true);
			// we have to redo the depth[], vTop, bBot arrays since the model is changed
		}
	} // for icont


	//	myModelSaveAs(getFileNameAppend(velMod->velSecName, _T("_MM1")));
	//	myModelSaveAs(velMod2, getFileNameAppend(velMod->velSecName, _T("_MM2")), FALSE);

	// now merging
	int k, n, m;
	bool isIgnore = myFloat(velMod2->getXmin(), 2) == myFloat(velMod->getXmax(), 2);
	for (icont = 0; icont < velMod->ncont; icont++) {
		n = velMod->nzed[icont];
		for (j = 0; j < velMod2->nzed[icont]; j++) {
			m = j + n;
			if (isIgnore && j == 0) continue;
			else if (isIgnore && j>0) m--;  // ignore the first point

			if (m < MOD_POINTS_MAX - 1) {
				velMod->xm[m][icont] = velMod2->xm[j][icont];
				velMod->zm[m][icont] = velMod2->zm[j][icont];
			}
			else {
				m = MOD_POINTS_MAX - 1;
				k = velMod2->nzed[icont] - 1;
				velMod->xm[m][icont] = velMod2->xm[j][icont];
				velMod->zm[m][icont] = velMod2->zm[j][icont];
				break;
			}
		}
		velMod->nzed[icont] = m + 1;

		if (icont < velMod->ncont - 1) {
			for (k = 0; k < 2; k++) {
				n = velMod->nvel[icont][k];
				if (n < 1 || n >= MOD_POINTS_MAX) continue;
				for (j = 0; j < velMod2->nvel[icont][k]; j++) {
					velMod->xvel[j + n][icont][k] = velMod2->xvel[j][icont][k];
					velMod->vf[j + n][icont][k] = velMod2->vf[j][icont][k];
				}
				velMod->nvel[icont][k] += velMod2->nvel[icont][k];
			} // for k
		} // if
	} // for icont



	return true;
}




void CVelMod::ClipModelXmin(CVelMod::VELMOD *velm, float cmMin, BOOL isTime)
{
	// please consider a boundary having only 1 point!!!
	int icont;
	if (velm->velSecName.IsEmpty()) {
		AfxMessageBox(_T(" Please save your current model before clipping"));
		return;
	}
	if (velm->nzed[0] < 1) return;
	if (isFloatEqual(cmMin, velm->xm[0][0])) return;

	CVelMod::VELMOD velMod2;
	CVelMod vm;
	//vm.velModSetArrays(&velMod2);

	if (cmMin >= velm->xm[0][0]) {
		// if here, we need to chop the model
		for (icont = 0; icont < velm->ncont; icont++) {
			int ii = 0;
			for (int i = 0; i < velm->nzed[icont]; i++) {
				if (velm->xm[i][icont] >= cmMin) {
					// found the left index
					ii = i-1;
					break;
				}
			}
		//	if (ii < 0 || ii>=velm->nzed[icont]-1) return;  // this is impossible
				// if ii points to the last point on the boundary, we cannot continue;

			velMod2.xm[0][icont] = cmMin;
			//velMod2.zm[0][icont] = velm->zm[ii][icont];

			register double x1 = velm->xm[ii][icont];
			register double x2 = velm->xm[ii + 1][icont];
			register double dx = (x2 - x1);
			if (isTime) {
				register double z1 = velm->tm[ii][icont];
				register double z2 = velm->tm[ii + 1][icont];
				register double z = fabs(x2 - x1) < MINAMP ? z1 :
					z1 + (z2 - z1) * (cmMin - x1) / dx;
				velMod2.tm[0][icont] = (float)z;
				velMod2.zm[0][icont] = velm->zm[ii + 1][icont];
			}
			else {
				register double z1 = velm->zm[ii][icont];
				register double z2 = velm->zm[ii + 1][icont];
				register double z = fabs(x2 - x1) < MINAMP ? z1 :
					z1 + (z2 - z1) * (cmMin - x1) / dx;
				velMod2.zm[0][icont] = (float)z;
				velMod2.tm[0][icont] = velm->tm[ii + 1][icont];
			}

			int count = 1;
			for (int i = ii+1; i < velm->nzed[icont]; i++) {
				float f = velm->xm[i][icont];
				if (!isFloatEqual(f, cmMin)) {
					velMod2.xm[count][icont] = f;
					velMod2.zm[count][icont] = velm->zm[i][icont];
					velMod2.tm[count][icont] = velm->tm[i][icont];
					count++;
				}
			}
			velm->nzed[icont] = count;
			for (int i = 0; i < count; i++) {
				velm->xm[i][icont] = velMod2.xm[i][icont];
				velm->zm[i][icont] = velMod2.zm[i][icont];
				velm->tm[i][icont] = velMod2.tm[i][icont];
			}
		} // for icont

		for (int k = 0; k < 2; k++) {
			for (icont = 0; icont < velm->ncont - 1; icont++) {
				velMod2.xvel[0][icont][k] = cmMin;
				velMod2.vf[0][icont][k] = velm->vf[0][icont][k];
				int count = 1;
				for (int i = 0; i < velm->nvel[icont][k]; i++) {
					if (velm->xvel[i][icont][k] > cmMin) {
						if (velm->xvel[i][icont][k] < 0.f) {
							int ii = 0;
						}
						velMod2.xvel[count][icont][k] = velm->xvel[i][icont][k];
						velMod2.vf[count][icont][k] = velm->vf[i][icont][k];
						count++;
					} // if
				} // for i
				velm->nvel[icont][k] = count;
				for (int i = 0; i < count; i++) {
					velm->xvel[i][icont][k] = velMod2.xvel[i][icont][k];
					velm->vf[i][icont][k] = velMod2.vf[i][icont][k];
				}
			} // for icont
		} // for k
	}
	else {
		// if here, we need to expand the model the left, so we just add an extra point for each boundary
		for (icont = 0; icont < velm->ncont; icont++) {
			if (velm->nzed[icont] < MOD_POINTS_MAX - 1) {
				for (int i = 0; i < velm->nzed[icont]; i++) {
					velMod2.xm[i][icont] = velm->xm[i][icont];
					velMod2.zm[i][icont] = velm->zm[i][icont];
					velMod2.tm[i][icont] = velm->tm[i][icont];
				}
				for (int i = 1; i < velm->nzed[icont] + 1; i++) {
					velm->xm[i][icont] = velMod2.xm[i - 1][icont];
					velm->zm[i][icont] = velMod2.zm[i - 1][icont];
					velm->tm[i][icont] = velMod2.tm[i - 1][icont];
				}
				velm->xm[0][icont] = cmMin;
				velm->zm[0][icont] = velm->zm[1][icont];
				velm->tm[0][icont] = velm->tm[1][icont];
				velm->nzed[icont] ++;
			} // if velMod is not too big
			else {
				velm->xm[0][icont] = cmMin;
			}
		} // for icont

		for (icont = 0; icont < velm->ncont - 1; icont++) {
			velm->xvel[0][icont][0] = cmMin;
			velm->xvel[0][icont][1] = cmMin;
		}
	}
	//vm.velModFreeArrays(&velMod2);


//	velm->xmin = cmMin;
}

bool CVelMod::velModVelocityPerturb(VELMOD *velm, double deltavPercent, int layer1, int layer2, bool isMinus)
{
	if (layer1 < 1) layer1 = 1;
	if (layer2 > velm->ncont - 1) layer2 = velm->ncont - 1;
	deltavPercent = isMinus ? -fabs(deltavPercent) : fabs(deltavPercent);
	deltavPercent = 1. + deltavPercent * 0.01;
	if (deltavPercent == 0. || velm->ncont < 2) return false;

	for (int k = 0; k < 2; k++) {
		for (int icont = layer1-1; icont < layer2; icont++) {
			for (int i = 0; i < velm->nvel[icont][k]; i++) {
				velm->vf[i][icont][k] *= (float)deltavPercent;
			}
		} // for icont
	} // for k

	return true;
}

/*
Alternatively, you may paste layer-Perturb pairs below
(first layer is numbered 0):
1   -.01
2  -0.01
*/
//bool CVelMod::velModVelocityPerturb(VELMOD *velm, CString strLines)
//{
//	if (strLines.GetLength() < 5) return false;
//
//	CStringArray *linesArr = getLinesFromCString(strLines, false);
//	if (!linesArr) return false;
//
//	size_t len = linesArr->GetCount();
//	if (len < 1) return false;
//
//	// CString sToken;
//	for (size_t i = 0; i < len; i++) {
//		CString str = linesArr->ElementAt(i);
//		str.Replace(TAB, SPACE); // convert tab to space, since if pasted from excel, it will be tab deliminated
//		str.Replace(_T("  "), SPACE); // make sure only 1 space exists between items, otherwise it will mis-calculate some
//		// AfxExtractSubString(sToken, str, 0, _T(' '));
//		int ind = str.Find(SPACE);
//		register int ilayer = _ttoi(str.Left(ind).Trim());
//		ilayer--;  // user input ilayer with 1 based; but our index is zero based. So we should subtract 1
//		// AfxExtractSubString(sToken, str, 1, _T(' '));
//		register double deltav = _tstof(str.Mid(ind).Trim());
//		if (ilayer >= 0 && ilayer < velm->ncont - 1 && fabs(deltav) < 1. && deltav != 0.) {
//			// perturb the velocity
//			for (int k = 0; k < 2; k++) {
//				for (int j = 0; j < velm->nvel[ilayer][k]; j++) {
//					velm->vf[j][ilayer][k] += (float)deltav;
//				}
//			} // for k
//		} // if
//	} // for i
//
//	linesArr->RemoveAll();
//	delete linesArr;
//	return true;
//
//}

// if isMinus is true, the *velm will use negative increments only
// otherwise, it will use positive increments.
bool CVelMod::velModVelocityPerturb(VELMOD *velm, CString strLines, bool isMinus)
{
	if (strLines.GetLength() < 5) return false;

	CStringArray *linesArr = getLinesFromCString(strLines, false);
	if (!linesArr) return false;

	size_t len = linesArr->GetCount();
	if (len < 1) return false;

	// CString sToken;
	for (size_t i = 0; i < len; i++) {
		CString str = linesArr->ElementAt(i);
		str.Replace(TAB, SPACE); // convert tab to space, since if pasted from excel, it will be tab deliminated
		str.Replace(_T("  "), SPACE); // make sure only 1 space exists between items, otherwise it will mis-calculate some
										// AfxExtractSubString(sToken, str, 0, _T(' '));
		int ind = str.Find(SPACE);
		register int ilayer = _ttoi(str.Left(ind).Trim());
		ilayer--;  // user input ilayer with 1 based; but our index is zero based. So we should subtract 1
				   // AfxExtractSubString(sToken, str, 1, _T(' '));
		register double deltav = _tstof(str.Mid(ind).Trim());
		deltav = fabs(deltav);
		if (isMinus) deltav = -deltav;
		if (ilayer >= 0 && ilayer < velm->ncont - 1 && deltav < 1. && deltav != 0.) {
			// perturb the velocity
			for (int k = 0; k < 2; k++) {
				for (int j = 0; j < velm->nvel[ilayer][k]; j++) {
					velm->vf[j][ilayer][k] += (float)deltav;
				}
			} // for k
		} // if
	} // for i

	linesArr->RemoveAll();
	delete linesArr;
	return true;

}

void CVelMod::ClipModelXmax(CVelMod::VELMOD *velm, float cmMax, BOOL isTime)
{
	int icont;
	if (velm->velSecName.IsEmpty()) {
		AfxMessageBox(_T(" Please save your current model before clipping"));
		return;
	}
	if (velm->nzed[0] < 1) return;
	if (isFloatEqual(cmMax, velm->xm[velm->nzed[0] - 1][0])) return;

	CVelMod::VELMOD velMod2;
	CVelMod vm;
	//vm.velModSetArrays(&velMod2);


	float xmaxInModel = velm->xm[velm->nzed[0] - 1][0];
	if (cmMax <= xmaxInModel) {
		// if here, we need top chop the model
		for (icont = 0; icont < velm->ncont; icont++) {
			int ii = 0;
			for (int i = 0; i < velm->nzed[icont] - 1; i++) {
				if (velm->xm[i][icont] < cmMax && velm->xm[i + 1][icont] >= cmMax) {
					// found the left index
					ii = i;
					break;
				}
			}
			// it is ok if ii==0, which means only the first point shall b e retained

			register int count = 0;
			for (int i = 0; i < ii + 1; i++) {
				velMod2.xm[count][icont] = velm->xm[i][icont];
				velMod2.zm[count][icont] = velm->zm[i][icont];
				velMod2.tm[count][icont] = velm->tm[i][icont];
				count++;
			}
			velMod2.xm[count][icont] = cmMax;
			register double x1 = velm->xm[ii][icont];
			register double x2 = velm->xm[ii + 1][icont];
			register double dx = (x2 - x1);
			if (isTime) {
				register double z1 = velm->tm[ii][icont];
				register double z2 = velm->tm[ii + 1][icont];
				register double z = fabs(x2 - x1) < MINAMP ? z1 :
					z1 + (z2 - z1) * (cmMax - x1) / dx;
				velMod2.tm[count][icont] = (float)z;
				velMod2.zm[count][icont] = velm->tm[ii + 1][icont];  // need to interpolate
			}
			else {
				register double z1 = velm->zm[ii][icont];
				register double z2 = velm->zm[ii + 1][icont];
				register double z = fabs(x2 - x1) < MINAMP ? z1 :
					z1 + (z2 - z1) * (cmMax - x1) / dx;
				velMod2.zm[count][icont] = (float)z;
				velMod2.tm[count][icont] = velm->tm[ii + 1][icont];  // need to interpolate
			}
			count++;

			// now copy from &velMod2 to velm
			velm->nzed[icont] = count;
			for (int i = 0; i < count; i++) {
				velm->xm[i][icont] = velMod2.xm[i][icont];
				velm->zm[i][icont] = velMod2.zm[i][icont];
				velm->tm[i][icont] = velMod2.tm[i][icont];
			}
		} // for icont

		for (int k = 0; k < 2; k++) {
			for (icont = 0; icont < velm->ncont - 1; icont++) {
				int count = 0;
				for (int i = 0; i < velm->nvel[icont][k]; i++) {
					if (velm->xvel[i][icont][k] < cmMax) {
						velMod2.xvel[count][icont][k] = velm->xvel[i][icont][k];
						velMod2.vf[count][icont][k] = velm->vf[i][icont][k];
						count++;
					} // if
				} // for i
				velMod2.xvel[count][icont][k] = cmMax;
				velMod2.vf[count][icont][k] = velm->vf[count - 1][icont][k];
				count++;

				velm->nvel[icont][k] = count;
				for (int i = 0; i < count; i++) {
					velm->xvel[i][icont][k] = velMod2.xvel[i][icont][k];
					velm->vf[i][icont][k] = velMod2.vf[i][icont][k];
				}
			} // for icont
		} // for k
	}
	else {
		// if here, we need to expand the model to the right, so we just add an extra point for each boundary
		for (icont = 0; icont < velm->ncont; icont++) {
			if (velm->nzed[icont] < MOD_POINTS_MAX - 1) {
				int j = velm->nzed[icont];
				velm->xm[j][icont] = cmMax;
				velm->zm[j][icont] = velm->zm[j - 1][icont];
				velm->tm[j][icont] = velm->tm[j - 1][icont];
				velm->nzed[icont] ++;
			} // if velMod is not too big
			else {
				velm->xm[0][icont] = cmMax;
			}
		} // for icont

		// for velocity values, we do not need to add any more points, we just move the right-most distance to cmMax
		for (icont = 0; icont < velm->ncont - 1; icont++) {
			int j = velm->nvel[icont][0] - 1;
			velm->xvel[j][icont][0] = cmMax;
		}
		for (icont = 0; icont < velm->ncont - 1; icont++) {
			int j = velm->nvel[icont][1] - 1;
			velm->xvel[j][icont][1] = cmMax;
		}
	}
	//vm.velModFreeArrays(&velMod2);

//	velm->xmax = cmMax;
	for (icont = 0; icont < velm->ncont - 1; icont++) {
		register int n = velm->nvel[icont][0] -1;
		velm->xvel[n][icont][0] = cmMax;
		velm->xvel[n][icont][1] = cmMax;
	}
}


bool CVelMod::LoadReidsmodel(VELMOD *velm, CString reidName)
{
	//start Steve Perry's code:
	// #define MAXLAYERS 40
	// #define MAXPOINTS 400
#define MAXVELS 40
#define DX 0.01
#define DZ 0.01

	FILE *infil;
	int iaz, nlayers, npts[MOD_LAYER_MAX], nvels[MOD_LAYER_MAX], i, j = 0, l;
	float vels[MOD_LAYER_MAX], grad[MOD_LAYER_MAX];
	float x[MOD_LAYER_MAX][MOD_POINTS_MAX], z[MOD_LAYER_MAX][MOD_POINTS_MAX];
	float xmin = 99999.f, xmax = ERRORVAL, zmax = 0.0f;
	float ttx[MOD_POINTS_MAX], ttz[MOD_POINTS_MAX];
	float veltop[MOD_LAYER_MAX][MAXVELS], velbot[MOD_LAYER_MAX][MAXVELS];
	float xvel[MOD_LAYER_MAX][MAXVELS], zvel[MOD_LAYER_MAX][MAXVELS];

	TCHAR *jpfile;
	jpfile = reidName.GetBuffer(reidName.GetLength());
	if ((_tfopen_s(&infil, jpfile, _T("r"))) != 0) {
		reidName.ReleaseBuffer();
		return false;
	}
	reidName.ReleaseBuffer();

	_ftscanf_s(infil, _T("%d"), &iaz);
	_ftscanf_s(infil, _T("%f %f"), &vels[j], &grad[j]);
	do {
		_ftscanf_s(infil, _T("%d"), &npts[j]);
		for (i = 0; i < npts[j]; i++) {
			_ftscanf_s(infil, _T("%f %f"), &x[j][i], &z[j][i]);
			if (z[j][i] > zmax) zmax = z[j][i];
			if (x[j][i] > xmax) xmax = x[j][i];
			if (x[j][i] < xmin) xmin = x[j][i];
		}
		j++;
	} while (_ftscanf_s(infil, _T("%f %f"), &vels[j], &grad[j]) > 0);
	nlayers = j;
	x[j][0] = x[j - 1][0]; x[j][1] = x[j - 1][npts[j - 1] - 1];
	z[j][0] = (float)(zmax + 0.1 / grad[j - 1]);
	z[j][1] = z[j][0];
	npts[j] = 2;

	/*
	*   Look for horizontal transition layers.
	*/

	l = 0;
	do {
		int l1, l2;
		float v1, v2;
		l1 = -1; l2 = -1;
		if (vels[l] == 0.0) {
			for (j = 0; j <= nlayers; j++) {
				if (j != l) {
					for (i = 0; i < npts[j]; i++) {
						if ((fabsf(x[j][i] - x[l][0]) <= DX)
							&& (fabsf(z[j][i] - z[l][0]) <= DZ)) {
							l1 = j;
							v1 = vels[j];
						}
						else if ((fabsf(x[j][i] - x[l][npts[l] - 1]) <= DX)
							&& (fabsf(z[j][i] - z[l][npts[l] - 1]) <= DZ)) {
							l2 = j;
							v2 = vels[j];
						}
					}
				}
			}
		}
		if (l1 >= 0 && l2 >= 0) {
			for (i = 1; i < (npts[l] - 1); i++) {
				x[l1][npts[l1] + i - 1] = x[l][i];
				z[l1][npts[l1] + i - 1] = z[l][i];
			}
			npts[l1] += (npts[l] - 2);
			for (i = 0; i < npts[l2]; i++) {
				x[l1][npts[l1] + i] = x[l2][i];
				z[l1][npts[l1] + i] = z[l2][i];
			}
			npts[l1] += npts[l2];
			veltop[l1][nvels[l1]] = vels[l2];
			xvel[l1][nvels[l1]] = x[l2][0];
			velbot[l1][nvels[l1]] = grad[l2];
			zvel[l1][nvels[l1]] = z[l2][0];
			veltop[l1][nvels[l1] + 1] = vels[l2];
			xvel[l1][nvels[l1] + 1] = x[l2][npts[l2] - 1];
			velbot[l1][nvels[l1] + 1] = grad[l2];
			zvel[l1][nvels[l1] + 1] = z[l2][npts[l2] - 1];
			nvels[l1] += 2;
			npts[l] = -npts[l]; npts[l2] = -npts[l2];
		}
		else {
			veltop[l][0] = vels[l]; xvel[l][0] = x[l][0];
			velbot[l][0] = grad[l]; zvel[l][0] = z[l][0];
			veltop[l][1] = vels[l]; xvel[l][1] = x[l][npts[l] - 1];
			velbot[l][1] = grad[l]; zvel[l][1] = z[l][npts[l] - 1];
			nvels[l] = 2;
		}
		l++;
	} while (l <= nlayers);

	/*
	*   Look for vertical transition layers.
	*/

	l = 1;
	do {
		float dh;
		if ((vels[l] == 0.0) && (vels[l - 1] != 0.0) && (npts[l] > 0)) {
			if ((fabsf(x[l][0] - x[l - 1][0]) <= DX)
				&& (fabsf(x[l][npts[l] - 1] - x[l - 1][npts[l - 1] - 1]) <= DX)) {
				dh = z[l][0] - z[l - 1][0];
				for (i = 0; i < npts[l - 1]; i++) {
					x[l][i] = x[l - 1][i];
					z[l][i] = z[l - 1][i] + dh;
				}
				npts[l] = npts[l - 1];
				for (i = 0; i < nvels[l - 1]; i++) {
					veltop[l][i] = veltop[l - 1][i] + dh*velbot[l - 1][i];
					xvel[l][i] = xvel[l - 1][i];
					velbot[l][i] = velbot[l - 1][i];
					zvel[l][i] = zvel[l - 1][i] + dh;
				}
				nvels[l] = nvels[l - 1];
			}
		}
		l++;
	} while (l < nlayers);

	/*
	*   Calculate layer bottom velocities.
	*/

	l = 0;
	do {
		if (npts[l] > 0) {
			int il, ij, bottom_found;
			for (il = 0; il < nvels[l]; il++) {
				bottom_found = 0;
				j = l + 1;
				do {
					if (npts[j] > 0) {
						ij = 0;
						do {
							if (fabsf(xvel[l][il] - x[j][ij]) < DX) {
								velbot[l][il] = (z[j][ij] - zvel[l][il])
									* velbot[l][il];
								velbot[l][il] += veltop[l][il];
								bottom_found = 1;
							}
							else if (ij > 0 && x[j][ij] > xvel[l][il]) {
								float zslope;
								zslope = (z[j][ij] - z[j][ij - 1])
									/ (x[j][ij] - x[j][ij - 1]);
								velbot[l][il] = (z[j][ij - 1] + zslope
									* (xvel[l][il] - x[j][ij - 1]) - zvel[l][il])
									* velbot[l][il];
								velbot[l][il] += veltop[l][il];
								bottom_found = 1;
							}
							ij++;
						} while (!bottom_found && ij < npts[j]);
					}
					j++;
				} while (!bottom_found && j <= nlayers);
			}
		}
		l++;
	} while (l < nlayers);



	//back to Deping's coding
	//found Steve's program a problem in dealing with blocks. So here is fix:


	velm->ncont = nlayers + 1;

	for (j = 0; j < MOD_POINTS_MAX; j++) {
		for (i = 0; i < 40; i++) {
			velm->nzed[i] = 0;
			velm->xm[j][i] = 0.f;
			velm->zm[j][i] = 0.f;
			velm->xvel[j][i][0] = 0.f;
			velm->xvel[j][i][1] = 0.f;
			velm->vf[j][i][0] = 0.f;
			velm->vf[j][i][1] = 0.f;
		}
	}


	for (i = 0; i < velm->ncont; i++) {
		velm->nzed[i] = npts[i];
		for (j = 0; j < velm->nzed[i]; j++) {
			velm->xm[j][i] = x[i][j];
			velm->zm[j][i] = z[i][j];
		}
	}
	for (i = 0; i < velm->ncont - 1; i++) {
		velm->nvel[i][0] = velm->nvel[i][1] = nvels[i];
		for (j = 0; j < nvels[i]; j++) {
			velm->xvel[j][i][0] = velm->xvel[j][i][1] = xvel[i][j];
			velm->vf[j][i][0] = veltop[i][j];
			velm->vf[j][i][1] = velbot[i][j];
		}
	}

	//try to merge two blocks to form a layer
	for (i = 0; i < velm->ncont - 1; i++) {
		int iNew, jNew;
		float xJoin = velm->xm[velm->nzed[i] - 1][i];
		if (xJoin < xmax && xJoin == velm->xm[0][i + 1]) {
			for (j = 1; j < velm->nzed[i + 1]; j++) {  //use j=1 to skip the first point.
				jNew = velm->nzed[i];
				velm->xm[jNew + j - 1][i] = velm->xm[j][i + 1];
				velm->zm[jNew + j - 1][i] = velm->zm[j][i + 1];
			}
			velm->nzed[i] += velm->nzed[i + 1] - 1;
			for (iNew = i + 1; iNew < velm->ncont - 1; iNew++) {
				for (j = 0; j < velm->nzed[iNew + 1]; j++) {
					velm->xm[j][iNew] = velm->xm[j][iNew + 1];
					velm->zm[j][iNew] = velm->zm[j][iNew + 1];
				}
				velm->nzed[iNew] = velm->nzed[iNew + 1];
			}
			velm->ncont--;

			for (j = 1; j < velm->nvel[i + 1][0]; j++) { //use j=1 to skip the first point.
				jNew = velm->nvel[i][0];
				velm->xvel[jNew + j - 1][i][0] = velm->xvel[j][i + 1][0];
				velm->vf[jNew + j - 1][i][0] = velm->vf[j][i + 1][0];
			}
			velm->nvel[i][0] += velm->nvel[i + 1][0] - 1;
			for (j = 1; j < velm->nvel[i + 1][1]; j++) {
				jNew = velm->nvel[i][1];
				velm->xvel[jNew + j - 1][i][1] = velm->xvel[j][i + 1][1];
				velm->vf[jNew + j - 1][i][1] = velm->vf[j][i + 1][1];
			}
			velm->nvel[i][1] += velm->nvel[i + 1][1] - 1;
			for (iNew = i + 1; iNew < velm->ncont - 1 - 1; iNew++) {
				for (j = 0; j < velm->nvel[iNew + 1][0]; j++)
					velm->xvel[j][iNew][0] = velm->xvel[j][iNew + 1][0];
				for (j = 0; j < velm->nvel[iNew + 1][1]; j++)
					velm->xvel[j][iNew][1] = velm->xvel[j][iNew + 1][1];
				velm->nvel[iNew][0] = velm->nvel[iNew + 1][0];
				velm->nvel[iNew][1] = velm->nvel[iNew + 1][1];
			}
			//velm->nlayer--;
		} //end of block merging.

		//now check for blocks starting from >xmin.
		if (velm->xm[0][i] > xmin) {
			//backup xm and zm first.
			for (j = 0; j < velm->nzed[i]; j++) {
				ttx[j] = velm->xm[j][i];
				ttz[j] = velm->zm[j][i];
			}
			for (iNew = i + 1; iNew < velm->nzed[iNew]; iNew++) {
				if (velm->xm[0][iNew] == xmin) {
					for (j = 0; j < velm->nzed[iNew]; j++) {
						if (velm->xm[j][iNew] < ttx[0]) {
							velm->xm[j][i] = velm->xm[j][iNew];
							velm->zm[j][i] = velm->zm[j][iNew];
						}
						else break;
						//	AfxMessageBox(_T("problem");
					}
					for (jNew = 0; jNew < velm->nzed[i]; jNew++) {
						velm->xm[jNew + j][i] = ttx[jNew];
						velm->zm[jNew + j][i] = ttz[jNew];
					}
					break;
				}
			} //end of fixing the small block for boundaries.
			velm->nzed[i] += j;
			velm->xvel[0][i][0] = xmin;
			velm->xvel[0][i][1] = xmin;
		}//end of fixing the small block.

		/*
		//now check for blocks ending at <xmax.
		if(velm->xm[velm->nzed[i]-1][i] < xmax && i>0)
		{
		for (iNew=i+1; iNew<velm->nzed[iNew]; iNew++)
		{
		int jJoin = velm->nzed[iNew];
		xJoin = velm->xm[jJoin-1][iNew];
		if(xJoin==xmax)
		{
		for (j=0; j<velm->nzed[iNew]; j++)
		{
		if(velm->xm[j][iNew]>xJoin)
		{
		velm->xm[jJoin+j][i] = velm->xm[j][iNew];
		velm->zm[jJoin+j][i] = velm->zm[j][iNew];
		velm->nzed[i]++;
		}
		}
		break;
		}
		} //end of fixing the small block for boundaries.
		jNew = velm->nvel[i][0]-1;
		velm->xvel[jNew][i][0] = xmax;
		jNew = velm->nvel[i][1]-1;
		velm->xvel[jNew][i][1] = xmax;
		}//end of fixing the small block.
		*/
	}  //end of checking irregularities.

	fclose(infil);

	//make sure all ends at xmin and xmax.
	for (i = 0; i < velm->ncont; i++) {
		velm->xm[0][i] = velm->xvel[0][i][0] = velm->xvel[0][i][1] = xmin;
		velm->xm[velm->nzed[i] - 1][i] = xmax;
	}

	for (i = 0; i < velm->ncont - 1; i++) {
		velm->xvel[velm->nvel[i][0] - 1][i][0] = xmax;
		velm->xvel[velm->nvel[i][1] - 1][i][1] = xmax;
	}

	return true;
}


bool CVelMod::loadModelZelt2(VELMOD *velm, CString zeltName)
{
	//this->velModSetArrays(velm); // this resets all the array counters after freeing existing arrays, so please call this first before any adjustments of velocity models
	int  nrz, nrv, i, j, j1, j2, icont, ilyr, icnt;
	TCHAR line[MAXLINE_ZELT];
	BOOL iEOF = FALSE;
	i = 0;

	FILE *jpfp;
	if (_tfopen_s(&jpfp, zeltName, _T("r")) != 0) return false;
	velm->velModReset(); // must free arrays before loading
	TRY
	{








		TCHAR *form = _T("%7f");


		for (icont = 0; icont < MOD_LAYER_MAX; icont++)
		{
			nrz = 1;
			j1 = 0;
			j2 = 9;
			while (true)
			{
				// sometimes a model contain more points than SeisWide
				if (j2 >= MOD_POINTS_MAX) {
					fclose(jpfp);
					CString tStr;
					tStr.Format(_T("Sorry, maximum number of points %d encountered. Please contact SeisWide developer to increase the limit!"), i);
					AfxMessageBox(tStr, MB_ICONQUESTION);
					return false;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) {
					iEOF = TRUE; // if not set TRUE, this line will be treated as a new boundary
					break;
				}
				if (_tcslen(line) < 2) continue;
				if (line[0] == '#') continue; // we do not need comment lines
				trimRight(line);
				if (_tcslen(line) < 3) {
					iEOF = TRUE;
					break;
				}

				_stscanf_s(line, _T("%2d "), &ilyr);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 7; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], form, &velm->xm[j][icont]) == 0) break;
					velm->nzed[icont]++;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				trimRight(line);
				_stscanf_s(line, _T("%2d "), &icnt);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 7; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], form, &velm->zm[j][icont]) == 0) break;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) {
					// if here, the last interface only has a depth with no velocity defined.
					iEOF = TRUE;
					break;
				}

				nrz++;
				if (icnt != 1) break;
				j1 += 10;
				j2 += 10;
			}

			if (iEOF) {
				// velm->ncont++; 
				break;
			}

			nrv = 1;
			j1 = 0;
			j2 = 9;
			while (true)
			{
				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) {
					iEOF = TRUE; break;
				}
				if (_tcslen(line) < 2) continue;
				if (line[0] == '#') continue; // we do not need comment lines
				trimRight(line);
				_stscanf_s(line, _T("%2d "), &ilyr);
				if (ilyr < 1)
				{
					iEOF = TRUE; break;
				}
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 7; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], form, &velm->xvel[j][icont][0]) == 0) break;
					velm->nvel[icont][0]++;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				trimRight(line);
				_stscanf_s(line, _T("%2d "), &icnt);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 7; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], form, &velm->vf[j][icont][0]) == 0) break;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				nrv++;
				if (icnt != 1) break;
				j1 += 10;
				j2 += 10;
			}

			if (iEOF) { break; }

			nrv = 1;
			j1 = 0;
			j2 = 9;
			while (true)
			{
				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				if (_tcslen(line) < 2) continue;
				if (line[0] == '#') continue; // we do not need comment lines
				trimRight(line);
				_stscanf_s(line, _T("%2d "), &ilyr);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 7; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], form, &velm->xvel[j][icont][1]) == 0) break;
					velm->nvel[icont][1]++;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				trimRight(line);
				_stscanf_s(line, _T("%2d "), &icnt);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 7; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], form, &velm->vf[j][icont][1]) == 0) break;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				nrv++;
				if (icnt != 1) break;
				j1 += 10;
				j2 += 10;

			}

		}



	}
	CATCH_ALL(e)
	{
		AfxMessageBox(_T("Failed loading."));
		return false;
	}
	END_CATCH_ALL

		fclose(jpfp);
	velm->ncont = icont + 1;
	//	velm->nlayer = velm->ncont-1;

	// additions: anisotropy added Oct., 2009 for model
	for (icont = 0; icont < velm->ncont - 1; icont++)
		velm->anisotropy[icont] = 0.f;

	return true;
}



bool CVelMod::loadModelZelt3(VELMOD *velm, CString zeltName)
{
	//this->velModSetArrays(velm); // this resets all the array counters after freeing existing arrays, so please call this first before any adjustments of velocity models

	int  nrz, nrv, i, j, j1, j2, icont, ilyr, icnt;
	TCHAR line[MAXLINE_ZELT];
	BOOL iEOF = FALSE;
	i = 0;

	FILE *jpfp;
	if (_tfopen_s(&jpfp, zeltName, _T("r")) != 0) return false;
	velm->velModReset(); // must free arrays before loading
	TRY
	{










		for (icont = 0; icont < MOD_LAYER_MAX; icont++)
		{
			nrz = 1;
			j1 = 0;
			j2 = 9;
			while (true)
			{
				// sometimes a model contain more points than SeisWide
				if (j2 >= MOD_POINTS_MAX) {
					fclose(jpfp);
					CString tStr;
					tStr.Format(_T("Sorry, maximum number of points %d encountered. Please contact SeisWide developer to increase the limit!"), i);
					AfxMessageBox(tStr, MB_ICONQUESTION);
					return false;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) {
					iEOF = TRUE; // if not set TRUE, this line will be treated as a new boundary
					break;
				}
				if (_tcslen(line) < 2) continue;
				if (line[0] == '#') continue; // we do not need comment lines
				trimRight(line);
				if (_tcslen(line) < 3) {
					iEOF = TRUE;
					break;
				}

				_stscanf_s(line, _T("%2d "), &ilyr);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 8; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], _T("%8f"), &velm->xm[j][icont]) == 0) break;
					velm->nzed[icont]++;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				trimRight(line);
				_stscanf_s(line, _T("%2d "), &icnt);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 8; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], _T("%8f"), &velm->zm[j][icont]) == 0) break;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) {
					// if here, the last interface only has a depth with no velocity defined.
					iEOF = TRUE;
					break;
				}

				nrz++;
				if (icnt != 1) break;
				j1 += 10;
				j2 += 10;
			}

			if (iEOF) {
				// velm->ncont++; 
				break;
			}

			nrv = 1;
			j1 = 0;
			j2 = 9;
			while (true)
			{
				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) {
					iEOF = TRUE; break;
				}
				if (_tcslen(line) < 2) continue;
				if (line[0] == '#') continue; // we do not need comment lines
				trimRight(line);
				_stscanf_s(line, _T("%2d "), &ilyr);
				if (ilyr < 1)
				{
					iEOF = TRUE; break;
				}
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 8; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], _T("%8f"), &velm->xvel[j][icont][0]) == 0) break;
					velm->nvel[icont][0]++;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				trimRight(line);
				_stscanf_s(line, _T("%2d "), &icnt);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 8; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], _T("%8f"), &velm->vf[j][icont][0]) == 0) break;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				nrv++;
				if (icnt != 1) break;
				j1 += 10;
				j2 += 10;
			}

			if (iEOF) { break; }

			nrv = 1;
			j1 = 0;
			j2 = 9;
			while (true)
			{
				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				trimRight(line);
				if (_tcslen(line) < 2) continue;
				if (line[0] == '#') continue; // we do not need comment lines
				_stscanf_s(line, _T("%2d "), &ilyr);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 8; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], _T("%8f"), &velm->xvel[j][icont][1]) == 0) break;
					velm->nvel[icont][1]++;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				trimRight(line);
				_stscanf_s(line, _T("%2d "), &icnt);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 8; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], _T("%8f"), &velm->vf[j][icont][1]) == 0) break;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				nrv++;
				if (icnt != 1) break;
				j1 += 10;
				j2 += 10;

			}

		}



	}
	CATCH_ALL(e)
	{
		AfxMessageBox(_T("Failed loading."));
		return false;
	}
	END_CATCH_ALL

		fclose(jpfp);
	velm->ncont = icont + 1;
	//	velm->nlayer = velm->ncont-1;


	// additions: anisotropy added Oct., 2009 for model
	for (icont = 0; icont < velm->ncont - 1; icont++)
		velm->anisotropy[icont] = 0.f;

	return true;
}

bool CVelMod::loadModelZelt4(VELMOD *velm, CString zeltName)
{
	//this->velModSetArrays(velm); // this resets all the array counters after freeing existing arrays, so please call this first before any adjustments of velocity models

	int  nrz, nrv, i, j, j1, j2, icont, ilyr, icnt;
	TCHAR line[MAXLINE_ZELT];
	BOOL iEOF = FALSE;
	i = 0;

	FILE *jpfp;
	if (_tfopen_s(&jpfp, zeltName, _T("r")) != 0) return false;
	velm->velModReset(); // must free arrays before loading
	TRY
	{










		for (icont = 0; icont < MOD_LAYER_MAX; icont++)
		{
			nrz = 1;
			j1 = 0;
			j2 = 9;
			while (true)
			{
				// sometimes a model contain more points than SeisWide
				if (j2 >= MOD_POINTS_MAX) {
					fclose(jpfp);
					CString tStr;
					tStr.Format(_T("Sorry, maximum number of points %d encountered. Please contact SeisWide developer to increase the limit!"), i);
					AfxMessageBox(tStr, MB_ICONQUESTION);
					return false;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) {
					iEOF = TRUE; // if not set TRUE, this line will be treated as a new boundary
					break;
				}
				if (_tcslen(line) < 2) continue;
				if (line[0] == '#') continue; // we do not need comment lines
				trimRight(line);
				if (_tcslen(line) < 3) {
					iEOF = TRUE;
					break;
				}

				_stscanf_s(line, _T("%2d "), &ilyr);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 9; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], _T("%9f"), &velm->xm[j][icont]) == 0) break;
					velm->nzed[icont]++;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				trimRight(line);
				_stscanf_s(line, _T("%2d "), &icnt);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 9; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], _T("%9f"), &velm->zm[j][icont]) == 0) break;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) {
					// if here, the last interface only has a depth with no velocity defined.
					iEOF = TRUE;
					break;
				}

				nrz++;
				if (icnt != 1) break;
				j1 += 10;
				j2 += 10;
			}

			if (iEOF) {
				// velm->ncont++; 
				break;
			}

			nrv = 1;
			j1 = 0;
			j2 = 9;
			while (true)
			{
				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) {
					iEOF = TRUE; break;
				}
				if (_tcslen(line) < 2) continue;
				if (line[0] == '#') continue; // we do not need comment lines
				trimRight(line);
				_stscanf_s(line, _T("%2d "), &ilyr);
				if (ilyr < 1)
				{
					iEOF = TRUE; break;
				}
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 9; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], _T("%9f"), &velm->xvel[j][icont][0]) == 0) break;
					velm->nvel[icont][0]++;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				trimRight(line);
				_stscanf_s(line, _T("%2d "), &icnt);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 9; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], _T("%9f"), &velm->vf[j][icont][0]) == 0) break;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				nrv++;
				if (icnt != 1) break;
				j1 += 10;
				j2 += 10;
			}

			if (iEOF) { break; }

			nrv = 1;
			j1 = 0;
			j2 = 9;
			while (true)
			{
				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				if (_tcslen(line) < 2) continue;
				if (line[0] == '#') continue; // we do not need comment lines
				trimRight(line);
				_stscanf_s(line, _T("%2d "), &ilyr);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 9; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], _T("%9f"), &velm->xvel[j][icont][1]) == 0) break;
					velm->nvel[icont][1]++;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				trimRight(line);
				_stscanf_s(line, _T("%2d "), &icnt);
				for (j = j1; j <= j2; j++)
				{
					UINT k = 3 + (j - j1) * 9; if (k >= _tcslen(line) - 1) break;
					if (_stscanf_s(&line[k], _T("%9f"), &velm->vf[j][icont][1]) == 0) break;
				}

				if (_fgetts(line, MAXLINE_ZELT, jpfp) == NULL) break;
				nrv++;
				if (icnt != 1) break;
				j1 += 10;
				j2 += 10;

			}

		}



	}
	CATCH_ALL(e)
	{
		AfxMessageBox(_T("Failed loading."));
		return false;
	}
	END_CATCH_ALL

		fclose(jpfp);
	velm->ncont = icont + 1;

	// additions: anisotropy added Oct., 2009 for model
	for (icont = 0; icont < velm->ncont - 1; icont++)
		velm->anisotropy[icont] = 0.f;


	return true;
}

//_variant_t GetAttribute(IXMLDOMElementPtr pElem, CString str)
//{
//	IXMLDOMNamedNodeMapPtr pAtrs;
//	pElem->get_attributes(&pAtrs);
//	if (pAtrs) {
//		pAtrs->getNamedItem(_T("regression"));
//	}
//
//}

// dz is only used in Z to T transformation
BOOL CVelMod::loadModelXML(VELMOD *velm, CString inName, double dz, CString& strErr)
{
	MSXML::IXMLDOMDocument2Ptr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		strErr.Format(_T("Failed to create the XML class instance"));
		return FALSE;
	}
	if (pXMLDoc->load(variant_t(inName)) == VARIANT_FALSE) {
		ShowError(pXMLDoc);
		return FALSE;
	}

	BOOL isSuc = FALSE;
	//CString tStr;
	TRY{
		bool isV3 = false;
		MSXML::IXMLDOMElementPtr pVelMod = pXMLDoc->GetdocumentElement();
		int j = getAttributeDef(pVelMod, _T("numHorizon"), 0);
		if (j > 0) {
			int i = getAttributeDef(pVelMod, _T("deployDist"), ERRORVAL);
			if (i == (int)ERRORVAL) {
				isV3 = true;
			}
		}

		if (isV3)
			isSuc = loadModelXML_V3(velm, pXMLDoc, dz, strErr);  // almost allways this is true
		else
			isSuc = loadModelXML_V2(velm, pXMLDoc, dz, strErr);

	}
	CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL

	if (!isSuc) return FALSE;

	// disabled auto converting from vxml to bin format... Nov. 5, 2014
		//// first decide if this directory is writable. If not, we will need to use the windows temp dir
		//if ( !isDirectoryWritable(GetPathFromFileSlash(velm->velSecName)) ) {
		//	// get the windows temp dir
		//	velm->velSecName = getSystemTempPathSlash() + GetBaseFileName(velm->velSecName);
		//}


		//if (GetFileExtension(velm->velSecName) != _T("bin"))
		//	velm->velSecName = SetFileExtension(velm->velSecName, _T("bin"));
		//if (isFileExistNotDirectory(velm->velSecName))
		//	velm->velSecName = GetFileNameAppendExt(velm->velSecName, _T("Mod"), _T("bin"));

	//if (!this->saveModelAs(velm, velm->velSecName, dz, FALSE, 0)) return FALSE;

	bool isModelInTimeOnly = false;
	for (int icont = 0; icont < velm->ncont; icont++) 
		for (long ip = 0; ip < velm->nzed[icont]; ip++) 
	{
			if (isModelInTimeOnly) break;

			if (velm->zm[ip][icont] == ERRORVAL 
				&& velm->tm[ip][icont] != ERRORVAL)
				isModelInTimeOnly = true;
	}

	velm->m_isConvert = true; // must be set manually
	if (isModelInTimeOnly) {
		myModel_TWTT2Depth(velm, dz);
	}
	else {
		myModel_Depth2TWTT(velm, dz);
	}
	this->formatCalcVelLimits(velm);

	return TRUE;
}

BOOL CVelMod::loadModelXML_V2(VELMOD *velm, MSXML::IXMLDOMDocument2Ptr pXMLDoc, double dz, CString& strErr)
{
	if (!pXMLDoc) return FALSE;
	// CString tStr;
	static _variant_t strBlank = _T("");

	TRY{
		MSXML::IXMLDOMElementPtr pVelMod = pXMLDoc->GetdocumentElement();
		try {
			velm->ncont = getAttributeDef(pVelMod, _T("numHorizon"), 0);
			velm->ncontMaxConstrained = getAttributeDef(pVelMod, _T("numHorizonValid"), 0);
			velm->nRegressionLayer = getAttributeDef(pVelMod, _T("regressionLayer"), 0);
			// velm->regressionLayer--;
			//if (velm->regressionLayer>=0) {
				// the defaults are for central Canada Basin in the Arctic
				velm->regVZero = getAttributeDef(pVelMod, _T("vzero"), 1.69);
				velm->regVInf = getAttributeDef(pVelMod, _T("vinf"), 5.33);
				velm->regAlpha = getAttributeDef(pVelMod, _T("alpha"), 0.42745);
			//}

			//velm->xmin = getAttributeDef(pVelMod, _T("xmin"), 0);
			//velm->xmax = getAttributeDef(pVelMod, _T("xmax"), 0);
			velm->zmin = getAttributeDef(pVelMod, _T("zmin"), 0);
			velm->zmax = getAttributeDef(pVelMod, _T("zmax"), 0);

			// meta info
			velm->deploys[0].metaTitle = getAttributeDef(pVelMod, _T("title"), strBlank);
			velm->metaCruise = getAttributeDef(pVelMod, _T("cruise"), strBlank);
			velm->metaProcessor = getAttributeDef(pVelMod, _T("processor"), strBlank);
			velm->metaCompany = getAttributeDef(pVelMod, _T("company"), strBlank);
			velm->metaDate = getAttributeDef(pVelMod, _T("date"), strBlank);


			//pXMLRootElem->setAttribute(_T("deployDist"), _variant_t(velm->deploys[0].deployDist));
			//pXMLRootElem->setAttribute(_T("deployLat"), _variant_t(myFloat8(velm->deploys[0].deployLatLong[0])));
			//pXMLRootElem->setAttribute(_T("deployLon"), _variant_t(myFloat8(velm->deploys[0].deployLatLong[1])));
			//pXMLRootElem->setAttribute(_T("startLat"), _variant_t(myFloat8(velm->deploys[0].startLatLong[0])));
			//pXMLRootElem->setAttribute(_T("startLon"), _variant_t(myFloat8(velm->deploys[0].startLatLong[1])));
			//pXMLRootElem->setAttribute(_T("endLat"), _variant_t(myFloat8(velm->deploys[0].endLatLong[0])));
			//pXMLRootElem->setAttribute(_T("endLon"), _variant_t(myFloat8(velm->deploys[0].endLatLong[1])));


			velm->deploys[0].deployDist = getAttributeDef(pVelMod, _T("deployDist"), ERRORVAL);
			velm->deploys[0].deployLatLong[0] = getAttributeDef(pVelMod, _T("deployLat"), ERRORVAL);
			velm->deploys[0].deployLatLong[1] = getAttributeDef(pVelMod, _T("deployLon"), ERRORVAL);
			velm->deploys[0].startLatLong[0] = getAttributeDef(pVelMod, _T("startLat"), ERRORVAL);
			velm->deploys[0].startLatLong[1] = getAttributeDef(pVelMod, _T("startLon"), ERRORVAL);
			velm->deploys[0].endLatLong[0] = getAttributeDef(pVelMod, _T("endLat"), ERRORVAL);
			velm->deploys[0].endLatLong[1] = getAttributeDef(pVelMod, _T("endLon"), ERRORVAL);



			velm->wells[0].wellLatLong[0] = getAttributeDef(pVelMod, _T("pseudoWellLat"), ERRORVAL);
			velm->wells[0].wellLatLong[1] = getAttributeDef(pVelMod, _T("pseudoWellLon"), ERRORVAL);
			velm->wells[0].wellDist = getAttributeDef(pVelMod, _T("pseudoWellDist"), ERRORVAL);
		}
		catch (...) {
			// it's ok to have no limits defined
			velm->zmin = velm->zmax = 0.0;
		}

		long len;
		//	long lenPoints; 
		MSXML::IXMLDOMNodeListPtr pBoundarys;
		MSXML::IXMLDOMNodePtr pBoundary;
		MSXML::IXMLDOMNodeListPtr pPoints;
		MSXML::IXMLDOMNodePtr point;
		MSXML::IXMLDOMNodePtr pAttr;
		pBoundarys = pXMLDoc->getElementsByTagName(_T("layer"));
		if (pBoundarys == NULL) {
			strErr.Format(_T("Found invalid layers."));  // we may not use this error message
			//AfxMessageBox(tStr, MB_ICONQUESTION);
			return FALSE;
		}
		len = pBoundarys->length;
		velm->ncont = (int)len;
		for (int icont = 0; icont < len; icont++) {
			pBoundary = pBoundarys->Getitem(icont);
			velm->anisotropy[icont] = getAttributeDef(pBoundary->attributes->getNamedItem(_T("anisotropy")), 0);

			register int isValidated = getAttributeDef(pBoundary->attributes->getNamedItem(_T("validated")), 1);
			if (isValidated == 0) {
				// this horizon/boundary is not validated, so we need to check
				if (velm->ncontMaxConstrained > icont+1)
					velm->ncontMaxConstrained = icont + 1;
			}

			MSXML::IXMLDOMNodePtr pCont = myGetSingleChildByName(pBoundary, _T("horizon"));
			if (pCont == NULL) {
				strErr.Format(_T("Found invalid boundary node for layer %d."), icont + 1);  // we may not use this error message
				//AfxMessageBox(tStr, MB_ICONQUESTION);
				return FALSE;
			}
			pPoints = pCont->selectNodes(_T("point"));
			if (pPoints == NULL) {
				strErr.Format(_T("Found invalid boundary node for layer %d."), icont + 1);  // we may not use this error message
				//AfxMessageBox(tStr, MB_ICONQUESTION);
				return FALSE;
			}
			velm->nzed[icont] = (int)pPoints->length;
			strErr.Format(_T("Found invalid boundary point for layer %d."), icont + 1);  // we may not use this error message
			for (long ip = 0; ip < velm->nzed[icont]; ip++) {
				point = pPoints->Getitem(ip);
				if (point == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				pAttr = point->attributes->getNamedItem("x");
				if (pAttr == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				velm->xm[ip][icont] = pAttr->nodeValue;
				pAttr = point->attributes->getNamedItem(_T("z"));
				if (pAttr == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				velm->zm[ip][icont] = pAttr->nodeValue;
				pAttr = point->attributes->getNamedItem(_T("t"));
				if (pAttr != NULL) 
					velm->tm[ip][icont] = pAttr->nodeValue;

				pAttr = point->attributes->getNamedItem("lat");
				if (pAttr != NULL)
					velm->latitude[ip][icont] = pAttr->nodeValue;

				pAttr = point->attributes->getNamedItem("lon");
				if (pAttr != NULL)
					velm->longitude[ip][icont] = pAttr->nodeValue;
			} // for ip

			if (icont >= len - 1) break;  // the last boundary is not associated with velocities

			MSXML::IXMLDOMNodePtr pTop = myGetSingleChildByName(pBoundary, _T("topVelocity"));
			MSXML::IXMLDOMNodePtr pBottom = myGetSingleChildByName(pBoundary, _T("bottomVelocity"));

			pAttr = pTop->attributes->getNamedItem("validated");
			if (icont == velm->ncontMaxConstrained - 1 && pAttr != NULL) {
				int i = pAttr->nodeValue;
				if (i == 0) velm->isVelBelowValid = false;
			}

			strErr.Format(_T("Found invalid velocity for top of layer %d."), icont + 1);  // we may not use this error message
			if (pTop == NULL) {
				//AfxMessageBox(tStr, MB_ICONQUESTION);
				return FALSE;
			}
			pPoints = pTop->selectNodes(_T("point"));
			if (pPoints == NULL) {
				//AfxMessageBox(tStr, MB_ICONQUESTION);
				return FALSE;
			}
			velm->nvel[icont][0] = (int)pPoints->length;
			for (long ip = 0; ip < velm->nvel[icont][0]; ip++) {
				point = pPoints->Getitem(ip);
				if (point == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				pAttr = point->attributes->getNamedItem("x");
				if (pAttr == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				velm->xvel[ip][icont][0] = pAttr->nodeValue;
				pAttr = point->attributes->getNamedItem(_T("v"));
				if (pAttr == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				velm->vf[ip][icont][0] = pAttr->nodeValue;
			} // for ip

			strErr.Format(_T("Found invalid velocity for bottom of layer %d."), icont + 1);  // we may not use this error message
			if (pBottom == NULL) {
			//	AfxMessageBox(tStr, MB_ICONQUESTION);
				return FALSE;
			}
			pPoints = pBottom->selectNodes(_T("point"));
			if (pPoints == NULL) {
			//	AfxMessageBox(tStr, MB_ICONQUESTION);
				return FALSE;
			}
			velm->nvel[icont][1] = (int)pPoints->length;
			for (long ip = 0; ip < velm->nvel[icont][1]; ip++) {
				point = pPoints->Getitem(ip);
				if (point == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				pAttr = point->attributes->getNamedItem("x");
				if (pAttr == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				velm->xvel[ip][icont][1] = pAttr->nodeValue;
				pAttr = point->attributes->getNamedItem(_T("v"));
				if (pAttr == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				velm->vf[ip][icont][1] = pAttr->nodeValue;
			} // for ip
		} // for icont
	}
		CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL

	return TRUE;
}

BOOL CVelMod::loadModelXML_V3(VELMOD *velm, MSXML::IXMLDOMDocument2Ptr pXMLDoc, double dz, CString& strErr)
{
	if (!pXMLDoc) return FALSE;
	//CString tStr;
	static _variant_t strBlank = _T("");

	TRY{
		MSXML::IXMLDOMElementPtr pVelMod = pXMLDoc->GetdocumentElement();
		try {
			velm->ncont = getAttributeDef(pVelMod, _T("numHorizon"), 0);
			velm->ncontMaxConstrained = getAttributeDef(pVelMod, _T("numHorizonValid"), 0);
			velm->nRegressionLayer = getAttributeDef(pVelMod, _T("regressionLayer"), 0);
			// velm->regressionLayer--;
			//if (velm->regressionLayer>=0) {
				// the defaults are for central Canada Basin in the Arctic
				velm->regVZero = getAttributeDef(pVelMod, _T("vzero"), 1.69);
				velm->regVInf = getAttributeDef(pVelMod, _T("vinf"), 5.33);
				velm->regAlpha = getAttributeDef(pVelMod, _T("alpha"), 0.42745);
			//}

			//velm->xmin = getAttributeDef(pVelMod, _T("xmin"), 0);
			//velm->xmax = getAttributeDef(pVelMod, _T("xmax"), 0);
			velm->zmin = getAttributeDef(pVelMod, _T("zmin"), 0);
			velm->zmax = getAttributeDef(pVelMod, _T("zmax"), 0);

			// meta info
			velm->metaCruise = getAttributeDef(pVelMod, _T("cruise"), strBlank);
			velm->metaProcessor = getAttributeDef(pVelMod, _T("processor"), strBlank);
			velm->metaCompany = getAttributeDef(pVelMod, _T("company"), strBlank);
			velm->metaDate = getAttributeDef(pVelMod, _T("date"), strBlank);

			getModelDeploysV3(pXMLDoc, velm->deploys);
			getModelDeploysV5(pXMLDoc, velm->deploys, velm->wells);

			// the following 3 attributes have been moved to deploy node. But older versions still have them.
			// So we need to read them
			if (velm->wells[0].wellDist == ERRORVAL) {
				velm->wells[0].wellLatLong[0] = getAttributeDef(pVelMod, _T("pseudoWellLat"), ERRORVAL);
				velm->wells[0].wellLatLong[1] = getAttributeDef(pVelMod, _T("pseudoWellLon"), ERRORVAL);
				velm->wells[0].wellDist = getAttributeDef(pVelMod, _T("pseudoWellDist"), ERRORVAL);
			}
		}
		catch (...) {
			// it's ok to have no limits defined
			velm->zmin = velm->zmax = 0.0;
		}

		long len;
		//	long lenPoints; 
		MSXML::IXMLDOMNodeListPtr pBoundarys;
		MSXML::IXMLDOMNodePtr pBoundary;
		MSXML::IXMLDOMNodeListPtr pPoints;
		MSXML::IXMLDOMNodePtr point;
		MSXML::IXMLDOMNodePtr pAttr;
		pBoundarys = pXMLDoc->getElementsByTagName(_T("layer"));
		if (pBoundarys == NULL) {
			strErr.Format(_T("Found invalid layers."));  // we may not use this error message
			//AfxMessageBox(tStr, MB_ICONQUESTION);
			return FALSE;
		}
		len = pBoundarys->length;
		velm->ncont = (int)len;
		CString tStr2;
		for (int icont = 0; icont < len; icont++) {
			pBoundary = pBoundarys->Getitem(icont);
			velm->anisotropy[icont] = getAttributeDef(pBoundary->attributes->getNamedItem(_T("anisotropy")), 0);
			velm->poisson[icont] = getAttributeDef(pBoundary->attributes->getNamedItem(_T("poisson")), 0);
			register int isValidated = getAttributeDef(pBoundary->attributes->getNamedItem(_T("validated")), 1);
			if (isValidated == 0) {
				// this horizon/boundary is not validated, so we need to check
				if (velm->ncontMaxConstrained > icont + 1)
					velm->ncontMaxConstrained = icont + 1;
			}

			MSXML::IXMLDOMNodePtr pCont = myGetSingleChildByName(pBoundary, _T("horizon"));
			if (pCont == NULL) {
				strErr.Format(_T("Found invalid boundary node for layer %d."), icont + 1);  // we may not use this error message
				//AfxMessageBox(tStr, MB_ICONQUESTION);
				return FALSE;
			}
			velm->xmMin[icont] = getAttributeDef(pCont->attributes->getNamedItem(_T("xmin")), ERRORVAL);
			velm->xmMax[icont] = getAttributeDef(pCont->attributes->getNamedItem(_T("xmax")), ERRORVAL);

			pPoints = pCont->selectNodes(_T("point"));
			if (pPoints == NULL) {
				strErr.Format(_T("Found invalid boundary node for layer %d."), icont + 1);  // we may not use this error message
				//AfxMessageBox(tStr);
				return FALSE;
			}
			velm->nzed[icont] = (int)pPoints->length;
			strErr.Format(_T("Found invalid boundary point for layer %d."), icont + 1);  // we may not use this error message
			for (long ip = 0; ip < velm->nzed[icont]; ip++) {
				velm->zm[ip][icont] = ERRORVAL; // initialize it.
				// since a model may not contain any z[] values

				point = pPoints->Getitem(ip);
				if (point == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				pAttr = point->attributes->getNamedItem("x");
				if (pAttr == NULL) {
				//	AfxMessageBox(tStr);
					return FALSE;
				}
				tStr2 = pAttr->nodeValue;
				velm->xm[ip][icont] = (float)_ttof(tStr2);


				pAttr = point->attributes->getNamedItem(_T("z"));
				if (pAttr != NULL) {
					tStr2 = pAttr->nodeValue;
					velm->zm[ip][icont] = (float)_ttof(tStr2);
				}

				pAttr = point->attributes->getNamedItem(_T("t"));
				if (pAttr != NULL) {
					tStr2 = pAttr->nodeValue;
					velm->tm[ip][icont] = (float)_ttof(tStr2);
				}

				pAttr = point->attributes->getNamedItem("lat");
				if (pAttr != NULL) {
					tStr2 = pAttr->nodeValue;
					velm->latitude[ip][icont] = (float)_ttof(tStr2);
				}

				pAttr = point->attributes->getNamedItem("lon");
				if (pAttr != NULL) {
					tStr2 = pAttr->nodeValue;
					velm->longitude[ip][icont] = (float)_ttof(tStr2);
				}
			} // for ip

			if (icont >= len - 1) break;  // the last boundary is not associated with velocities

			MSXML::IXMLDOMNodePtr pTop = myGetSingleChildByName(pBoundary, _T("topVelocity"));
			MSXML::IXMLDOMNodePtr pBottom = myGetSingleChildByName(pBoundary, _T("bottomVelocity"));

			pAttr = pTop->attributes->getNamedItem("validated");
			if (icont == velm->ncontMaxConstrained - 1 && pAttr != NULL) {
				int i = pAttr->nodeValue;
				velm->isVelBelowValid = (i != 0);
			}

			strErr.Format(_T("Found invalid velocity for top of layer %d."), icont + 1);  // we may not use this error message
			if (pTop == NULL) {
			//	AfxMessageBox(tStr, MB_ICONQUESTION);
				return FALSE;
			}
			pPoints = pTop->selectNodes(_T("point"));
			if (pPoints == NULL) {
				//AfxMessageBox(tStr, MB_ICONQUESTION);
				return FALSE;
			}
			velm->nvel[icont][0] = (int)pPoints->length;
			for (long ip = 0; ip < velm->nvel[icont][0]; ip++) {
				point = pPoints->Getitem(ip);
				if (point == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				pAttr = point->attributes->getNamedItem("x");
				if (pAttr == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				velm->xvel[ip][icont][0] = pAttr->nodeValue;
				pAttr = point->attributes->getNamedItem(_T("v"));
				if (pAttr == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				velm->vf[ip][icont][0] = pAttr->nodeValue;
			} // for ip

			strErr.Format(_T("Found invalid velocity for bottom of layer %d."), icont + 1);  // we may not use this error message
			if (pBottom == NULL) {
			//	AfxMessageBox(tStr, MB_ICONQUESTION);
				return FALSE;
			}
			pPoints = pBottom->selectNodes(_T("point"));
			if (pPoints == NULL) {
			//	AfxMessageBox(tStr, MB_ICONQUESTION);
				return FALSE;
			}
			velm->nvel[icont][1] = (int)pPoints->length;
			for (long ip = 0; ip < velm->nvel[icont][1]; ip++) {
				point = pPoints->Getitem(ip);
				if (point == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				pAttr = point->attributes->getNamedItem("x");
				if (pAttr == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				velm->xvel[ip][icont][1] = pAttr->nodeValue;
				pAttr = point->attributes->getNamedItem(_T("v"));
				if (pAttr == NULL) {
				//	AfxMessageBox(tStr, MB_ICONQUESTION);
					return FALSE;
				}
				velm->vf[ip][icont][1] = pAttr->nodeValue;
			} // for ip
		} // for icont
	}
		CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL


	return TRUE;
}



int CVelMod::getVelocityIntFromRms(int ntrms, float *tau, float *vrms, float *vintOut) {
	// tau[0] should be at the surface with zero time
	if (tau == NULL || vrms == NULL || vintOut == NULL || ntrms < 2) return 0;
	int it;

	float *vrmsTrc = (float *)ealloc1float(ntrms);

	// now square it
	for (it = 0; it < ntrms; it++) {
		vrmsTrc[it] = vrms[it] * vrms[it];
	}

	// now obtain the interval velocity square
	for (it = 0; it < ntrms - 1; it++) {
		if (tau[it + 1] - tau[it] < 0.00001f) {
			vintOut[it] = vrmsTrc[it];
		}
		else {
			vintOut[it] = vrmsTrc[it + 1] * tau[it + 1] - vrmsTrc[it] * tau[it];
			vintOut[it] /= (tau[it + 1] - tau[it]);
		}
	} // for it
	vintOut[ntrms - 1] = vintOut[ntrms - 2];
	free1float(vrmsTrc);

	// showArrayInFile(&vint[0], nt);

	for (it = 0; it < ntrms; it++) {
		vintOut[it] = (float)sqrt(vintOut[it]);
	}

	return ntrms;
}

bool CVelMod::getVelocityIntFromRms(int ntrms, float *tau, float *vrms, int nt, float dt, float ft, float *vint) {
	// the results of this routine is not perrfect. Recommend to Use the simpler instead.
	if (tau == NULL || vrms == NULL || vint == NULL || ntrms < 2 || nt < 2) return false;
	int itao, it, it2, it3;
	float t, delta;

	float *vrmsTrc = (float *)ealloc1float(nt);

	// first do an interpolation of vrms[] and store into vrmsTrc[]
	for (it = 0; it < nt; it++) {
		t = ft + it*dt;
		if (t > 3.674) {
			int ter = 1;
		}
		if (t <= tau[0]) {
			vrmsTrc[it] = vrms[0];
			continue;
		} // if t
		if (t > tau[ntrms - 1] && it > 0) {
			vrmsTrc[it] = vrmsTrc[it - 1];
			continue;
		}
		vrmsTrc[it] = -1.f;
		for (itao = 0; itao < ntrms - 1; itao++) {
			if (t >= tau[itao] && t <= tau[itao + 1]) {
				// ..... vrms[itao]...vrmsTrc[it].....vrms[itao+1]....
				//.......tau[itao]....t...............tau[itao+1].....
				delta = tau[itao + 1] - tau[itao];
				if (delta < 0.00001f) {
					vrmsTrc[it] = vrms[itao];
				}
				else {
					vrmsTrc[it] = vrms[itao] + (vrms[itao + 1] - vrms[itao]) * (t - tau[itao]) / delta;
				}
				break;
			} // if 
		} // for j
		if (vrmsTrc[it] < 0.f) {
			// problem
			int fdfsd = 1;
		}
	} // for it

	// now square it
	for (it = 0; it < nt; it++) {
		vrmsTrc[it] *= vrmsTrc[it];
	}


	// do a smooth using a 3-point filter
	//vint[0] = vrmsTrc[0];
	//for (it=1; it<nt; it++) {
	//	vint[it] = 0.f;
	//	for (it2=-1; it2<=1;it2++) {
	//		it3 = it+it2;
	//		if (it3>nt-1) it3 = nt-1;
	//		vint[it] += vrmsTrc[it3];
	//	} // for it2
	//	vint[it] = vrmsTrc[it] * 0.333333f;
	//} // for it
	//for (it=0; it<nt; it++) {
	//	vrmsTrc[it] = vint[it];
	//}



	// now obtain the interval velocity square
	vint[0] = vrmsTrc[0];
	for (it = 1; it < nt; it++) {
		t = ft + it*dt;
		vint[it] = vrmsTrc[it] * t - vrmsTrc[it - 1] * (t - dt);
		vint[it] /= dt;
		if (t > 7.8f) {
			int ttty = 1;
		}
		if (vint[it] < 0.f || vint[it] > vint[it - 1] * 3.f) {
			vint[it] = vint[it - 1];
		}
	}

	// showArrayInFile(&vint[0], nt);


	// do a smooth using a 3-point filter
	for (it = 0; it < nt; it++) {
		vrmsTrc[it] = 0.f;
		for (it2 = -1; it2 <= 1; it2++) {
			it3 = it + it2;
			if (it3 < 0) it3 = 0;
			else if (it3 > nt - 1) it3 = nt - 1;
			vrmsTrc[it] += vint[it3];
		} // for it2
		vrmsTrc[it] = vrmsTrc[it] * 0.333333f;
	} // for it

	for (it = 0; it < nt; it++) {
		vint[it] = (float)sqrt(vrmsTrc[it]);
	}


	free1float(vrmsTrc);



	return true;
}


const bool CVelMod::getVelocityRmsMod(CVelMod::VELMOD_X *modX, double dz, double dist, double *twttMod, float *VrmsTopMod, float *VrmsBotMod) {
	// note: outputting VrmsMod[icont] at all model boundaries
	if (twttMod == NULL || VrmsTopMod == NULL || VrmsBotMod == NULL || modX->ncont < 2 || dz <= 0.f) return FALSE;

	int nzmaxModelAll = (int)(modX->depthMod[modX->ncont - 1] / dz + 2.0); // only plus 2 makes it complete
	if (nzmaxModelAll < 2) return FALSE;

	float *twttTrc = (float *)ealloc1float(nzmaxModelAll);
	float *vintTrc = (float *)ealloc1float(nzmaxModelAll);
	float *vrmsTrc = (float *)ealloc1float(nzmaxModelAll);
	memset((void *)&twttTrc[0], 0, sizeof(float)*nzmaxModelAll);
	memset((void *)&vintTrc[0], 0, sizeof(float)*nzmaxModelAll);
	memset((void *)&vrmsTrc[0], 0, sizeof(float)*nzmaxModelAll);

	double z;
	int iz;
	int icont;

	getTzFromZMod(modX, nzmaxModelAll, 0.f, dz, twttTrc);
	//getTwttFromVz(ncont, (double *)&depth[0],
	//	(double *)&vTop[0], (double *)&vBot[0],
	//	nzmaxModelAll, 0.f, dz,
	//	twttTrc);

	twttMod[0] = 0.f;
	for (icont = 1; icont < modX->ncont; icont++) {
		twttMod[icont] = -1.f;
		for (iz = 1; iz < nzmaxModelAll; iz++) {
			z = (double)(iz*dz);
			if (z < modX->depthMod[icont]) continue;
			if (z > modX->depthMod[icont]) {
				twttMod[icont] = (float)(twttTrc[iz - 1] + (twttTrc[iz] - twttTrc[iz - 1]) / dz * (modX->depthMod[icont] - (iz - 1)*dz));
				break;
			}
		} // for iz
		if (twttMod[icont] < 0.f) {
			// failed to obtain TWTT. this can happen when the model boundary is deeper than pointed by nzmaxModelAll
			// twttMod[icont] = twttMod[icont-1] + (twttTrc[nzmaxModelAll-1] - twttMod[icont-1]) * (depth[icont] - 
			twttMod[icont] = twttMod[icont - 1];
		}
	} // for icont

	for (icont = 0; icont < modX->ncont - 1; icont++) {
		// Note: depth[ncont-1] is the deepest boundary in the model
		float deltazMod = (float)(modX->depthMod[icont + 1] - modX->depthMod[icont]);
		for (iz = 0; iz<nzmaxModelAll; iz++) {
			z = (double)(iz*dz);
			if (z < modX->depthMod[icont]) continue;
			if (z > modX->depthMod[icont + 1]) break;
			if (deltazMod > 0.00001f)
				vintTrc[iz] = (float)(modX->vTop[icont] + (modX->vBot[icont] - modX->vTop[icont]) * (z - modX->depthMod[icont]) / deltazMod);
			else
				vintTrc[iz] = (float)modX->vTop[icont];
		} // for iz
	} // for icont

	float *nomi = (float *)ealloc1float(nzmaxModelAll);
	float *denom = (float *)ealloc1float(nzmaxModelAll);
	nomi[0] = 0.;
	denom[0] = 0.;
	for (iz = 1; iz < nzmaxModelAll; iz++) {
		nomi[iz] = nomi[iz - 1] + vintTrc[iz - 1] * vintTrc[iz - 1] * (twttTrc[iz] - twttTrc[iz - 1]);
		denom[iz] = denom[iz - 1] + (twttTrc[iz] - twttTrc[iz - 1]);
	} // for iz
	vrmsTrc[0] = vintTrc[0];
	for (iz = 1; iz < nzmaxModelAll; iz++) {
		vrmsTrc[iz] = (float)sqrt(nomi[iz] / denom[iz]);
	} // for iz
	free1float(nomi);
	free1float(denom);
	free1float(twttTrc);
	free1float(vintTrc);

	VrmsTopMod[0] = vrmsTrc[0]; // surface
	for (icont = 1; icont < modX->ncont - 1; icont++) {
		VrmsTopMod[icont] = 0.f;  // initialize
		for (iz = 1; iz < nzmaxModelAll; iz++) {
			z = (double)(iz*dz);
			if (z >= modX->depthMod[icont]) {
				VrmsTopMod[icont] = (float)(vrmsTrc[iz - 1] + (vrmsTrc[iz] - vrmsTrc[iz - 1]) / dz * (modX->depthMod[icont] - (iz - 1)*dz));
				break;
			} // if z
		} // for iz
	} // for icont

	for (icont = 0; icont < modX->ncont - 1; icont++) {
		VrmsBotMod[icont] = 0.f;  // initialize
		for (iz = nzmaxModelAll - 2; iz >= 0; iz--) {
			z = (double)(iz*dz);
			if (z < modX->depthMod[icont + 1]) {
				VrmsBotMod[icont] = (float)(vrmsTrc[iz] + (vrmsTrc[iz + 1] - vrmsTrc[iz]) / dz * (modX->depthMod[icont + 1] - z));
				break;
			} // if z
		} // for iz
	} // for icont

	free1float(vrmsTrc);
	return true;
}



const bool CVelMod::getVelocityRmsTrc(CVelMod::VELMOD_X *modX, double dz, int nzmaxModelAll, float *twttOut, float *vrmsOut) {
	if (modX->ncont < 2 || dz <= 0.f || twttOut == NULL || vrmsOut == NULL) return false;
	memset((void *)&twttOut[0], 0, sizeof(float)*nzmaxModelAll);
	memset((void *)&vrmsOut[0], 0, sizeof(float)*nzmaxModelAll);

	float *vintTrc = (float *)ealloc1float(nzmaxModelAll);
	memset((void *)&vintTrc[0], 0, sizeof(float)*nzmaxModelAll);

	double z;
	int iz;
	int icont;

	getTzFromZMod(modX, nzmaxModelAll, 0.f, dz, twttOut);
	//getTwttFromVz(ncont, (double *)&depth[0],
	//	(double *)&vTop[0], (double *)&vBot[0],
	//	nzmaxModelAll, 0.f, dz,
	//	twttOut);
	for (icont = 0; icont < modX->ncont - 1; icont++) {
		// Note: depth[ncont-1] is the deepest boundary in the model
		float deltazMod = (float)(modX->depthMod[icont + 1] - modX->depthMod[icont]);
		for (iz = 0; iz<nzmaxModelAll; iz++) {
			z = (double)(iz*dz);
			if (z < modX->depthMod[icont]) continue;
			if (z > modX->depthMod[icont + 1]) break;
			if (deltazMod > 0.00001f)
				vintTrc[iz] = (float)(modX->vTop[icont] + (modX->vBot[icont] - modX->vTop[icont]) * (z - modX->depthMod[icont]) / deltazMod);
			else
				vintTrc[iz] = (float)modX->vTop[icont];
		} // for iz
	} // for icont

	//for (iz=0; iz<nzmaxModelAll; iz++) {
	//	vintTrc[iz] = vintTrc[iz]*vintTrc[iz]; 
	//}

	float *nomi = (float *)ealloc1float(nzmaxModelAll);
	float *denom = (float *)ealloc1float(nzmaxModelAll);
	nomi[0] = 0.;
	denom[0] = 0.;
	for (iz = 1; iz < nzmaxModelAll; iz++) {
		nomi[iz] = nomi[iz - 1] + vintTrc[iz - 1] * vintTrc[iz - 1] * (twttOut[iz] - twttOut[iz - 1]);
		denom[iz] = denom[iz - 1] + (twttOut[iz] - twttOut[iz - 1]);
	} // for iz
	vrmsOut[0] = vintTrc[0];
	for (iz = 1; iz < nzmaxModelAll; iz++) {
		vrmsOut[iz] = (float)sqrt(nomi[iz] / denom[iz]);
	} // for iz
	free1float(nomi);
	free1float(denom);
	free1float(vintTrc);

	return true;
}


// static function
CString CVelMod::getPseudoWellText(CVelMod::VELMOD *velm, 
	double dist, 
	double dz, 
	bool isTime)
{
	// 2. print out at decimal scales
	if (velm->ncont < 1) return _T("");

	int icont;
	CString tStr;
	CVelMod::VELMOD_X modX;
	modX.dist = dist;
	CVelMod vm;
	velm->m_isConvert = true;
	vm.initModX(velm, &modX, isTime, true, dz);


	float *VrmsTopMod = (float *)ealloc1float(modX.ncont);
	float *VrmsBotMod = (float *)ealloc1float(modX.ncont);
	vm.getVelocityRmsMod(&modX, dz, dist, modX.timeMod, VrmsTopMod, VrmsBotMod);

	CStringArray arr;
	//tStr.Format(_T("#Pseudo-well at Model Distance %7.3f (offset %7.3f km; V4 marker at %7.3f km)(Params: %7.3f km/s; %7.3f km)\n")
	//	, (float)dist
	//	);
	//arr.Add(tStr);
	tStr.Format(_T("#TWT    VEL    DEPTH   VRMS   LAYER (s-km/s-km-km/s)"));
	arr.Add(tStr);

	for (icont = 0; icont < modX.ncont - 1; icont++) {
		if (icont > velm->ncontMaxConstrained - 1)
			break;

		float tz = (float)modX.timeMod[icont];
		float zz = (float)modX.depthMod[icont];
		if (icont > 0) {
			tz += 0.001f;
			zz += 0.001f;
		}
		tStr.Format(_T("%6.3f %6.3f %6.3f %6.3f  %2d"), tz, modX.vTop[icont], zz, VrmsTopMod[icont], icont + 1);
		arr.Add(tStr);

		if (icont == velm->ncontMaxConstrained - 1)
			break;

		tz = (float)modX.timeMod[icont + 1];
		zz = (float)modX.depthMod[icont + 1];
		if (tz - modX.timeMod[icont] < 0.001f) {
			tz = (float)modX.timeMod[icont] + 0.001f;
			zz = (float)modX.depthMod[icont] + 0.001f;
		}
		tStr.Format(_T("%6.3f %6.3f %6.3f %6.3f  %2d"), tz, modX.vBot[icont], zz, VrmsBotMod[icont], icont + 1);
		arr.Add(tStr);
	}

	free1float(VrmsTopMod);
	free1float(VrmsBotMod);

	CString out = convStringArrayToCString(&arr, true);
	arr.RemoveAll();
	return out;
}

 
// depth[4000],vTop[4000],vBot[4000] // these 3 arrays must be big enough to expand
//	ncont = formatVelocityArray(dvUniform, velMod.ncont, 4000, (double *)&depth[0], (double *)&vTop[0], (double *)&vBot[0]);
int CVelMod::formatVelocityArray(float dvUniform, int ncont, int nmax, double depth[], double vTop[], double vBot[])
{
	if (dvUniform <= 0.f) dvUniform = 0.01f;
	if (ncont >= nmax) return ncont; // we cannot split any more since the array size limit is reached

	for (int i = 0; i < ncont; i++) {
		if (fabs(vBot[i] - vTop[i]) > dvUniform) {
			// if here, the velocity gradient is too great, we split the layer into smaller pieces 
			for (int icont = ncont - 1; icont > i; icont--) {
				depth[icont + 1] = depth[icont];
				vTop[icont + 1] = vTop[icont]; // no meaning for vTop[ncont]
				vBot[icont + 1] = vBot[icont];
			}
			depth[i + 1] = depth[i] + dvUniform * (depth[i + 1] - depth[i]) / (vBot[i] - vTop[i]);
			vTop[i + 1] = vTop[i] + dvUniform;
			vBot[i + 1] = vBot[i];
			vBot[i] = vTop[i + 1];
			ncont++;
			return formatVelocityArray(dvUniform, ncont, nmax, (double *)&depth[0], (double *)&vTop[0], (double *)&vBot[0]);
		}
	} // for i
	return ncont;
}



const bool CVelMod::getZtFromTmodGrad2(CVelMod::VELMOD_X *modX, int nt, double dt, double depth[]) {

	if (depth == NULL || modX->ncont < 2 || nt < 2 || dt <= 0.f) return false;

	if (!getZmodFromTmodGrad(modX, dt)) return false;

	double dth = 0.5 * dt;
	//double dzInc[MOD_LAYER_MAX];
	//for (int ilayer = 0; ilayer<modX->ncont - 1; ilayer++) {
	//	register double deltat = modX->timeMod[ilayer + 1] - modX->timeMod[ilayer];
	//	register double v2ov1 = modX->vBot[ilayer] / modX->vTop[ilayer];
	//	register double betat = log(v2ov1) / modX->vTop[ilayer];
	//	dzInc[ilayer] = (fabs(betat)<0.00001 || fabs(deltat)<0.00001) ?
	//		dth * modX->vTop[ilayer] :
	//	(v2ov1 - 1.) * modX->vTop[ilayer] * dth / log(v2ov1);
	//}

	double depthWater = modX->depthMod[NUM_WATER_LAYER];
	double twtWater = modX->timeMod[NUM_WATER_LAYER];
	depth[0] = 0.;
	register double v;
	for (int it = 1; it<nt; it++) {
		register double t = it*dt;
		depth[it] = -1.;  // initialize value

		// first search for proper velocity for the current depth
		for (int ilayer = 0; ilayer < modX->ncont - 1; ilayer++) {
			if (t >= modX->timeMod[ilayer] && t <= modX->timeMod[ilayer+1]) {
				if (modX->isSedimentRegression(ilayer)) {
					depth[it] = depthWater + modX->regTime2Depth(t - twtWater, dt);
				}
				else {
					// adjust t before committing
					t -= dth;
					if (t < modX->timeMod[ilayer]) t = modX->timeMod[ilayer];
					register double dtMod = modX->timeMod[ilayer + 1] - modX->timeMod[ilayer];
					v = modX->vTop[ilayer];
					if (dtMod > 0.)
						v += (t - modX->timeMod[ilayer])*(modX->vBot[ilayer] - modX->vTop[ilayer]) / dtMod;
					depth[it] = depth[it - 1] + dth * v;
				}
				break;
			}
		} // for ilayer

		if (depth[it] < 0.) {
			// unhandled depth[]
			depth[it] = depth[it - 1] + modX->vBot[modX->ncont-2]*dth;
		}
	} // for it

	return true;
}

/*
this simple function 
*/
const bool CVelMod::getTmodFromZmodGrad(CVelMod::VELMOD_X *modX, double dz) {
	if (modX->ncont < 2) return false;

	// fix up some intesecting horizons problems
	for (int ilayer = 0; ilayer<modX->ncont - 1; ilayer++) {
		if (modX->depthMod[ilayer] > modX->depthMod[ilayer + 1]) 
			modX->depthMod[ilayer] = modX->depthMod[ilayer + 1];
	}

	modX->timeMod[0] = 0.;
	double depthWater = modX->depthMod[NUM_WATER_LAYER];
	for (int ilayer = 0; ilayer < modX->ncont - 1; ilayer++) {
		if (modX->isSedimentRegression(ilayer)) {
			//// now we deal with the small gap under z
			//deltat += (deltaz - z) / v;
			register double delta = modX->regDepth2Time(modX->depthMod[ilayer + 1] - depthWater);
			modX->timeMod[ilayer + 1] = delta + modX->timeMod[NUM_WATER_LAYER];
		}
		else {
			register double oVtop = 1. / modX->vTop[ilayer];
			register double v2ov1 = modX->vBot[ilayer] * oVtop;
			////double dzh = dz * 0.5; // for speed up work
			//if (fabs(deltaz) < 0.00001 || (v2ov1-1.) < 0.00001) {
			//	// constant vel or too thin
			//	deltat = 2. * deltaz * oVtop;
			//}
			//else {
			//	// gradient case
			//	// register double beta = (deltaz > 0.00001) ? (v2ov1-1.) / deltaz : 0.;
			//	// deltat = 2. * log(1. + beta * deltaz) * oVtop / beta;
			//	deltat = 2. * deltaz * oVtop * log(v2ov1) / (v2ov1 - 1.);
			//}

			register double deltat = 2. * oVtop * getVKai(v2ov1) * (modX->depthMod[ilayer + 1] - modX->depthMod[ilayer]);
			modX->timeMod[ilayer + 1] = modX->timeMod[ilayer] + deltat;

			// testing for reverse transformation
			//if (modX->dist == -11.16217f) {
			//	register double beta = (deltaz > 0.00001) ? (v2ov1 - 1.) / deltaz : 0.;
			//	double testDeltaZ = (v2ov1 - 1.) * modX->vTop[ilayer] * deltat*0.5 / log(v2ov1);
			//	double diffA = testDeltaZ - deltaz;
			//}
		}
	} // for ilayer

	return true;
}


/*
this simple function may produce vertically shifted depth in very complex models
since it assumes linear interpolation in the time domain;
however, the linear interpolation is only supposed to be in the depth domain, which,
when transformed to time domain, may not be a straight line. So use with care!!!
*/
bool const CVelMod::getZmodFromTmodGrad(CVelMod::VELMOD_X *modX, double dt) {
	if (modX->ncont < 2) return false;

	for (int ilayer = 0; ilayer<modX->ncont - 1; ilayer++) {
		if (modX->timeMod[ilayer] > modX->timeMod[ilayer + 1])
			modX->timeMod[ilayer] = modX->timeMod[ilayer + 1];
	}

	modX->depthMod[0] = 0.;
	double twtWater = modX->timeMod[NUM_WATER_LAYER];
	for (int ilayer = 0; ilayer < modX->ncont - 1; ilayer++) {
		if (modX->isSedimentRegression(ilayer)) {
			// Note: even though vTop and vBot already has regression embedded, the regressioned velocity is not linear inside the layer,
			//		so we should calculate in detail
			register double depthWater = modX->depthMod[NUM_WATER_LAYER]; // this is defined during this loop
			register double delta = modX->regTime2Depth(modX->timeMod[ilayer + 1] - twtWater, dt);
			modX->depthMod[ilayer + 1] = depthWater + delta;
		}
		else {
			register double oVtop = 1. / modX->vTop[ilayer];
			register double v2ov1 = modX->vBot[ilayer] * oVtop; // if (v2ov1 < 0.) v2ov1 = 1.;
			register double deltath = (modX->timeMod[ilayer + 1] - modX->timeMod[ilayer]) * 0.5;
			//register double betat = log(v2ov1) * oVtop;
			//if (fabs(betat) < 0.00001 || fabs(deltath) < 0.00001) {
			//	// constant vel or too thin layer
			//	deltaz = modX->vTop[ilayer] * deltath;
			//}
			//else {
			//	// gradient case
			//	// deltaz = (v2ov1 - 1.) * modX->vTop[ilayer] * deltath / log(v2ov1);
			//	deltaz = deltath * (v2ov1 - 1.) / betat;
			//}

			// 1-way depth increment, to calculate
			register double deltaz = deltath * modX->vTop[ilayer] / getVKai(v2ov1);
			modX->depthMod[ilayer + 1] = modX->depthMod[ilayer] + deltaz;
		}
	} // for ilayer

	return true;
}

// this will only modify modX->depthMod[icontWorking]. Please make sure 
// that before calling this routine, these values must be correct:
//   modX->depthMod[0:icontWorking-1], and modX->timeMod[0:icontWorking]
const bool CVelMod::getZmodFromTmodGrad2(CVelMod::VELMOD_X *modX, int icontWorking, double dt) {
	int ilayerWorking = icontWorking - 1;
	if (modX->ncont < 2 || icontWorking<0 || icontWorking >= modX->ncont) return false;
	if (modX->isSedimentRegression(ilayerWorking)) {
			// Note: even though vTop and vBot already has regression embedded, the regressioned velocity is not linear inside the layer,
			//		so we should calculate in detail
			register double depthWater = modX->depthMod[NUM_WATER_LAYER]; // this MUST be pre-defined 
			double twtWater = modX->timeMod[NUM_WATER_LAYER];
			register double delta = modX->regTime2Depth(modX->timeMod[icontWorking] - twtWater, dt);
			modX->depthMod[icontWorking] = depthWater + delta;
	}
	else {
			register double v2ov1 = modX->vBot[ilayerWorking] / modX->vTop[ilayerWorking]; // if (v2ov1 < 0.) v2ov1 = 1.;
			register double deltath = (modX->timeMod[icontWorking] - modX->timeMod[ilayerWorking]) * 0.5;
			//register double betat = log(v2ov1) / modX->vTop[ilayerWorking];
			//if (fabs(betat) < 0.00001 || fabs(deltath) < 0.00001) {
			//	// constant vel or too thin layer
			//	deltaz = modX->vTop[ilayerWorking] * deltath;
			//}
			//else {
			//	// gradient case
			//	deltaz = (v2ov1 - 1.) * modX->vTop[ilayerWorking] * deltath / log(v2ov1);
			//}
			register double deltaz = deltath * modX->vTop[ilayerWorking] / getVKai(v2ov1);
			modX->depthMod[icontWorking] = modX->depthMod[ilayerWorking] + deltaz;
	}

	return true;
}

const bool CVelMod::getTzFromZModGrad(CVelMod::VELMOD_X *modX, int nz, double dz, double Tz[]) {
	if (Tz == NULL || modX->ncont < 2 || nz < 2 || dz <= 0.f) return false;

	for (int ilayer = 0; ilayer<modX->ncont - 1; ilayer++) {
		if (modX->depthMod[ilayer] > modX->depthMod[ilayer + 1]) {
			// abnormal layering
			modX->depthMod[ilayer] = modX->depthMod[ilayer + 1];
		}
	}

	if (!getTmodFromZmodGrad(modX, dz)) return false;

	double dz2 = 2. * dz;
	double dtvInc[MOD_LAYER_MAX], betas[MOD_LAYER_MAX];
	for (int ilayer = 0; ilayer<modX->ncont - 1; ilayer++) {
		register double deltaz = modX->depthMod[ilayer + 1] - modX->depthMod[ilayer];
		register double v2ov1 = modX->vBot[ilayer] / modX->vTop[ilayer];
		betas[ilayer] = (deltaz > 0.00001) ? (v2ov1 - 1.) / deltaz : 0.;
		dtvInc[ilayer] = (fabs(betas[ilayer])<0.00001 || fabs(deltaz)<0.00001) ?
dz :  log(1. + betas[ilayer] * dz) / betas[ilayer];
	}
	double rVZero = fabs(modX->regVZero) == 0. ? 0. : 1. / modX->regVZero;
	double rVInf = fabs(modX->regVInf) == 0. ? 0. : 1. / modX->regVInf;
	double rRegVmin = 1. / modX->regVmin;
	double rRegVmax = 1. / modX->regVmax2;
	Tz[0] = 0.f;
	for (int iz = 1; iz < nz; iz++) {
		register double z = iz*dz;
		Tz[iz] = -1.;

		// first search for proper velocity for the current depth
		for (int ilayer = 0; ilayer < modX->ncont - 1; ilayer++) {
				register double zTop = modX->depthMod[ilayer];
				if (z >= modX->depthMod[ilayer] && z <= modX->depthMod[ilayer + 1]) {
					// it points inside the layer icont in between [z1, z2] 
						// register double vov0 = 1. + betas[ilayer]*(z-dz-zTop); // V/V0 = 1 + beta*dz
					if (modX->isSedimentRegression(ilayer)) {
						// consider regression
						// Note: even though vTop and vBot already has regression embedded, the regressioned velocity is not linear inside the layer,
						//		so we should calculate in detail
						register double rv1 = -1.;
						register double rv2 = -1.;
						rv1 = rVInf + (rVZero - rVInf) * exp(-modX->regAlpha * (z - dz - zTop));
						rv2 = rVInf + (rVZero - rVInf) * exp(-modX->regAlpha * (z - zTop));
						if (rv1>rRegVmin) rv1 = rRegVmin;
						if (rv1<rRegVmax) rv1 = rRegVmax;
						if (rv2>rRegVmin) rv2 = rRegVmin;
						if (rv2<rRegVmax) rv2 = rRegVmax;
						Tz[iz] = Tz[iz - 1] + dz * (rv1 + rv2);
						break;
					}
					else {
						register double v1 = modX->vTop[ilayer] * (1. + betas[ilayer] * (z - dz - zTop)); // v1 is the calculated velocity at z-dz;
						//register double v2 = modX->vTop[ilayer] * (1. + betas[ilayer] * (z - zTop)); // v2 is the calculated velocity at z;
						//Tz[iz] = Tz[iz - 1] + dtvInc[ilayer] * (v1 + v2) / (v1*v2);  // I later think this is not very accurate
						Tz[iz] = Tz[iz - 1] + dtvInc[ilayer] * 2. / v1; // this is very accurate since dtvInc[] already contains linear gradient formula
						break;
					}
				}
		} // for icont
		if (Tz[iz] < 0.) {
			// not handled yet. Please do something to fix it
			Tz[iz] = Tz[iz - 1] + dz2 / modX->vTop[modX->ncont-2];
		}
	} // for iz

	return true;
}


void CVelMod::loadInitVel(VELMODS *velms, CString str, double dz) {
	CStringArray arrVelNamesEx;
	bool isTrue = velNamesExParse(str, velms->xShiftArr, velms->xposArr, &arrVelNamesEx);
	velms->num = (int)arrVelNamesEx.GetCount();
	if (velms->num > MOD_LOAD_MAX) velms->num = MOD_LOAD_MAX;
	
	// Help!!! need to sort the distance of xShiftArr[], no Need
	
	CVelMod vm;
	CString tStr;
	for (int i = 0; i < velms->num; i++) {
		vm.loadInitVel(&velms->velm[i], arrVelNamesEx.ElementAt(i), dz, tStr);
	}
}


// If the parsing not succeed, it means the string is not valid info. This is important to have.
// Important: make sure xposArr[] and xShiftArr[] lengths do not exceed limits, cannot checked inside this routine
bool CVelMod::velNamesExParse(CString str, double xShiftArr[], double xposArr[], CStringArray *arrVelNamesEx)
{
	if (arrVelNamesEx == NULL) return false;
	arrVelNamesEx->RemoveAll();
	CString tStr, tStr2, tStr3;


	float xShift;
	float xpos;
	//const CString strNewLine = CARRAGE;

	// first, reformat each line for proper column extraction later
	CStringArray arr;
	if (str.GetLength()>0) {
		if (str.Find(_T(",")) < 0) {
			// if here, the string does not contain any commas. I guess that the user has chosen to use space instead of commas for delimation
			str.Replace(_T("  "), SPACE);  // first make sure only one space for each delimation
			str.Replace(SPACE, CARRAGE); // now all space will be line feed
		}
		else {
			str.Replace(_T(",\n"), CARRAGE);
			str.Replace(_T(",\r\n"), CARRAGE);
			str.Replace(_T(","), CARRAGE);
		}
		SplitStringArr(str, CARRAGE, &arr, false);
		for (int i=0; i<arr.GetCount(); i++) {
			tStr = arr.ElementAt(i).Trim();
			xShift = xpos = ERRORVAL;
			if (IsNumeric(tStr)) {
				xShift = (float)StringToFloat(arr.ElementAt(i).Trim());
				i++;
			}
			tStr = arr.ElementAt(i).Trim();
			if (IsNumeric(tStr)) {
				xpos = (float)StringToFloat(arr.ElementAt(i).Trim());
				i++;
			}

			tStr = arr.ElementAt(i).Trim();
			if (!IsNumeric(tStr)) {
				// example:  80\r\n115\r\nG:\arcticSBs\sb2007\sb21\model0721New.bin
				tStr2.Format(_T("%g %g %s"), xShift, xpos, tStr);
				arrVelNamesEx->Add(tStr2);
			}
		} // for i
	} // if
	arr.RemoveAll();

	// now pass for xShiftArr[] and xposArr[].
	// NOTE: After that, arrVelNamesEx will contain only the velocity file names!!!
	CString stmp;
	TCHAR ch[256];
	int ii = 0;
	for (int i=0; i<arrVelNamesEx->GetCount(); i++) {
		// example:  80, 115, G:\arcticSBs\sb2007\sb21\model0721New.bin
		tStr = arrVelNamesEx->ElementAt(i);
		// if ( _stscanf_s(tStr, _T("%*f,%*f,%s"),&xposArr[i],&xShiftArr[i], tStr) == 0) {
		// if ( _stscanf_s(arrVelNamesEx->ElementAt(i), _T("%[^,],%[^,],%s"), tStr, tStr2, tStr2) == 0) {
		if (_stscanf_s(tStr, _T("%g %g %s"), &xShift, &xpos, ch, 256) != 0) {
			xShiftArr[ii] = xShift;
			xposArr[ii] = xpos;
			arrVelNamesEx->SetAtGrow(ii, ch);
			ii++;
		}
	} // for i

	return true;
}

// if nothing to exclude, set iExclude=-1
CString CVelMod::velNamesExToString(double xShiftArr[], double xposArr[], CStringArray *arrVelNamesEx, int iExclude) {
	CString tStr, tStr2;
	for (int i = 0; i<arrVelNamesEx->GetCount(); i++) {
		if (iExclude == i) continue;
		tStr2.Format(_T("%g  %g  %s"), xShiftArr[i], xposArr[i], arrVelNamesEx->ElementAt(i));
		tStr += tStr2;
		if (i < arrVelNamesEx->GetCount() - 1)
			tStr += CARRAGE;
	}
	return tStr;
}


CString CVelMod::velNamesExReformat(CString str) {
	CStringArray *arrVelNamesEx = new CStringArray();
	double xposArr[MOD_POINTS_MAX];
	double xShiftArr[MOD_POINTS_MAX];
	velNamesExParse(str, xShiftArr, xposArr, arrVelNamesEx);

	CString tStr;
	tStr = velNamesExToString(xShiftArr, xposArr, arrVelNamesEx);
	//for (int i=0; i<arrVelNamesEx->GetCount(); i++) {
	//	tStr2.Format(_T("%g  %g  %s"), xShiftArr[i], xposArr[i], arrVelNamesEx->ElementAt(i));
	//	tStr += tStr2;
	//	if (i < arrVelNamesEx->GetCount() - 1)
	//		tStr += CARRAGE;
	//}

	arrVelNamesEx->RemoveAll();
	delete arrVelNamesEx;
	arrVelNamesEx = NULL;
	return tStr;
}




//###############################################
// not recommended!!!
BOOL CVelMod::GetVelGrid(float fx, int nx, float dx, float fz, int nz, float dz, float *v, CVelMod::VELMOD *velm)
{
	//this routine calculates a velocity grid based on information
	//in struct velMod.
	/* Required parameters:
	(1) global struct velMod has all velocity information stored.
	(2) global tpp, a sample interval.
	Output: vt[0 to nt-1], with vt[0] pointing to sample it0.
	written by: Deping Chian on Aug. 1, 1997.
	*/

	TCHAR s[80];
	long iz, icont, ix, j;
	WORD totShot = 0;
	double z, depth[MOD_LAYER_MAX], vTop[MOD_LAYER_MAX], vBot[MOD_LAYER_MAX];
	float range;
	double x1, x2, x3, x4, v1, v2, v3, v4;
	BOOL vStat;
	for (ix = 0; ix < nx; ix++) {
		range = fx + ix*dx;

		for (icont = 0; icont < velm->ncont; icont++) {
			if (range < velm->xm[0][icont])  {
				depth[icont] = velm->zm[0][icont];
				continue;
			}
			if (range > velm->xm[velm->nzed[icont] - 1][icont])  {
				depth[icont] = velm->zm[velm->nzed[icont] - 1][icont];
				continue;
			}

			for (j = 1; j < velm->nzed[icont]; j++) {
				if (range >= velm->xm[j - 1][icont] && range <= velm->xm[j][icont]) {
					if (fabs(velm->xm[j][icont] - velm->xm[j - 1][icont]) < 0.00001f)
						depth[icont] = velm->zm[j][icont];
					else
						depth[icont] = velm->zm[j - 1][icont] + (range - velm->xm[j - 1][icont])*
						(velm->zm[j][icont] - velm->zm[j - 1][icont]) / (velm->xm[j][icont] - velm->xm[j - 1][icont]);
					break;
				}
			}
		}

		//_stprintf(s, "Depth: %d %d %d %d",(long)(1000*depth[0]),(long)(1000*depth[1]),(long)(1000*depth[2]),(long)(1000*depth[3]));
		//AfxMessageBox(s);
		//Now locate the four corners that define the velocity of for all points at range.
		//*****x1***********x2******* with range at depth[icont]
		//**********range************
		//***************************
		//**x3*********************x4 with range at depth[icont+1]
		for (icont = 0; icont < velm->ncont - 1; icont++) {
			if (range < velm->xm[0][icont])
			{
				vTop[icont] = velm->vf[0][icont][0];
				vBot[icont] = velm->vf[0][icont][1];
				continue;
			}
			if (range > velm->xm[velm->nzed[icont] - 1][icont])
			{
				vTop[icont] = velm->vf[velm->nvel[icont][0] - 1][icont][0];
				vBot[icont] = velm->vf[velm->nvel[icont][1] - 1][icont][1];
				continue;
			}
			for (j = 1; j < velm->nvel[icont][0]; j++)
			{
				if (range >= velm->xvel[j - 1][icont][0] && range <= velm->xvel[j][icont][0])
				{
					x1 = velm->xvel[j - 1][icont][0];
					x2 = velm->xvel[j][icont][0];
					v1 = velm->vf[j - 1][icont][0];
					v2 = velm->vf[j][icont][0];
					break;
				}
			}
			for (j = 1; j < velm->nvel[icont][1]; j++)
			{
				if (range >= velm->xvel[j - 1][icont][1] && range <= velm->xvel[j][icont][1])
				{
					x3 = velm->xvel[j - 1][icont][1];
					x4 = velm->xvel[j][icont][1];
					v3 = velm->vf[j - 1][icont][1];
					v4 = velm->vf[j][icont][1];
					break;
				}
			}
			vTop[icont] = v1 + (v2 - v1)*(range - x1) / (x2 - x1);
			vBot[icont] = v3 + (v4 - v3)*(range - x3) / (x4 - x3);
		}

		//Bilinear interpolation and stores into vTrc[]:
		//Note that depth[0:nlayer], while vBot[0:nlayer-1].
		for (iz = 0; iz<nz; iz++)
		{
			z = fz + iz*dz;
			vStat = FALSE;
			if (z > depth[velm->ncont - 1])
			{
				v[ix*nz + iz] = (float)vBot[velm->ncont - 1 - 1];
				vStat = TRUE;
				continue;
			}
			for (icont = 0; icont < velm->ncont - 1; icont++)
			{
				if (depth[icont] >= (depth[icont + 1] - MINAMP))
				{
					if (z >= depth[icont] && z <= depth[icont + 1])
					{
						vStat = TRUE;
						v[ix*nz + iz] = (float)(vTop[icont] + vBot[icont])*0.5f;
						break;
					}
					else continue;
				}
				else if (z >= depth[icont] && z <= depth[icont + 1])
				{
					v[ix*nz + iz] = (float)(vTop[icont] + (vBot[icont] - vTop[icont])*
						(z - depth[icont]) / (depth[icont + 1] - depth[icont]));
					vStat = TRUE;
					break;
				}
				else continue;
			}
			if (!vStat)
			{
				_stprintf_s(s, _T("Depth z= %d in meters not determined, Stop!"), (long)(z * 1000));
				AfxMessageBox(s);
				return FALSE;
			}

		}  //end of iz loop.
	}  //end of ix loop.
	for (ix = 0; ix < nx; ix++)
		for (iz = 0; iz < nz; iz++)
			v[ix*nz + iz] *= 1000.f;

	return TRUE;
}


CString CVelMod::checkModelError(CVelMod::VELMOD *velm)
{
	int icont, ix, nx, j;
	double range, depth[MOD_LAYER_MAX][500];
	nx = 500;
	double dxVDesired = (velm->getXmax() - velm->getXmin()) / (double)(nx-1);

	for (ix = 0; ix < nx; ix++) {
		if (ix == (nx - 1)) range = velm->getXmax();
		else range = velm->getXmin() + dxVDesired*(double)ix;
		for (icont = 0; icont < velm->ncont; icont++) {
			for (j = 1; j < velm->nzed[icont]; j++) {
				if (range >= velm->xm[j - 1][icont] && range <= velm->xm[j][icont])
				{
					if (fabs(velm->xm[j][icont] - velm->xm[j - 1][icont]) < 0.00001f)
						depth[icont][ix] = velm->zm[j][icont];
					else
						depth[icont][ix] = velm->zm[j - 1][icont] + (range - velm->xm[j - 1][icont])*
						(velm->zm[j][icont] - velm->zm[j - 1][icont]) / (velm->xm[j][icont] - velm->xm[j - 1][icont]);
					break;
				}
			}
		}
	}

	for (ix = 0; ix < nx; ix++) {
		for (icont = 1; icont < velm->ncont; icont++) {
			if (depth[icont][ix] < depth[icont - 1][ix]) {
				if (ix == (nx - 1)) range = velm->getXmax();
				else range = velm->getXmin() + dxVDesired*(double)ix;
				CString tStr;
				tStr.Format(_T("Error due to boundary overlap detected during velocity interpolation \n   at boundary no.: %2d, distance: %10.4f, with depth of %10.4f < the above boundary which is at %10.4f"), icont + 1, range, depth[icont][ix], depth[icont - 1][ix]);
				return tStr;
			}
		}
	}

	return _T("");
}

void CVelMod::DrawVelocityNodeValueTwtt(CDC* pDC, CCoords::SEI_COORD* pcd, CVelMod::VELMOD* velm, double xShift, float fLabelDistance3, int iLayerFocus, double dz)
{
	if (!isFileExistNotDirectory(velm->velSecName)) return;
	if (iLayerFocus < 0 || iLayerFocus >= velm->ncont - 1) return;
	CString tStr;

	// Initialize a LOGFONT structure for the fonts.
	LOGFONT logFontNum;  // font for drawing numbers
	pDC->GetCurrentFont()->GetLogFont(&logFontNum);
	float xyRatio = (float)logFontNum.lfWidth / (float)logFontNum.lfHeight;
	int numXLogical = (int)myRound(fLabelDistance3 * pcd->device2Logical * pcd->getDefDisplayAdjust(pDC) * 12.);
	int numYLogical = (int)(numXLogical / xyRatio);
	logFontNum.lfHeight = numYLogical;
	logFontNum.lfWidth = numXLogical;
	logFontNum.lfEscapement = 0;
	logFontNum.lfOrientation = 0;
	logFontNum.lfWeight = FW_NORMAL;
	logFontNum.lfItalic = 0;
	logFontNum.lfUnderline = 0;
	logFontNum.lfStrikeOut = 0;
	logFontNum.lfCharSet = ANSI_CHARSET;
	logFontNum.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFontNum.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFontNum.lfQuality = DEFAULT_QUALITY;
	logFontNum.lfPitchAndFamily = FF_SWISS;
	_tcscpy_s(logFontNum.lfFaceName, _T("Arial"));

	CFont fontSmall;
	fontSmall.CreateFontIndirect(&logFontNum);

	CFont* oldFont;
	oldFont = pDC->SelectObject(&fontSmall);

	CFont fontBold;
	logFontNum.lfWeight = FW_BOLD;
	fontBold.CreateFontIndirect(&logFontNum);

	int ixPlot = 0;
	int ixLogicalShifted = 0;
	int i;
	CPoint p0;
	CPoint p1, p2;

	CVelMod vm;
	CVelMod::VELMOD_X modX;
	int icont = iLayerFocus;
	double x;
	pDC->SetTextColor(RGB(0, 255, 200));

	// top boundary of the layer
	bool isPlotPointExist = false;
	double vPlot;
	int num = velm->nvel[icont][0];
	for (i = 0; i < num; i++) {
			x = velm->xvel[i][icont][0];
			if (x + xShift >= pcd->X1() && x + xShift <= pcd->X2()) {
				isPlotPointExist = true;
				break;
			}
	}
	if (!isPlotPointExist) {
			modX.dist = pcd->X1();
			velm->m_isConvert = true;
			vm.initModX(velm, &modX, true, true, dz);
			p1 = pcd->mm_coord(modX.dist + xShift, (float)modX.timeMod[icont]);
			if (modX.timeMod[icont] >= pcd->Y1() && modX.timeMod[icont] <= pcd->Y2()) {
				vPlot = velm->vf[0][icont][0];
				if (vPlot < 4.)
					tStr.Format(_T("%1.4g"), myFloat(vPlot, 4));
				else
					tStr.Format(_T("%1.3g"), myFloat(vPlot, 3));
				pDC->TextOut(p1.x + 10 + ixLogicalShifted, p1.y, tStr);
			} // if
	}
	else {
			velm->m_isConvert = true;
			for (i = 0; i < num; i++) {
				x = velm->xvel[i][icont][0];
				if (x + xShift >= pcd->X1() && x + xShift <= pcd->X2()) {
					modX.dist = x;
					vm.initModX(velm, &modX, true, true, dz);
					p1 = pcd->mm_coord(x + xShift, (float)modX.timeMod[icont]);
					if (modX.timeMod[icont] >= pcd->Y1() && modX.timeMod[icont] <= pcd->Y2()) {
						vPlot = modX.vTop[iLayerFocus];
						if (vPlot < 4.)
							tStr.Format(_T("%1.4g"), myFloat(vPlot, 4));
						else
							tStr.Format(_T("%1.3g"), myFloat(vPlot, 3));
						pDC->TextOut(p1.x + 10 + ixLogicalShifted, p1.y, tStr);
					} // if
				}
			}
	}

	// bottom boundary of the layer
	isPlotPointExist = false;
	num = velm->nvel[icont][1];
	for (i = 0; i < num; i++) {
			x = velm->xvel[i][icont][1];
			if (x + xShift >= pcd->X1() && x + xShift <= pcd->X2()) {
				isPlotPointExist = true;
				break;
			}
	}
	if (!isPlotPointExist) {
			modX.dist = pcd->X1();
			velm->m_isConvert = true;
			vm.initModX(velm, &modX, true, true, dz);
			p1 = pcd->mm_coord(modX.dist + xShift, (float)modX.timeMod[icont]);
			p2 = pcd->mm_coord(modX.dist + xShift, (float)modX.timeMod[icont + 1]);
			if (modX.timeMod[icont + 1] >= pcd->Y1() && modX.timeMod[icont + 1] <= pcd->Y2()) {
				if (abs(p2.y - p1.y) > logFontNum.lfHeight + 2) {
					vPlot = velm->vf[0][icont][1];
					if (vPlot < 4.)
						tStr.Format(_T("%1.4g"), myFloat(vPlot, 4));
					else
						tStr.Format(_T("%1.3g"), myFloat(vPlot, 3));
					pDC->TextOut(p2.x + 10 + ixLogicalShifted, p2.y - logFontNum.lfHeight - 2, tStr);
				}
			} // if
	}
	else {
			velm->m_isConvert = true;
			for (i = 0; i < num; i++) {
				x = velm->xvel[i][icont][1];
				if (x + xShift >= pcd->X1() || x + xShift <= pcd->X2()) {
					modX.dist = x;
					vm.initModX(velm, &modX, true, true, dz);
					p1 = pcd->mm_coord(x + xShift, (float)modX.timeMod[icont]);
					p2 = pcd->mm_coord(x + xShift, (float)modX.timeMod[icont + 1]);
					if (modX.timeMod[icont + 1] >= pcd->Y1() && modX.timeMod[icont + 1] <= pcd->Y2()) {
						bool isOnTheSide = i == 0 || i == num - 1;
						if (!isOnTheSide || (isOnTheSide && abs(p2.y - p1.y) > logFontNum.lfHeight + 2)) {
							// vPlot = velm->vf[i][icont][1];
							vPlot = modX.vBot[iLayerFocus];
							//tStr.Format(_T("%1.3g"), myFloat(vPlot, 3));
							if (vPlot < 4.)
								tStr.Format(_T("%1.4g"), myFloat(vPlot, 4));
							else
								tStr.Format(_T("%1.3g"), myFloat(vPlot, 3));
							pDC->TextOut(p2.x + 10 + ixLogicalShifted, p2.y - logFontNum.lfHeight - 2, tStr);
						}
					} // if
				}
			}
	}


	pDC->SelectObject(oldFont);

}

void CVelMod::DrawVelocityLabelTwtt2(CDC* pDC, CCoords::SEI_COORD *pcd,
	CVelMod::VELMOD *velm, double xShift, double xWell, COLORREF cr,
	float fLabelDistance3, bool isMainModelShowing, BOOL isShowDetails,
	int iLayerFocus, int drawnodes, double dz)
{
	if (!isFileExistNotDirectory(velm->velSecName)) return;
	double xpos = xWell;
	//velm->wells[0].wellDist;  // note: xpos does not include shifting; you may add to it yourself
	if (!pcd->isVisibleOrNot(xpos + xShift)) {
		if (isMainModelShowing && drawnodes == 2)
			DrawVelocityNodeValueTwtt(pDC, pcd, velm, xShift, fLabelDistance3, iLayerFocus, dz);
		return;
	}
//	void DrawVelocityNodeValueTwtt(CDC * pDC, CCoords::SEI_COORD * pcd, CVelMod::VELMOD * velm, double xShift, float fLabelDistance3, int iLayerFocus, double dz)

	// if the pseudo-well cannot show, we will show the deploy position instead
	//if (xpos<velm->getXmin() || xpos>velm->getXmax())
	//	xpos = velm->deploys[0].deployDist;
	//else if (isMainModelShowing && (xpos<pcd->xmin || xpos>pcd->xmax))
	//	xpos = velm->deploys[0].deployDist; // in case the pseudo-well is not visible, we want to show it only if it is the main model!
	if (xpos<velm->getXmin() || xpos>velm->getXmax()) return; // give up

	
	int oldBkMode = pDC->SetBkMode(OPAQUE);
	CString tStr;

	// Initialize a LOGFONT structure for the fonts.
	LOGFONT logFontNum;  // font for drawing numbers
	pDC->GetCurrentFont()->GetLogFont(&logFontNum);
	float xyRatio = (float)logFontNum.lfWidth / (float)logFontNum.lfHeight;
	int numXLogical = (int)myRound(fLabelDistance3 * pcd->device2Logical * pcd->getDefDisplayAdjust(pDC) * 6.);
	int numYLogical = (int)(numXLogical / xyRatio);
	logFontNum.lfHeight = numYLogical;
	logFontNum.lfWidth = numXLogical;
	logFontNum.lfEscapement = 0;
	logFontNum.lfOrientation = 0;
	logFontNum.lfWeight = FW_NORMAL;
	logFontNum.lfItalic = 0;
	logFontNum.lfUnderline = 0;
	logFontNum.lfStrikeOut = 0;
	logFontNum.lfCharSet = ANSI_CHARSET;
	logFontNum.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFontNum.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFontNum.lfQuality = DEFAULT_QUALITY;
	logFontNum.lfPitchAndFamily = FF_SWISS;
	_tcscpy_s(logFontNum.lfFaceName, _T("Arial"));

	CFont fontSmall;
	fontSmall.CreateFontIndirect(&logFontNum);

	CFont* oldFont;
	oldFont = pDC->SelectObject(&fontSmall);

	CFont fontBold;
	logFontNum.lfWeight = FW_BOLD;
	fontBold.CreateFontIndirect(&logFontNum);

	CSize size = pDC->GetWindowExt();
	int xplot, yplot;

	//bool isTime = true;
	// TWTT display of model: we need to show velocity labels at the rightmost side
	CVelMod vm;
	CVelMod::VELMOD_X modX;
	modX.dist = xpos;
	velm->m_isConvert = true;
	vm.initModX(velm, &modX, true, true, dz);

	COLORREF oldColor = pDC->SetTextColor(cr);

	int ixPlot = 0;
	int ixLogicalShifted = 0;
	int i;
	double v;


	// we do not want to show the vel name for the main model since it is already shown at the top-left
	pDC->SetTextColor(cr);
	int dPlt = (int)(8. * pcd->device2Logical * fLabelDistance3); // size of oval
	CPoint p0;
	CPoint p1, p2;
	bool isDeployRightOfModel = true; // this variable is depreciated

	if (isShowDetails) {
	  // the last boundary has not been plotted yet
		i = velm->ncont - 1;
		if (i < velm->ncontMaxConstrained) {
			float t2 = (float)modX.timeMod[i];
			if (t2 > pcd->Y1() && t2 < pcd->Y2()) {
				tStr.Format(_T("%.3f "), myFloat(modX.depthMod[velm->ncont - 1]), 3);
				p2 = pcd->mm_coord(xpos + xShift, t2);
				ixPlot = p2.x - logFontNum.lfWidth*(tStr.GetLength() + 1);
				pDC->TextOut(ixPlot + ixLogicalShifted,
					(int)myRound(p2.y - (float)logFontNum.lfHeight * 0.5), tStr);
			}
		}

		// draw a V-Z curve 
		if (isMainModelShowing) {
			CPoint plt[MOD_LAYER_MAX * 2];
			double deltax = pcd->xWidth() / 30. / 7. * fLabelDistance3;
			double aSign = ((xpos - pcd->X1()) > (pcd->X2() - xpos)) ?
				-1. : 1.;
			deltax *= aSign;
			int count = 0;
			for (i = 0; i < modX.ncont - 1; i++) {
				if (modX.timeMod[i] < pcd->Y1() && modX.timeMod[i + 1]>pcd->Y1()) {
					// we need to insert a point and then continue the loop
					v = modX.vTop[i] + (modX.vBot[i] - modX.vTop[i])*
						(pcd->Y1() - modX.timeMod[i]) / (modX.timeMod[i + 1] - modX.timeMod[i]);
					plt[count++] = pcd->mm_coord(xpos + xShift + deltax*(v - 1.5), pcd->Y1());
					continue;
				}
				else if (modX.timeMod[i] < pcd->Y1())
					continue;
				plt[count++] = pcd->mm_coord(xpos + xShift + deltax*(modX.vTop[i] - 1.5), modX.timeMod[i]);
				if (modX.timeMod[i + 1] < pcd->Y2())
					plt[count++] = pcd->mm_coord(xpos + xShift + deltax*(modX.vBot[i] - 1.5), modX.timeMod[i + 1]);
				else {
					// deal with the last point
					register double deltaTime = (modX.timeMod[i + 1] - modX.timeMod[i]);
					v = (deltaTime>0.) ? modX.vTop[i] + (modX.vBot[i] - modX.vTop[i])*(pcd->Y2() - modX.timeMod[i]) / deltaTime : modX.vTop[i];
					plt[count++] = pcd->mm_coord(xpos + xShift + deltax*(v - 1.5), pcd->Y2());
					break;
				}
			} // for i
			if (count > 1) {
				int aWidth = (int)(fLabelDistance3 * 60);
				CPen purplePen(PS_SOLID, aWidth, RGB(105, 105, 105));
				CPen* oldPen = pDC->SelectObject(&purplePen);
				pDC->Polyline(plt, count);
				pDC->SelectObject(oldPen);
			}
		} // if isMainShowing
	} // if isShowDetails


	for (i = 0; i < velm->ncont - 1; i++) {
		if (velm->nzed[i] < 1 || velm->nzed[i + 1] < 1) continue;  // no points defined for this layer
		register float z1 = (float)modX.timeMod[i];
		register float z2 = (float)modX.timeMod[i + 1];
		if (z2 <= z1) continue;  // no points defined for this layer
		register float zavg = (z1 + z2) * 0.5f;
		p1 = pcd->mm_coord(xpos + xShift, z1);
		p2 = pcd->mm_coord(xpos + xShift, z2);

		if (modX.isSedimentRegression(i))
			pDC->SetTextColor(GREEN);
		else
			pDC->SetTextColor(cr);

		if (isShowDetails && (z1 > pcd->Y1() && z1 < pcd->Y2())) {
			if (i + 1>velm->ncontMaxConstrained)
				pDC->SetTextColor(GRAY2);
			tStr.Format(_T("%.3g "), myFloat(modX.depthMod[i]), 3);
			ixPlot = p1.x - logFontNum.lfWidth*(tStr.GetLength() + 1);
			pDC->TextOut(ixPlot + ixLogicalShifted,
				(int)myRound(p1.y - (float)logFontNum.lfHeight * 0.5), tStr);
		}

		if (velm->isLayerGreyed(i))
			pDC->SetTextColor(GRAY2);
		//if (i + 1>velm->ncontMaxConstrained || (i + 1 == velm->ncontMaxConstrained && !velm->isVelBelowValid))
		//	pDC->SetTextColor(GREY);

		if (velm->poisson[i] > 0.00001) {
			tStr.Format(_T("%0.4g"), velm->poisson[i]);
			p0 = pcd->mm_coord(xpos + xShift, zavg);
			if (!isDeployRightOfModel) {
				p0.x -= (LONG)(5* numXLogical + numXLogical * tStr.GetLength());
			}
			else {
				p0.x += (LONG)(5* numXLogical);
			}
			p0.y -= (LONG)(numYLogical * 0.5);
			
			// why I use 2*numYLogical? It is because the bottom text is already ocupied by "km/s   km S" et al.
			if (p0.y > pcd->logicalAxes.bottom - 2*numYLogical)
				p0.y = pcd->logicalAxes.bottom - 2*numYLogical;

			oldFont = pDC->SelectObject(&fontBold);
			pDC->TextOut(p0.x, p0.y, tStr);
			pDC->SelectObject(oldFont);
		}




		// the near surface velocity will not show, otherwise it is too complicated
		if (i>0 && z1 >= pcd->Y1() && z1 <= pcd->Y2()) {
			v = modX.vTop[i];
			//if (isFloat1(v, 4.1))
			//	tStr.Format(_T("%1.2g"), myFloat(v, 1));
			//else
				tStr.Format(_T("%1.4g"), myFloat(v, 4));
			pDC->TextOut(p1.x + ixLogicalShifted, p1.y, tStr);
		} // if


		if (z2 >= pcd->Y1() && z2 <= pcd->Y2()) {
			//if (i + 1 == velm->ncontMaxConstrained)
			//	pDC->SetTextColor(GREY);
			v = modX.vBot[i];
			//if (isFloat1(v, 4.1))
			//	tStr.Format(_T("%1.2g"), myFloat(v, 1));
			//else
				tStr.Format(_T("%1.4g"), myFloat(v, 4));
			if (p2.y - p1.y >= logFontNum.lfHeight * 2) {
				pDC->TextOut(p2.x + ixLogicalShifted, p2.y - logFontNum.lfHeight + 1, tStr);
			} // if
		} // if

		if (isMainModelShowing && (zavg >= pcd->Y1() || zavg <= pcd->Y2())) {
			// we do not want to print out the layer number when it is not the main model, otherwise it can be too crowded
			xplot = p2.x - 2 * logFontNum.lfWidth - 1;  // why -1 pixel? because otherwise it will cut the border if x=0km
			if (ixPlot < pcd->logicalAxes.left) ixPlot = pcd->logicalAxes.left;
			if (p2.x >= pcd->logicalAxes.left && xplot < 1) xplot = 1;
			yplot = (p1.y + p2.y) / 2 - logFontNum.lfHeight / 2;
			if (yplot > 0 && yplot < size.cy && xplot >= 0 && xplot < size.cx) {
				tStr.Format(_T("%d"), i + 1);
				pDC->TextOut(xplot, yplot, tStr);
			}
		}
	} // for i

	if (isMainModelShowing) {
		if (xpos + xShift < velm->getXmin() || xpos + xShift > velm->getXmax())
			p1 = pcd->mm_coord(velm->getXmin() + xShift, pcd->Y2());
		else
			p1 = pcd->mm_coord(xpos + xShift, pcd->Y2());

		ixPlot = p1.x - logFontNum.lfWidth* tStr.GetLength() / 2;
		if (ixPlot < 1) ixPlot = 1;
		if (ixPlot > pcd->viewLogicalTotal.right - logFontNum.lfWidth*(tStr.GetLength() + 1))
			ixPlot = pcd->viewLogicalTotal.right - logFontNum.lfWidth*(tStr.GetLength() + 1);
		tStr.Format(_T("   km    km/s"));
		pDC->TextOut(ixPlot, p1.y - logFontNum.lfHeight - 1, tStr);
	}

	if (isMainModelShowing && drawnodes == 2) {
		DrawVelocityNodeValueTwtt(pDC, pcd, velm, xShift, fLabelDistance3, iLayerFocus, dz);
	} // if


	pDC->SetTextColor(oldColor);
	pDC->SelectObject(oldFont);
	pDC->SetBkMode(oldBkMode);
}





BOOL CVelMod::DrawVelocityColormapDepth(CDC* pDC, CVelMod::VELMOD *velm, CCoords::SEI_COORD *pCoord, 
	CColorHelper::VELCOLOR *velColor, 
	BOOL isNoNodes, double xShift, int iLayerFocus, BOOL isNoColorMap, int nodesMode)
{
	// this function only draws colormap, and nodes. It will not draw layer boundaries, which are drawn by another function
	// int nodesMode; Mode for editting boundary and velocity nodes (0,1,2,3).
	//if 1 then boundary mode; if 2 then vel mode; 3=text mode (disabled)

	// this function will not use *velm struct as is, not reloading from file.

	// note: if draw.isNoColorMap is true, we still need to continue this function, as we will 
	//       make a different display
	// if this model is not visible, we should not draw it
	CVelMod vm;
	if (!velm->isModelVisible(pCoord, xShift)) return FALSE;

	CString tstr;
	short icont;
	short ix, iv, nv;
	double rangeShifted;
	double a, b, v, dv, temp;
	CPoint p, p1, plt[MOD_POINTS_MAX * 2];


	//Start plotting velocity color map.
	CPen whitePen(PS_NULL, 0, WHITE);
	CPen* oldPen;
	CBrush* oldBrush;

	//For each velocity contour, look for the right depth for each range.
	//_stprintf(s, _T("at v,vtop,vbot=%d %d %d %d %d",(long)v,(long)vTop[0][ix],(long)vBot[0][ix],(long)vTop[1][ix],(long)vBot[1][ix]);
	//AfxMessageBox(s);
	//Cycle thru each layer and plot vel within each layer.
	double vminLayer = 99999., vmaxLayer = 0.;
	if (!vm.myVelModInterpolDisplay(velm, pCoord->xmin, pCoord->xmax, xShift, velColor->dxVDesired))
		return FALSE;
	//double grdXmax = velm->grdXmin + (velm->grdNx - 1)*velm->grdDx;
	for (icont = 0; icont < velm->ncont - 1; icont++) {
		//Find the min and max vel for this layer.
		vminLayer = 99999.;
		for (ix = 0; ix<velm->grdNx; ix++)
			if (vminLayer>velm->vTop[icont][ix]) vminLayer = velm->vTop[icont][ix];
		for (ix = 0; ix<velm->grdNx; ix++)
			if (vminLayer>velm->vBot[icont][ix]) vminLayer = velm->vBot[icont][ix];
		vmaxLayer = 0.;
		for (ix = 0; ix < velm->grdNx; ix++)
			if (vmaxLayer < velm->vTop[icont][ix]) vmaxLayer = velm->vTop[icont][ix];
		for (ix = 0; ix < velm->grdNx; ix++)
			if (vmaxLayer < velm->vBot[icont][ix]) vmaxLayer = velm->vBot[icont][ix];


		dv = velColor->dv1;
		if (fabs(dv) < 0.001) dv = 0.001; // to avoid overflow of floating point

		//vminLayer should be rounded to a multiple of dv.
		vminLayer = (int)(vminLayer / dv) * dv;

		// we only plot a max of  500 points in resolution. Others will interpolate
		nv = (int)((vmaxLayer - vminLayer) / dv + .5);
		if (nv < 2) nv = 2;
		if (nv >= 500) {
			dv = (vmaxLayer - vminLayer) / 499;
			nv = 500;
		}	//Make sure plt[] array do not exceed 500.

		//Loop through colors for this layer.
		for (iv = 0; iv < 500; iv++) {
			v = vminLayer + iv*dv;
			if (v > vmaxLayer) break;

			COLORREF colThis = CColorHelper::getVelocityColor((float)v, velColor);

			//Search by interpolation for the curve for v in layer icont.
			for (ix = 0; ix < velm->grdNx; ix++) {
				//if (ix == (velm->grdNx - 1)) range = grdXmax;
				rangeShifted = velm->grdXmin + velm->grdDx *(double)ix;
				temp = velm->vBot[icont][ix] - velm->vTop[icont][ix];
				if (velm->grdDepth[icont + 1][ix] < velm->grdDepth[icont][ix]) {
					a = velm->grdDepth[icont + 1][ix];
					b = velm->grdDepth[icont + 1][ix];
				}
				else if (fabs(temp) < 0.001) {
					//const vel, odd fix.
					if (nv == 2 && iv == 0) { a = velm->grdDepth[icont][ix]; b = velm->grdDepth[icont + 1][ix]; }
					else if (nv == 2 && iv == 1) a = b = velm->grdDepth[icont + 1][ix];
					else if (fabs(v - velm->vTop[icont][ix]) <= dv) a = velm->grdDepth[icont][ix];
					else if (iv <= nv - 1) a = velm->grdDepth[icont][ix];
					else a = velm->grdDepth[icont + 1][ix];
				}
				else {
					a = velm->grdDepth[icont][ix] + (velm->grdDepth[icont + 1][ix] - velm->grdDepth[icont][ix])
						*(v - velm->vTop[icont][ix]) / temp;
					b = velm->grdDepth[icont][ix] + (velm->grdDepth[icont + 1][ix] - velm->grdDepth[icont][ix])
						*(v + dv - velm->vTop[icont][ix]) / temp;
				}
				if (a < velm->grdDepth[icont][ix]) a = velm->grdDepth[icont][ix];
				if (a > velm->grdDepth[icont + 1][ix]) a = velm->grdDepth[icont + 1][ix];
				if (a < pCoord->m_zmin) a = pCoord->m_zmin;
				if (a > pCoord->m_zmax) a = pCoord->m_zmax;
				if (b < velm->grdDepth[icont][ix]) b = velm->grdDepth[icont][ix];
				if (b > velm->grdDepth[icont + 1][ix]) b = velm->grdDepth[icont + 1][ix];
				if (b < pCoord->m_zmin) b = pCoord->m_zmin;
				if (b > pCoord->m_zmax) b = pCoord->m_zmax;

				plt[ix] = pCoord->mm_coord(rangeShifted, a);
				plt[2 * velm->grdNx - ix - 1] = pCoord->mm_coord(rangeShifted, b);
			}

			// color map
			CBrush brush;
			if (!isNoColorMap)  {
				oldPen = pDC->SelectObject(&whitePen);
				if (nodesMode == 2 && icont == iLayerFocus)
					brush.CreateHatchBrush(HS_DIAGCROSS, colThis);
				else
					brush.CreateSolidBrush(colThis);
				oldBrush = pDC->SelectObject(&brush);
				pDC->Polygon(plt, 2 * velm->grdNx);
				pDC->SelectObject(oldBrush);
				pDC->SelectObject(oldPen);
			}
			else if (nodesMode == 2 && icont == iLayerFocus) {
				oldPen = pDC->SelectObject(&whitePen);
				brush.CreateHatchBrush(HS_DIAGCROSS, colThis);
				oldBrush = pDC->SelectObject(&brush);
				pDC->Polygon(plt, 2 * velm->grdNx);
				pDC->SelectObject(oldBrush);
				pDC->SelectObject(oldPen);
			}
			brush.DeleteObject();

			// contours
			if (pCoord->drawmode == 4)
				pDC->Polyline(plt, velm->grdNx);
		} //End of loop iv.
	}  //End of loop icont.

	return(TRUE);
}


void CVelMod::DrawVelocityLabelDepth2(CDC* pDC, CCoords::SEI_COORD *pcd, 
	CVelMod::VELMOD *velm, double xShift, double xWell, COLORREF cr,
	float fLabelDistance3, bool isMainModelShowing, 
	int iLayerFocus, int drawnodes, double dz)
{
	if (!isFileExistNotDirectory(velm->velSecName)) return;
	
	// Initialize a LOGFONT structure for the fonts.
	LOGFONT logFontNum;  // font for drawing numbers
	pDC->GetCurrentFont()->GetLogFont(&logFontNum);
	float xyRatio = (float)logFontNum.lfWidth / (float)logFontNum.lfHeight;
	int numXLogical = (int)myRound(fLabelDistance3 * pcd->device2Logical * pcd->getDefDisplayAdjust(pDC) * 6.);
	int numYLogical = (int)(numXLogical / xyRatio);

	logFontNum.lfHeight = numYLogical;
	logFontNum.lfWidth = numXLogical;
	logFontNum.lfEscapement = 0;
	logFontNum.lfOrientation = 0;
	logFontNum.lfWeight = FW_NORMAL;
	logFontNum.lfItalic = 0;
	logFontNum.lfUnderline = 0;
	logFontNum.lfStrikeOut = 0;
	logFontNum.lfCharSet = ANSI_CHARSET;
	logFontNum.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFontNum.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFontNum.lfQuality = DEFAULT_QUALITY;
	logFontNum.lfPitchAndFamily = FF_SWISS;
	_tcscpy_s(logFontNum.lfFaceName, _T("Arial"));

	CFont fontBold;
	logFontNum.lfWeight = FW_BOLD;
	fontBold.CreateFontIndirect(&logFontNum);

	CFont fontSmall;
	fontSmall.CreateFontIndirect(&logFontNum);
	CFont* oldFont = pDC->SelectObject(&fontSmall);
	int oldBkMode = pDC->SetBkMode(OPAQUE);

	// TWTT display of model: we need to show velocity labels at the rightmost side
	CString tStr;
	CVelMod vm;
	CVelMod::VELMOD_X modX;

	CPoint p1, p2;
	double v;
	int i;


	// we do not want to show the vel name for the main model since it is already shown at the top-left
	pDC->SetTextColor(cr);
	int dPlt = (int)(8. * pcd->device2Logical * fLabelDistance3); // size of oval
	bool isDeployRightOfModel = true;

	COLORREF oldColorText;
	oldColorText = pDC->SetTextColor(BLUE);
	if (isMainModelShowing && drawnodes == 2 && iLayerFocus >= 0 && iLayerFocus < velm->ncont - 1) {
			float x;
			//pDC->SetTextColor(RGB(0, 255, 200));
			pDC->SetTextColor(RGB(0, 0, 0));

			// top boundary of the layer
			bool isPlotPointExist = false;
			double vPlot;
			for (i = 0; i < velm->nvel[iLayerFocus][0]; i++) {
				x = velm->xvel[i][iLayerFocus][0];
				if (x + xShift >= pcd->X1() && x + xShift <= pcd->X2()) {
					isPlotPointExist = true;
					vPlot = velm->vf[i][iLayerFocus][0];
					break;
				}
			}
			if (!isPlotPointExist) {
				vPlot = velm->vf[0][iLayerFocus][0];
				modX.dist = pcd->X1();
				velm->m_isConvert = false;
				vm.initModX(velm, &modX, false, true, dz);
				p1 = pcd->mm_coord(modX.dist + xShift, (float)modX.depthMod[iLayerFocus]);
				if (modX.depthMod[iLayerFocus] >= pcd->m_zmin && modX.depthMod[iLayerFocus] <= pcd->m_zmax) {
					if (vPlot < 4.)
						tStr.Format(_T("%1.4f"), myFloat(vPlot, 4));
					else
						tStr.Format(_T("%1.3f"), myFloat(vPlot, 3));
					pDC->TextOut(p1.x + 10, p1.y, tStr);
				} // if
			}
			else {
				velm->m_isConvert = false;
				for (i = 0; i < velm->nvel[iLayerFocus][0]; i++) {
					x = velm->xvel[i][iLayerFocus][0];
					if (x + xShift >= pcd->X1() && x + xShift <= pcd->X2()) {
						modX.dist = x;
						vm.initModX(velm, &modX, false, true, dz);
						p1 = pcd->mm_coord(x + xShift, (float)modX.depthMod[iLayerFocus]);
						if (modX.depthMod[iLayerFocus] >= pcd->m_zmin && modX.depthMod[iLayerFocus] <= pcd->m_zmax) {
							vPlot = modX.vTop[iLayerFocus];
							tStr.Format(_T("%1.5g"), myFloat(vPlot, 5));
							pDC->TextOut(p1.x + 10, p1.y, tStr);
						} // if
					}
				}
			}

			// bottom boundary of the layer
			isPlotPointExist = false;
			for (i = 0; i < velm->nvel[iLayerFocus][1]; i++) {
				x = velm->xvel[i][iLayerFocus][1];
				if (x + xShift >= pcd->X1() && x + xShift <= pcd->X2()) {
					isPlotPointExist = true;
					vPlot = velm->vf[i][iLayerFocus][1];
					break;
				}
			}
			if (!isPlotPointExist) {
				vPlot = velm->vf[0][iLayerFocus][1];
				modX.dist = pcd->X1();
				velm->m_isConvert = false;
				vm.initModX(velm, &modX, false, true, dz);
				p1 = pcd->mm_coord(pcd->X1(), (float)modX.depthMod[iLayerFocus]);
				p2 = pcd->mm_coord(pcd->X1(), (float)modX.depthMod[iLayerFocus + 1]);
				if (modX.depthMod[iLayerFocus + 1] >= pcd->m_zmin && modX.depthMod[iLayerFocus + 1] <= pcd->m_zmax) {
					if (abs(p2.y - p1.y) > logFontNum.lfHeight + 2) {
						if (vPlot < 4.)
							tStr.Format(_T("%1.4f"), myFloat(vPlot, 4));
						else
							tStr.Format(_T("%1.3f"), myFloat(vPlot, 3));
						pDC->TextOut(p2.x + 10, p2.y - logFontNum.lfHeight - 2, tStr);
					}
				} // if
			}
			else {
				velm->m_isConvert = false;
				for (i = 0; i < velm->nvel[iLayerFocus][1]; i++) {
					x = velm->xvel[i][iLayerFocus][1];
					if (pcd->isVisibleOrNot(x + xShift)) {
						modX.dist = x;
						vm.initModX(velm, &modX, false, true, dz);
						p1 = pcd->mm_coord(x + xShift, (float)modX.depthMod[iLayerFocus]);
						p2 = pcd->mm_coord(x + xShift, (float)modX.depthMod[iLayerFocus + 1]);
						if (modX.depthMod[iLayerFocus + 1] >= pcd->m_zmin && modX.depthMod[iLayerFocus + 1] <= pcd->m_zmax) {
							bool isOnTheSide = i == 0 || i == velm->nvel[iLayerFocus][1] - 1;
							if (!isOnTheSide || (isOnTheSide && abs(p2.y - p1.y) > logFontNum.lfHeight + 2)) {
								vPlot = modX.vBot[iLayerFocus];
								tStr.Format(_T("%1.5g"), myFloat(vPlot, 5));
								pDC->TextOut(p2.x + 10, p2.y - logFontNum.lfHeight - 2, tStr);
							}
						} // if
					}
				}
			}
	} // if (drawnodes == 2

	double xpos = xWell;
	//velm->wells[0].wellDist;
	//if (isMainModelShowing && (xpos<pcd->xmin || xpos>pcd->xmax))
	//	xpos = velm->deploys[0].deployDist; // in case the pseudo-well is not visible, we want to show it only if it is the main model!
	if (!pcd->isVisibleOrNot(xpos + xShift)) return;



	if (isMainModelShowing) {
		// draw a V-Z curve; do this before modX is changed later!
		CPoint plt[MOD_LAYER_MAX * 2];
		double deltax = pcd->xWidth() / 30. / 7. * fLabelDistance3;
		double aSign = ((xpos - pcd->X1()) > (pcd->X2() - xpos)) ?
			-1. : 1.;
		deltax *= aSign;
		int count = 0;
		double v;
		modX.dist = xpos;
		velm->m_isConvert = true;
		vm.initModX(velm, &modX, false, true, dz);
		for (i = 0; i < modX.ncont - 1; i++) {
			if (modX.depthMod[i] < pcd->Y1() && modX.depthMod[i + 1]>pcd->Y1()) {
				// we need to insert a point and then continue the loop
				v = modX.vTop[i] + (modX.vBot[i] - modX.vTop[i])*
					(pcd->Y1() - modX.depthMod[i]) / (modX.depthMod[i + 1] - modX.depthMod[i]);
				plt[count++] = pcd->mm_coord(xpos + xShift + deltax*(v - 1.5), pcd->Y1());
				continue;
			}
			else if (modX.depthMod[i] < pcd->Y1())
				continue;
			plt[count++] = pcd->mm_coord(xpos + xShift + deltax*(modX.vTop[i] - 1.5), modX.depthMod[i]);
			if (modX.depthMod[i + 1] < pcd->Y2())
				plt[count++] = pcd->mm_coord(xpos + xShift + deltax*(modX.vBot[i] - 1.5), modX.depthMod[i + 1]);
			else {
				// deal with the last point
				register double deltaDepth = (modX.depthMod[i + 1] - modX.depthMod[i]);
				v = (deltaDepth>0.) ? modX.vTop[i] + (modX.vBot[i] - modX.vTop[i])*(pcd->Y2() - modX.depthMod[i]) / deltaDepth : modX.vTop[i];
				plt[count++] = pcd->mm_coord(xpos + xShift + deltax*(v - 1.5), pcd->Y2());
				break;
			}
		} // for i
		if (count > 1) {
			int aWidth = (int)(fLabelDistance3 * 60);
			CPen purplePen(PS_SOLID, aWidth, RGB(105, 105, 105));
			CPen* oldPen = pDC->SelectObject(&purplePen);
			pDC->Polyline(plt, count);
			pDC->SelectObject(oldPen);
		}
	}



	modX.dist = xpos;
	velm->m_isConvert = true;
	vm.initModX(velm, &modX, false, true, dz);
	oldColorText = pDC->SetTextColor(cr);
	for (i = 0; i < velm->ncont - 1; i++) {
		register int n1 = velm->nzed[i];
		register int n2 = velm->nzed[i + 1];
		if (n1 < 1 || n2<1) continue;  // no points defined for this layer
		register float z1 = (float)modX.depthMod[i];
		register float z2 = (float)modX.depthMod[i + 1];
		if (z2 <= z1) continue;  // no points defined for this layer

		register float zavg = (z1 + z2)*0.5f;
		//if (z1<pcd->zmax && zmid>pcd->zmax) zmid = (float)pcd->zmax;
		//else if (z2>pcd->zmin && zmid < pcd->zmin) zmid = (float)pcd->zmin;
		p1 = pcd->mm_coord(xpos + xShift, z1);
		p2 = pcd->mm_coord(xpos + xShift, z2);

		if (modX.isSedimentRegression(i))
			pDC->SetTextColor(GREEN);
		else
			pDC->SetTextColor(cr);

		if (velm->isLayerGreyed(i))
			pDC->SetTextColor(GRAY2);
		//if (i + 1>velm->ncontMaxConstrained || (i + 1 == velm->ncontMaxConstrained && !velm->isVelBelowValid))
		//	pDC->SetTextColor(GREY);

		if (velm->poisson[i] > 0.00001) {
			tStr.Format(_T("%0.3g"), velm->poisson[i]);
			register CPoint p0 = pcd->mm_coord(xpos + xShift, zavg);
			if (isDeployRightOfModel) {
				p0.x -= (LONG)(5 * numXLogical + numXLogical * tStr.GetLength());
			}
			else {
				p0.x += (LONG)(5 * numXLogical);
			}
			p0.y -= (LONG)(numYLogical * 0.5);
			if (p0.y > pcd->logicalAxes.bottom - numYLogical)
				p0.y = pcd->logicalAxes.bottom - numYLogical;
			oldFont = pDC->SelectObject(&fontBold);
			pDC->TextOut(p0.x, p0.y, tStr);
			pDC->SelectObject(oldFont);
		}


		// the near surface velocity will not show, otherwise it is too complicated
		if (i>0 && z1 >= pcd->m_zmin && z1 <= pcd->m_zmax) {
			v = modX.vTop[i];
			if (isFloat1DecOnly(v, 4.1))
				tStr.Format(_T("%1.2g"), myFloat(v, 1));
			else
				tStr.Format(_T("%1.3g"), myFloat(v, 2));
			pDC->TextOut(p1.x + 2, p1.y, tStr);
		} // if

		


		if (isMainModelShowing && zavg >= pcd->m_zmin && zavg <= pcd->m_zmax) {
			// we do not want to print out the layer number when it is not the main model, otherwise it can be too crowded
			p1 = pcd->mm_coord(xpos, zavg);
			if (zavg == (float)pcd->m_zmin)
				p1.y += 2;
			else if (zavg == (float)pcd->m_zmax)
				p1.y -= logFontNum.lfHeight - 2;
			else
				p1.y -= (int)myRound(logFontNum.lfHeight / 2.0);

			tStr.Format(_T("%d"), i + 1);
			register int ixPlot = p1.x - 2 * logFontNum.lfWidth;
			if (ixPlot < pcd->logicalAxes.left) ixPlot = pcd->logicalAxes.left;
			if (p1.x >= pcd->logicalAxes.left && ixPlot < 1) ixPlot = 1;

			pDC->TextOut(ixPlot, p1.y, tStr);
		}

		if (z2 >= pcd->m_zmin && z2 <= pcd->m_zmax) {
			if (i + 1 >= velm->ncontMaxConstrained) {
				pDC->SetTextColor(GRAY2);  // note: velm->isLayerGreyed() cannot be applied to here
			}
			v = modX.vBot[i];
			if (isFloat1DecOnly(v, 4.1))
				tStr.Format(_T("%1.2g"), myFloat(v, 1));
			else
				tStr.Format(_T("%1.3g"), myFloat(v, 2));
			if (p2.y - p1.y >= logFontNum.lfHeight * 2) {
				// pDC->TextOut(rect.right-logFontNum.lfWidth*tStr.GetLength()-8 - lenRShiftBot, p2.y-logFontNum.lfHeight, tStr);
				pDC->TextOut(p2.x + 2, p2.y - logFontNum.lfHeight + 1, tStr);
			} // if
		} // if
	} // for i

	// not showing the main model, so we draw label at the top of xpos
	pDC->SetTextColor(oldColorText);
	pDC->SelectObject(oldFont);
	pDC->SetBkMode(oldBkMode);

}

// valued as 1,2,3     with 3 being the biggest node.  
void CVelMod::DrawSonobuoyPos(CDC* pDC, CCoords::SEI_COORD *pcd,
	CVelMod::VELMOD *velm, double xShift, int bigNodesSize, COLORREF color, BOOL isShowDetails) {
	// draw sonobuoy symbol
	CPoint p0;

	// bigNodesSize== 1,2,3,4
	int fontSize;
	if (bigNodesSize < 2)
		fontSize = 6;
	else if (bigNodesSize < 3)
		fontSize = 7;
	else if (bigNodesSize < 4)
		fontSize = 8;
	else
		fontSize = 10;


	// Initialize a LOGFONT structure for the fonts.
	LOGFONT logFontNum;  // font for drawing numbers
	pDC->GetCurrentFont()->GetLogFont(&logFontNum);
	float xyRatio = (float)logFontNum.lfWidth / (float)logFontNum.lfHeight;
	int numXLogical = (int)myRound(fontSize * pcd->device2Logical);
	int numYLogical = (int)(numXLogical / xyRatio);

	logFontNum.lfHeight = numYLogical;
	logFontNum.lfWidth = numXLogical;
	logFontNum.lfEscapement = 0;
	logFontNum.lfOrientation = 0;
	logFontNum.lfWeight = FW_NORMAL;
	logFontNum.lfItalic = 0;
	logFontNum.lfUnderline = 0;
	logFontNum.lfStrikeOut = 0;
	logFontNum.lfCharSet = ANSI_CHARSET;
	logFontNum.lfOutPrecision = OUT_DEFAULT_PRECIS;
	logFontNum.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	logFontNum.lfQuality = DEFAULT_QUALITY;
	logFontNum.lfPitchAndFamily = FF_SWISS;
	_tcscpy_s(logFontNum.lfFaceName, _T("Arial"));

	CFont fontSmall;
	fontSmall.CreateFontIndirect(&logFontNum);

	CFont fontBold;
	logFontNum.lfWeight = FW_BOLD;
	fontBold.CreateFontIndirect(&logFontNum);

	int oldBkMode = pDC->SetBkMode(OPAQUE);
	COLORREF oldColorText = pDC->SetTextColor(color);

	CPoint pStart, pEnd;

	CString tStr;
	int dPlt = (int)(3. * pcd->device2Logical * bigNodesSize);
	pDC->SetTextColor(color);


	CFont* oldFont = pDC->SelectObject(&fontSmall);
	double xmin = velm->getXmin();
	double xmax = velm->getXmax();

	for (int i = 0; i < TX_SHOTS_MAX; i++) {
		register double x = velm->deploys[i].deployDist;
		if (x == ERRORVAL) break;
		register double z = velm->deploys[i].deployDepth;
		register BOOL isVisible = pcd->isVisibleOrNot(x + xShift, z);
		if (!isVisible) {
			// z = velm->getDepthWater(x);
			z = pcd->Y1();
			isVisible = pcd->isVisibleOrNot(x + xShift, z);
		}
		if (isVisible) {

			register CPoint p = pcd->mm_coord(x + xShift, z);
			register int ix1 = p.x - 2 * dPlt;
			register int iy1 = p.y - 2 * dPlt;
			if (ix1 < pcd->logicalAxes.left) {
				ix1 = pcd->logicalAxes.left;
			//	iy1 -= 2 * dPlt;
			}
			register int ix2 = p.x + 2 * dPlt;
			register int iy2 = p.y + dPlt;
			if (ix2 > pcd->logicalAxes.right) {
				ix2 = pcd->logicalAxes.right;
			//	iy1 -= 2 * dPlt;
			}
			if (iy1 < pcd->logicalAxes.top) {
			//	register int delty = iy2 - iy1;
				iy1 = pcd->logicalAxes.top;
			//	iy2 = iy1 + delty;
			}

			CPen aPen(PS_SOLID, 1, color);
			CPen* oldPen = pDC->SelectObject(&aPen);
			pDC->Ellipse(ix1, iy1, ix2, iy2);
			pDC->SelectObject(oldPen);




			// now print out the title for the deployment
			tStr = velm->deploys[i].metaTitle;
			// if no details are desired, we do not show the deploy title otherwise it can be too messy
			if (isShowDetails && tStr.Compare(GetBaseFileName(velm->velSecName)) != 0) {
				p0.x = pcd->mm_coordx(velm->deploys[i].deployDist + xShift);
				pStart.x = pcd->mm_coordx(velm->deploys[i].startDist + xShift);
				pEnd.x = pcd->mm_coordx(velm->deploys[i].endDist + xShift);
				register double dxEnd = fabs(velm->deploys[i].deployDist - velm->deploys[i].endDist);
				register double dxStart = fabs(velm->deploys[i].deployDist - velm->deploys[i].startDist);
				register double dxRatio = dxEnd / dxStart;
				if (dxRatio > 1.) dxRatio = 1. / dxRatio;
				if (dxRatio > 0.05) {
					// this deployment was not near any side of the model
					p0.x -= (LONG)(numXLogical * tStr.GetLength() * 0.5);
				}
				else if (dxStart < dxEnd) {
					// this deployment was near start side of the model
					if (pStart.x < pEnd.x)
						p0.x = pStart.x + (LONG)(dPlt + 2 * pcd->device2Logical);
					else
						p0.x = pStart.x - (LONG)(dPlt + numXLogical * tStr.GetLength());
				}
				else {
					// this deployment was near end side of the model
					if (pStart.x < pEnd.x)
						p0.x = pEnd.x + (LONG)(dPlt + 2 * pcd->device2Logical);
					else
						p0.x = pEnd.x - (LONG)(dPlt + numXLogical * tStr.GetLength());
				}
				p0.y = (LONG)(pcd->logicalAxes.top + dPlt + 2 * pcd->device2Logical);

				if (i % 2 == 0) 
					pDC->SetBkMode(OPAQUE);
				else 
					pDC->SetBkMode(TRANSPARENT);

				pDC->TextOut(p0.x, p0.y, tStr);
			}




		}
	} // for i





	  // we do not want to show the vel name for the main model since it is already shown at the top-left
	double x = (xmin + xmax) * 0.5 + xShift;
	p0.x = pcd->mm_coordx(x);
	p0.x -= (LONG)(numXLogical * tStr.GetLength() * 0.5);
	p0.y = (int)(pcd->logicalAxes.top + dPlt + numYLogical + 5*pcd->device2Logical);

	// if we have many small models displayed, the model names may overlap, making them hard to recognize. So we want to shift them a little to make them more visible
	static bool posSwitch = false;
	if (posSwitch) {
		p0.y -= (int)(numYLogical * 1.1);
		posSwitch = false;
	}
	else
		posSwitch = true;

	// now plot the title text of model
	tStr = _T("[") + GetBaseFileName(velm->velSecName) + _T("]");
	pDC->SelectObject(&fontBold);

	pDC->SetBkMode(OPAQUE);
	pDC->TextOut(p0.x, p0.y, tStr);




	pDC->SetTextColor(oldColorText);
	pDC->SelectObject(oldFont);
	pDC->SetBkMode(oldBkMode);
}


void CVelMod::DrawModelDashCurve(CDC* pDC, CCoords::SEI_COORD *pcd, 
	CVelMod::VELMOD *velm, int numWaterLayers, double h)
{
	// this is called only by DrawVelocityTheoreticalDepth() since it calls many times
	if (numWaterLayers < 1) return;
	int num = velm->nzed[numWaterLayers];
	if (num < 2) return;

	double xpos, ypos;
	int ii = 0;
	CPoint *plt = (CPoint *)alloc1CPoint(num);
	for (int j = 0; j < num; j++) {
		xpos = velm->xm[j][numWaterLayers];
		ypos = velm->zm[j][numWaterLayers] + h;
		if (pcd->isVisibleOrNot(xpos, ypos)) {
			plt[ii] = pcd->mm_coord(xpos, ypos);
			ii++;
		}
	} // for
	if (ii>1)  pDC->Polyline(plt, ii);
	free1CPoint(plt);
}



