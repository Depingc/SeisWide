#include "stdafx.h"
#include "LicenseCheck.h"
#include "base64.h"
#include "util.h"
#include "AutoUpdater.h"
#include "ealloc_d.h"
static const char depingCharKeyOrig[] = "Miche66c"; // this is just the first part of the key

//static CString depingKey = _T("Miche66c63");
	//CString s3 = _T("德");
	//CString s4 = _T("A");
	//CString s5 = _T("德000");
	//int i1 = sizeof(char);
	//int i2 = sizeof(TCHAR);
	//int i3 = s3.GetLength();
	//int i4 = s4.GetLength();
	//int i5 = s5.GetLength();



//#include <iostream>
//int main() {
//  const std::string s = "ADP GmbH\nAnalyse Design & Programmierung\nGesellschaft mit beschränkter Haftung" ;
//
//  std::string encoded = base64_encode(reinterpret_cast<const unsigned char*>(s.c_str()), s.length());
//  std::string decoded = base64_decode(encoded);
//
//  std::cout << "encoded: " << encoded << std::endl;
//  std::cout << "decoded: " << decoded << std::endl;
//
//  return 0;
//}
//

CLicenseCheck::CLicenseCheck()
{
	this->licenseStruct.curTime = CTime::GetCurrentTime();
}

CLicenseCheck::~CLicenseCheck()
{
}




// Return Unique String ID each time  
#pragma comment( lib, "rpcrt4.lib" )
// Prints the MAC address stored in a 6 byte array to stdout

const CString CLicenseCheck::formatMACaddress(unsigned char MACData[])
{

	CString str;
	str.Format(TEXT("%02X%02X-%02X%02X-%02X%02X"), 
		MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);
	// AfxMessageBox(str); // TESTING
	return str;
}

// Fetches the MAC address and prints it
const CString CLicenseCheck::getMACaddressSingle()
{
	unsigned char MACData[6];
	 
	UUID uuid;
	UuidCreateSequential( &uuid );    // Ask OS to create UUID using the MAC of ethernet card
//	UuidCreate(&uuid);    // Ask OS to create UUID not related to ethernet
//	UINT ua = GetProcessorLoad();  // testing

	for (int i=2; i<8; i++)  // Bytes 2 through 7 inclusive  are MAC address
		MACData[i - 2] = uuid.Data4[i];
 
	//CString str;
	//str.Format(TEXT("%02X%02X-%02X%02X-%02X%02X"), 
	//	MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);
	//return str;
  return formatMACaddress(MACData);         // Print MAC address
}

// Fetches the MAC address for multiple network cards; we will only pick the first one
#pragma comment(lib, "IPHLPAPI.lib")
const CString CLicenseCheck::getMACaddress()
{
	IP_ADAPTER_INFO AdapterInfo[16];       // Allocate information 
	// for up to 16 NICs
	DWORD dwBufLen = sizeof(AdapterInfo);  // Save memory size of buffer

	DWORD dwStatus = GetAdaptersInfo(      // Call GetAdapterInfo
		AdapterInfo,                 // [out] buffer to receive data
		&dwBufLen);                  // [in] size of receive data buffer
	ASSERT(dwStatus == ERROR_SUCCESS);  // Verify return value is 
	// valid, no buffer overflow

	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to
	// current adapter info

	unsigned char MACData[6];
	do {
		for (int i = 2; i<8; i++)  // Bytes 2 through 7 inclusive  are MAC address
			MACData[i - 2] = (unsigned char)pAdapterInfo->Address[i];
		return formatMACaddress(MACData);

		pAdapterInfo = pAdapterInfo->Next;    // Progress through 
		// linked list
	} while (pAdapterInfo);                    // Terminate if last adapter

	AfxMessageBox(_T("Sorry, a network card is not found. You need such card to get a hardware ID even though a network is not be needed to run SeisWide except its automatic updating."), MB_ICONQUESTION);
	return _T("");
}

