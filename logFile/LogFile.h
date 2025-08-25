#ifndef _ATA_LOGFILE_
#define _ATA_LOGFILE_

/////////////////////////////////////////////////////////////////
//features:	not depends on MFC ore ATL.
//			file name could use absolute path or just the name, in which case the 
//			file will be created at the same place with the binary module, no concern 
//			with curret directory, which always bring me truble.
//			every log line has a time stamp attached, with million seconds.
//			uses printf like format to write log lines
//			uses a preprocessor definition _DEBUG_LOG to switch if write log file
//			multi thread safe, finally added:)

////////////////////////////////////////////////////
//	CLogFile, a debug log file wrapper
class CLogFile
{
public:
	bool CreateDirectories(CString filename);
	//	Constructor, open the logfile
	CLogFile();

	//	Destructor, close if logfile if opened
	~CLogFile();

	void OpenFile(LPCTSTR strFile, bool bAppend = TRUE, long lTruncate = 4096);
	void ChangeFile(LPCTSTR strFile, bool bAppend = TRUE, long lTruncate = 4096);
	void CloseFile();

	//	Write log info into the logfile, with printf like parameters support
	void Write(LPCTSTR pszFormat, ...)
	{
#ifdef _DEBUG_LOG
		if (!m_pLogFile)
			return;

		EnterCriticalSection(&m_cs);
		//write the formated log string to szLog
		TCHAR	szLog[256];
		va_list argList;
		va_start( argList, pszFormat );
		vsprintf( szLog, pszFormat, argList );
		va_end( argList );

		//Trancate if the file grow too large
//		long	lLength = ftell(m_pLogFile);
//		if (lLength > m_lTruncate)
//			rewind(m_pLogFile);

		//Get current time
		SYSTEMTIME	time;
		::GetLocalTime(&time);
		TCHAR	szLine[256];
		
		// ido kijelzessel
//		sprintf(szLine, "%02d:%02d:%02d:%03d \t%s\n", 
//			time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
//			szLog);
		
		sprintf(szLine, "%04d/%02d/%02d %02d:%02d:%02d:%03d \t%s\n", 
			time.wYear, time.wMonth, time.wDay,
			time.wHour, time.wMinute, time.wSecond, time.wMilliseconds,
			szLog);
		
		fputs(szLine, m_pLogFile);

		fflush(m_pLogFile);

		LeaveCriticalSection(&m_cs);
#endif
	}

private:
	FILE*	m_pLogFile;
	long	m_lTruncate;
	CRITICAL_SECTION	m_cs;

	CString	m_filename;
};

#endif //_ATA_LOGFILE_
