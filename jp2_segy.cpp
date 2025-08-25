#include "StdAfx.h"
#include "jp2_segy.h"
#include "util.h"
#include "suSwapByte.h"
#include "gzip.h"
#include "ealloc_d.h"
#include "SegyWrapper.h"
#include "xallocator.h"

//using namespace zlib;
//#include <stdlib.h>
//#include <iostream>
//#include <fstream>


jp2_segy::jp2_segy(void)
{
	img_transform_gain = 1.;
	img_transform_offset = 0.;
	number_of_significant_bits_in_input_stream = 8;
}

//void jp2_segy::freeSegyArray(HEAD_READ *headRead)
//{
//	// copied from CSegyWrapper class
//	if (headRead->trArr) {
//		freeSegyDataArrayOnly(headRead->trArr, headRead->nxmax);
//		if (headRead->trArr)
//			free(headRead->trArr);  // note: if nx==0, I cannot free trArr since it would give me an exception.
//		headRead->trArr = NULL;
//	} // if
//	if (headRead->pBh)
//		free(headRead->pBh);
//	headRead->pBh = NULL;
//	// end copied
//
//}



// depreciated
CString jp2_segy::saveJp2AsSegyDepreciated(HWND h, CString f)
{
	CString aPath;
	CString title = _T("Select your directory for output");
	aPath = BrowseFolderSlash(title);
	if (aPath.IsEmpty() || !isDirectoryWritable(aPath)) return _T("");
	SetCurrentDirectory(aPath);

	CString out = aPath + GetBaseFileName(f) + _T("_conv.sgy");
	setCurrentPathExactAs(out);
		
	if (isFileExistNotDirectory(out)) {
		if (AfxMessageBox(_T("A segy version is found. use it? Clicking NO to overwrite it."),
			MB_YESNO | MB_ICONINFORMATION| MB_ICONQUESTION) == IDYES) {
			return out;
		}
		//else
		//	if (!DeleteFile(out))
		//		return _T("");
	}

	if (this->saveJp2AsSegy(f, out))
		return out;

	return _T("");
}


bool jp2_segy::saveBitmapAsSegy(CxImage *image, CString fOutSegy, double xmin, double xmax, double tmin, double tmax, bool isTraceDomain)
{
	if (!image->IsValid() || image->GetWidth()<2 || image->GetHeight() < 2){
		//CString s = image->GetLastError();
		//delete image;
		//image = NULL;
		return false;
	}
	// WORD iPrecDecoded = image->GetBpp();  //  return: 1, 4, 8, 24.

	int numsamp = image->GetHeight();
	double dt = (tmax - tmin) / (double)(numsamp - 1);

	int nxmax = image->GetWidth();
	if (dt < 0.0000001) dt = 0.004;
	unsigned int idt = (unsigned short)(dt * MIL);
	if (idt > 65535) {
		idt = 65535;
		dt = (double)idt * OMIL;
	}
	double dx = (xmax - xmin) / (double)(nxmax -1);

	SegyWrapper sw(fOutSegy);
	memset((void *)&sw.headRead.bh, (int) '\0', 400);
	sw.headRead.bh.format = 5;
	sw.headRead.setnx(nxmax);
	sw.headRead.setnumsamp(numsamp);
	sw.headRead.setdt(dt);
	sw.headRead.trArr = (segydynamic *)xmalloc(sizeof(segydynamic) * nxmax);
	sw.headRead.ascHeadc.Format(_T("Segy reconstructed by Seiswide from an image file."));

	// allocate data array
	for (int ix = 0; ix < nxmax; ix++) {
		memset((void *)&sw.headRead.trArr[ix].tracl, (int) '\0', 240);
		if (isTraceDomain) {
			sw.headRead.trArr[ix].tracl = ix + (int)xmin;
		}
		else {
			sw.headRead.trArr[ix].tracl = ix + 101;
			register float x = (float)(xmin + ix*dx);
			sw.headRead.trArr[ix].setDistance(x);
			sw.headRead.trArr[ix].setOffset(x);
		}
		sw.headRead.trArr[ix].dt = (unsigned short)idt;
		sw.headRead.trArr[ix].ns = (unsigned short)numsamp;
		sw.headRead.trArr[ix].data = ealloc1float(numsamp);
	}
	if (ERROR == sw.headRead.trArr[nxmax - 1].data) {
		sw.freeSegyArray(&sw.headRead);
		return false;
	}

	for (int ix = 0; ix< nxmax; ix++) {
		for (int j = 0; j < numsamp; j++) {
		//	sw.headRead.trArr[ix].data[j] = (float)image->GetPixelIndex(ix, numsamp - 1 - j); // plots are upside down
			int ind = (numsamp - 1 - j) * image->info.dwEffWidth + ix;
			sw.headRead.trArr[ix].data[j] = image->info.pImage[ind];
		}
	} 
	delete image;

	float val;
	for (int ix = 0; ix < nxmax; ix++) {
		// now remove avg value
		val = (float)getMedianAbsolute(sw.headRead.trArr[ix].data, numsamp);
		for (int j = 0; j < numsamp; j++) {
			sw.headRead.trArr[ix].data[j] -= val;
		}
	}
	// now, we have everything except the binary file header and binary trace header
	// sw.writeHeaderOnly();
	sw.writeDataAll(fOutSegy);
	return true;

}


