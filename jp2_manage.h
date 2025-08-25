#pragma once
#include "segy.h"

class jp2_manage
{
public:
	jp2_manage(void);



	// jp2 routines for meta data only
	static bool is_valid_jp2(CFile* file);
	static CString get_next_box_header(CFile *filea, UINT *box_length);
//	static int segyHeadInitJp2(CString t, HEAD *head, int numRequired);
	static UINT find_box(CFile* file, CString boxname);
	static int getNumTraceInXImage(CString f);
	static int getNumTraceInJp2(CString f);
	static bool getTraceMetaJp2More(CString f, HEAD *head);





	// xml functions
	static bool loadSxml2Head(CString strXml, HEAD *head);
	static bool saveHead2Sxml(HEAD *head, CString strXml);


	static bool openTraceJp2(CString f, HEAD *head, segydynamic *trArr, int numTraceDesired=0, 
		int trace1=0, int trace2=999999, int traceStep=1, double dyMin=0.);

	// write an serializable object to an xml file
	//static bool write_object_to_xml_text_file(Object^ input_object, CString xml_text_file_name);
	//static Object^ read_object_from_xml_text_file(System::Type^ input_object_type, CString xml_text_file_name);


	// write a serializable object to a gzipped  xml file
	//static bool  write_object_to_xml_gzip_file(Object^ input_object, CString xml_gzip_file_name);

private:
	static bool openTraceJp2Private(CString f, HEAD *head, segydynamic *trArr, int traceStep, int colMin, int colMax);



};