//
//// http://www.codeproject.com/Articles/5853/Getting-Information-from-WMI-in-Visual-C
//#include "wbemidl.h"
// this function will need wbemuuid.lib  to be linked
//UINT CLicenseCheck::GetProcessorLoadNotUsed()
//{
//	//CUsingWMIDlg* dlg = (CUsingWMIDlg*)pParam;
//
//	CoInitialize(NULL);
//
//	HRESULT hRes = CoInitializeSecurity(NULL, 
//		-1, 
//		NULL, 
//		NULL, 
//		RPC_C_AUTHN_LEVEL_DEFAULT, // RPC_C_AUTHN_LEVEL_PKT,  // authentication level 
//		RPC_C_IMP_LEVEL_DEFAULT, // RPC_C_IMP_LEVEL_IMPERSONATE, // impersonation level
//		NULL,
//		EOAC_NONE, // no special capabilities
//		NULL);
//
//	if (hRes != S_OK) return 0;
//
//	CComPtr<IWbemLocator> pIWbemLocator;
//	CComPtr<IWbemServices> pIWbemServices;
//	CComPtr<IEnumWbemClassObject> pIEnumObject;
//
//	CComBSTR bstrNamespace(_T("root\\cimv2"));
//
//	hRes = pIWbemLocator.CoCreateInstance(CLSID_WbemAdministrativeLocator);
//	if (hRes != S_OK) return 0;
//
//	hRes = pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, NULL, 0, NULL, NULL, 
//		&pIWbemServices);
//	if (hRes != S_OK) return 0;
//
//	while (true)
//	{
//		CComBSTR bstrQuery(_T("Select * from Win32_Processor"));
//		CComBSTR bstrQL(_T("WQL"));
//
//		hRes = pIWbemServices->ExecQuery(bstrQL, bstrQuery, WBEM_FLAG_RETURN_IMMEDIATELY, NULL, 
//			&pIEnumObject);
//
//		ULONG uCount = 1, uReturned;
//		CComPtr<IWbemClassObject> pIClassObject;
//
//		hRes = pIEnumObject->Reset();
//
//		hRes = pIEnumObject->Next(WBEM_INFINITE, uCount, &pIClassObject, &uReturned);
//
//		if (hRes != S_OK) return 0;
//
//		CComVariant var;
//
//		CComBSTR bstrProp(_T("LoadPercentage"));
//
//		hRes = pIClassObject->Get(bstrProp, 0, &var, NULL, NULL);
//		if (hRes != S_OK) return 0;
//
//		int * value = new int(var.lVal);
//	//	dlg->PostMessage(WM_USER + 1, (WPARAM)value);
//
//		pIEnumObject.Release();
//	}
//	return 1;
//}
//CString CLicenseCheck::getMACaddressWmic()
//{
//	unsigned char MACData[6];
//
//	UUID uuid;
//	UuidCreateSequential(&uuid);    // Ask OS to create UUID using the MAC of ethernet card
//	//	UuidCreate(&uuid);    // Ask OS to create UUID not related to ethernet
//
//	for (int i = 2; i<8; i++)  // Bytes 2 through 7 inclusive  are MAC address
//		MACData[i - 2] = uuid.Data4[i];
//
//	return formatMACaddress(MACData);         // Print MAC address
//}


