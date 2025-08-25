#include "StdAfx.h"
#include "jp2_manage.h"
#include "util.h"

#include "suSwapByte.h"
#include "gzip.h"
//using namespace zlib;



jp2_manage::jp2_manage(void)
{
}


//CFile file;
//if (!file.Open(f, CFile::modeRead | CFile::typeBinary)) {
//	return;
//}
//segy tr;
//bhed bh;
//file.Read(head->ascHead, 3200L);
//if (!isAsciiPure(head->ascHead, 3200)) {
//	ebc2asc(head->ascHead, 3200);
//	if (!isAsciiPure(head->ascHead, 3200)) {
//		asc2ebc(head->ascHead, 3200); // wrong conversion, so undo
//		asc2ebc(head->ascHead, 3200); // after undo, convert the other direction
//	}
//}

//file.Read(&bh, 400L);
//BOOL isUnix = IsUnix(bh.format);
//if (isUnix) SwapTapeHeader(&bh);

//int sampByte = (bh.format != 3) ? 4 : 2;
//int traceDataBytes = sampByte * bh.hns;

//int i = 0;
//while (file.Read(&tr, 240) == (UINT)240) {
//	if (isUnix)
//		SwapTraceHeader(&tr);
//	head->traces[i] = tr.tracl;
//	head->tminArr[i] = getTraceDelay(&tr);
//	head->cmp[i] = tr.cdp;
//	head->ffid[i] = tr.fldr;
//	head->dist[i] = getDistance(&tr);
//	head->offsets[i] = getOffset(&tr);
//	head->pDataArr[i] = file.Seek(0, CFile::current);
//	i++;
//	if (i >= head->numTraces) break;  // no more traces can be read in
//	file.Seek(traceDataBytes, CFile::current);
//}
//file.Close();
//head->tminFile = getMinValue(head->tminArr, head->numTraces);
//head->tmaxFile = getMaxValue(head->tminArr, head->numTraces) + (double)((bh.hns - 1) * (double)bh.hdt / (double)MIL);
//head->modifiedtime = aTime;

#include <stdlib.h>
#include <iostream>
#include <fstream>

// Kakadu core includes
#include "kdu_arch.h"
#include "kdu_elementary.h"
#include "kdu_messaging.h"
//#include "kdu_params.h"
//#include "kdu_compressed.h"
#include "kdu_sample_processing.h"
// Application level includes
//#include "kdu_file_io.h"
//#include "kdu_compressed.h"
#include "kdu_sample_processing.h"
#include "kdu_stripe_decompressor.h"

// Application includes
#include "kdu_file_io.h"
#include "jp2.h"

// head->aveAmp amd head->maxAmp  will be calculated 
// in a function that calls openTrace() to supply 100 traces for calculation

/* ========================================================================= */
/*                         Set up messaging services                         */
/* ========================================================================= */

class kdu_stream_message : public kdu_message {
public: // Member classes
	kdu_stream_message(std::ostream *stream)
	{ this->stream = stream; }
	void put_text(const char *string)
	{ (*stream) << string; }
	void flush(bool end_of_message=false)
	{ stream->flush(); }
private: // Data
	std::ostream *stream;
};

static kdu_stream_message cout_message(&std::cout);
static kdu_stream_message cerr_message(&std::cerr);
static kdu_message_formatter pretty_cout(&cout_message);
static kdu_message_formatter pretty_cerr(&cerr_message);


/* ========================================================================= */
/*                               kd_output _file                             */
/* ========================================================================= */

struct kd_output_file {
public: // Lifecycle functions
	kd_output_file()
	{ bytes_per_sample=1; precision=8;
	is_signed=true; size=kdu_coords(0,0); stripe_index=0; }
	void write_stripe(int *heights, kdu_int16 **bufs, int num_component);
public: // Data
	segydynamic * trArr;
	int bytes_per_sample;
	int precision; // Num bits, 8 or 16 only
	bool is_signed; // for the input file
	kdu_coords size; // Width, and remaining rows
	int stripe_index;
};


/*****************************************************************************/
/*                         kd_output_file::write_stripe                      */
/*****************************************************************************/

