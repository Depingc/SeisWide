#pragma once
#include "segy.h"
#include "util.h"
#include "ealloc_d.h"

class CColorHelper
{
public:
	struct HsvColor {
		unsigned char h;
		unsigned char s;
		unsigned char v;
	};

	typedef struct {
		double h;       // angle in degrees, [0-360]
		double s;       // a fraction between 0 and 1
		double v;       // a fraction between 0 and 1
	} hsv;
	typedef struct {
		double r;       // a fraction between 0 and 1
		double g;       // a fraction between 0 and 1
		double b;       // a fraction between 0 and 1
	} rgb; // this is [0, 1] in contrast to COLORREF which is [0, 255]

	//SEI_TRIPLE hsv;
	//SEI_TRIPLE rgb; // this is [0, 1] in contrast to COLORREF which is [0, 255]

	struct LINECOLORS{
		// CString lineColFileName;
		static const int NUM_LINECOLORS = 11; // num of colors in the arrays. Always 11 as we use it. 
		int r[NUM_LINECOLORS];
		int g[NUM_LINECOLORS];
		int b[NUM_LINECOLORS];
	};
	static int getLineColorArray(CColorHelper::LINECOLORS *lineColors, CString f);  // return number of items, must be 11





	// VELCOLOR struct is for specifying parameters for plotting velocities that are defined by VELMOD struct
	struct VELCOLOR{
		double dv;  // note: this is used for plotting only, not to use it for data processing such as RMS velocity analysis
		double dxVDesired;
		double dz;
		// double dt;  // for time-depth conversions and vice versa. If zero, we will use velColor.dz instead. Not used yet.  If I enable thie line, Seiswide crash. Why???
		double cxExt, cxPos, cyExt, cyPos;
		double dv1;
		double vmin, vmid, vmax;
		double vmid2, vmid3, vmid4;
		double vmid5, vmid6, vmid7, vmid8, vmid9;

		int vminR, vminG, vminB, vmidR, vmidG, vmidB, vmaxR, vmaxG, vmaxB,
			vmidR2, vmidR3, vmidR4, vmidG2, vmidG3, vmidG4, vmidB2, vmidB3, vmidB4;
		int vmidR5, vmidG5, vmidB5;
		int vmidR6, vmidG6, vmidB6;
		int vmidR7, vmidG7, vmidB7;
		int vmidR8, vmidG8, vmidB8;
		int vmidR9, vmidG9, vmidB9;

		// additional 3 colors are needed, so we add here; Sept., 2011

		// support for Thomas format: file name: colPaletteThomas.cpt, Aug 6, 2020
		double colThomasV[40];
		int colThomasR[40];
		int colThomasG[40];
		int colThomasB[40];
		CTime colThomasModified;


		float colBarVmin;
		float colBarVmax;
		VELCOLOR() : colBarVmin(0), colBarVmax(8), dz(0.004) {
			memset(&colThomasV, 0, sizeof(double) * 40);
			memset(&colThomasR, 0, sizeof(int) * 40);
			memset(&colThomasG, 0, sizeof(int) * 40);
			memset(&colThomasB, 0, sizeof(int) * 40);
			colThomasModified = 0;

			vmin = 1.45;
			vmid = 2.5;
			vmid2 = 4.2;
			vmid3 = 4.3;
			vmid4 = 5.5;
			vmid5 = 6.9;
			vmid6 = 7.2;
			vmid7 = 7.7;
			vmid8 = 7.8;
			vmid9 = 8.0;
			vmax = 9.0;

			vminR = 210;
			vmidR = 117;
			vmidR2 = 3;
			vmidR3 = 19;
			vmidR4 = 252;
			vmidR5 = 255;
			vmidR6 = 255;
			vmidR7 = 255;
			vmidR8 = 170;
			vmidR9 = 170;
			vmaxR = 170;

			vminG = 220;
			vmidG = 200;
			vmidG2 = 6;
			vmidG3 = 255;
			vmidG4 = 255;
			vmidG5 = 151;
			vmidG6 =  70;
			vmidG7 =  30;
			vmidG8 = 150;
			vmidG9 = 90;
			vmaxG = 17;

			vminB = 255;
			vmidB = 255;
			vmidB2 = 255;
			vmidB3 = 0;
			vmidB4 = 0;
			vmidB5 = 0;
			vmidB6 = 0;
			vmidB7 = 30;
			vmidB8 = 250;
			vmidB9 = 253;
			vmaxB = 255;

			cxExt = 8.;
			cxPos = 2.;
			cyExt = .5;
			cyPos = 2.;
			dv1 = .1;
			dxVDesired = 0.01;  // temporary variable should be in constroctur
		};
	} ;
	static COLORREF getVelocityColorDepreciatedDelete(float v, CColorHelper::VELCOLOR vel);
	static COLORREF getVelocityColor(float v, CColorHelper::VELCOLOR *pvel);