/*
* This getCurrentLicenseStat7() will need to access registry in order to write starting date of usage.
* But my newer OS in office will not let me to have admin password, so I cannot write to registry.
* Therefore I will need to use internet time for this, which is implimented in getCurrentLicenseStat8() below.
*/
void CLicenseCheck::getCurrentLicenseStat7() {
	licenseStruct.type = this->demo;
	licenseStruct.nDaysLeftover = -1;

		
	CString str, str2;
	if (!this->parseLicenseFile2()) {
		if (!this->licenseStruct.strLicenseHardId.IsEmpty())
			AfxMessageBox(_T("License file invalid."), MB_ICONQUESTION);
		// if m_strLicenseHardId is not empty, it means the license file has something in it, but invalid
		return;
	}

	CString hardWareIdThisMachine = this->getMACaddress();
	if (hardWareIdThisMachine.IsEmpty() ||
		0 != this->licenseStruct.strLicenseHardId.Compare(hardWareIdThisMachine)) {
		this->licenseStruct.strLicenseHardId = _T("");
		this->licenseStruct.type = this->demo;
		AfxMessageBox(_T("License file does not match this machine."), MB_ICONQUESTION);
		return;
	}

	HKEY key;
	DWORD disp;
	LPCTSTR sk = TEXT("SOFTWARE\\SeisWide\\8.0");
	LPCTSTR value = TEXT("Activation");

	LONG openRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sk, 0, KEY_READ, &key);


	if (openRes != ERROR_SUCCESS) {
		// the key does not exist. we need to create it using admin mode
		if (!DoElevateToAdmin()) {
			licenseStruct.type = this->regi;
			return;
		}



		RegCloseKey(key);
		openRes = RegCreateKeyEx(
    			HKEY_LOCAL_MACHINE,
    			sk,
    			0, NULL, 0,
    			KEY_WRITE | KEY_READ, NULL, 
				&key, &disp);
		if (openRes!=ERROR_SUCCESS) {
			RegCloseKey(key);
			AfxMessageBox(_T("Unable to create registry key; Try right click and rerun in administrative mode."), MB_ICONQUESTION);
			licenseStruct.type = this->regi;
		   return;
		}

		// now prepare to write the startup time to registry
		// this->m_currTime.ParseDateTime(str, VAR_DATEVALUEONLY, LANG_ENGLISH);
		CStringEncodeBase64(dateTimeToCString(this->licenseStruct.curTime), str2);
		str = this->XOR_Encryption(str2);
		LPCTSTR data = str.GetBuffer(str.GetLength()+1);
		LONG setRes = RegSetValueEx (key, value, 0, REG_SZ, (LPBYTE)data, (str.GetLength()+1) * sizeof(TCHAR));
		str.ReleaseBuffer();

		if (setRes != ERROR_SUCCESS) {
			RegCloseKey(key);
		    AfxMessageBox(_T("Error writing to registry."), MB_ICONQUESTION);
			licenseStruct.type = this->regi;
		   return;
		} else
		   AfxMessageBox(_T("Registration success."), MB_ICONQUESTION);
	} // end if openRes



	// now read the startup time from registry
	LPCTSTR data = str.GetBuffer(1024);
	DWORD keyNameSizBuf = 1024;
    if(RegQueryValueEx(key,value, 0,NULL,(LPBYTE)data,  &keyNameSizBuf) == ERROR_SUCCESS ) {
    }
	str.ReleaseBuffer();

	// finished rerading registry. now close it
	LONG closeOut = RegCloseKey(key);


	// process the license issue
	// licenseStruct.type = this->getLicenseStruct(this->m_strLicenseType);
	// CString strTime = this->XOR_Encryption(str);
	CStringDecodeBase64(this->XOR_Encryption(str), str2);
	licenseStruct.nDaysLeftover = this->getLicenseLeftoverDays(ctringToCTime(str2));

	if (licenseStruct.nDaysLeftover < 0) {
		// expired
		licenseStruct.type = this->demo;
	} 


	return;
}

void CLicenseCheck::getCurrentLicenseStat8() {
	licenseStruct.type = this->demo;
	licenseStruct.nDaysLeftover = -1;


	if (!this->parseLicenseFile2()) {
		if (!this->licenseStruct.strLicenseHardId.IsEmpty())
			AfxMessageBox(_T("License file invalid."), MB_ICONQUESTION);
		// if m_strLicenseHardId is not empty, it means the license file has something in it, but invalid
		return;
	}

	CString hardWareIdThisMachine = this->getMACaddress();
	if (0 != this->licenseStruct.strLicenseHardId.Compare(hardWareIdThisMachine)) {
		this->licenseStruct.strLicenseHardId = _T("");
		this->licenseStruct.type = this->demo;
		AfxMessageBox(_T("License file does not match this machine."), MB_ICONQUESTION);
		return;
	}

	CString usageTimeStarted = this->getUsageTimeInRegistry();
	if (!usageTimeStarted.IsEmpty() &&
		ctringToCTime(usageTimeStarted) < this->licenseStruct.offerTimeStart) {
		// this means the time in registry is older version, and we need to ignore it
		usageTimeStarted = _T("");
	}

	int defaultLeftOverTime =
		this->getLicenseLeftoverDays(this->licenseStruct.offerTimeStart);

	CString aPath = getSeisWideParaPath() + "\\license.txt";
	CString str = getLineSecond(aPath).Trim();
	int usageStat = 0;
//	if (!str.IsEmpty() && !isDigit(str.GetAt(0))) 
	usageStat = StringToInt(str.GetAt(0));

	CAutoUpdater updater;
	CString serverAddr = _T("http://seiswide.hopto.org/seiswide/time.php");
	CString curTimeInServer = updater.GetUrlContents(serverAddr); // something like: 2015-4-1

	if (usageTimeStarted.IsEmpty() && usageStat == 0) {
		// this is the first time, and try to use registry first
		if (DoElevateToAdmin()) {
			this->setStartTimeInRegistry(this->licenseStruct.curTime);
			usageTimeStarted = this->getUsageTimeInRegistry();
			usageStat = 1;
			str.Format(_T("\r\n%d"), usageStat);
			appendToTextFile(aPath, str);
		}
	}

	if (usageTimeStarted.IsEmpty() && usageStat == 0) {
		usageTimeStarted = curTimeInServer;
		usageStat = 2;

		// backup a copy of licence 
		CopyFile(aPath, aPath + _T(".bak"), FALSE);

		str.Format(_T("Since no admin previliage can be granted, internet is needed for all subsequent usage. Will try to making this rule permanent. Original license file is backed up as license.txt.bak."));
		AfxMessageBox(str, MB_ICONQUESTION);

		str.Format(_T("\r\n%d"), usageStat);
		appendToTextFile(aPath, str);
	}

	if (usageTimeStarted.IsEmpty() && usageStat == 2) {
		usageTimeStarted = curTimeInServer;
	}

	// process the license issue
	licenseStruct.nDaysLeftover = 
		this->getLicenseLeftoverDays(ctringToCTime(usageTimeStarted));

	if (licenseStruct.nDaysLeftover < 0) {
		// expired
		licenseStruct.type = this->demo;
	}


	return;
}