bool jp2_segy::saveBitmapAsSegy(CString fBitmap, CString fOutSegy, double xmin, double xmax, double tmin, double tmax, bool isTraceDomain)
{
	CString ext = GetFileExtension(fBitmap);
	int type = CxImage::GetTypeIdFromName(ext);

	// now get the data
	CxImage *image = new CxImage(fBitmap, type);

	return jp2_segy::saveBitmapAsSegy(image, fOutSegy, xmin, xmax, tmin, tmax, isTraceDomain);

	//if (!image->IsValid() || image->GetWidth()<2 || image->GetHeight() < 2){
	//	CString s = image->GetLastError();
	//	delete image;
	//	image = NULL;
	//	return false;
	//}
	//// WORD iPrecDecoded = image->GetBpp();  //  return: 1, 4, 8, 24.

	//int numsamp = image->GetHeight();
	//double dt = (tmax - tmin) / (double)(numsamp - 1);

	//int nxmax = image->GetWidth();
	//if (dt < 0.0000001) dt = 0.004;
	//unsigned int idt = (unsigned short)(dt * MIL);
	//if (idt > 65535) {
	//	idt = 65535;
	//	dt = (double)idt * OMIL;
	//}
	//double dx = (xmax - xmin) / (double)(nxmax - 1);

	//SegyWrapper sw(fOutSegy);
	//memset((void *)&sw.headRead.bh, (int) '\0', 400);
	//sw.headRead.bh.format = 5;
	//sw.headRead.setnx(nxmax);
	//sw.headRead.setnumsamp(numsamp);
	//sw.headRead.setdt(dt);
	//sw.headRead.trArr = (segydynamic *)malloc(sizeof(segydynamic) * nxmax);
	//sw.headRead.ascHeadc.Format(_T("Segy reconstructed by Seiswide from image file: %s"), fBitmap);

	//// allocate data array
	//for (int ix = 0; ix < nxmax; ix++) {
	//	memset((void *)&sw.headRead.trArr[ix].tracl, (int) '\0', 240);
	//	if (isTraceDomain) {
	//		sw.headRead.trArr[ix].tracl = ix + (int)xmin;
	//	}
	//	else {
	//		sw.headRead.trArr[ix].tracl = ix + 101;
	//		register float x = (float)(xmin + ix*dx);
	//		setDistance(&sw.headRead.trArr[ix], x);
	//		setOffset(&sw.headRead.trArr[ix], x);
	//	}
	//	sw.headRead.trArr[ix].dt = (unsigned short)idt;
	//	sw.headRead.trArr[ix].ns = (unsigned short)numsamp;
	//	sw.headRead.trArr[ix].data = ealloc1float(numsamp);
	//}
	//if (ERROR == sw.headRead.trArr[nxmax - 1].data) {
	//	sw.freeSegyArray(nxmax);
	//	return false;
	//}

	//for (int ix = 0; ix< nxmax; ix++) {
	//	for (int j = 0; j < numsamp; j++) {
	//		//	sw.headRead.trArr[ix].data[j] = (float)image->GetPixelIndex(ix, numsamp - 1 - j); // plots are upside down
	//		int ind = (numsamp - 1 - j) * image->info.dwEffWidth + ix;
	//		sw.headRead.trArr[ix].data[j] = image->info.pImage[ind];
	//	}
	//}
	//delete image;

	//float val;
	//for (int ix = 0; ix < nxmax; ix++) {
	//	// now remove avg value
	//	val = (float)getMedianAbsolute(sw.headRead.trArr[ix].data, numsamp);
	//	for (int j = 0; j < numsamp; j++) {
	//		sw.headRead.trArr[ix].data[j] -= val;
	//	}
	//}
	//// now, we have everything except the binary file header and binary trace header
	//// sw.writeHeaderOnly();
	//sw.writeDataAll(fOutSegy);
	//return true;

}
bool jp2_segy::saveJp2AsSegy(CString f, CString out)
{
	CString ext = GetFileExtension(f);
	int type = CxImage::GetTypeIdFromName(ext);
	if (ext.Compare(_T("jp2")) != 0) return false;

	// now get the data
	// pFrame->myCaption(_T("Parsing JP2 file"));
	CxImage *image = new CxImage(f, type);
	if (!image->IsValid()){
		CString s = image->GetLastError();
		delete image;
		image = NULL;
		return false;
	}

	CString strXml = getJp2XML(f);
	bool isTrue = saveJp2AsSegy(image, out, strXml);
	delete image;

	return isTrue;
}