	struct SEISBICOLOR{
		int	maxAmpR, maxAmpG, maxAmpB;
		int negAmpR, negAmpG, negAmpB;
		inline COLORREF getColor(bool isPositive) {
			if (isPositive)
				return RGB(maxAmpR, maxAmpG, maxAmpB);
			else
				return RGB(negAmpR, negAmpG, negAmpB);
		}
		inline void setColor(COLORREF negAmp, COLORREF maxAmp) {
			negAmpR = GetRValue(negAmp);
			negAmpG = GetGValue(negAmp);
			negAmpB = GetBValue(negAmp);

			maxAmpR = GetRValue(maxAmp);
			maxAmpG = GetGValue(maxAmp);
			maxAmpB = GetBValue(maxAmp);
		}
		SEISBICOLOR(int r = 128, int g = 0, int b = 128, int r2 = 0, int g2 = 0, int b2 = 255) :
			maxAmpR(r), maxAmpG(g), maxAmpB(b),
			negAmpR(r2), negAmpG(g2), negAmpB(b2) {}
	};
	struct AMPCOLORDYN{
		int num; // num of colors in the arrays. Always 11 as we use it. But the actual colors plotted are interpolated between these 11 colors and so can be many more.
		COLORREF *colorRef;
		hsv hsvTemp[11]; // for convenience of computation, only temporary
		float *percentAmp;
		float *DeltaAmp;  // Calculated field based on *percentAmp, for speed up work
		CString fileName; // this name can be color pallete if ending with colPalette.txt
		float m_ampPower; // default 1.f means not raising power for display
		double m_maxAmp;
		bool isBitmap; // amp colors for a bitmap is different, greyscale 
		bool isBitmapC; // amp colors for a bitmap is different, color
			// otherwise, it is assumed to be segy

		AMPCOLORDYN(CString f = BLANK) : num(0), DeltaAmp(NULL), isBitmap(false), isBitmapC(false)
		{
			fileName = f;
			colorRef = NULL;
			percentAmp = NULL;
			m_ampPower = 1.f;
			m_maxAmp = 0.;
		};

		// I have designed a self destructor, so you do not need to free yourself!
		~AMPCOLORDYN() {
			if (num > 0) {
				if (colorRef) free1(colorRef);
				if (percentAmp) free1float(percentAmp);
				if (DeltaAmp) free1float(DeltaAmp);
				colorRef = NULL;
				percentAmp = NULL;
				DeltaAmp = NULL;
				num = 0;
			}
		}

		const inline double getDrawAmp(double in) {
			// if (ampPower < 0.1f || ampPower > 10.f) ampPower = 1.f;
			if (m_ampPower == 1.f) return in;

			double aSign = sgn(in);
			if (m_ampPower == 2.f) return aSign*in*in;

			return aSign * pow(fabs(in), m_ampPower);
		}

		const inline double getDrawAmp(float in) {
			if (m_ampPower == 1.f) return (double)in;
			double aSign = sgn(in);
			if (m_ampPower == 2.f) return aSign*in*in;

			return aSign * pow(fabs(in), m_ampPower);
		}

		const inline COLORREF getColor(int i) {
			if (i >= 0 && i < num) {
				return colorRef[i];
			}

			return WHITE;
		}
		inline void setColor(COLORREF color, int i) {
			if (i >= 0 && i < num) {
				colorRef[i] = color;
			}
		}
		inline BOOL ampColorLower()
		{
			if (num < 1) return FALSE;
			for (int i = 1; i<num - 1; i++) {
				if (fabs(percentAmp[i]) < 1.0f) {
					percentAmp[i] *= 0.5f;
				}
			} // for i
			return TRUE;
		}

		inline BOOL ampColorHigher()
		{
			if (num < 1) return FALSE;
			for (int i = 1; i<num - 1; i++) {
				if (fabs(percentAmp[i]) < 0.5f)
					percentAmp[i] *= 2.0f;
			} // for i
			return TRUE;
		}

		inline BOOL initAmpColorArray(int n)
		{
			// If you do initialize them, you will destroy the existing values which I may want to keep!
			if (n > 0)
				freeAmpColorArray(); // free whatever arrays if they exist
			colorRef = (COLORREF *)ealloc1int(n);
			percentAmp = (float *)ealloc1float(n);
			DeltaAmp = (float *)ealloc1float(n);
			this->num = n;

			return TRUE;
		}
		inline void freeAmpColorArray()
		{
			if (num == 0) return;
			if (colorRef) free1(colorRef);
			if (percentAmp) free1float(percentAmp);
			if (DeltaAmp) free1float(DeltaAmp);
			colorRef = NULL;
			percentAmp = NULL;
			DeltaAmp = NULL;
			num = 0;
		}


		// use this function to clip the array, assuming
		// we only want the array to have colors for amps between [percAmpColorMin, percAmpColorMax]
		// note: we must have at least 1 value retained, otherwise it causes display problem
		// output is the last valid color retained after clipping
		// NOTE: 1. ampColorOut->percentAmp[i] already contains max amp, while percAmpColorMin does not contain it 
		//       2. Clipped lower values should reflect the exact color at the clip point, otherwise a gap can be seen in display
		//			But clipped high values should be WHITE otherwise it masks the overal panel
		inline COLORREF clipAmpColorArray(double percAmpColorMin, double percAmpColorMax, double maxAmp)
		{
			if (fabs(percAmpColorMax - percAmpColorMin) < 0.01 || percAmpColorMin>percAmpColorMax)
				return BLACK;  // without clipping, what's the average color retained? Not sure. Maybe BLACK?
			int i;
			percAmpColorMin *= maxAmp;
			percAmpColorMax *= maxAmp;
			COLORREF colorMin = getColorRef4Amp(percAmpColorMin); // this func takes real amp values
			COLORREF colorMax = getColorRef4Amp(percAmpColorMax); // this func takes real amp values
			for (i = 0; i < this->num; i++) {
				if (this->percentAmp[i] < percAmpColorMin) {
					this->colorRef[i] = WHITE;
					continue;
				}
				else if (this->percentAmp[i] > percAmpColorMax) {
					this->colorRef[i] = WHITE;
					continue;
				}
			} // for i

			return CColorHelper::interpolateHSV(colorMin, colorMax, 0.5f);
		}