BOOL CLicenseCheck::parseLicenseFile2()
{
	return parseLicenseFile2(this->decodeActLicenseFromFile());

}

BOOL CLicenseCheck::parseLicenseFile2(CString str)
{
	// read the license file
	// this->m_strLicenseType = _T("DEMO");
	// Note: if the current machine has a different hardware ID, it will still go ahead. You need to check for this when you check the license status

	licenseStruct.strLicenseCode = str;
	int i = str.Find(_T("####"));
	if (i<0) return FALSE;
	int j = str.Find(_T("####"), i+4);
	if (j<0) return FALSE;
	int k = str.Find(_T("####"), j+4);
	if (k<0) return FALSE;
	int m = str.Find(_T("####"), k+4);
	if (m<0) return FALSE;
	int n = str.Find(_T("####"), m+4);
	if (n<0) return FALSE;

	this->licenseStruct.strLicenseHardId = str.Left(i);
	CString strLicenseType = str.Mid(i+4, j-i-4);
	this->licenseStruct.magicNumber = _ttoi(strLicenseType.Mid(4));

	// we only need the first 4 characters for the type
	strLicenseType = strLicenseType.Left(4);
	this->licenseStruct.type = this->licenseStr2Enum(strLicenseType.Left(4));

	// determine if this license file has been scrambled
	CString tStr;
	tStr.Format(_T("%s####%s"), this->licenseStruct.strLicenseHardId, strLicenseType);
	// need to get a magic number in order to make the key safer, to append to the end of strUsageMode
	int computedMagicNumber = this->getMagicNumberFromString(tStr);
	if (computedMagicNumber != this->licenseStruct.magicNumber) {
		// the magic numner is different: maybe the user has tried to modify the license?
		return FALSE;
	}



	this->licenseStruct.nDaysAllowed = _ttoi(str.Mid(j+4, k-j-4));

	CString str3 = str.Mid(k+4, m-k-4);	// example: 10/22/2013
	CString str2 = str.Mid(m+4, n-m-4);	  // example: 
	this->licenseStruct.offerTimeStart = ctringToCTime(str3);
	this->licenseStruct.offerTimeExpire = ctringToCTime(str2);
	this->licenseStruct.strLicenseName = str.Mid(n+4); // example: Deping Chian, Chian Consulting Inc
	return TRUE;
	// this->m_offerExpireTime.ParseDateTime(str2, VAR_DATEVALUEONLY, LANG_ENGLISH);
}

char* CLicenseCheck::getPersonalKey()
{
	// NOTE: You must free the result in your calling function!
    size_t iKey = (strlen(depingCharKeyOrig) + 1) * sizeof(TCHAR);
	char* depingCharKey = (char*)alloc1(iKey, sizeof(char));
    ZeroMemory(depingCharKey, iKey);
	strcpy_s(depingCharKey, iKey, depingCharKeyOrig);
	strcpy_s(&depingCharKey[strlen(depingCharKeyOrig)], 4*sizeof(TCHAR), "1963");  // Note: size can trigger exception if incorrect
	return depingCharKey;  // this shall always be   Miche66cxxxx , but we cannot make it explicit in the program!
}


