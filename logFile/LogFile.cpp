#include "stdafx.h"

#include "LogFile.h"
#include <direct.h>

void CLogFile::OpenFile(LPCTSTR strFile, bool bAppend, long lTruncate)
{
	m_lTruncate = lTruncate;
	m_filename = strFile;

	TCHAR	szFile[MAX_PATH+1];
	if (_tcslen(strFile)>3 && strFile[1]!=':')	//no absolute path designated
	{
		::GetModuleFileName(NULL, szFile, MAX_PATH);
		long llength = _tcslen(szFile);
		TCHAR*	pcat = szFile+(llength - 1);	//point to the last char
		while (llength--)
		{
			pcat--;
			if (*pcat == '\\')
				break;
		}
		if (*pcat == '\\')
		{
			pcat++;
			_tcscpy(pcat, strFile);
		}
		else	//something wrong, use the original filename, ignore path problem
			_tcscpy(szFile, strFile);
	}
	else
		_tcscpy(szFile, strFile);

	if (!(m_pLogFile = fopen(szFile, bAppend ? "a" : "w")))
		{
		CreateDirectories(szFile);

		m_pLogFile = fopen(szFile, bAppend ? "a" : "w");
		}

	InitializeCriticalSection(&m_cs);
}

CLogFile::CLogFile()
{
	OpenFile("demo.log", TRUE);
}

	/////////////////////////////////
	//	Destructor, close if logfile if opened
CLogFile::~CLogFile()
{
	CloseFile();
}

void CLogFile::CloseFile()
{
	if (m_pLogFile)
		{
		fputs("\n===============Finish Loging================\n\n", m_pLogFile);

		fclose(m_pLogFile);
		}

	DeleteCriticalSection(&m_cs);
}

void CLogFile::ChangeFile(LPCTSTR strFile, bool bAppend, long lTruncate)
{
	if (strFile != m_filename)
		{
		CloseFile();
		OpenFile(strFile, bAppend, lTruncate);
		}
}

bool CLogFile::CreateDirectories(CString filename)
{
	CString drivename, path;
	int drive, per;

	drivename = filename.Left(3);
	drive = drivename[0] - 'A' + 1;
	_chdrive(drive);

	for (per = 1; per <= 20; per++)
		_chdir("..");

	filename = filename.Mid(3);
	while (!filename.IsEmpty())
		{
		per = filename.Find('\\');

		if (per == -1)
			break;

		path = filename.Left(per);
		if (_chdir(path))
			{
			_mkdir(path);
			_chdir(path);
			}

		filename = filename.Mid(per + 1);
		}

}
