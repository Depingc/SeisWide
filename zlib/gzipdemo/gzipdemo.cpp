// gzipdemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "gzip.h"
#include <stdlib.h>

int main(int argc, char* argv[])
{
	using namespace zlib;

	CGZip gzip;

	// creating buffer...
	const size_t tsLen=1024000;
	char szTestString[tsLen+1];
	for (size_t i=0;i<tsLen;i++)
	{
		szTestString[i]=(char)(65+(double)rand()/RAND_MAX*(90-65) );
	}
	szTestString[tsLen]='\0';

	printf(_T("GZip wrapper Test: Writing reading 1Mb string\b\nCoded by Jonathan de Halleux, dehalleux@pelikhan.com, 2002\n\n"));
	///////////////////////////////////////////////////////////////////////////////////////
	// Writing
	printf(_T("GZip file creation test\n"));
	if (!gzip.Open(_T("gziptest.txt.gz"), CGZip::ArchiveModeWrite))
	{
		printf(_T("Could not create file\n"));
		exit(-1);
	}
	printf(_T("\tFile opening success\n"));

	// writing test string....
	if (!gzip.WriteString(szTestString ))
	{
		printf(_T("Could not write test string\n"));
		exit(-1);
	}
	printf(_T("\tString writing success\n"));

	if (!gzip.Close())
	{
		printf(_T("Could not close file\n"));
		exit(-1);
	}
	printf(_T("\tFile closing success\n"));


	///////////////////////////////////////////////////////////////////////////////////////
	// Reading
	printf(_T("\n\nGZip file reading test\n"));
	if (!gzip.Open(_T("gziptest.txt.gz"), CGZip::ArchiveModeRead))
	{
		printf(_T("Could not create file\n"));
		exit(-1);
	}
	printf(_T("\tFile opening success\n"));

	// reading buffer
	char* pBuffer=NULL;
	printf(_T("\tReading String\n"));
	gzip.ReadString(&pBuffer);

//	printf("%s\n\n%s\n\n",szTestString,pBuffer);
	// comparing...
	if (memcmp( pBuffer, szTestString, sizeof(szTestString)) == 0)
		printf(_T(" \tDecompression successful\n"));
	else
		printf(_T("\tDecompression failed\n"));

	delete[] pBuffer;

	if (!gzip.Close())
	{
		printf(_T("Could not close file\n"));
		exit(-1);
	}
	printf(_T("\tFile closing success\n"));

	return 0;
}

