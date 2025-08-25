// AutoUpdater.cpp: implementation of the CAutoUpdater class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoUpdater.h"
#include "util.h"
#include "ealloc_d.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define TRANSFER_SIZE 4096

CAutoUpdater::CAutoUpdater()
{
	// Initialize WinInet
	hInternet = InternetOpen(_T("AutoUpdateAgent"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);	
}

CAutoUpdater::~CAutoUpdater()
{
	if (hInternet) {
		InternetCloseHandle(hInternet);
	}
}


CString CAutoUpdater::GetUrlContents(CString url)
{
	CString strBlank = BLANK;
	if (!InternetOkay())
	{
		return strBlank;
	}

	// First we must check the remote configuration file to see if an update is necessary
	// CString URL = UpdateServerURL + CString(LOCATION_UPDATE_FILE_CHECK);
	HINTERNET hSession = GetSession(url);
	if (!hSession)
	{
		return strBlank;
	}

	BYTE pBuf[TRANSFER_SIZE];
	memset(pBuf, NULL, sizeof(pBuf));

	bool bTransferSuccess = false;
	bTransferSuccess = DownloadConfig(hSession, pBuf, TRANSFER_SIZE);
	InternetCloseHandle(hSession);
	if (!bTransferSuccess) {
		return strBlank;
	}

	CString tStr = CString(pBuf);
	return tStr;
}



// this will simply download and update a targetFile from my server at UpdateServerURL.
// Note: UpdateServerURL must have appended a slash.
// targetFileComplete must be just a single file with folder string.
CAutoUpdater::ErrorType CAutoUpdater::JustUpdate(CString UpdateServerURL, CString targetFileComplete)
{
	if (!InternetOkay()) {
		return InternetConnectFailure;
	}


	// First we must check the remote configuration file to see if an update is necessary
	CString URL = UpdateServerURL + CString(LOCATION_UPDATE_FILE_CHECK);
	HINTERNET hSession = GetSession(URL);
	if (!hSession) {
		return InternetSessionFailure;
	}

	BYTE pBuf[TRANSFER_SIZE];
	memset(pBuf, NULL, sizeof(pBuf));
	bool bTransferSuccess = DownloadConfig(hSession, pBuf, TRANSFER_SIZE);
	InternetCloseHandle(hSession);
	if (!bTransferSuccess) {
		return ConfigDownloadFailure;
	}

	// Proceed with the update
	// Download the updated file
	TCHAR tmpPath[MAX_PATH];
	GetTempPath(MAX_PATH, tmpPath);
	CString tmpPath2Use = tmpPath;
	CString targetFileNameOnly = getFileNameOnly(targetFileComplete);
	URL = UpdateServerURL + targetFileNameOnly + _T(".gz");
	hSession = GetSession(URL);
	if (!hSession) {
		return InternetSessionFailure;
	}
	CString updateFileLocationComplete = tmpPath2Use + targetFileNameOnly + _T(".gz");  // only download .gz version
	bTransferSuccess = DownloadFile(hSession, updateFileLocationComplete);
	InternetCloseHandle(hSession);
	if (!bTransferSuccess) {
		return FileDownloadFailure;
	}

	CString targetFolderOnly = GetPathFromFileSlash(targetFileComplete);
	if (!Switch(targetFileComplete, updateFileLocationComplete, false)) {
		// If not exe file, this Switch() will fail, so we just need to move the file
		MoveFile(updateFileLocationComplete, targetFileComplete);
		if (!isFileExistNotDirectory(targetFileComplete))
			return UpdateNotComplete;
	}

	return Success;
}




// Check if an update is required
//
CAutoUpdater::ErrorType CAutoUpdater::CheckForUpdateOld(LPCTSTR UpdateServerURL)
{		
	if (!InternetOkay()) {
		return InternetConnectFailure;
	}

	bool bTransferSuccess = false;

	// First we must check the remote configuration file to see if an update is necessary
	CString URL = UpdateServerURL + CString(LOCATION_UPDATE_FILE_CHECK);
	HINTERNET hSession = GetSession(URL);
	if (!hSession) {
		return InternetSessionFailure;
	}

	BYTE pBuf[TRANSFER_SIZE];
	memset(pBuf, NULL, sizeof(pBuf));
	bTransferSuccess = DownloadConfig(hSession, pBuf, TRANSFER_SIZE);
	InternetCloseHandle(hSession);
	if (!bTransferSuccess) {
		return ConfigDownloadFailure;
	}

	// Get the version number of our executable and compare to see if an update is needed
	CString executable = GetExecutable();
	CString fileVersion = GetFileVersion(executable);
	if (fileVersion.IsEmpty()) {
		return NoExecutableVersion;
	}

	CString updateVersion = (char *) pBuf;
	if (CompareVersions(updateVersion, fileVersion) != 1) {	
		return UpdateNotRequired;
	}

	// At this stage an update is required	
	TCHAR path[MAX_PATH];
	GetTempPath(MAX_PATH, path);
	CString exeName = executable.Mid(1+executable.ReverseFind(_T('\\')));
	
	CString msg;
	msg.Format(_T("An update of %s is now available. Proceed with the update? Once update process starts, please wait for the update completion message."), exeName);
	if (IDNO == MessageBox(GetActiveWindow(), msg, _T("Update is available"), MB_YESNO|MB_ICONQUESTION)) {
		return UpdateNotComplete;	
	}

	// Proceed with the update
	// Download the updated file
	CString directory = path;
	URL = UpdateServerURL + exeName + _T(".gz");
	hSession = GetSession(URL);
	if (!hSession) {
		return InternetSessionFailure;
	}
	CString updateFileLocation = directory + exeName + _T(".gz");  // only download .gz version
	bTransferSuccess = DownloadFile(hSession, updateFileLocation);
	InternetCloseHandle(hSession);
	if (!bTransferSuccess) {
		return FileDownloadFailure;
	}	

	if (!Switch(executable, updateFileLocation, false)) {
		return UpdateNotComplete;
	}
	
	return Success;
}

CString getExeNameDownload() {
	CString out;
	BOOL is32Bit = IsOS(OS_WOW6432);
	return is32Bit ? _T("SeisWide32.exe") : _T("SeisWide64.exe");
}

CAutoUpdater::ErrorType CAutoUpdater::CheckForUpdateOld2(LPCTSTR UpdateServerURL)
{
	if (!InternetOkay()) {
		return InternetConnectFailure;
	}

	bool bTransferSuccess = false;

	// First we must check the remote configuration file to see if an update is necessary
	CString URL = UpdateServerURL + CString(LOCATION_UPDATE_FILE_CHECK);
	HINTERNET hSession = GetSession(URL);
	if (!hSession) {
		return InternetSessionFailure;
	}

	BYTE pBuf[TRANSFER_SIZE];
	memset(pBuf, NULL, sizeof(pBuf));
	bTransferSuccess = DownloadConfig(hSession, pBuf, TRANSFER_SIZE);
	InternetCloseHandle(hSession);
	if (!bTransferSuccess) {
		return ConfigDownloadFailure;
	}

	// Get the version number of our executable and compare to see if an update is needed
	CString executable = GetExecutable();
	CString fileVersion = GetFileVersion(executable);
	if (fileVersion.IsEmpty()) {
		return NoExecutableVersion;
	}

	CString updateVersion = (char *)pBuf;
	if (CompareVersions(updateVersion, fileVersion) != 1) {
		return UpdateNotRequired;
	}

	// At this stage an update is required	
	//TCHAR path[MAX_PATH];
	//GetTempPath(MAX_PATH, path);
	// CString exeName = executable.Mid(1 + executable.ReverseFind(_T('\\')));

	CString astr = GetBaseFileName(executable);

	// CString exeName2Download = getExeNameDownload();



	CString msg;
	msg.Format(_T("An update of %s is now available. Proceed with the update? Once update process starts, please wait for the update completion message."), astr);
	if (IDNO == MessageBox(GetActiveWindow(), msg, _T("Update is available"), MB_YESNO | MB_ICONQUESTION)) {
		return UpdateNotComplete;
	}

	if (JustUpdate((CString)UpdateServerURL, executable) == Success) {
		// now we need to download extra files
		//CString aPath = GetPathFromFileSlash(executable) + _T("shaders/");
		//if (!isDirectoryExist(aPath)) {
		//	if (!CreateDirectory(aPath, NULL)) {
		//		astr.Format(_T("Failed to create a sub-directory in Seiswide program root path. Please download the full version and extract it manually for the new update."));
		//		AfxMessageBox(astr);
		//		return UpdateNotComplete;
		//	}
		//}
		//JustUpdate((CString)UpdateServerURL + _T("shaders/"), aPath + _T("ShadowMapping.fragmentshader"));
		//JustUpdate((CString)UpdateServerURL + _T("shaders/"), aPath + _T("ShadowMapping.vertexshader"));

		//aPath = GetPathFromFileSlash(executable);
		//if (_WIN64)
		//	JustUpdate((CString)UpdateServerURL + _T("shaders/x64/"), aPath + _T("glew32.dll"));
		//else
		//	JustUpdate((CString)UpdateServerURL + _T("shaders/Win32/"), aPath + _T("glew32.dll"));
	}

	return Success;
}


CAutoUpdater::ErrorType CAutoUpdater::CheckForUpdate(LPCTSTR UpdateServerURL)
{
	if (!InternetOkay()) {
		return InternetConnectFailure;
	}

	bool bTransferSuccess = false;

	// First we must check the remote configuration file to see if an update is necessary
	CString URL = UpdateServerURL + CString(LOCATION_UPDATE_FILE_CHECK);
	HINTERNET hSession = GetSession(URL);
	if (!hSession) {
		return InternetSessionFailure;
	}

	BYTE pBuf[TRANSFER_SIZE];
	memset(pBuf, NULL, sizeof(pBuf));
	bTransferSuccess = DownloadConfig(hSession, pBuf, TRANSFER_SIZE);
	InternetCloseHandle(hSession);
	if (!bTransferSuccess) {
		return ConfigDownloadFailure;
	}

	// Get the version number of our executable and compare to see if an update is needed
	CString executable = GetExecutable();
	CString fileVersion = GetFileVersion(executable);
	if (fileVersion.IsEmpty()) {
		return NoExecutableVersion;
	}

	CString updateVersion = (char *)pBuf;
	if (CompareVersions(updateVersion, fileVersion) != 1) {
		return UpdateNotRequired;
	}

	// At this stage an update is required	
	// CString astr = GetBaseFileName(executable);  // this does not consider 32 or 64 bit
	CString astr = getExeNameDownload();
	CString msg;
	msg.Format(_T("An update of %s is now available. Proceed with the update? Once update process starts, please wait for the update completion message."), astr);
	if (IDNO == MessageBox(GetActiveWindow(), msg, _T("Update is available"), MB_YESNO | MB_ICONQUESTION)) {
		return UpdateNotComplete;
	}

	if (JustUpdate((CString)UpdateServerURL, executable) == Success) {
		// now we need to download extra files
		//CString aPath = GetPathFromFileSlash(executable) + _T("shaders/");
		//if (!isDirectoryExist(aPath)) {
		//	if (!CreateDirectory(aPath, NULL)) {
		//		astr.Format(_T("Failed to create a sub-directory in Seiswide program root path. Please download the full version and extract it manually for the new update."));
		//		AfxMessageBox(astr);
		//		return UpdateNotComplete;
		//	}
		//}
		//JustUpdate((CString)UpdateServerURL + _T("shaders/"), aPath + _T("ShadowMapping.fragmentshader"));
		//JustUpdate((CString)UpdateServerURL + _T("shaders/"), aPath + _T("ShadowMapping.vertexshader"));

		//aPath = GetPathFromFileSlash(executable);
		//if (_WIN64)
		//	JustUpdate((CString)UpdateServerURL + _T("shaders/x64/"), aPath + _T("glew32.dll"));
		//else
		//	JustUpdate((CString)UpdateServerURL + _T("shaders/Win32/"), aPath + _T("glew32.dll"));
	}

	return Success;
}


// Ensure the internet is ok to use
//
bool CAutoUpdater::InternetOkay()
{
	if (hInternet == NULL) {
		return false;
	}

	// Important step - ensure we have an internet connection. We don't want to force a dial-up.
	DWORD dwType;
	if (!InternetGetConnectedState(&dwType, 0)) {
		return false;
	}

	return true;
}

// Get a session pointer to the remote file
//
HINTERNET CAutoUpdater::GetSession(CString &URL)
{
	// Canonicalization of the URL converts unsafe characters into escape character equivalents
	TCHAR canonicalURL[1024];
	DWORD nSize = 1024;
	InternetCanonicalizeUrl(URL, canonicalURL, &nSize, ICU_BROWSER_MODE);		
	
	DWORD options = INTERNET_FLAG_NEED_FILE|INTERNET_FLAG_HYPERLINK|INTERNET_FLAG_RESYNCHRONIZE|INTERNET_FLAG_RELOAD;
	HINTERNET hSession = InternetOpenUrl(hInternet, canonicalURL, NULL, NULL, options, 0);
	URL = canonicalURL;

	return hSession;
}

// Download a file into a memory buffer
//
bool CAutoUpdater::DownloadConfig(HINTERNET hSession, BYTE *pBuf, DWORD bufSize)
{	
	DWORD	dwReadSizeOut;
	InternetReadFile(hSession, pBuf, bufSize, &dwReadSizeOut);
	if (dwReadSizeOut <= 0)
	{
		return false;
	}

	
	return true;
}

// Download a file to a specified location
//
bool CAutoUpdater::DownloadFile(HINTERNET hSession, LPCTSTR localFile)
{	
	HANDLE	hFile;
	BYTE	pBuf[TRANSFER_SIZE];
	DWORD	dwReadSizeOut, dwTotalReadSize = 0;

	hFile = CreateFile(localFile, GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return false;

	do {
		DWORD dwWriteSize, dwNumWritten;
		BOOL bRead = InternetReadFile(hSession, pBuf, TRANSFER_SIZE, &dwReadSizeOut);
		dwWriteSize = dwReadSizeOut;

		if (bRead && dwReadSizeOut > 0) {
			dwTotalReadSize += dwReadSizeOut;
			WriteFile(hFile, pBuf, dwWriteSize, &dwNumWritten, NULL); 
			// File write error
			if (dwWriteSize != dwNumWritten) {
				CloseHandle(hFile);					
				return false;
			}
		}
		else {
			if (!bRead)
			{
				// Error
				CloseHandle(hFile);	
				return false;
			}			
			break;
		}
	} while(1);

	CloseHandle(hFile);
	return true;
}

// Get the version of a file
//
const CString CAutoUpdater::GetFileVersion(LPCTSTR file)
{
	CString version;
	VS_FIXEDFILEINFO *pVerInfo = NULL;
	DWORD	dwTemp, dwSize, dwHandle = 0;
	BYTE	*pData = NULL;
	UINT	uLen;

	try {
		dwSize = GetFileVersionInfoSize((LPTSTR) file, &dwTemp);
		if (dwSize == 0) throw 1;

		// pData = new BYTE[dwSize];
		pData = (BYTE *)alloc1(dwSize, sizeof(BYTE));
		if (pData == NULL) throw 1;

		if (!GetFileVersionInfo((LPTSTR) file, dwHandle, dwSize, pData))
			throw 1;

		if (!VerQueryValue(pData, _T("\\"), (void **) &pVerInfo, &uLen)) 
			throw 1;

		DWORD verMS = pVerInfo->dwFileVersionMS;
		DWORD verLS = pVerInfo->dwFileVersionLS;

		int ver[4];
		ver[0] = HIWORD(verMS);
		ver[1] = LOWORD(verMS);
		ver[2] = HIWORD(verLS);
		ver[3] = LOWORD(verLS);

		// Are lo-words used?
		if (ver[2] != 0 || ver[3] != 0)
		{
			version.Format(_T("%d.%d.%d.%d"), ver[0], ver[1], ver[2], ver[3]);
		}
		else if (ver[0] != 0 || ver[1] != 0)
		{
			version.Format(_T("%d.%d"), ver[0], ver[1]);
		}

		// delete pData;
		free1(pData);
		return version;
	}
	catch(...) {
		return BLANK;
	}	
}

// Compare two versions 
//
const int CAutoUpdater::CompareVersions(CString ver1, CString ver2)
{
	TCHAR seps[] = _T(".");
	int  wVer1[4], wVer2[4];
	int	 i;
	TCHAR *pVer1 = ver1.GetBuffer(256);
	TCHAR *pVer2 = ver2.GetBuffer(256);

	for (i=0; i<4; i++)
	{
		wVer1[i] = 0;
		wVer2[i] = 0;
	}

	// Get version 1 to DWORDs
	TCHAR *pToken = wcstok(pVer1, seps);
	if (pToken == NULL)
	{
		return -21;
	}

	i=3;
	while(pToken != NULL)
	{
		if (i<0 || !IsDigits(pToken)) 
		{			
			return -21;	// Error in structure, too many parameters
		}		
		wVer1[i] = _wtoi (pToken);
		pToken = wcstok(NULL, seps);
		i--;
	}
	ver1.ReleaseBuffer();

	// Get version 2 to DWORDs
	pToken = wcstok(pVer2, _T("."));
	if (pToken == NULL)
	{
		return -22;
	}

	i=3;
	while(pToken != NULL)
	{
		if (i<0 || !IsDigits(pToken)) 
		{
			return -22;	// Error in structure, too many parameters
		}		
		wVer2[i] = _wtoi (pToken);
		pToken = wcstok(NULL, seps);
		i--;
	}
	ver2.ReleaseBuffer();

	// Compare the versions
	for (i=3; i>=0; i--)
	{
		if (wVer1[i] > wVer2[i])
		{
			return 1;		// ver1 > ver 2
		}
		else if (wVer1[i] < wVer2[i])
		{
			return -1;
		}
	}

	return 0;	// ver 1 == ver 2
}

// Ensure a string contains only digit characters
//
const bool CAutoUpdater::IsDigits(CString text)
{
	for (int i=0; i<text.GetLength(); i++)
	{
		TCHAR c = text.GetAt(i);
		if (c >= _T('0') && c <= _T('9'))
		{
		}
		else
		{
			return false;
		}
	}

	return true;
}

const CString CAutoUpdater::GetExecutable()
{
	HMODULE hModule = ::GetModuleHandle(NULL);
    ASSERT(hModule != 0);
    
    TCHAR path[MAX_PATH];
    VERIFY(::GetModuleFileName(hModule, path, MAX_PATH));
    return path;
}

#include "gzip.h"
bool CAutoUpdater::Switch(CString executable, CString update, bool WaitForReboot)
{
	// if executable is compressed file, we need to uncompress it first, and then continue
	if (GetFileExtension(update).CompareNoCase(_T("gz")) == 0) {
		zlib::CGZip gzip;
		if (!gzip.Open(update, zlib::CGZip::ArchiveModeRead)) {
			return false;
		}
		void* pBuffer = NULL;
		size_t len;
		gzip.ReadBuffer(&pBuffer, len);
		gzip.Close();
		update = update.Left(update.GetLength() - 3);
		HANDLE hOutputFile = ::CreateFile(update,
			GENERIC_WRITE,
			0,
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if (INVALID_HANDLE_VALUE == hOutputFile)
			return false;

		// output
		DWORD dwBytesWritten = 0;
		if (!::WriteFile(hOutputFile, pBuffer, (DWORD)len, &dwBytesWritten, NULL) ||
			dwBytesWritten != (DWORD)len) {
			len = 0;
		}
		CloseHandle(hOutputFile);
		free(pBuffer);
		if (len == 0) return false;
	} // end decompress gz

	int type = (WaitForReboot) ? MOVEFILE_DELAY_UNTIL_REBOOT : MOVEFILE_COPY_ALLOWED;

	const TCHAR *backup = _T("OldExecutable.bak");
	CString directory = executable.Left(executable.ReverseFind(_T('\\')));	
	CString backupFile = directory + _T('\\') + CString(backup);

	// now we may need to have admin rights if we cannot write anything to the program dir
	if ( !isDirectoryWritable(GetPathFromFileSlash(executable)) ) {
		if (!DoElevateToAdmin()) return false;
	}


	DeleteFile(backupFile);
	MoveFileEx(executable, backupFile, type);
	//if (!MoveFileEx(executable, backupFile, type)) 
	//{
	//	return false;
	//}

	BOOL bMoveOK = (MoveFileEx(update, executable, type) == TRUE);
	int i = GetLastError();

	return bMoveOK == TRUE;	
}