/***********************************************************
XOR Process
for both encryption/decryption processing
***********************************************************/
const CString CLicenseCheck::XOR_Encryption(CString toBeEncrypted)
{
	// return toBeEncrypted;

	char* depingCharKey = this->getPersonalKey();
	size_t iKey = strlen(depingCharKey);

	size_t lenInBytes = (toBeEncrypted.GetLength() + 1)*sizeof(TCHAR);

    char *pInput = (char*)alloc1(lenInBytes, sizeof(char));
	char *pOutput = (char*)alloc1(lenInBytes, sizeof(char));
	
	ZeroMemory(pInput, lenInBytes);
    ZeroMemory(pOutput, lenInBytes);

    strcpy_s(pInput, lenInBytes, CT2A(toBeEncrypted));  // after converted to ascii, the original string length is doubled, which is why we should use bytes to count 

	
	
	
	// the actual nul-terminated string lenth in ascii should be re-calculated
	size_t inLen = strlen(pInput);
	size_t klen = strlen(depingCharKey);
	for (size_t i = 0; i < inLen; i++) {
		char c = depingCharKey[i % klen] & 10;
		pOutput[i] = pInput[i] ^ c;
    }


	CString szOutput = CA2T(pOutput);
    free1(pInput);
    free1(pOutput);
	free1(depingCharKey);
    return szOutput;
}

// A license key would look like this: BASE32(CONCAT(DATA, PRIVATE_KEY_ENCRYPTED(HASH(DATA)))) The biggest challenge here is that the classical public key algorithms have large signature sizes. RSA512 has an 1024-bit signature.
//   0050-56C0-0008:REFL:Deping, Chian Consulting Inc.
CString CLicenseCheck::createActLicense(CString hardwareId, CString strUsageMode, CString licenseName, int usageDaysAllowed)
{
	// CString licenseName = _T("Deping Chian, Chian Consulting Inc.");
	CString str, str2;
	CTimeSpan EvaluationPeriod(usageDaysAllowed+30,0,0,0);  // we add 30 days for it to expire, in case the user does not start using right away
	this->licenseStruct.offerTimeStart = this->licenseStruct.curTime;
	this->licenseStruct.offerTimeExpire = this->licenseStruct.offerTimeStart + EvaluationPeriod;
	str = dateTimeToCString(this->licenseStruct.offerTimeStart);
	str2 = dateTimeToCString(this->licenseStruct.offerTimeExpire);

	// need to get 10 random numbers in order to make the key safer, to append to the end of strUsageMode
	//int foo = (int)myRound((double)rand()/(double)RAND_MAX * MIL);

	CString finalCode;
	finalCode.Format(_T("%s####%s"), hardwareId, strUsageMode);
	// need to get a magic number in order to make the key safer, to append to the end of strUsageMode
	int magicNumber = this->getMagicNumberFromString(finalCode);

	finalCode.Format(_T("%s####%s%d####%d####%s####%s####%s"), hardwareId, strUsageMode, magicNumber, usageDaysAllowed, str, str2, licenseName);
	AfxMessageBox(finalCode);
	return this->encodeActLicense(finalCode);
}




//
//CString CLicenseCheck::encodeActLicenseOld(CString aLicense)
//{
//	CString str;
//	int num = CStringEncodeBase64(aLicense, str);
//	return this->XOR_Encryption(str);
//}
//
//CString CLicenseCheck::decodeActLicenseOld(CString aLicense)
//{
//	CString str;
//	int num = CStringDecodeBase64( this->XOR_Encryption(aLicense), str);
//	return str;
//}


CString CLicenseCheck::encodeActLicense(CString aLicense)
{
	aLicense = this->XOR_Encryption(aLicense);
	CString str;
	size_t num = CStringEncodeBase64(aLicense, str);
	return str;
}

CString CLicenseCheck::decodeActLicense(CString aLicense)
{
	CString str;
	size_t num = CStringDecodeBase64(aLicense, str);
	return this->XOR_Encryption(str);
}


//CString CLicenseCheck::encodeActLicense(CString aLicense)
//{
//	CString str = encodeActLicenseInner(aLicense);
//	CString str2 = CStringDecodeBase64(str);
//	return this->encodeActLicenseInner(str + str2);
//}
//
//CString CLicenseCheck::decodeActLicense(CString aLicense)
//{
//	return this->decodeActLicenseInner(decodeActLicenseInner(aLicense));
//}




CString CLicenseCheck::decodeActLicenseFromFile()
{
	// the license file is not supplied in this function. We get from the same dir as this module
	CString aPath = getSeisWideParaPath() + "\\license.txt";
	CString aLicense = getLineFirst(aPath);
	CString str = decodeActLicense(aLicense);
//	int num = CStringDecodeBase64( this->XOR_Encryption(aLicense), str);

	return str;
}


