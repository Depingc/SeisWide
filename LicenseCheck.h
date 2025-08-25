#pragma once

class CLicenseCheck
{
public:
   enum LICENSE {
      demo,
	  regi,
      refl,
      refr,
      full
   };

	struct LICENSE_STRUCT {
		CString strLicenseCode;  // the original license string read in
		CString strLicenseName;  // parsed name
		CString strLicenseHardId; // parsed hardware ID
		LICENSE type;  // parsed license type
		int magicNumber; // parsed magic number
		CTime offerTimeStart, offerTimeExpire; // parsed staring and end time of the offer
		int nDaysAllowed;	// parsed num of days in the offer; the user may start using it after the offer starting time, 
							// but cannot use it before the offer starting time. This is to avoid the effect of user changing system clock backward
		CTime curTime;		// the current time when this struct is called
		int nDaysLeftover; // calculated number of days allowed currently
		LICENSE_STRUCT(): strLicenseName(_T("")) {};
	} licenseStruct; 


	CLicenseCheck();
	~CLicenseCheck();

	BOOL parseLicenseFile2(CString str);
	BOOL parseLicenseFile2();
	void getCurrentLicenseStat7();
	void getCurrentLicenseStat8();


	const CString formatMACaddress(unsigned char MACData[]);
	const CString getMACaddressSingle();
	const CString getMACaddress();
	//UINT GetProcessorLoadNotUsed();
	//CString getMACaddressWmic();

//	bool setSerialNum(CString str);
//	CString getSerialNum();
	CString createActLicense(CString hardwareId, CString strUsageMode, CString licenseName, int usageDaysAllowed);
	//CString decodeActLicenseOld(CString aKey);
	//CString encodeActLicenseOld(CString aKey);
	CString decodeActLicenseFromFile();

	CString decodeActLicense(CString aKey);
	CString encodeActLicense(CString aKey);

	char * getPersonalKey(); // this cannot be made const, why???
	LICENSE licenseStr2Enum(CString str);
	CString licenseEnum2Str(CLicenseCheck::LICENSE type);


	const CString getUsageTimeInRegistry();
	const bool isNew2SeisWide();
	void setDefaultLicenseFile(CString hid);
	bool setStartTimeInRegistry(CTime curTime);
	const int getLicenseLeftoverDays(CTime usageTimeInRegistry);


//	LICENSE m_license;
private:
	const CString XOR_Encryption(CString toBeEncrypted);

	//CString decodeActLicenseInner(CString aKey);
	//CString encodeActLicenseInner(CString aKey);

	// depreciated functions follows
	BOOL loadTimeFromFile(CTime* t, CString aFile);
	BOOL saveTimeToBinaryFile(CTime* t, CString aFile);
	LICENSE CheckLicenseTimeSystem32();
	int getMagicNumberFromString(CString str);

};