void
	kd_output_file::write_stripe(int *heights, kdu_int16 **bufs, int num_component)
{
	int i,n;
	kdu_int16 val, off = (is_signed)?0:((1<<precision)>>1);


	for (int icomp=0; icomp<num_component; icomp++) {
		int num_samples = heights[icomp]*size.x;
		int num_bytes = num_samples * bytes_per_sample;
		if (num_samples <= 0) continue;
		kdu_int16 *sp = bufs[icomp];
		if (bytes_per_sample == 1)
		{ // Reduce to an 8-bit unsigned representation
			kdu_byte *dp=(kdu_byte *) bufs[icomp];
			for (n=num_samples; n > 0; n--)
				*(dp++) = (kdu_byte)(*(sp++) + off);
		}
		else if (bytes_per_sample == 2)
		{ // Swap byte order to small endian and make representation unsigned if necessary
			for (n=num_samples; n > 0; n--)
			{ val = *sp+off; val = (val<<8) + ((val>>8)&0x00FF); *(sp++) = val; }
		}
		else if (off != 0)
		{ // Make representation unsigned
			for (n=num_samples; n > 0; n--)
			{ val = *sp+off; *(sp++) = val; }
		}
	}

	if (bytes_per_sample == 1) {
		kdu_byte **dps=(kdu_byte **) bufs;
		if (num_component==1) {
			int j1 = stripe_index*heights[0];
			for (n=0; n=size.x; n++) {
				for (i=0; i<heights[0]; i++) {
					trArr[n].data[j1+heights[0]] = (float)(255 - dps[0][i*size.x + n]);
				}
			}
		}
		else {
			// 3 components case, help me!!!
			int j1 = stripe_index*heights[0];
			for (n=0; n=size.x; n++) {
				for (i=0; i<heights[0]; i++) {
					register int j = i*size.x + n;
					trArr[n].data[j1+heights[0]] = (float)RGB(dps[0][j], dps[1][j], dps[2][j]);
				}
			}
		}
	}
	else {
		// 2 bytes per sample
		// bufs[0][] is the first component. bufs[2][] is the third component
		if (num_component==1) {
			int j1 = stripe_index*heights[0];
			for (n=0; n=size.x; n++) {
				for (i=0; i<heights[0]; i++) {
					trArr[n].data[j1+heights[0]] = (float)(bufs[0][i*size.x + n]);
				}
			}
		}
		else {
			// 3 components case, not existing???
		}
	}
}


/*****************************************************************************/
/* STATIC                     check_jp2_family_file                          */
/*****************************************************************************/

static bool
	check_jp2_family_file(const char *fname)
	/* This function opens the file and checks its first box, to see if it
	contains the JP2-family signature.  It then closes the file and returns
	the result.  This should avoid some confusion associated with files
	whose suffix has been unreliably names. */
{
	jp2_family_src src;
	jp2_input_box box;
	src.open(fname);
	bool result = box.open(&src) && (box.get_box_type() == jp2_signature_4cc);
	src.close();
	return result;
}

/*****************************************************************************/
/* STATIC                        get_bpp_dims                                */
/*****************************************************************************/

static kdu_long
	get_bpp_dims(siz_params *siz)
	/* Figures out the number of "pixels" in the image, for the purpose of
	converting byte counts into bits per pixel, or vice-versa. */
{
	int comps, max_width, max_height, n;

	siz->get(Scomponents,0,0,comps);
	max_width = max_height = 0;
	for (n=0; n < comps; n++)
	{
		int width, height;
		siz->get(Sdims,n,0,height);
		siz->get(Sdims,n,1,width);
		if (width > max_width)
			max_width = width;
		if (height > max_height)
			max_height = height;
	}
	return ((kdu_long) max_height) * ((kdu_long) max_width);
}




/*****************************************************************************/
/* STATIC                         print_usage                                */
/*****************************************************************************/

static void
	print_usage(char *prog)
{
	kdu_message_formatter out(&cout_message);

	out << "Usage: \"" << prog << " <PNM input file> <J2C output file>\"\n";
	exit(0);
}

// before calling this function, head* must already be assigned
bool jp2_manage::openTraceJp2(CString f, HEAD *head, segydynamic *trArr, int numTraceDesired, 
							  int trace1, int trace2, int traceStep, double dyMin)
{
	bool isTrue = openTraceJp2Private(f, head, trArr, traceStep, trace1, trace2);

	// now need to trim the data according to display params
	return true;
}