const bool CLicenseCheck::isNew2SeisWide()
{
	// return false; // testing

	HKEY key;
	LPCTSTR sk = TEXT("SOFTWARE\\SeisWide\\8.0");
	LPCTSTR value = TEXT("Activation");
	LONG openRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sk, 0, KEY_READ, &key);
	if (openRes != ERROR_SUCCESS) {
		RegCloseKey(key);
		// no Seiswide key exist in registry
		if (!this->parseLicenseFile2()) {
			if (!this->licenseStruct.strLicenseHardId.IsEmpty())
			//	AfxMessageBox(_T("License file invalid."), MB_ICONQUESTION);
			// if m_strLicenseHardId is not empty, it means the license file has something in it, but invalid
			return true;
		}

		CString hardWareIdThisMachine = this->getMACaddress();
		if (0 != this->licenseStruct.strLicenseHardId.Compare(hardWareIdThisMachine)) {
			//this->licenseStruct.strLicenseHardId = _T("");
			//this->licenseStruct.type = this->demo;
			//AfxMessageBox(_T("License file does not match this machine."), MB_ICONQUESTION);
			return true;
		}

	}

	return false;
}

void CLicenseCheck::setDefaultLicenseFile(CString hid)
{
	CString aPath = getSeisWideParaPath() + "\\license.txt";
	CString strLicense = this->createActLicense(hid, _T("FULL"), _T("Default"), 30);

	CStringArray* strArr = new CStringArray();
	strArr->Add(strLicense);
	bool isSuc = saveStringArrayToFile(strArr, aPath);
	strArr->RemoveAll();
	delete strArr;

	if (!isSuc) {
		CString tStr;
		tStr.Format(_T("Please manually save the next Notepad content to license.txt file in SeisWide's folder."));
		AfxMessageBox(tStr);
	}
}

const CString CLicenseCheck::getUsageTimeInRegistry()
{
	HKEY key;
	LPCTSTR sk = TEXT("SOFTWARE\\SeisWide\\8.0");
	LPCTSTR value = TEXT("Activation");
	LONG openRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sk, 0, KEY_READ, &key);
	if (openRes != ERROR_SUCCESS) {
		RegCloseKey(key);
		return _T("");
	}

	// now read the startup time from registry
	CString str;
	LPCTSTR data = str.GetBuffer(1024);
	DWORD keyNameSizBuf = 1024;
	if (RegQueryValueEx(key, value, 0, NULL, (LPBYTE)data, &keyNameSizBuf) == ERROR_SUCCESS) {
	}
	str.ReleaseBuffer();

	// finished rerading registry. now close it
	LONG closeOut = RegCloseKey(key);

	CString str2;
	CStringDecodeBase64(this->XOR_Encryption(str), str2);
	return str2;
}

bool CLicenseCheck::setStartTimeInRegistry(CTime curTime)
{
	HKEY key;
	DWORD disp;
	LPCTSTR sk = TEXT("SOFTWARE\\SeisWide\\8.0");
	LPCTSTR value = TEXT("Activation");
	LONG openRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, sk, 0, KEY_READ, &key);
	RegCloseKey(key);
	openRes = RegCreateKeyEx(
		HKEY_LOCAL_MACHINE,
		sk,
		0, NULL, 0,
		KEY_WRITE | KEY_READ, NULL,
		&key, &disp);
	if (openRes != ERROR_SUCCESS) {
		RegCloseKey(key);
		AfxMessageBox(_T("Unable to create registry key; Try right click and rerun in administrative mode."), MB_ICONQUESTION);
		return false;
	}

	// now prepare to write the startup time to registry
	// this->m_currTime.ParseDateTime(str, VAR_DATEVALUEONLY, LANG_ENGLISH);
	CString str, str2;
	CStringEncodeBase64(dateTimeToCString(curTime), str2);
	str = this->XOR_Encryption(str2);
	LPCTSTR data = str.GetBuffer(str.GetLength() + 1);
	LONG setRes = RegSetValueEx(key, value, 0, REG_SZ, (LPBYTE)data, 
		(str.GetLength() + 1) * sizeof(TCHAR));
	str.ReleaseBuffer();

	if (setRes != ERROR_SUCCESS) {
		RegCloseKey(key);
		AfxMessageBox(_T("Error writing to registry."), MB_ICONQUESTION);
		return false;
	}
	else
		AfxMessageBox(_T("Registration success."), MB_ICONQUESTION);

	// finished rerading registry. now close it
	LONG closeOut = RegCloseKey(key);
	return true;
}