		// clip color array at the a minimum amp
		inline COLORREF clipAmpColorMin(double percAmpColorMin, double maxAmp)
		{
			int i;
			percAmpColorMin *= maxAmp;
			// COLORREF colorMin = getColorRef4Amp(percAmpColorMin); // this func takes real amp values
			COLORREF colorMax = colorRef[num - 1]; // this func takes real amp values
			for (i = 0; i < this->num; i++) {
				if (this->percentAmp[i] < percAmpColorMin) {
					this->colorRef[i] = WHITE;
					continue;
				}
			} // for i

			return colorMax;
		}
		inline COLORREF clipAmpColorMax(double percAmpColorMax, double maxAmp)
		{
			int i;
			percAmpColorMax *= maxAmp;
			COLORREF colorMin = colorRef[0]; // this func takes real amp values
			// COLORREF colorMax = getColorRef4Amp(percAmpColorMax); // this func takes real amp values
			for (i = 0; i < this->num; i++) {
				if (this->percentAmp[i] > percAmpColorMax) {
					this->colorRef[i] = WHITE;
					continue;
				}
			} // for i

			return colorMin;
		}

		const inline void getAmpFromGray(BYTE colors[], int numsamp, float out[]) {
			int i, j;
			BYTE colTemplate[11];
			if (this->num != 11) {
				return;
			}
			for (i = 0; i < 11; i++) {
				colTemplate[i] = GetRValue(this->colorRef[i]);
			}

			bool isIncreasing = colTemplate[1] > colTemplate[0];

			if (isIncreasing) {
				for (i = 1; i < 11; i++) {
					if (colTemplate[i] <= colTemplate[i - 1])
						colTemplate[i] = colTemplate[i - 1] + 1;
				}
				for (j = 0; j < numsamp; j++) {
					if (colors[j] <= colTemplate[0]) {
						out[j] = this->percentAmp[0];
					}
					else if (colors[j] >= colTemplate[10]) {
						out[j] = this->percentAmp[10];
					}
					else {
						out[j] = ERRORVAL;
						for (i = 1; i < this->num; i++) {
							if (colors[j] >= colTemplate[i - 1] && colors[j] <= colTemplate[i]) {
								out[j] = this->percentAmp[i - 1]
									+ (this->percentAmp[i] - this->percentAmp[i - 1])
									* (colors[j] - colTemplate[i - 1])
									/ (colTemplate[i] - colTemplate[i - 1]);
								break;
							}
						} // for i
						if (out[j] == ERRORVAL) {
							int pppp = 1;
						}
					}
				} // for j
			}
			else {
				// in decreasing order
				for (i = 1; i < 11; i++) {
					if (colTemplate[i] >= colTemplate[i - 1])
						colTemplate[i] = colTemplate[i - 1] - 1;
				}
				for (j = 0; j < numsamp; j++) {
					if (colors[j] >= colTemplate[0]) {
						out[j] = this->percentAmp[0];
					}
					else if (colors[j] <= colTemplate[10]) {
						out[j] = this->percentAmp[10];
					}
					else {
						out[j] = ERRORVAL;
						for (i = 1; i < this->num; i++) {
							if (colors[j] <= colTemplate[i - 1] && colors[j] >= colTemplate[i]) {
								out[j] = this->percentAmp[i - 1]
									+ (this->percentAmp[i] - this->percentAmp[i - 1])
									* (colors[j] - colTemplate[i - 1])
									/ (colTemplate[i] - colTemplate[i - 1]);
								break;
							}
						} // for i
						if (out[j] == ERRORVAL) {
							int pppp = 1;
						}
					}
				} // for j
			}
		}

		const inline COLORREF getColorRef4Amp(double ampl) {
			if (this->num == 0) return WHITE;
			if (ampl <= this->percentAmp[0])
				return this->colorRef[0];

			int iLast = this->num - 1;
			if (ampl >= this->percentAmp[iLast])
				return this->colorRef[iLast];

			for (int i = 1; i < this->num; i++) {
				if (ampl >= this->percentAmp[i - 1] && ampl <= this->percentAmp[i]) {
					// this function will be very deep in loop. So we want it to be as fast as possible!!! So we drop complicated calculation
					// register double delta = ampColorDyn2->percentAmp[i] - ampColorDyn2->percentAmp[i-1];
					register double f = (ampl - this->percentAmp[i - 1]) * this->DeltaAmp[i];
					return interpolateHSV(this->colorRef[i - 1], this->colorRef[i], (float)f);
				}
			}

			// now the rate is not covered. This cannot happen unless program errors
			return WHITE;
		}


		// this function will be very deep in loop. So we want it to be as fast as possible!!! So we drop complicated calculation
		const inline bool getColorRef4Amp(float ampls[], COLORREF colors[], int n) {
			if (n < 1) return false;
			int ic, j;
			if (this->isBitmap) {
				// bitmap mode: we will use amp as color directly
				DWORD a;
				for (j = 0; j < n; j++) {
					a = (DWORD)ampls[j];
					colors[j] = RGB(a, a, a);
				}
				return true;
			}
			else if (this->isBitmapC) {
				// bitmap mode: we will use amp as color directly
				for (j = 0; j < n; j++) {
					colors[j] = (DWORD)ampls[j];
				}
				return true;
			}

			if (this->num == 0) {
				for (j = 0; j < n; j++)
					colors[j] = WHITE;
				return true;
			}
			int iLast = this->num - 1;
			for (j = 0; j < n; j++) {
				register double ampFinal = getDrawAmp(ampls[j]);
				if (ampFinal <= this->percentAmp[0])
					colors[j] = this->colorRef[0];
				else if (ampFinal >= this->percentAmp[iLast])
					colors[j] = this->colorRef[iLast];
				else {
					colors[j] = WHITE; // initialize
					for (ic = 1; ic < this->num; ic++) {
						if (ampFinal >= this->percentAmp[ic - 1]
							&& ampFinal <= this->percentAmp[ic]) {
							register double f = (ampFinal - this->percentAmp[ic - 1])
								* this->DeltaAmp[ic];
							colors[j] =
								interpolateHSV(this->colorRef[ic - 1], this->colorRef[ic], (float)f);
							break; // ic loop
						}
					} // for ic
				}
			} // for j
			// now the rate is not covered. This cannot happen unless program errors
			return true;
		} // getColorRef4Amp

