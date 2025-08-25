#ifndef INIREADER_H
#define INIREADER_H
class CIniReader
{
public:
 CIniReader(CString szFileName); 
 int ReadInteger(CString  szSection, CString  szKey, int iDefaultValue);
 float ReadFloat(CString  szSection, CString  szKey, float fltDefaultValue);
 bool ReadBoolean(CString  szSection, CString  szKey, bool bolDefaultValue);
 CString  ReadString(CString  szSection, CString  szKey, const CString  szDefaultValue = _T(""));


private:
  CString m_szFileName;
};
#endif //INIREADER_H