const int CLicenseCheck::getLicenseLeftoverDays(CTime usageTimeInRegistry)
{
	CTimeSpan timeSpan = usageTimeInRegistry - this->licenseStruct.offerTimeStart;
	LONGLONG nDays = timeSpan.GetDays();
	CTime tStart = (nDays > 0)  ?  usageTimeInRegistry  :  this->licenseStruct.offerTimeStart;


	CTimeSpan period(this->licenseStruct.nDaysAllowed, 0, 0, 0);
	CTime tExpire = tStart + period;
	timeSpan = this->licenseStruct.offerTimeExpire - tExpire;
	nDays = timeSpan.GetDays();
	CTime tEnd = (nDays < 0)  ?  this->licenseStruct.offerTimeExpire  :  tExpire;


	CTimeSpan ts1 = this->licenseStruct.curTime - tStart;
	CTimeSpan ts2 = tEnd - this->licenseStruct.curTime;
	LONGLONG nDays1 = ts1.GetDays();
	LONGLONG nDays2 = ts2.GetDays();

	if (nDays1 < -1) {
		return -1;
	} else if (nDays2 < 0) {
		// expired
		return -1;
		// AfxMessageBox(strTime);
	} 
	
	return (int)nDays2;
}

int CLicenseCheck::getMagicNumberFromString(CString str)
{
	// produce a magic number from a license string, usually hardwareId"####"FULL
	int out = 0;
	for (int i=0; i<str.GetLength(); i++) {
		out += str.GetAt(i);
	}

	return out;
}


CLicenseCheck::LICENSE CLicenseCheck::licenseStr2Enum(CString str)
{
	if (0== str.Compare(_T("DEMO")))
		return this->demo;
	else
		if (0== str.Compare(_T("REGI")))
			return this->regi;
	else
		if (0== str.Compare(_T("REFL")))
			return this->refl;
	else
		if (0== str.Compare(_T("REFR")))
			return this->refr;
	else
		if (0== str.Compare(_T("FULL")))
			return this->full;
	else
		return this->demo;
}


CString CLicenseCheck::licenseEnum2Str(CLicenseCheck::LICENSE type)
{
	switch (type) 
	{
	case this->demo:
		return _T("DEMO");
		break;
	case this->regi:
		return _T("REGI");
		break;
	case this->refl:
		return _T("REFL");
		break;
	case this->refr:
		return _T("REFR");
		break;
	case this->full:
		return _T("FULL");
		break;
	}
	return _T("DEMO");
}