		// to know if we are having a gray scale, we only need to check the first and last points
		const inline bool isGrayScale() {
			BYTE r = GetRValue(colorRef[0]);
			BYTE g = GetGValue(colorRef[0]);
			BYTE b = GetBValue(colorRef[0]);
			if (r != g || g != b) return false;

			r = GetRValue(colorRef[num-1]);
			g = GetGValue(colorRef[num - 1]);
			b = GetBValue(colorRef[num - 1]);
			return r == g && g == b;
		}
		const inline bool getColorRef4Ampf(float ampls[], CColorHelper::rgb colorsf[], int n) {
			if (n < 1) return false;
			int ic, j;

			if (this->isBitmap) {
				// bitmap mode: we will use amp as color directly
				DWORD a;
				for (j = 0; j < n; j++) {
					a = (DWORD)ampls[j];
					colorsf[j].r = colorsf[j].g = colorsf[j].b = a*O255;
				}
				return true;
			}
			else if (this->isBitmapC) {
				// bitmap mode: we will use amp as color directly
				for (j = 0; j < n; j++) {
					colorsf[j].r = GetRValue((DWORD)ampls[j])* O255;
					colorsf[j].g = GetGValue((DWORD)ampls[j])* O255;
					colorsf[j].b = GetBValue((DWORD)ampls[j])* O255;
				}
				return true;
			}

			if (this->num == 0) {
				for (j = 0; j < n; j++) {
					colorsf[j].r = colorsf[j].g = colorsf[j].b = 1.f;
				}
				return true;
			}

			bool isGray = this->isGrayScale();
			int iLast = this->num - 1;
			for (j = 0; j < n; j++) {
				register double ampFinal = getDrawAmp(ampls[j]);
				if (ampFinal <= this->percentAmp[0]) {
					colorsf[j].r = GetRValue(this->colorRef[0])* O255;
					colorsf[j].g = GetGValue(this->colorRef[0])* O255;
					colorsf[j].b = GetBValue(this->colorRef[0])* O255;
				}
				else if (ampFinal >= this->percentAmp[iLast]) {
					colorsf[j].r = GetRValue(this->colorRef[iLast])* O255;
					colorsf[j].g = GetGValue(this->colorRef[iLast])* O255;
					colorsf[j].b = GetBValue(this->colorRef[iLast])* O255;
				}
				else {
					//  initialize
					colorsf[j].r = colorsf[j].g = colorsf[j].b = 1.;
					for (ic = 1; ic < this->num; ic++) {
						if (ampFinal >= this->percentAmp[ic - 1]
							&& ampFinal <= this->percentAmp[ic]) {
							register double f = (ampFinal - this->percentAmp[ic - 1])
								* this->DeltaAmp[ic];
							//colorsf[j] = interpolateRGBf(this->colorRef[ic - 1], this->colorRef[ic], (float)f);
							if (isGray) 
								colorsf[j] = interpolateRGBf(this->colorRef[ic - 1], this->colorRef[ic], (float)f);
							else
								colorsf[j] = interpolateHSVf(this->hsvTemp[ic - 1], this->hsvTemp[ic], (float)f);
							break; // ic loop
						}
					} // for ic
				}
			} // for j
			  // now the rate is not covered. This cannot happen unless program errors
			return true;
		} // getColorRef4Amp

		inline void colorInterpolate(int iFrom, int iTo) {
			CColorHelper::AMPCOLORDYN ampColorDlg(BLANK);
			if (iFrom < 0 || iTo < 0 || iFrom == iTo) return;
			int num = 11;
			if (!ampColorDlg.initAmpColorArray(num)) return;
			for (int i = 0; i<11; i++) {
				if (iFrom < iTo) {
					if (i<iFrom || i>iTo) {
						ampColorDlg.colorRef[i] = this->colorRef[i];
					}
					else {
						// here it is very tricky
						int iBase = iFrom - 1;
						if (iBase <0) {
							// if here, the first point is dragged, we just need to duplicate the values for every point
							ampColorDlg.colorRef[i] = this->colorRef[iFrom];
						}
						else {
							// if here, we need to interpolate between iBase and iTo
							ampColorDlg.colorRef[i] = CColorHelper::interpolateHSV(this->colorRef[iBase], this->colorRef[iTo],
								(float)(i - iBase) / (float)(iTo - iBase));
						}
					}
				}
				else {
					if (i<iTo - 1) {
						ampColorDlg.colorRef[i] = this->colorRef[i];
					}
					else if (i == iTo - 1) {
						ampColorDlg.colorRef[i] = CColorHelper::interpolateHSV(this->colorRef[iFrom], this->colorRef[i], 0.5f);
					}
					else if (i == iTo) {
						ampColorDlg.colorRef[i] = this->colorRef[iFrom];
					}
					else {
						float x = (float)iFrom + (float)(11 - iFrom) / (float)(11 - iTo) * (float)(i - iTo);
						int ix = (int)x;

						// now the interpolation should be between  ix    and    ix+1
						if (ix <= 0) {
							ampColorDlg.colorRef[i] = this->colorRef[0];
						}
						else if (ix >= 10) {
							ampColorDlg.colorRef[i] = this->colorRef[10];
						}
						else {
							ampColorDlg.colorRef[i] = CColorHelper::interpolateHSV(this->colorRef[ix], this->colorRef[ix + 1], (float)(x - ix));
						}
					}
				}
			} // for i

			for (int i = 0; i<11; i++) {
				this->colorRef[i] = ampColorDlg.colorRef[i];
			}

		} //colorInterpolate2()