bool jp2_manage::openTraceJp2Private(CString f, HEAD *head, segydynamic *trArr, int traceStep, int colMin, int colMax)
{
	if (trArr != NULL) {
		freeSegyDataArrayOnly(trArr, head->numTraces);
		if (head->numTraces > 0) free(trArr);  // note: if nx==0, I cannot free trArr since it would give me an exception.
		trArr = NULL;
	} // if


	int preferred_min_stripe_height=8, absolute_max_stripe_height=1024;
	kdu_dims region;
	int num_threads;

	if ((num_threads = kdu_get_num_processors()) < 2)
		num_threads = 0;

	// Custom messaging services
	kdu_customize_warnings(&pretty_cout);
	kdu_customize_errors(&pretty_cerr);

	// Load image into a buffer
	int num_components;

	// convert CString to char*
	int nLeng    = (f.GetLength() + 1)*sizeof(TCHAR);
	char *ifname = new char[nLeng];
	strcpy_s(ifname, nLeng, CT2A(f));

	kdu_compressed_source *input = NULL;
	kdu_simple_file_source file_in;
	jp2_family_src jp2_ultimate_src;
	jp2_source jp2_in;
	if (check_jp2_family_file(ifname))
	{
		input = &jp2_in;
		jp2_ultimate_src.open(ifname);
		if (!jp2_in.open(&jp2_ultimate_src))
		{ kdu_error e; e << "Supplied input file, \"" << ifname << "\", does "
		"not appear to contain any boxes."; }
		jp2_in.read_header();
	}
	else
	{
		input = &file_in;
		file_in.open(ifname);
	}
	delete[] ifname;

	// Create the code-stream, and apply any restrictions/transformations
	kdu_codestream codestream;
	codestream.create(input);

	kdu_long max_bytes = KDU_LONG_MAX;
	codestream.set_max_bytes(max_bytes,true);

	codestream.apply_input_restrictions(0,0,
		0,0,NULL,
		KDU_WANT_OUTPUT_COMPONENTS);

	kdu_dims *reg_ptr = NULL;
	if (region.area() > 0)
	{
		kdu_dims dims; codestream.get_dims(0,dims,true);
		dims &= region;
		if (!dims)
		{ kdu_error e; e << "Region supplied via `-int_region' argument "
		"has no intersection with the first image component to be "
		"decompressed, at the resolution selected."; }
		codestream.map_region(0,dims,region,true);
		reg_ptr = &region;
		codestream.apply_input_restrictions(0,0,
			0,0,reg_ptr,
			KDU_WANT_OUTPUT_COMPONENTS);
	}

	// If you wish to have geometric transformations folded into the
	// decompression process automatically, this is the place to call
	// `kdu_codestream::change_appearance'.


	// Find the dimensions of each image component we will be decompressing
	num_components = codestream.get_num_components(true);
	kdu_dims *comp_dims = new kdu_dims[num_components];
	for (int n=0; n < num_components; n++)
		codestream.get_dims(n,comp_dims[n],true);

	// Next, prepare the output files
	// Deping mod: I cannot accept to have different components with different size, prevision and is_signed values
	//             and I am assuming them to be the same for each component
	kd_output_file *out = new kd_output_file;
	out->size = comp_dims[0].size;
	out->precision = codestream.get_bit_depth(0,true);
	out->is_signed = codestream.get_signed(0,true);
	assert(out->precision > 0);
	if (out->precision > 16)
		out->precision = 16; // Can't store more than 16 bits/sample
	out->bytes_per_sample = (out->precision > 8)?2:1;

	// we already have:
	head->numTraces = out->size.x;
	head->numsampFile = out->size.y;
	if (num_components>1) {
		head->isDataRgb = true;
		for (int n=0; n < num_components; n++) {
			if (out->size != comp_dims[n].size  ||
				out->precision != codestream.get_bit_depth(n,true) ||
				out->is_signed != codestream.get_signed(n,true))
				{ kdu_error e; e << "Components have different size, precision or sign conventions, not supported by SeisWide."; }
		}
	}
	trArr = (segydynamic *)malloc( sizeof(segydynamic) * head->numTraces );
	for (int nxStore=0; nxStore< head->numTraces; nxStore++) {
		trArr[nxStore].data = (float *)malloc(sizeof(float)*head->numsampFile);
		if (ERROR == trArr[nxStore].data) 
			{ kdu_error e; e << "Insufficient memory to allocate all trace buffers."; }
	} // for nxStore


	// Construct multi-threaded processing environment, if requested.  Note that
	// all we have to do to leverage the presence of multiple physical processors
	// is to create the multi-threaded environment with at least one thread for
	// each processor, pass a reference (`env_ref') to this environment into
	// `kdu_stripe_decompressor::start', and destroy the environment once we are
	// all done.
	//    If you are going to run the processing within a try/catch
	// environment, with an error handler which throws exceptions rather than
	// exiting the process, the only extra thing you need to do to realize
	// robust multi-threaded processing, is to arrange for your `catch' clause
	// to invoke `kdu_thread_entity::handle_exception' -- i.e., call
	// `env.handle_exception(exc)', where `exc' is the integer-valued exception
	// code which you catch.  Even this is not necessary if you are happy for
	// the `kdu_thread_env' object to be destroyed when an error/exception
	// occurs.
	kdu_thread_env env, *env_ref=NULL;
	if (num_threads > 0)
	{
		env.create();
		for (int nt=1; nt < num_threads; nt++)
			if (!env.add_thread())
				num_threads = nt; // Unable to create all the threads requested
		env_ref = &env;
	}

	// Construct the stripe-decompressor object (this does all the work) and
	// assigns stripe buffers for incremental processing.  The present application
	// uses `kdu_stripe_decompressor::get_recommended_stripe_heights' to find
	// suitable stripe heights for processing.  If your application has its own
	// idea of what constitutes a good set of stripe heights, you may generally
	// use those values instead (could be up to the entire image in one stripe),
	// but note that whenever the image is tiled, the stripe heights can have an
	// impact on the efficiency with which the image is decompressed (a
	// fundamental issue, not a Kakadu implementation issue).  For more on this,
	// see the extensive documentation provided for
	// `kdu_stripe_decompressor::pull_stripe'.
	int *stripe_heights = new int[num_components];
	int *max_stripe_heights = new int[num_components];
	kdu_int16 **stripe_bufs = new kdu_int16 *[num_components];
	// Note: we will be using 16-bit stripe buffers throughout for
	// this demonstration, but you can supply 8-bit stripe buffers
	// to `kdu_stripe_compressor::push_stripe' if you prefer.
	kdu_stripe_decompressor decompressor;
	decompressor.start(codestream,false,false,env_ref,NULL,0);
	decompressor.get_recommended_stripe_heights(preferred_min_stripe_height,
		absolute_max_stripe_height,
		stripe_heights,
		max_stripe_heights);
	for (int n=0; n < num_components; n++)
		if ((stripe_bufs[n] =
			new kdu_int16[comp_dims[n].size.x*max_stripe_heights[n]]) == NULL)
	{ 
		kdu_error e; e << "Insufficient memory to allocate stripe buffers."; 
	}

	int *precisions = new int[num_components];
	for (int n=0; n < num_components; n++)
		precisions[n] = out->precision;

	// Now for the incremental processing
	out->stripe_index = 0;
	out->trArr = trArr;
	bool continues=true;
	while (continues)
	{
		decompressor.get_recommended_stripe_heights(preferred_min_stripe_height,
			absolute_max_stripe_height,
			stripe_heights,NULL);
		continues = decompressor.pull_stripe(stripe_bufs,stripe_heights,NULL,NULL,
			precisions);
		out->write_stripe(stripe_heights,stripe_bufs, num_components);
		out->stripe_index++;
	}
	
	// Clean up
	decompressor.finish();
	if (env.exists())
		env.destroy(); // You can do this either before or after you destroy the
	// codestream, but not until after `kdu_stripe_decompressor::finish'.
	codestream.destroy();
	input->close();
	if (jp2_ultimate_src.exists())
		jp2_ultimate_src.close();
	for (int n=0; n < num_components; n++)
		delete[] stripe_bufs[n];
	delete[] stripe_bufs;
	if (precisions != NULL)
		delete[] precisions;
	delete[] stripe_heights;
	delete[] max_stripe_heights;
	delete[] comp_dims;

	// now assign the trace header to trArr
	for (int nxStore=0; nxStore<head->numTraces; nxStore++) {

	}

	return true;
}