// depreciated
CLicenseCheck::LICENSE  CLicenseCheck::CheckLicenseTimeSystem32()
{
	CString WinSeisRun, WinSeisStartup;
	ULONGLONG nSize;

	TCHAR lpszSystemInfo[MAX_PATH];   /* buffer for concat'd. str. */ 
	nSize = GetSystemDirectory(lpszSystemInfo, MAX_PATH);
	_tcscat_s(lpszSystemInfo, _T("\\Winseidp.dll"));
	WinSeisRun = lpszSystemInfo;

	nSize = GetSystemDirectory(lpszSystemInfo, MAX_PATH);
	_tcscat_s(lpszSystemInfo, _T("\\chssmcd0.dll"));
	WinSeisStartup = lpszSystemInfo;

	CTime tLastUsage, tOrig;
	CTime curTime = CTime::GetCurrentTime();
	BOOL isLastUsage = this->loadTimeFromFile(&tLastUsage, WinSeisRun);
	BOOL isOrig = this->loadTimeFromFile(&tOrig, WinSeisStartup);
	if (!isOrig) {
		// original file not existing. We create one
		isOrig = this->saveTimeToBinaryFile(&curTime, WinSeisStartup);
	}

	if (!isLastUsage) {
		// original file not existing. We create one
		tLastUsage = curTime;
		CTime t1(1999, 3, 19, 22, 15, 0); // 10:15 PM March 19, 1999
		CTime t2(1999, 3, 19, 23, 15, 0); // 10:15 PM March 19, 1999
		tLastUsage += t2 - t1;  // this time is slightly different, required to avoid the user copy these 2 files
		isLastUsage = this->saveTimeToBinaryFile(&tLastUsage, WinSeisRun);
	}

	if (!isLastUsage || !isOrig) {
	//	AfxMessageBox(_T("Full version requires administrative rights."));
		return this->demo;
	}

	// we have  tLastUsage, tOrig, curTime;
	//note that t and t1 are the time in the 4th line of each file.
	CTimeSpan timeSpan2Orig = curTime - tOrig;
	CTimeSpan timeSpan2Last = curTime - tLastUsage;
	ULONGLONG nDaysOrig, nDaysLast;
	nDaysOrig = timeSpan2Orig.GetDays();
	nDaysLast = timeSpan2Last.GetDays();

	if (timeSpan2Last > 0) {
		// last time Seiswide was run before today, so we write today's time to file
		this->saveTimeToBinaryFile(&tLastUsage, WinSeisRun);
	} else if (timeSpan2Last < -10) {
		// the user mess up the days. the system clock is moved backwards
		timeSpan2Orig = tLastUsage - tOrig;
		nDaysOrig = timeSpan2Orig.GetDays();
	}



	// read the license file
	CString aLicense = this->decodeActLicenseFromFile();
	CString hardWareIdActual = this->getMACaddress();
	int i = aLicense.Find(_T("####"));
	if (i<0) return this->demo;
	int j = aLicense.Find(_T("####"), i+4);
	if (j<0) return this->demo;
	int k = aLicense.Find(_T("####"), j+4);
	if (k<0) return this->demo;
	int m = aLicense.Find(_T("####"), k+4);
	if (m<0) return this->demo;
	CString hardWareIdInLicense = aLicense.Left(i);
	CString usage = aLicense.Mid(i+4, j);
	int nDaysAllowed = (hardWareIdInLicense==hardWareIdActual)  ?  _ttoi(aLicense.Mid(j+4, k))  :  0;


	CString str = aLicense.Mid(k, m);	// example: 10/22/2013
	CString str2 = aLicense.Mid(m);	  // example: 12/22/2013
//	COleDateTimeSpan EvaluationPeriod(usageDaysAllowed+30,0,0,0);  // we add 30 days for it to expire, in case the user does not start using right away


	CString tStr;
	
	int nDaysLeftOver = (int)(nDaysAllowed - nDaysOrig);
	if (nDaysLeftOver >10) 
		return this->licenseStr2Enum(usage);
	else if (nDaysLeftOver >0) {
		tStr.Format(_T("%d days left before expiration; Email to seiswide@gmail.com for updated license for SeisWide."), nDaysLeftOver);
		AfxMessageBox(tStr, MB_ICONQUESTION);
		return this->licenseStr2Enum(usage);
	} else {
		tStr.Format(_T("SeisWide usage expired; Email to seiswide@gmail.com for updated license."));
		AfxMessageBox(tStr);
		return this->demo;
	}
}


// depreciated
BOOL CLicenseCheck::loadTimeFromFile(CTime* t, CString aFile) {
	// aFile must have absolute path
	CFile file;
	if (!file.Open( aFile, CFile::modeRead|CFile::typeBinary | CFile::shareDenyNone)) {
		return FALSE;
	}
	file.Seek(4*4, CFile::begin); //skip 4 records.
	file.Read(t, 4);
	file.Close();
	return true;
}

// depreciated
BOOL CLicenseCheck::saveTimeToBinaryFile(CTime* t, CString aFile) {
	// aFile must have absolute path
	CFile file;
	if (!file.Open( aFile, CFile::modeWrite|CFile::typeBinary)) {
		return FALSE;
	}
	// file.Seek(4*4, CFile::begin); //skip 4 records.
		// make random 4 records for hiding the actual data
	CTime t1(1999, 3, 19, 22, 15, 0); // 10:15 PM March 19, 1999
	CTime t2(1999, 4, 20, 22, 15, 0); // 10:15 PM March 20, 1999
	CTimeSpan ts = t2 - t1;             // Subtract 2 CTimes
	CTime tt = *t + ts;
	file.Write(&tt, 4);
	
	CTime t3(1999, 3, 19, 22, 15, 0); // 10:15 PM March 19, 1999
	CTime t4(1999, 4, 25, 25, 19, 5); // 10:15 PM March 20, 1999
	tt += t4 - t3;
	file.Write(&tt, 4);

	CTime t5(1999, 3, 19, 22, 15, 0); // 10:15 PM March 19, 1999
	CTime t6(1999, 4, 23, 27, 12, 7); // 10:15 PM March 20, 1999
	tt += t6 - t5;
	file.Write(&tt, 4);

	CTime t7(1999, 3, 19, 22, 15, 0); // 10:15 PM March 19, 1999
	CTime t8(1999, 4, 25, 27, 14, 7); // 10:15 PM March 20, 1999
	tt += t8 - t7;
	file.Write(&tt, 4);

	// only the last line should be used
	file.Write(t, 4);
	file.Close();
	return true;
}
