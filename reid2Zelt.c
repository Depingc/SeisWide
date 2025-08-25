
#include <stdio.h>
#include <math.h>

#define MAXLAYERS 40
#define MAXPOINTS 400
#define MAXVELS 40
#define DX 0.01
#define DZ 0.01

  FILE *infil, *outfil;
  int iaz, nlayers, npts[MAXLAYERS], nvels[MAXLAYERS], i, j=0, l;
  float vel[MAXLAYERS], grad[MAXLAYERS];
  float x[MAXLAYERS][MAXPOINTS], z[MAXLAYERS][MAXPOINTS], zmax=0.0;
  float veltop[MAXLAYERS][MAXVELS], velbot[MAXLAYERS][MAXVELS];
  float xvel[MAXLAYERS][MAXVELS], zvel[MAXLAYERS][MAXVELS];

  fscanf(infil,"%d",&iaz);
  fscanf(infil,"%f %f",&vel[j],&grad[j]);
  do {
      fscanf(infil,"%d",&npts[j]);
      for (i=0; i<npts[j]; i++) {
fscanf(infil,"%f %f",&x[j][i],&z[j][i]);
if (z[j][i] > zmax) zmax = z[j][i];
      }
      j++;
  } while (fscanf(infil,"%f %f",&vel[j],&grad[j]) != EOF);
  nlayers = j;
  x[j][0] = x[j-1][0]; x[j][1] = x[j-1][npts[j-1]-1];
  z[j][0] = zmax + 0.1/grad[j-1]; z[j][1] = z[j][0];
  npts[j] = 2;

/*
  *   Look for horizontal transition layers.
  */

  l = 0;
  do {
      int l1, l2;
      float v1, v2;
      l1 = -1; l2 = -1;
      if (vel[l] == 0.0) {
  for (j=0; j<=nlayers; j++) {
      if (j != l ) {
  for (i=0; i<npts[j]; i++) {
            if ((fabsf(x[j][i]-x[l][0]) <= DX) 
   && (fabsf(z[j][i]-z[l][0]) <= DZ)) {
l1 = j;
v1 = vel[j];
                    }
                    else if ((fabsf(x[j][i]-x[l][npts[l]-1]) <= DX)
   && (fabsf(z[j][i]-z[l][npts[l]-1]) <= DZ)) {
l2 = j;
v2 = vel[j];
                    }
  }
              }
          }
      }
      if (l1 >= 0 && l2 >= 0) {
  for (i=1; i<(npts[l]-1); i++) {
      x[l1][npts[l1]+i-1] = x[l][i];
      z[l1][npts[l1]+i-1] = z[l][i];
          }
  npts[l1] += (npts[l]-2);
  for (i=0; i<npts[l2]; i++) {
      x[l1][npts[l1]+i] = x[l2][i];
      z[l1][npts[l1]+i] = z[l2][i];
  }
  npts[l1] += npts[l2];
  veltop[l1][nvels[l1]] = vel[l2];
  xvel[l1][nvels[l1]] = x[l2][0];
  velbot[l1][nvels[l1]] = grad[l2];
  zvel[l1][nvels[l1]] = z[l2][0];
  veltop[l1][nvels[l1]+1] = vel[l2];
  xvel[l1][nvels[l1]+1] = x[l2][npts[l2]-1];
  velbot[l1][nvels[l1]+1] = grad[l2];
  zvel[l1][nvels[l1]+1] = z[l2][npts[l2]-1];
  nvels[l1] += 2;
  npts[l] = -npts[l]; npts[l2] = -npts[l2];
      }
      else {
veltop[l][0] = vel[l]; xvel[l][0] = x[l][0];
velbot[l][0] = grad[l]; zvel[l][0] = z[l][0];
veltop[l][1] = vel[l]; xvel[l][1] = x[l][npts[l]-1];
velbot[l][1] = grad[l]; zvel[l][1] = z[l][npts[l]-1];
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
      if ((vel[l] == 0.0) && (vel[l-1] != 0.0) && (npts[l] > 0)) {
  if ((fabsf(x[l][0]-x[l-1][0]) <= DX) 
       && (fabsf(x[l][npts[l]-1]-x[l-1][npts[l-1]-1]) <= DX)) {
      dh = z[l][0] - z[l-1][0];
      for (i=0; i<npts[l-1]; i++) {
  x[l][i] = x[l-1][i];
  z[l][i] = z[l-1][i] + dh;
      }
      npts[l] = npts[l-1];
      for (i=0; i<nvels[l-1]; i++) {
  veltop[l][i] = veltop[l-1][i] + dh*velbot[l-1][i];
  xvel[l][i] = xvel[l-1][i];
  velbot[l][i] = velbot[l-1][i];
  zvel[l][i] = zvel[l-1][i] + dh;
      }
      nvels[l] = nvels[l-1];
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
  for (il=0; il<nvels[l]; il++) {
      bottom_found = 0;
      j = l + 1;
      do {
          if (npts[j] > 0) {
      ij = 0;
      do {
  if (fabsf(xvel[l][il]-x[j][ij]) < DX) {
      velbot[l][il] = (z[j][ij]-zvel[l][il]) 
       * velbot[l][il];
      velbot[l][il] += veltop[l][il];
      bottom_found = 1;
  }
                          else if (ij > 0 && x[j][ij] > xvel[l][il]) {
      float zslope;
      zslope = (z[j][ij]-z[j][ij-1]) 
       / (x[j][ij]-x[j][ij-1]);
      velbot[l][il] = (z[j][ij-1] + zslope 
      * (xvel[l][il]-x[j][ij-1]) - zvel[l][il])
      * velbot[l][il];
      velbot[l][il] += veltop[l][il];
      bottom_found = 1;
  }
  ij++;
      } while (!bottom_found && ij < npts[j]);
          }
  j++;
              } while (!bottom_found && j<=nlayers);
  }
      }
      l++;
  } while (l < nlayers);