		  // this is the only function that connects to CdlgSegyColor2 to get the user selected color pallete
		const inline BOOL getAmpColorArrayOld(double amaxVal, bool isPositive, double logScaleThreshold, double aAmpPower) {
			// ampColorOut->fileName must have been set during struct initializing
			this->m_ampPower = (float)aAmpPower;
			this->m_maxAmp = amaxVal; // save for later usage
			CString colFileName = GetPathFromFileSlash(this->fileName) + _T("colPalette.txt");
			CColorHelper::AMPCOLORDYN ampColorIn(colFileName);  // must be 11 colors
			if (!ampColorIn.getAmpColorArray2(2, true)) return FALSE;
			// note that I have made sure that color2.ampColorDyn only has 11 items!!!
			if (ampColorIn.num != 11) return FALSE;
			if (!this->initAmpColorArray(11)) return FALSE;

			double delta;
			double rate;
			int i;
			//for (i = 0; i < this->num; i++) {
			//	this->colorRef[i] = ampColorIn.colorRef[i];
			//}
			memcpy(this->colorRef, ampColorIn.colorRef, this->num * sizeof(COLORREF));
			memcpy(hsvTemp, ampColorIn.hsvTemp, 11 * sizeof(hsv));
			 
			if (colFileName.GetLength() > 3 && colFileName.Find(_T("rms")) < 0 && !isPositive) {
				// now convert ampColorDyn.percent[i]  to   real amplitude levels
				delta = ampColorIn.percentAmp[5] - ampColorIn.percentAmp[0];
				rate = (fabs(delta)<0.001) ? 0. : 1. / delta;
				this->percentAmp[0] = -1.f;
				this->percentAmp[1] = -1.f + (ampColorIn.percentAmp[1] - ampColorIn.percentAmp[0])*(float)rate;
				this->percentAmp[2] = -1.f + (ampColorIn.percentAmp[2] - ampColorIn.percentAmp[0])*(float)rate;
				this->percentAmp[3] = -1.f + (ampColorIn.percentAmp[3] - ampColorIn.percentAmp[0])*(float)rate;
				this->percentAmp[4] = -1.f + (ampColorIn.percentAmp[4] - ampColorIn.percentAmp[0])*(float)rate;
				this->percentAmp[5] = 0.f;

				delta = ampColorIn.percentAmp[10] - ampColorIn.percentAmp[5];
				rate = (fabs(delta)<0.001) ? 0. : 1. / delta;
				this->percentAmp[6] = (ampColorIn.percentAmp[6] - ampColorIn.percentAmp[5])*(float)rate;
				this->percentAmp[7] = (ampColorIn.percentAmp[7] - ampColorIn.percentAmp[5])*(float)rate;
				this->percentAmp[8] = (ampColorIn.percentAmp[8] - ampColorIn.percentAmp[5])*(float)rate;
				this->percentAmp[9] = (ampColorIn.percentAmp[9] - ampColorIn.percentAmp[5])*(float)rate;
				this->percentAmp[10] = 1.f;
			} // if
			else {
				for (i = 0; i < this->num; i++) {
					delta = ampColorIn.percentAmp[this->num - 1] - ampColorIn.percentAmp[0];
					rate = (fabs(delta)<0.001) ? 0. : 1. / delta;
					this->percentAmp[i] = (ampColorIn.percentAmp[i] - ampColorIn.percentAmp[0])*(float)rate;
				} // for i
			}

			double fac = logScaleThreshold * 0.01;
			amaxVal = getDrawAmp(amaxVal);
			double logAmpCut = fac*logScalingPure(amaxVal);  // e.g.  logScalingPure(10000)=4
			double ampCut = logScalingInvPure(logAmpCut);  // this way, the ampThreshald can be 100 when amax=10,000
			double amax = logScalingPure(amaxVal);
			for (i = 0; i < this->num; i++) {
				this->percentAmp[i] *= (float)amax;
			} // for i
			ampColorIn.freeAmpColorArray();
			double oLogAmpCut = 1. / logAmpCut;
			for (i = 0; i < this->num; i++) {
				register double x = fabs(this->percentAmp[i]);
				if (x < logAmpCut) {
					// use linear 
					this->percentAmp[i] = (float)(sgn(this->percentAmp[i]) * ampCut * x * oLogAmpCut);
				}
				else {
					this->percentAmp[i] = (float)logScalingInvPure(this->percentAmp[i]);
				}
			} // for i
			for (i = 1; i < this->num; i++) {
				delta = this->percentAmp[i] - this->percentAmp[i - 1];
				rate = (fabs(delta)<0.001) ? 0. : 1. / delta;
				this->DeltaAmp[i] = (float)rate;
			}
			this->DeltaAmp[0] = 0.;
			return TRUE;
		} // getAmpColorArray()