bool jp2_manage::loadSxml2Head(CString strXml, HEAD *head)
{
	// note: *head arrays are already initialized by now. If not, we cannot initialize it again since otherwise it would come to this routine again
	MSXML::IXMLDOMDocument2Ptr pXMLDoc;
	HRESULT hr = pXMLDoc.CreateInstance(__uuidof(DOMDocument60));
	if (FAILED(hr)) {
		AfxMessageBox(_T("Failed to create the XML class instance"));
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
			pNode = myGetSingleChildByName(pExt->Getitem(0), _T("minimum_trace_time"));
			head->tminFile = 0;
			if (pNode) head->tminFile = (int)_wtoi(pNode->Gettext()) * 0.001;

			pNode = myGetSingleChildByName(pExt->Getitem(0), _T("maximum_trace_time_x0020_"));
			head->tmaxFile = 0;
			if (pNode) head->tmaxFile = (int)_wtoi(pNode->Gettext()) * 0.001;

			pNode = myGetSingleChildByName(pExt->Getitem(0), _T("number_of_traces"));
			num = 0;
			if (pNode) num = (int)_wtoi(pNode->Gettext());

			pNode = myGetSingleChildByName(pExt->Getitem(0), _T("number_of_samples_per_trace"));
			num2 = 0;
			if (pNode) num2 = (int)_wtoi(pNode->Gettext());

			if (num>0 && num != head->numTraces) {
				// found different num traces, we should re-initialize the *head arrays
				head->numTraces = num;
				head->numsampFile = num2;
				segyHeadInitArrays(head, num);
			}
		}

		// now process for file binary header
		MSXML::IXMLDOMNodeListPtr pBinhead = 
			pXMLDoc->selectNodes(_T("/JPEG2000_SEGY/FileInfo/input_file_contents/segy_contents/binary_file_header"));
		if (pBinhead != NULL) {
			pNode = myGetSingleChildByName(pBinhead->Getitem(0), _T("data_sample_format_code"));
			head->format = 1;
			if (pNode) head->format = (int)_wtoi(pNode->Gettext());
		}

		pExt = 
			pXMLDoc->selectNodes(_T("/JPEG2000_SEGY/FileInfo/jpeg2000_compression_parameters"));
		if (pExt != NULL) {
			pNode = myGetSingleChildByName(pExt->Getitem(0), _T("transform_gain"));
			if (pNode) head->img_transform_gain = (double)strtod(pNode->Gettext(), NULL);

			pNode = myGetSingleChildByName(pExt->Getitem(0), _T("transform_offset"));
			if (pNode) head->img_transform_offset = (double)strtod(pNode->Gettext(), NULL);
		}


		// now process for file ascii header
		MSXML::IXMLDOMNodeListPtr pAschead = 
			pXMLDoc->selectNodes(_T("/JPEG2000_SEGY/FileInfo/input_file_contents/segy_contents/text_file_header/text"));
		if (pAschead != NULL) {
			// tStr = _T("");
			CComBSTR bstr;
			for (int i = 0; i < pAschead->length; i++) {
				bstr.AppendBSTR(pAschead->Getitem(i)->Gettext());
			}
			tStr = CString(bstr.Detach());
			memcpy(head->ascHead, tStr, 3200);
		}

		MSXML::IXMLDOMNodeListPtr pTraces = 
			pXMLDoc->selectNodes(_T("/JPEG2000_SEGY/FileInfo/input_file_contents/segy_contents/binary_trace"));
		if (pTraces != NULL) 
			for (int i = 0; i < pTraces->length; i++) {
				//if (i>2193) {
				//	int sdfsdd = pTraces->length;
				//	int dfs = 1;
				//}
				register bool isRef = 
					getAttributeDef(pTraces->Getitem(i)->attributes->getNamedItem(_T("is_reference_header")), false);
				if (isRef) {
					head->dt = 0.004;
					pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("sample_interval__x0028_usec_x0029_"));
					if (pNode) head->dt = strtod(pNode->Gettext(), NULL) / (double)MIL;
				}

				// processing for trace header
				if (i>= head->numTraces) break;  // cannot continue to read it
				head->traces[i] = (int)getAttributeDef(pTraces->Getitem(i)->attributes->getNamedItem(_T("trc_num")), 0);

				pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("energy_source_point"));
				head->ffid[i] = 0;
				if (pNode) head->ffid[i] = (int)_wtoi(pNode->Gettext());

				head->offsets[i] = 0;
				head->dist[i] = 0; // these were not implemented by Bob's JP2 yet

				pNode = myGetSingleChildByName(pTraces->Getitem(i), _T("ensemble"));
				head->cmp[i] = 0;
				if (pNode) head->cmp[i] = (int)_wtoi(pNode->Gettext());

			}  // end processing for traces




	}
	catch (...) {
		// it's ok to have no limits defined
	}

	return true;
}