bool jp2_segy::saveJp2AsSegy(CxImage *image, CString out, CString strXml)
{
	if (!image) return false;

	SegyWrapper sw(out);
//	sw.m_isUnixInFile = false;  // writing in unix format needs other consideration!!!
	memset((void *)&sw.headRead.bh, (int) '\0', 400);

	sw.headRead.bh.format = 5;

	bool isSuc = this->loadSxml2Head(strXml, &sw.headRead);
	if (!isSuc) {
		// free headRead before return ???
		return false;
	}

	// allocate data array
	for (int ix = 0; ix < sw.headRead.getnx(); ix++) {
		sw.headRead.trArr[ix].data = ealloc1float(sw.headRead.getns());
	}
	if (ERROR == sw.headRead.trArr[sw.headRead.getnx() - 1].data) {
		sw.freeSegyArray(&sw.headRead);
		AfxMessageBox(_T("Insufficient memory to allocate all trace buffers."));
		return false;
	}

	WORD iPrecDecoded = image->GetBpp();
	int shifted = number_of_significant_bits_in_input_stream - iPrecDecoded;

	// to prevent any NULL pointers, this should not happen.
	if (image->pAlpha == NULL)
		shifted = 0;
	int numsamp = sw.headRead.getns();
	for (int ix = 0; ix< sw.headRead.getnx(); ix++) {
		for (int j = 0; j < numsamp; j++) {
			sw.headRead.trArr[ix].data[j] = (float)image->GetPixelIndex(ix, numsamp - 1 - j);
			// plots are upside down

			int ind = (numsamp - 1 - j) * image->info.dwEffWidth + ix;
			//BYTE b = image->info.pImage[ind];
			if (shifted > 0) {
				// the image->GetPixelIndex() is shifted by  shifted bits
				BYTE c = (BYTE)(image->pAlpha[ind] << (8 - shifted));
				c = c >> (8 - shifted); 
				sw.headRead.trArr[ix].data[j] = (float)(image->info.pImage[ind] << shifted) + c;
			}
			else
				sw.headRead.trArr[ix].data[j] = image->info.pImage[ind];
		}
	} // for nxStore

	if (sw.headRead.trArr[0].tracl == 0) {
		// need a fix here, since my program may not work with zero trace number
		for (int ix = 0; ix < sw.headRead.getnx(); ix++) {
			sw.headRead.trArr[ix].tracl += 101;
		}
	}

	float val;
//	int numsamp = sw.headRead.numsamp();
	for (int ix = 0; ix < sw.headRead.getnx(); ix++) {
		if (img_transform_gain > MINAMP) {
			for (int j = 0; j < numsamp; j++) {
				sw.headRead.trArr[ix].data[j] /= (float)img_transform_gain;
				sw.headRead.trArr[ix].data[j] += (float)img_transform_offset;
			}
		}

		// now remove avg value
		val = (float)getMedianAbsolute(sw.headRead.trArr[ix].data, numsamp);
		for (int j = 0; j < numsamp; j++) {
			sw.headRead.trArr[ix].data[j] -= val;
		}
	}
	// now, we have everything except the binary file header and binary trace header
	sw.writeHeaderOnly();
	sw.writeDataAll();

	return true;
}