		const inline BOOL getAmpColorArray(double amaxVal, bool isPositive, double logScaleThreshold, double aAmpPower) {
			// ampColorOut->fileName must have been set during struct initializing
			this->m_ampPower = (float)aAmpPower;
			this->m_maxAmp = amaxVal; // save for later usage
			CString colFileName = GetPathFromFileSlash(this->fileName) + _T("colPalette.txt");
			CColorHelper::AMPCOLORDYN ampColorIn(colFileName);  // must be 11 colors
			if (!ampColorIn.getAmpColorArray2(2, true)) return FALSE;
			// note that I have made sure that color2.ampColorDyn only has 11 items!!!
			if (ampColorIn.num != 11) return FALSE;
			if (!this->initAmpColorArray(11)) return FALSE;

			double delta;
			double rate;
			int i;
			//for (i = 0; i < this->num; i++) {
			//	this->colorRef[i] = ampColorIn.colorRef[i];
			//}
			memcpy(this->colorRef, ampColorIn.colorRef, this->num * sizeof(COLORREF));
			memcpy(hsvTemp, ampColorIn.hsvTemp, 11 * sizeof(hsv));

			if (colFileName.GetLength() > 3 && colFileName.Find(_T("rms")) < 0 && !isPositive) {
				// now convert ampColorDyn.percent[i]  to   real amplitude levels
				delta = ampColorIn.percentAmp[5] - ampColorIn.percentAmp[0];
				rate = (fabs(delta)<0.001) ? 0. : 1. / delta;
				this->percentAmp[0] = -1.f;
				this->percentAmp[1] = -1.f + (ampColorIn.percentAmp[1] - ampColorIn.percentAmp[0])*(float)rate;
				this->percentAmp[2] = -1.f + (ampColorIn.percentAmp[2] - ampColorIn.percentAmp[0])*(float)rate;
				this->percentAmp[3] = -1.f + (ampColorIn.percentAmp[3] - ampColorIn.percentAmp[0])*(float)rate;
				this->percentAmp[4] = -1.f + (ampColorIn.percentAmp[4] - ampColorIn.percentAmp[0])*(float)rate;
				this->percentAmp[5] = 0.f;

				delta = ampColorIn.percentAmp[10] - ampColorIn.percentAmp[5];
				rate = (fabs(delta)<0.001) ? 0. : 1. / delta;
				this->percentAmp[6] = (ampColorIn.percentAmp[6] - ampColorIn.percentAmp[5])*(float)rate;
				this->percentAmp[7] = (ampColorIn.percentAmp[7] - ampColorIn.percentAmp[5])*(float)rate;
				this->percentAmp[8] = (ampColorIn.percentAmp[8] - ampColorIn.percentAmp[5])*(float)rate;
				this->percentAmp[9] = (ampColorIn.percentAmp[9] - ampColorIn.percentAmp[5])*(float)rate;
				this->percentAmp[10] = 1.f;
			} // if
			else {
				for (i = 0; i < this->num; i++) {
					delta = ampColorIn.percentAmp[this->num - 1] - ampColorIn.percentAmp[0];
					rate = (fabs(delta)<0.001) ? 0. : 1. / delta;
					this->percentAmp[i] = (ampColorIn.percentAmp[i] - ampColorIn.percentAmp[0])*(float)rate;
				} // for i
			}
			ampColorIn.freeAmpColorArray();

			amaxVal = getDrawAmp(amaxVal);
			//amaxVal = 1.;
			double amaxLog = logScalingPure(amaxVal); // e.g.  logScalingPure(10000)=4
			double ampCutLog = logScaleThreshold * 0.01 * amaxLog;
			double ampCut = logScalingInvPure(ampCutLog);  // this way, the ampThreshald can be 100 when amax=10,000

			for (i = 0; i < this->num; i++) {
				this->percentAmp[i] *= (float)amaxVal;
			} // for i

//inline double logScalingPure(double ampl) {
//	return sgn(ampl) * log10(1. + fabs(ampl));
//}
//
//inline double logScalingInvPure(double ampLog) {
//	return sgn(ampLog) * (pow(10., fabs(ampLog)) - 1.);
//}
			// e.g. if amaxVal=1, and logScaleThreshold=93%, we have ampCutLog=0.27996, ampCut=0.90528
			//double aa = logScalingInvPure(-1.); // -9
			//double aa = logScalingInvPure(-.9); // -6.9
			//double bb = logScalingInvPure(-.7); // -4.0
			//double aa = logScalingInvPure(-.4); // -1.5
			//double bb = logScalingInvPure(-.1); // -.26
			if (fabs(amaxLog - ampCutLog) > MINAMP) {
				double fac = (amaxVal - ampCut) / (amaxLog - ampCutLog);
				for (i = 0; i < this->num; i++) {
					register double ax = this->percentAmp[i];
					if (fabs(ax) > ampCut) {
						// amp1=ampCut, log1 = ampCutLog
						// amp2=this->percentAmp[i], log2=logScalingPure(amp2)
						// But we cannot use log2 value directly
						// The final value should be: 
						// amp2Mod = ampCut + [amp2-ampCut] *fac
						this->percentAmp[i] = (float)(sgn(ax)*(ampCut + (fabs(logScalingPure(ax)) - ampCutLog) * fac));
					}
				} // for i
			}



			for (i = 1; i < this->num; i++) {
				delta = this->percentAmp[i] - this->percentAmp[i - 1];
				rate = (fabs(delta)<0.001) ? 0. : 1. / delta;
				this->DeltaAmp[i] = (float)rate;
			}
			this->DeltaAmp[0] = 0.;
			return TRUE;
		} // getAmpColorArray()


