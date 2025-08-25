#include "stdafx.h"
#include "IniReader.h"
#include "iostream"
#include "Windows.h"
#include "ealloc_d.h"

CIniReader::CIniReader(CString  szFileName)
{
	//memset(m_szFileName, 0x00, 255);
	//memcpy(m_szFileName, szFileName, _tcslen(szFileName));

	m_szFileName = szFileName;
}

CString trimComments(TCHAR in[])
{
	CString out = CString(in);
	out = out.Trim();
	int i = out.Find(_T(";"));
	if (i < 0) return out;
	out = out.Mid(0, i);
	out = out.Trim();
	return out.MakeLower();
}
CString CIniReader::ReadString(CString  szSection, CString  szKey, const CString  szDefaultValue)
{
	TCHAR* szResult = (TCHAR*)alloc1(255, sizeof(TCHAR));
	memset(szResult, 0x00, 255);
	GetPrivateProfileString(szSection, szKey, szDefaultValue, szResult, 255, m_szFileName);
	CString t = trimComments(szResult);
	free1(szResult);
	return t;
}

int CIniReader::ReadInteger(CString  szSection, CString  szKey, int iDefaultValue)
{
	// int iResult = GetPrivateProfileInt(szSection,  szKey, iDefaultValue, m_szFileName); 

	CString szDefaultValue;
	szDefaultValue.Format(_T("%d"), iDefaultValue);
	CString t = ReadString(szSection, szKey, szDefaultValue);
	int iResult = _ttoi(t);

	return iResult;
}
float CIniReader::ReadFloat(CString  szSection, CString  szKey, float fltDefaultValue)
{
	//TCHAR szResult[255];
	//TCHAR szDefault[255];
	//float fltResult;
	//_stprintf_s(szDefault, _T("%f"),fltDefaultValue);
	//GetPrivateProfileString(szSection,  szKey, szDefault, szResult, 255, m_szFileName); 
	//fltResult =  (float)_tstof(trimComments(szResult));

	CString szDefaultValue;
	szDefaultValue.Format(_T("%g"), fltDefaultValue);
	CString t = ReadString(szSection, szKey, szDefaultValue);
	float fltResult = (float)_ttof(t);
	return fltResult;
}
bool CIniReader::ReadBoolean(CString  szSection, CString  szKey, bool bolDefaultValue)
{
	//TCHAR szResult[255];
	//GetPrivateProfileString(szSection, szKey, szDefault, szResult, 255, m_szFileName); 
	//CString t = trimComments(szResult);


	TCHAR szDefault[255];
	_stprintf_s(szDefault, _T("%s"), bolDefaultValue ? _T("True") : _T("False"));
	CString t = ReadString(szSection, szKey, szDefault);
	// bool bolResult =  (_tcscmp(t, _T("true")) == 0) ? true : false;
	bool bolResult = (t.Compare(_T("true")) == 0) ? true : false;
	return bolResult;
}