bool jp2_segy::loadSxml2Head(CString strXml, HEAD_READ *headRead)
{
	// note: *head arrays are already initialized by now. If not, we cannot initialize it again since otherwise it would come to this routine again
	MSXML::IXMLDOMDocument2Ptr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		AfxMessageBox(_T("Failed to create the XML class instance"), MB_ICONQUESTION);
		return false;
	}

	CString tStr;
	int i = strXml.Find(_T("?>"));
	if (i>0)
		strXml = strXml.Mid(i+2);
	_bstr_t xStr(strXml);

	hr = pXMLDoc->loadXML(xStr);
	if (hr == VARIANT_FALSE) {
		ShowError(pXMLDoc);
		return false;
	}

	MSXML::IXMLDOMNodePtr pNode;
	MSXML::IXMLDOMNodePtr pAttr;
	MSXML::IXMLDOMElementPtr pRoot = pXMLDoc->GetdocumentElement();
	int num, num2;
	try {
		MSXML::IXMLDOMNodeListPtr pExt = 
			pXMLDoc->selectNodes(_T("/JPEG2000_SEGY/FileInfo/bounding_box"));
		if (pExt != NULL) {
			pNode = myGetSingleChildByName(pExt->Getitem(0), _T("number_of_traces"));
			num = 0;
			if (pNode) num = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pExt->Getitem(0), _T("number_of_samples_per_trace"));
			num2 = 0;
			if (pNode) num2 = (int)_wtoi(pNode->Gettext());

			headRead->setnx(num);
			headRead->setnumsamp(num2);
			headRead->trArr = (segydynamic *)xmalloc(sizeof(segydynamic) * num);

			// do not allocate data arrays as we do not add data here, only the headers
			//for (int ix = 0; ix < headRead->nxmax; ix++) {
			//	headRead->trArr[ix].data = ealloc1float(headRead->numsamp());
			//}
		}

		// now process for file binary header
		MSXML::IXMLDOMNodeListPtr pBinhead = 
			pXMLDoc->selectNodes(_T("/JPEG2000_SEGY/FileInfo/input_file_contents/segy_contents/binary_file_header"));
		if (pBinhead != NULL) {
			pNode = myGetSingleChildByName(pBinhead->Getitem(0), _T("data_sample_format_code"));
			headRead->bh.format = 5;
		//	if (pNode) head->formatFile = (int)_wtoi(pNode->Gettext());
		}

		pExt = 
			pXMLDoc->selectNodes(_T("/JPEG2000_SEGY/FileInfo/jpeg2000_compression_parameters"));
		if (pExt != NULL) {
			pNode = myGetSingleChildByName(pExt->Getitem(0), _T("transform_gain"));
			if (pNode) img_transform_gain = (double)strtod(pNode->Gettext(), NULL);

			pNode = myGetSingleChildByName(pExt->Getitem(0), _T("transform_offset"));
			if (pNode) img_transform_offset = (double)strtod(pNode->Gettext(), NULL);

			pNode = myGetSingleChildByName(pExt->Getitem(0), _T("number_of_significant_bits_in_input_stream"));
			if (pNode) number_of_significant_bits_in_input_stream = (int)strtod(pNode->Gettext(), NULL);
		}

		
		// now process for file ascii header
		MSXML::IXMLDOMNodeListPtr pAschead = 
			pXMLDoc->selectNodes(_T("/JPEG2000_SEGY/FileInfo/input_file_contents/segy_contents/text_file_header/text"));
		if (pAschead != NULL) {
			// tStr = _T("");
			int tegerfw = pAschead->length;
			CComBSTR bstr;
			_bstr_t bstr2;
			CComBSTR bstrSpace(OLESTR(" "));
			for (int i = 0; i < pAschead->length; i++) {
				bstr2 = pAschead->Getitem(i)->Gettext();
				int len2 = bstr2.length();
				bstr.AppendBSTR(bstr2);
				if (len2 < 80) {
					for (int j = 0; j < 80 - len2; j++) {
						bstr.AppendBSTR(bstrSpace);
					}
				}
			}
			headRead->ascHeadc = CString(bstr.Detach());
			//memcpy(head->ascHead, tStr, 3200);
		}

		MSXML::IXMLDOMNodeListPtr pTraces = 
			pXMLDoc->selectNodes(_T("/JPEG2000_SEGY/FileInfo/input_file_contents/segy_contents/binary_trace"));
		int num_traces = min(pTraces->length, headRead->getnx());
		if (num_traces < headRead->getnx()) {
			// todo: need to de-alloc extra memories
			for (int i = num_traces; i<headRead->getnx(); i++) {
				if (headRead->trArr[i].data != NULL) {
					free1float(headRead->trArr[i].data);
					headRead->trArr[i].data = NULL;
				}
			} // for
			headRead->setnx(num_traces);
		}

		register bool isRef =
			getAttributeDef(pTraces->Getitem(0)->attributes->getNamedItem(_T("is_reference_header")), false);
		if (isRef) {
			// this is always the first trace, numbered 0 by Bob. But it should be something like 101 
			memset((void *)&headRead->trArr[0], (int) '\0', 240);
			headRead->trArr[0].tracl = (int)getAttributeDef(pTraces->Getitem(0)->attributes->getNamedItem(_T("trc_num")), 0);
			headRead->setdt(0.004);
			// headRead->dtRead = 0.004;
			pNode = myGetSingleChildByName(pTraces->Getitem(0), _T("sample_interval__x0028_usec_x0029_"));
			if (pNode) headRead->setdt(strtod(pNode->Gettext(), NULL) / (double)MIL);
			headRead->trArr[0].dt = headRead->bh.hdt = headRead->bh.dto = 
				(unsigned short)(headRead->getdt() * MIL);
			headRead->trArr[0].ns = (unsigned short)headRead->getns();

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("year_data_recorded"));
			headRead->trArr[i].year = 0;
			if (pNode) headRead->trArr[i].year = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("day"));
			headRead->trArr[i].day = 0;
			if (pNode) headRead->trArr[i].day = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("hr"));
			headRead->trArr[i].hour = 0;
			if (pNode) headRead->trArr[i].hour = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("min"));
			headRead->trArr[i].minute = 0;
			if (pNode) headRead->trArr[i].minute = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("sec"));
			headRead->trArr[i].sec = 0;
			if (pNode) headRead->trArr[i].sec = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("lag_time_A"));
			headRead->trArr[i].laga = 0;
			if (pNode) headRead->trArr[i].laga = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("lag_time_B"));
			headRead->trArr[i].lagb = 0;
			if (pNode) headRead->trArr[i].lagb = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("delay_record_time"));
			headRead->trArr[i].delrt = 0;
			if (pNode) headRead->trArr[i].delrt = (short)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("coordinate_units"));
			headRead->trArr[i].counit = 0;
			if (pNode) headRead->trArr[i].counit = (short)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("rx_x"));
			headRead->trArr[i].gx = 0;
			if (pNode) headRead->trArr[i].gx = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("rx_y"));
			headRead->trArr[i].gy = 0;
			if (pNode) headRead->trArr[i].gy = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("tx_x"));
			headRead->trArr[i].sx = 0;
			if (pNode) headRead->trArr[i].sx = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(0), _T("tx_y"));
			headRead->trArr[0].sy = 0;
			if (pNode) headRead->trArr[0].sy = (int)_wtoi(pNode->Gettext());
		}



		for (int i = 0; i < num_traces; i++) {
			// processing for trace header
			if (i>0)
				memcpy((void *)& headRead->trArr[i], (void *)& headRead->trArr[i - 1], 240L);

			headRead->trArr[i].tracl = (int)getAttributeDef(pTraces->Getitem(i)->attributes->getNamedItem(_T("trc_num")), 0);

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("rx_x"));
			headRead->trArr[i].gx = 0;
			if (pNode) headRead->trArr[i].gx = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("rx_y"));
			headRead->trArr[i].gy = 0;
			if (pNode) headRead->trArr[i].gy = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("tx_x"));
			headRead->trArr[i].sx = 0;
			if (pNode) headRead->trArr[i].sx = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("ensemble"));
			headRead->trArr[i].cdp = 0;
			if (pNode) headRead->trArr[i].cdp = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("energy_source_point"));
			headRead->trArr[i].ep = 0;
			if (pNode) headRead->trArr[i].ep = (int)_wtoi(pNode->Gettext());
			if (headRead->trArr[i].fldr == 0)
				headRead->trArr[i].fldr = headRead->trArr[i].ep;

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("trace_in_orig_field_record"));
			headRead->trArr[i].tracr = 0;
			if (pNode) headRead->trArr[i].tracr = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("trace_in_file"));
			headRead->trArr[i].tracf = 0;
			if (pNode) headRead->trArr[i].tracf = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("trace_in_line"));
			headRead->trArr[i].cdpt = 0;
			if (pNode) headRead->trArr[i].cdpt = (int)_wtoi(pNode->Gettext());
		}  // end processing for traces




	}
	catch (...) {
		// it's ok to have no limits defined
	}

	return true;
}