		// colorScaleCode = 0:  black and white; 1=bi-color; 2=quad-color
		const inline BOOL getAmpColorArray2(int colorScaleCode, bool isUpdatePercent) {
			BOOL isSuc = FALSE;
			int r, g, b;
			int i;
			
			CString colFileName = GetPathFromFileSlash(this->fileName) + _T("colPalette.txt");
			CColorHelper::AMPCOLORDYN ampColorDlg(colFileName);
			CStringArray *arr = getLinesFromFile(colFileName);
			if (arr == NULL || arr->GetSize() < 2) {
				// the color palette file is invalid, so we create a default
				int num = 11;
				if (!ampColorDlg.initAmpColorArray(num)) {
					arr->RemoveAll();
					delete arr;
					return FALSE;
				}

				if (colFileName.Find(_T("rms")) >= 0) {
					static const int rms_r[11] = { 0, 21, 149, 255, 255, 255, 255, 255, 255, 254, 254 };
					static const int rms_g[11] = { 102, 117, 194, 255, 246, 193, 148, 132, 97, 24, 0 };
					static const int rms_b[11] = { 204, 206, 105, 0, 0, 1, 1, 1, 1, 0, 0 };
					for (i = 0; i < ampColorDlg.num; i++)
						ampColorDlg.colorRef[i] = RGB(rms_r[i], rms_g[i], rms_b[i]);
					if (isUpdatePercent) {
						static const float rms_p[11] =
						{ 0.218636f, 0.453930f, 0.570588f, 0.591002f, 0.592703f, 0.594404f, 0.700166f,
							0.804227f, 0.831786f, 0.870486f, 0.900000f };
						memcpy(ampColorDlg.percentAmp, rms_p, sizeof(float) * 11);
					}
				}
				else {
					CColorHelper::getDefaultAmpColor(&ampColorDlg, colorScaleCode, isUpdatePercent);
				}
				ampColorDlg.num = num;
				isSuc = TRUE;
			}
			else {
				int num = (int)arr->GetSize();
				if (ampColorDlg.initAmpColorArray(num)) {
					for (i = 0; i<num; i++) {
						_stscanf_s(arr->ElementAt(i).Trim(), _T("%d%d%d%f"), &r, &g, &b, &ampColorDlg.percentAmp[i]);
						ampColorDlg.colorRef[i] = RGB(r, g, b);
					}
					ampColorDlg.num = num;
					isSuc = TRUE;
				}

			} // else

			if (arr) {
				arr->RemoveAll();
				delete arr;
			}

			// Note: Although ampColorDlg struct is designed to have any number of elements, but outputs 
			// m_ampColorref[] and m_ampPercent[]  will only be 11 elements.
			this->initAmpColorArray(11);
			if (ampColorDlg.num == 11) {
				for (i = 0; i<11; i++) {
					this->colorRef[i] = ampColorDlg.colorRef[i];
					if (isUpdatePercent)
						this->percentAmp[i] = ampColorDlg.percentAmp[i];
				}
			}
			else if (ampColorDlg.num > 11) { // now need to interpolate
				int j;  // for indexing in ampColorDlg struct
				for (i = 0; i<11; i++) {
					j = (int)((double)i * .1 * (double)(ampColorDlg.num - 1) + 0.5);
					if (j > ampColorDlg.num - 1) j = ampColorDlg.num - 1;
					this->colorRef[i] = ampColorDlg.colorRef[j];
					if (isUpdatePercent)
						this->percentAmp[i] = ampColorDlg.percentAmp[j];
				}
			}

			// convert this->rgb[0:10]  to  this->hsvTemp[0:10]
			if (ampColorDlg.num == 11) {
				// only do this when it is 11 items
				for (i = 0; i < 11; i++) {
					this->hsvTemp[i] = rgb2hsvf(this->colorRef[i]);
				}
			}


			ampColorDlg.freeAmpColorArray();
			return isSuc;
		} // getAmpColorArray2()

	}; // struct AMPCOLORDYN


	CColorHelper();
	~CColorHelper();

	// for gradient displays
	// These constants are common levels of translucency.
	const BYTE k_opaque = 0xFF;
	const BYTE k_transparent = 0x00;

	const BYTE k_alpha100 = k_opaque;       //100%, 0xFF == 255
	const BYTE k_alpha80 = 0xCC;           // 80%, 0xCC == 204
	const BYTE k_alpha75 = 0xC0;           // 75%, 0xC0 == 192
	const BYTE k_alpha60 = 0x99;           // 60%, 0x99 == 153
	const BYTE k_alpha50 = 0x80;           // 50%, 0x80 == 128
	const BYTE k_alpha40 = 0x66;           // 40%, 0x66 == 102
	const BYTE k_alpha25 = 0x40;           // 25%, 0x40 == 64
	const BYTE k_alpha20 = 0x33;           // 20%, 0x33 == 51
	const BYTE k_alpha0 = k_transparent;  //  0%, 0x00 == 0

  /* Utilities *****************************************************************/
	inline static COLOR16 ToColor16(BYTE byte) { return byte << 8; }
	inline static COLOR16 RVal16(COLORREF color) { return ToColor16(GetRValue(color)); }
	inline static COLOR16 GVal16(COLORREF color) { return ToColor16(GetGValue(color)); }
	inline static COLOR16 BVal16(COLORREF color) { return ToColor16(GetBValue(color)); }

