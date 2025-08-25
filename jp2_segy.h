#pragma once
#include "segy.h"
#include "ximage.h"


// for JP2 format reading only
//union SDAT {
//	unsigned short datashort[SU_NFLTS];
//	byte databyte[SU_NFLTS * 2];
//};


class jp2_segy
{
public:
	jp2_segy(void);



	// jp2 routines for meta data only
	const bool is_valid_jp2(CFile* file);
	const CString get_next_box_header(CFile *filea, UINT *box_length);
	const UINT find_box(CFile* file, CString boxname);

	CString saveJp2AsSegyDepreciated(HWND h, CString f);
	CString getJp2XML(CString f);
	bool saveJp2AsSegy(CxImage *image, CString out, CString strXml);
	bool saveJp2AsSegy(CString f, CString out);
	static bool saveBitmapAsSegy(CxImage *image, CString fOutSegy, double xmin, double xmax, double tmin, double tmax, bool isTraceDomain);
	static bool saveBitmapAsSegy(CString fBitmap, CString fOutSegy, double xmin, double xmax, double tmin, double tmax, bool isTraceDomain);

private:
	// void freeSegyArray(HEAD_READ *headRead);

	// xml functions
	bool loadSxml2Head(CString strXml, HEAD_READ *headRead);
//	bool saveHead2Sxml(HEAD *head, CString strXml);


	// consider these before saving to segy file
	double img_transform_gain;
	double img_transform_offset;
	int number_of_significant_bits_in_input_stream;
	// CString ascHead;
};