/////////////////////////////////////////////////////////////////
// Jp2   routines
const bool jp2_segy::is_valid_jp2(CFile* file)
{
	unsigned char asc[5];
	ULONGLONG pos = file->Seek(4, CFile::begin);
	file->Read(&asc[0], 4L);
	file->Seek(pos-4, CFile::begin);
	// if(String::Compare(this->get_next_box_header(),"jP  ") == 0 ) 
	bool isTrue = (asc[0] == 0x6A
		&& asc[1] == 0x50
		&& asc[2] == 32
		&& asc[3] == 32);
	return isTrue;
}

// before the call, the file pointer must be at the beginning of this box
const CString jp2_segy::get_next_box_header(CFile *file, UINT *box_length)
{
	ULONGLONG pos = file->Seek(0, CFile::current);
	if (pos >= file->GetLength()-8) return _T("");  // no box existign

	file->Read(box_length, 4L);
	swap_u_int_4(box_length);

	unsigned char asc[5];
	file->Read(&asc[0], 4); 
	asc[4] = 0;
	CString box_label = CString(asc);
	return box_label;
}

// after this call, the file pointer is at the end of the box label,
// and the length of the box will be returned. If return 0, no data
const UINT jp2_segy::find_box(CFile* file, CString boxname)
{
	if( !is_valid_jp2(file)) return false;

	// loop through boxes and box contents
	UINT box_length;
	while (CString name = get_next_box_header(file, &box_length)) {
		if (name.IsEmpty() || box_length<1) break;
		if ( name.Compare(boxname) != 0 )  {
			// this is not the box. let's skip the data and go to the next one
			file->Seek(box_length-8, CFile::current);
			continue;
		}

		// now the file pointer is at the end of the box lebel
		return box_length;
	}

	return false;
}