/////////////////////////////////////////////////////////////////
// Jp2   routines
bool jp2_manage::is_valid_jp2(CFile* file)
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
CString jp2_manage::get_next_box_header(CFile *file, UINT *box_length)
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
UINT jp2_manage::find_box(CFile* file, CString boxname)
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

#include "ximage.h"
int jp2_manage::getNumTraceInXImage(CString f) {

	CString ext = GetFileExtension(f);
	int type = CxImage::GetTypeIdFromName(ext);
	CxImage *image = new CxImage(f, type);
	if (!image->IsValid()){
		CString s = image->GetLastError();
		//AfxMessageBox(s);
		delete image;
		image = NULL;
		return 0;
	}

	DWORD num = image->GetWidth();
	delete image; 
	return num;
}



//
//int jp2_manage::segyHeadInitJp2(CString t, HEAD *head, int numRequired)
//{
//	if (GetFileExtension(t).Compare(_T("jp2")) != 0) return 0;
//
//
//	if (!isFileExistNotDirectory(t)) return -1;
//	int numTotalExisting = getNumTraceInJp2(t);
//	if (numTotalExisting < 1) return 0; // no need to initialize anything
//
//	CTime aTime = getFileModifiedTime(t);
//	if (head->numTraces == numTotalExisting && aTime == head->modifiedtime)
//		return 1;
//	if (numRequired<1)
//		numRequired = numTotalExisting;
//
//	int i = segyHeadInitArrays(head, numRequired);  // ok to share jp2 with sgy routines
//	if (head->pDataArr == NULL || head->tminArr == NULL || head->dist == NULL || 
//		head->offsets == NULL || head->cmp == NULL || head->numTraces<1) {
//		return false;
//	}
//	if (!getTraceMetaJp2More(t, head)) {
//		i = 0;
//		head->numTraces = 0; // failed
//	}
//	return i;
//}
// end Jp2 routines