	// hsv[] has length of   nx*ny
	//inline static void filterLoG5f(CColorHelper::hsv hsv[], LONG nx, LONG ny) {
	//	register double f;
	//	//LONG nx = m_pBIH->biWidth;
	//	//LONG ny = m_pBIH->biHeight;
	//	for (int y = 2; y < ny - 2; y++) {
	//		register ULONGLONG pos0 = nx * (y - 2);
	//		register ULONGLONG pos1 = nx * (y - 1);
	//		register ULONGLONG pos2 = nx * y;
	//		register ULONGLONG pos3 = nx * (y + 1);
	//		register ULONGLONG pos4 = nx * (y + 2);
	//		for (int x = 2; x < nx - 2; x++) {
	//			f =
	//				-hsv[pos0 + x].v
	//				- hsv[pos1 + x - 1].v - 2. * hsv[pos1 + x].v - hsv[pos1 + x + 1].v
	//				- hsv[pos2 + x - 2].v - 2. * hsv[pos2 + x - 1].v + 16. * hsv[pos2 + x].v - 2.*hsv[pos2 + x + 1].v - hsv[pos2 + x + 2].v
	//				- hsv[pos3 + x - 1].v - 2. * hsv[pos3 + x].v - hsv[pos3 + x + 1].v
	//				- hsv[pos4 + x].v;
	//			// hsv[pos2 + x].v = f;
	//			//hsv[pos2 + x].v *= 0.5;
	//		}
	//	}
	//}


	// 1D color interpolation
	static COLORREF interp1D_RGB(COLORREF ca, COLORREF cb, double xa, double xb, double x); // used in seismic amp plotting
	static COLORREF interp1D_HSV(COLORREF ca, COLORREF cb, double xa, double xb, double x); // used in seismic amp plotting

	static double interpolLinear(double a, double b, float t);
	static int interpolLinear(int a, int b, float t);
	static COLORREF interpolate(COLORREF a, COLORREF b, COLORREF c, COLORREF d, double t, double s);

	static COLORREF HsvToRgbi(HsvColor hsv);
	static HsvColor RgbToHsvi(COLORREF rgb);
	static COLORREF interpolateHSV(COLORREF a, COLORREF b, float t);
	static COLORREF interpolateRGB(COLORREF a, COLORREF b, float t);

	static CColorHelper::hsv rgb2hsvf(COLORREF);
	static CColorHelper::hsv rgb2hsvf(CColorHelper::rgb);
	static CColorHelper::rgb hsv2rgbf(CColorHelper::hsv);
	static CColorHelper::rgb interpolateHSVf(CColorHelper::hsv a, CColorHelper::hsv b, float t);
	static CColorHelper::rgb interpolateRGBf(COLORREF a, COLORREF b, float t);

	static COLORREF getColorRef4Amp(double ampl, int numColors, float percentAmp[], COLORREF colArr[]);
	static BOOL getDefaultAmpColor(CColorHelper::AMPCOLORDYN *ampColorDyn2, int colorScaleCode, bool isUpdatePercent);
	static BOOL saveAmpColorToFile2(CColorHelper::AMPCOLORDYN *, CString f);
	static BOOL saveAmpColorToFile1(CColorHelper::AMPCOLORDYN *);

	//static CColorHelper::rgb interpolateHSVf(COLORREF a, COLORREF b, float t);
	//static CColorHelper::rgb rgbRef2Dec2(COLORREF);
	// static COLORREF rgbDec2Ref2(CColorHelper::rgb);
	//static COLORREF interpolateHSVDec2(COLORREF a, COLORREF b, float t);
	//	static COLORREF interpolateHSV3_Delete(int rA, int gA, int bA, int rB, int gB, int bB, float t);

	//static BOOL saveAmpColorToFile();
	//	static void colorInterpolate2(CColorHelper::AMPCOLORDYN *pAmpColor11, int iFrom, int iTo);
	// 0:  black and white; 
	// 1: bi-color; 
	// 2: more colors
	// 3: black and red tainted 
	// 4: bright-gray mode
	// 5: Bright-Low Amp mode;
	//static BOOL getAmpColorArray2(
	//	CColorHelper::AMPCOLORDYN *pAmpColor11, 
	//	int colorScaleCode = 2, 
	//	bool isUpdatePercent = true);

	// will read color from  colPallette.txt file, scale it up to amaxVal using logScaleThreshold, 
	// and put proper values to *ampColorDyn2  struct. If isPositive, will scale using positive only.
	// if isPositive==false, will scale using both polarities.
	//static BOOL getAmpColorArray(
	//	CColorHelper::AMPCOLORDYN *ampColorOut,
	//	double amaxVal,
	//	bool isPositive,
	//	double logScaleThreshold);


	// this is used only for contouring function, which is not tuned up well.
	//static bool getExpandedColorTableDepreciated(COLORREF *colArr,
	//	float percentAmpExpanded[],
	//	int numExpanded,
	//	CString colFileName,
	//	BOOL isAvgEnergy,
	//	double maxAmp,
	//	double logScaleThreshold);


	static double getInterpol4Points(double x,
		double x0, double x1, double x2, double x3,
		double y0, double y1, double y2, double y3);

	// for input 4 locations x0, x1, x2, x3, we have arrays y0[0:num-1], y1[], y2[], y3[]. We want to calculate yOut[] at location x
	static void CColorHelper::getInterpol4Points(
		float x0, float x1, float x2, float x3,
		float y0[], float y1[], float y2[], float y3[],
		float x, int num,
		float yOut[]);

	static CString colorRef2Hex(COLORREF color) {
		CString buff;
		const unsigned char r = GetRValue(color);
		const unsigned char g = GetGValue(color);
		const unsigned char b = GetBValue(color);
		buff.Format(_T("#%02x%02x%02x"), r, g, b);
		return buff;
	}



};



