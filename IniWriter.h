#ifndef INIWRITER_H
#define INIWRITER_H

class CIniWriter
{
public:
	CIniWriter(CString szFileName);
	void WriteInteger(CString szSection, CString szKey, int iValue);
	void WriteFloat(CString szSection, CString szKey, float fltValue);
	void WriteBoolean(CString szSection, CString szKey, bool bolValue);
	void WriteString(CString szSection, CString szKey, CString szValue);

	// This is purely for Yang Chenzhi's medical function
	static CString convTxtMedicalLotus2Excel(CString aFile);

private:
	CString m_szFileName;
};
#endif //INIWRITER_H 