bool jp2_manage::getTraceMetaJp2More(CString f, HEAD *head)
{
	// This routine tries to determine the smallest tmin for all teaces; should consider unix as well!
	// note: *head arrays are already initialized by now. If not, we should initialize it again

	CTime aTime = getFileModifiedTime(f);
	if (head->numTraces>0 && aTime == head->modifiedtime) return false;

	CFile file;
	if (!file.Open(f, CFile::modeRead | CFile::typeBinary)) {
		return false;
	}


	UINT size = find_box(&file, _T("sxml"));
	size -= 8;  // exclude the first 8 bytes
	if (size < 4) {
		file.Close();
		return  false;
	}

	// now we need to read this box data for size bytes.
	//	unsigned char *pData = (unsigned char *)malloc((size_t)(1+size));
	unsigned char *pData = new unsigned char [(unsigned int)(1+size)];
	if( pData == NULL) {
		file.Close();
		return  false;
	}

	try{
		UINT sizeRead = file.Read(pData, size);
		if (sizeRead != size) {
			// failed
			file.Close();
			return false;
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
			return false;
		fileOut.Write(pData, size);
		fileOut.Close();

		zlib::CGZip gzip;
		if(!gzip.Open(fStr, zlib::CGZip::ArchiveModeRead)) {
			return false;
		}
		delete [] pData;
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
	delete [] pData;

	return jp2_manage::loadSxml2Head(strXml, head);


}///////////////////////////////////////////////////////////////////////////