CString jp2_segy::getJp2XML(CString f)
{
	// This routine tries to determine the smallest tmin for all teaces; should consider unix as well!
	// note: *head arrays are already initialized by now. If not, we should initialize it again

	//CTime aTime = getFileModifiedTime(f);
	//if (head->numTraces>0 && aTime == head->modifiedtime) return false;
	CString tStrBlank = _T("");
	CFile file;
	if (!file.Open(f, CFile::modeRead | CFile::typeBinary | CFile::shareDenyNone)) {
		return tStrBlank;
	}


	UINT size = find_box(&file, _T("sxml"));
	size -= 8;  // exclude the first 8 bytes
	if (size < 4) {
		file.Close();
		return  tStrBlank;
	}

	// now we need to read this box data for size bytes.
	//unsigned char *pData = new unsigned char [(unsigned int)(1+size)];
	unsigned char *pData = (unsigned char*)alloc1(1 + size, sizeof(unsigned int));

	if( pData == NULL) {
		file.Close();
		return  tStrBlank;
	}

	try{
		UINT sizeRead = file.Read(pData, size);
		if (sizeRead != size) {
			// failed
			file.Close();
			return tStrBlank;
		}
	}
	catch( CFileException *e ){
		e->Delete();
	}

	file.Close();

	if (pData[0] == 31 && pData[1] == 139) {
		// this is gzipped xml
		CString fStr = getSystemTempPathSlash() + _T("SeisWideTmp.gz");
		CFile fileOut;
		if( !fileOut.Open( fStr, CFile::modeCreate | CFile::modeWrite ) )
			return tStrBlank;
		fileOut.Write(pData, size);
		fileOut.Close();

		zlib::CGZip gzip;
		if(!gzip.Open(fStr, zlib::CGZip::ArchiveModeRead)) {
			return tStrBlank;
		}
		free1(pData);
		//char* pBuffer = NULL;
		size_t *plen = 0;
		gzip.ReadString(&pData);
		gzip.Close();
	}
	else {
		// this is pure xml
		//pData[size] = 0;
		//if( !fileOut.Open( _T("H:\\aa.xml"), CFile::modeCreate | CFile::modeWrite ) )
		//	return false;
		//fileOut.Write(pData, size);
	}

	// now the data is inside pBuffer
	CString strXml = CString(pData);
	free1(pData);

	return strXml;


}///////////////////////////////////////////////////////////////////////////


