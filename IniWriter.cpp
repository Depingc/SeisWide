#include "stdafx.h"
#include "IniWriter.h"
#include "iostream"
#include "Windows.h"
 
CIniWriter::CIniWriter(CString szFileName)
{
	//memset(m_szFileName, 0x00, 255); 
	//memcpy(m_szFileName, szFileName, _tcslen(szFileName));
	m_szFileName = _T("");
}
void CIniWriter::WriteInteger(CString szSection, CString szKey, int iValue)
{
	TCHAR szValue[255];
	_stprintf_s(szValue, _T("%d"), iValue);
	WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
}
void CIniWriter::WriteFloat(CString szSection, CString szKey, float fltValue)
{
	TCHAR szValue[255];
	_stprintf_s(szValue, _T("%f"), fltValue);
	WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
}
void CIniWriter::WriteBoolean(CString szSection, CString szKey, bool bolValue)
{
	TCHAR szValue[255];
	_stprintf_s(szValue, _T("%s"), bolValue ? _T("True") : _T("False"));
	WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName); 
}
void CIniWriter::WriteString(CString szSection, CString szKey, CString szValue)
{
	WritePrivateProfileString(szSection,  szKey, szValue, m_szFileName);
}


// ######################################################################################################
// The following is purely for Yang Chenzhi's medical function ##########################################
#include "util.h"
CString getParamValueSpaced(CString cs, CString param) {
	cs.Replace(TAB, SPACE);
	int lenParam = param.GetLength();
	int i = cs.Find(param); // line 1: LOCUS       AB221035                 979 bp    RNA     linear   VRL 26-JUL-2016 
	if (i >= 0) {
		cs = cs.Mid(i + lenParam);  // "       AB221035                 979 bp    RNA     linear   VRL 26-JUL-2016"
		cs = cs.Trim(); // "AB221035                 979 bp    RNA     linear   VRL 26-JUL-2016"
		i = cs.Find(SPACE);
		if (i >= 0) {
			cs = cs.Mid(0, i); // "AB221035"
			return stripQuotes(cs);
		}
	}
	return BLANK; // not found
}

CString getParamValueDelim(CString cs, CString param, CString delim) {
	cs.Replace(TAB, SPACE);
	int lenParam = param.GetLength();
	int i = cs.Find(param); // line XXX:             /country="Viet Nam: Tay Nguyen"
	if (i >= 0) {
		cs = cs.Mid(i + lenParam);   // ="Viet Nam: Tay Nguyen"
		cs = cs.Trim();   // ="Viet Nam: Tay Nguyen"
		i = cs.Find(delim);   
		int lenDelim = delim.GetLength(); // usually 1, delim is either "=" or ":"
		if (i >= 0) {
			cs = cs.Mid(i+lenDelim).Trim(); // ="Viet Nam: Tay Nguyen"
			return stripQuotes(cs);
		}
	}
	return BLANK; // not found
}

CString getParamLine(CStringArray *arr, CString param) {
	for (int i = 0; i < arr->GetCount(); i++) {
		if (arr->ElementAt(i).Find(param) >= 0) {
			return arr->ElementAt(i);
		}
	}
	return BLANK;
}

int getIStart(CStringArray *arr) {
	CString tStr;
	for (int i = 0; i < arr->GetCount(); i++) {
		tStr = arr->ElementAt(i).Trim(); // "CDS             1..1701"
		if (tStr.Find(_T("CDS ")) == 0) {
			int j = tStr.Find(_T(".."));
			if (j > 0) {
				tStr = tStr.Mid(0, j); // "CDS             1"
				
				// now need to extract the last number in tStr
				j = tStr.ReverseFind(' ');
				if (j > 0) {
					tStr = tStr.Mid(j + 1); // NOTE: tStr can be prepended with <1 because the original string can be: " CDS             <1..>920"
					for (j = 0; j < tStr.GetLength(); j++) {
						if (isDigit(tStr.GetAt(j)))
							return StringToInt(tStr.Mid(j));
					}

				}
			}
		}
	}
	return 1;
}

CString getParamValueMultiLine(CStringArray *arr, CString param, CString delim) {
	for (int i = 0; i < arr->GetCount(); i++) {
		CString aline = arr->ElementAt(i);
		if (aline.Find(param) >= 0) {
			CString out = getParamValueDelim(aline, param, delim);
			int k = aline.Find(out);
			if (k < 0) continue; // not expected situation
			if (aline.Find(QUOTE, k+1) < 0) {
				for (int j = i + 1; j < arr->GetCount(); j++) {
					aline = arr->ElementAt(j);
					out += stripQuotes(aline.Trim());
					if (aline.Find(QUOTE) >= 0) break;
				} // for j
			}
			return out;
		}
	}
	return BLANK;
}

CString getOriginValueMultiLine(CStringArray *arr) {
	int iStart = -1;
	CString aline;
	for (int i = 0; i < arr->GetCount(); i++) {
		aline = arr->ElementAt(i);
		aline = aline.Trim();
		if (aline == _T("ORIGIN")) {
			iStart = i + 1;
			break;
		}
	}

	if (iStart < 0) return BLANK;
	int iEnd = 0;
	for (int i = iStart; i < arr->GetCount(); i++) {
		aline = arr->ElementAt(i);
		if (aline.Find(_T("//")) >= 0) {
			iEnd = i;
			break;
		}
	}
	if (iEnd - iStart < 1) return BLANK;

	CString out = BLANK;
	for (int i = iStart; i < iEnd; i++) {
		aline = arr->ElementAt(i);
		aline = aline.Trim();
		int k = aline.Find(SPACE);
		if (k < 0) continue;
		out += aline.Mid(k + 1);
	}

	out.Replace(SPACE, BLANK);
	return out;
}


void addProteinStat(CStringArray *arr) {
	int i,j,k;
	CString aline;
	int rows = arr->GetCount();
	TCHAR** protein = (TCHAR **)alloc2(600, rows -1, sizeof(TCHAR));
	TCHAR ** origin = (TCHAR **)alloc2(1800, rows -1, sizeof(TCHAR));
	for (i = 1; i < rows-1; i++) {
		aline = arr->ElementAt(i);
		aline = aline.Trim();
		CStringArray* arrLine = CString2LinesByComma(aline);
		int nl = arrLine->GetCount();
		if (nl < 5) continue; // skip too short lines
		for (j = 4; j < 604; j++) {
			aline = arrLine->ElementAt(j);
			protein[i - 1][j - 4] = aline.GetAt(0);
		}

		memset(origin[i - 1], (int)' ', 1800 * sizeof(TCHAR));
		for (j = 604; j < arrLine->GetSize(); j++) {
			aline = arrLine->ElementAt(j);
			aline = aline.Trim();

			//if (j == 604 && i == 69) {
			//	int sdfesw = 1;
			//}

			if (aline.GetLength() < 3) 
				continue; // skip too short ones
			k = j - 604;
			origin[i - 1][k*3] = aline.GetAt(0);
			origin[i - 1][k*3 + 1] = aline.GetAt(1);
			origin[i - 1][k*3 + 2] = aline.GetAt(2);
		}
		arrLine->RemoveAll();
		delete arrLine;
	}

	//free2((void **)protein);
	//free2((void **)origin);
	//return;

	static TCHAR targOrigin[64] = {
		_T('F'),
		_T('F'),
		_T('L'),
		_T('L'),
		_T('S'),
		_T('S'),
		_T('S'),
		_T('S'),
		_T('Y'),
		_T('Y'),
		_T('Y'),
		_T('Y'),
		_T('C'),
		_T('C'),
		_T('W'),
		_T('W'),
		_T('L'),
		_T('L'),
		_T('L'),
		_T('L'),
		_T('P'),
		_T('P'),
		_T('P'),
		_T('P'),
		_T('H'),
		_T('H'),
		_T('Q'),
		_T('Q'),
		_T('R'),
		_T('R'),
		_T('R'),
		_T('R'),
		_T('I'),
		_T('I'),
		_T('I'),
		_T('M'),
		_T('T'),
		_T('T'),
		_T('T'),
		_T('T'),
		_T('N'),
		_T('N'),
		_T('K'),
		_T('K'),
		_T('S'),
		_T('S'),
		_T('R'),
		_T('R'),
		_T('V'),
		_T('V'),
		_T('V'),
		_T('V'),
		_T('A'),
		_T('A'),
		_T('A'),
		_T('A'),
		_T('D'),
		_T('D'),
		_T('E'),
		_T('E'),
		_T('G'),
		_T('G'),
		_T('G'),
		_T('G')
	};
	static CString targOrigins[64] = {
		_T("ttt"),
		_T("ttc"),
		_T("tta"),
		_T("ttg"),
		_T("tct"),
		_T("tcc"),
		_T("tca"),
		_T("tcg"),
		_T("tat"),
		_T("tac"),
		_T("taa"),
		_T("tag"),
		_T("tgt"),
		_T("tgc"),
		_T("tga"),
		_T("tgg"),
		_T("ctt"),
		_T("ctc"),
		_T("cta"),
		_T("ctg"),
		_T("cct"),
		_T("ccc"),
		_T("cca"),
		_T("ccg"),
		_T("cat"),
		_T("cac"),
		_T("caa"),
		_T("cag"),
		_T("cgt"),
		_T("cgc"),
		_T("cga"),
		_T("cgg"),
		_T("att"),
		_T("atc"),
		_T("ata"),
		_T("atg"),
		_T("act"),
		_T("acc"),
		_T("aca"),
		_T("acg"),
		_T("aat"),
		_T("aac"),
		_T("aaa"),
		_T("aag"),
		_T("agt"),
		_T("agc"),
		_T("aga"),
		_T("agg"),
		_T("gtt"),
		_T("gtc"),
		_T("gta"),
		_T("gtg"),
		_T("gct"),
		_T("gcc"),
		_T("gca"),
		_T("gcg"),
		_T("gat"),
		_T("gac"),
		_T("gaa"),
		_T("gag"),
		_T("ggt"),
		_T("ggc"),
		_T("gga"),
		_T("ggg")
	};
	int** nOrigin = (int **)alloc2(600, 64, sizeof(int));
	for (int ii = 0; ii < 64; ii++) {
		memset(nOrigin[ii], 0, 600 * sizeof(int));
		if (targOrigins[ii].GetLength() < 3) 
			continue;
		register TCHAR c1 = targOrigins[ii].GetAt(0);
		register TCHAR c2 = targOrigins[ii].GetAt(1);
		register TCHAR c3 = targOrigins[ii].GetAt(2);
		for (j = 0; j < 600; j++) {
			for (i = 0; i < rows - 1; i++) {
				if (origin[i][j*3] == c1 && origin[i][j * 3+1]==c2 && origin[i][j * 3+2]==c3)
					nOrigin[ii][j]++;
			}
		}
	}

	//CStringArray* test = new CStringArray();
	//int testCount = 0;
	//int untestCount = 0;
	//register TCHAR c1 = _T('a');
	//register TCHAR c2 = _T('t');
	//register TCHAR c3 = _T('g');
	//for (i = 0; i < rows - 1; i++) {
	//	aline = arr->ElementAt(i + 1);  // the first line is header only, and should be skipped
	//	aline = aline.Trim();
	//	if (aline.GetLength() < 4) break;
	//	CStringArray* arrLine = CString2LinesByComma(aline);
	//	if (origin[i][0] == c1 && origin[i][1] == c2 && origin[i][2] == c3) {
	//		testCount++;
	//		aline.Format(_T("%s   %d  %d"), arrLine->ElementAt(604), testCount, i+2);
	//	}
	//	else {
	//		untestCount++;
	//		if (i + 2 == 611) {
	//			aline = arrLine->ElementAt(603);
	//			aline = arrLine->ElementAt(604);
	//			aline = arrLine->ElementAt(605);
	//		}
	//		aline.Format(_T("%s   %d  %d"), arrLine->ElementAt(604), untestCount, i+2);
	//	}
	//	test->Add(aline);
	//	arrLine->RemoveAll();
	//	delete arrLine;
	//}
	//showStringArrayToFile(test);
	//test->RemoveAll();
	//delete test;


	static TCHAR targProtein[20] = {
		_T('F'),
		_T('L'),
		_T('I'),
		_T('M'),
		_T('V'),
		_T('S'),
		_T('P'),
		_T('T'),
		_T('A'),
		_T('Y'),
		_T('H'),
		_T('Q'),
		_T('N'),
		_T('K'),
		_T('D'),
		_T('E'),
		_T('C'),
		_T('W'),
		_T('R'),
		_T('G')
	};
	int* nProtein = (int *)alloc1(600, sizeof(int));


	CStringArray *arrLine = new CStringArray();
	for (int ii = 0; ii < 64; ii++) {
		arrLine->RemoveAll();
		memset(nProtein, 0, 600 * sizeof(int));
		if (ii < 20) {
			for (j = 0; j < 600; j++) {
				for (i = 0; i < rows - 1; i++) {
					if (protein[i][j] == targProtein[ii])
						nProtein[j]++;
				}
			}
			arrLine->Add(_T(",,,"));
			arrLine->Add(targProtein[ii]);

			for (j = 0; j < 598; j++) {
				if (protein[0][j] < 65 || protein[0][j] > 122) { // this line needs revision!!!
					arrLine->Add(COMMA);
				}
				else {
					aline.Format(_T(",%d"), nProtein[j]);
					arrLine->Add(aline);
				}
			}
		}
		else {
			for (j = 0; j < 601; j++) {
				aline.Format(_T(","));
				arrLine->Add(aline);
			}
		}

		// now add stats for origins
		arrLine->Add(_T(","));
		arrLine->Add(targOrigin[ii]);
		arrLine->Add(_T(","));
		arrLine->Add(targOrigins[ii]);
		for (j = 0; j < 600; j++) {
			if (origin[0][j] < 65 || origin[0][j]>122) { // this line needs revision!!!
				break;
			}
			aline.Format(_T(",%d"), nOrigin[ii][j]);
			arrLine->Add(aline);
		}
		arr->Add(convStringArrayToCString(arrLine, false));
	}

	// add the last two rows: for printing total sum
	arrLine->RemoveAll();
	for (j = 0; j < 602; j++) {
		aline.Format(_T(","));
		arrLine->Add(aline);
	}
	aline.Format(_T(",SUM"));
	arrLine->Add(aline);
	for (j = 0; j < 600; j++) {
		if (origin[0][j] < 65 || origin[0][j]>122) { // this line needs revision!!!
			break;
		}
		register int sum = 0;
		for (int ii = 0; ii < 64; ii++) {
			sum += nOrigin[ii][j];
		}
		aline.Format(_T(",%d"), sum);
		arrLine->Add(aline);
	}
	arr->Add(SPACE);
	arr->Add(convStringArrayToCString(arrLine, false));

	// finally, clean the memory
	arrLine->RemoveAll();
	delete arrLine;
	free1((void *)nProtein);
	free2((void **)nOrigin);

	free2((void **)protein);
	free2((void **)origin);
}




CString CIniWriter::convTxtMedicalLotus2Excel(CString aFile) {
	int col, row = 0;
	CStringArray *arrCsv = new CStringArray();
	CStringArray *arrLine = new CStringArray();
	CString strErr = BLANK;

	CString tStr;
	CString param;
	int count = 0;

	// put label line
	arrLine->Add(_T("VID,SOURCE,COUNTRY,COLLDATE"));
	for (col = 0; col < 600; col++) {
		tStr.Format(_T(",P%d"), col + 1);
		arrLine->Add(tStr);
	}
	for (col = 0; col < 600; col++) {
		tStr.Format(_T(",O%d"), col + 1);
		arrLine->Add(tStr);
	}
	arrCsv->Add(convStringArrayToCString(arrLine, false));

	CString str = getTextFromFileUnix(aFile);
	while (true) {
		arrLine->RemoveAll();
		CStringArray* arr = getLinesFromString(str, true, _T("LOCUS"), count + 1);
		if (!arr) {
			delete arr;
			break;
		}
		row = count * 3;

		// line 1: LOCUS       AB221035 
		CString cs = arr->ElementAt(0);
		CString val = getParamValueSpaced(cs, _T("LOCUS"));  // "AB221035"
		if (val.GetLength() < 2) break; // invalid ID
		arrLine->Add(val);
		tStr = val;

		param = _T("/host");
		cs = getParamLine(arr, param);
		val = getParamValueDelim(cs, param, _T("="));  // "human, gender:M Vietnam"
		if (val.IsEmpty()) {
			// try another way
			param = _T("/isolation_source");
			cs = getParamLine(arr, param);
			val = getParamValueDelim(cs, param, _T("="));  // "human, gender:M Vietnam"
		}
		int i = val.Find(_T(";"));
		if (i > 0)
			val = val.Mid(0, i); // human
		else {
			i = val.Find(_T(","));
			if (i > 0) val = val.Mid(0, i); // human
		}
		arrLine->Add(_T(","));
		arrLine->Add(addQuotes(val));

		param = _T("/country");
		cs = getParamLine(arr, param);
		val = getParamValueDelim(cs, param, _T("="));  // "Viet Nam: Tay Nguyen"
		arrLine->Add(_T(","));
		arrLine->Add(addQuotes(val));

		param = _T("collection date");
		cs = getParamLine(arr, param);
		val = getParamValueDelim(cs, param, _T(":"));  // "9/25/2003"
		if (val.IsEmpty()) {
			param = _T("collection_date");
			cs = getParamLine(arr, param);
			val = getParamValueDelim(cs, param, _T("="));  // "9/25/2003"
		}
		if (val.IsEmpty()) {
			param = _T("/strain");
			cs = getParamLine(arr, param);
			val = getParamValueDelim(cs, param, _T("="));  // "A/Hanoi/TX09/2006"
			i = val.ReverseFind(_T('/'));
			if (i >= 2) {
				int j = val.Mid(0, i).ReverseFind(_T('/'));
				if (j >= 0) val = val.Mid(j + 1);
				else val = val.Mid(i - 2);
				if (val.GetLength()>2 && val.Mid(0, 1) == _T('/'))
					val = val.Mid(1);
			}
		}
		arrLine->Add(_T(","));
		arrLine->Add(addQuotes(val));

		param = _T("/translation");
		val = getParamValueMultiLine(arr, param, _T("="));  // "Viet Nam: Tay Nguyen"
		int lenProt = val.GetLength();
		if (lenProt > 600) lenProt = 600;
		for (col = 0; col < lenProt; col++) {
			arrLine->Add(_T(","));
			arrLine->Add(val.Mid(col, 1));
		}
		if (lenProt < 600) {
			for (col = lenProt; col < 600; col++)
				arrLine->Add(_T(","));
		}



		//param = _T("/codon_start");
		//cs = getParamLine(arr, param);
		//val = getParamValueDelim(cs, param, _T("="));  // /codon_start=1
		//int iStartOrigin = StringToInt(val);
		//if (iStartOrigin < 1) continue;
		int iStartOrigin = getIStart(arr);
		if (iStartOrigin < 1) {
			strErr += _T("\nWrong starting origin for ") + tStr;
		}
		val = getOriginValueMultiLine(arr);
		lenProt = val.GetLength();
		for (col = iStartOrigin - 1; col < lenProt; col++) {
			arrLine->Add(_T(","));
			arrLine->Add(val.Mid(col, 3));
			col += 2;
		}
		arrCsv->Add(convStringArrayToCString(arrLine, false));

		count++;
		arr->RemoveAll();
		delete arr;
	} // while
	arrCsv->Add(convStringArrayToCString(arrLine, false));

	// now do stat analsys to the CSV format file
	addProteinStat(arrCsv);



	CString newName = GetFileNameAppendExt(aFile, _T("Conv"), _T("csv"));
	if (!saveStringArrayToFile(arrCsv, newName))
		newName = BLANK;
	arrCsv->RemoveAll();
	delete arrCsv;
	arrLine->RemoveAll();
	delete arrLine;

	if (!strErr.IsEmpty())
		AfxMessageBox(strErr);
	return newName;
}



//
//#include "BasicExcel.h"
//CString convTxtMedicalLotus2Excel256(CString aFile) {
//	int col, row = 0;
//	YExcel::BasicExcel xls;
//	// Create a new workbook with 2 worksheets and write some contents.
//	xls.New(1);
//	YExcel::BasicExcelWorksheet* sheet = xls.GetWorksheet((size_t)0);
//
//	CString tStr;
//	CString param;
//	int count = 0;
//	CString str = getTextFromFileUnix(aFile);
//	YExcel::BasicExcelCell* cell;
//
//	while (true) {
//		CStringArray* arr = getLinesFromString(str, true, _T("LOCUS"), count + 1);
//		if (!arr) {
//			delete arr;
//			break;
//		}
//		row = count * 3;
//
//		// line 1: LOCUS       AB221035 
//		CString cs = arr->ElementAt(0);
//		CString val = getParamValueSpaced(cs, _T("LOCUS"));  // "AB221035"
//		if (val.GetLength() < 2) break; // invalid ID
//		if (count == 0) {
//			cell = sheet->Cell(row, 0); cell->Set("ID");
//		}
//		cell = sheet->Cell(row + 1, 0); cell->Set(val);
//
//		param = _T("/isolation_source");
//		cs = getParamLine(arr, param);
//		val = getParamValueDelim(cs, param, _T("="));  // "human, gender:M Vietnam"
//		int i = val.Find(_T(","));
//		if (i > 0) val = val.Mid(0, i); // human
//		if (count == 0) {
//			cell = sheet->Cell(row, 1); cell->Set("SOURCE");
//		}
//		cell = sheet->Cell(row + 1, 1); cell->Set(val);
//
//		param = _T("/country");
//		cs = getParamLine(arr, param);
//		val = getParamValueDelim(cs, param, _T("="));  // "Viet Nam: Tay Nguyen"
//		if (count == 0) {
//			cell = sheet->Cell(row, 2); cell->Set("COUNTRY");
//		}
//		cell = sheet->Cell(row + 1, 2); cell->Set(val);
//
//		param = _T("collection date");
//		cs = getParamLine(arr, param);
//		val = getParamValueDelim(cs, param, _T(":"));  // "9/25/2003"
//		if (count == 0) {
//			cell = sheet->Cell(row, 3); cell->Set("COLL_DATE");
//		}
//		cell = sheet->Cell(row + 1, 3); cell->Set(val);
//
//		param = _T("/translation");
//		val = getParamValueMultiLine(arr, param, _T("="));  // "Viet Nam: Tay Nguyen"
//		int lenProt = val.GetLength();
//		for (col = 0; col <= lenProt / 10; col++) {
//			if (col > 252) {
//				break;
//			}
//			tStr.Format(_T("PROT%d"), col * 10 + 1);
//			if (count == 0) {
//				cell = sheet->Cell(row, col + 4); if (!cell) break; cell->Set(tStr);
//			}
//			cell = sheet->Cell(row + 1, col + 4); if (!cell) break; cell->Set(val.Mid(col * 10, 10));
//		}
//
//		val = getOriginValueMultiLine(arr);
//		lenProt = val.GetLength();
//		for (col = 0; col <= lenProt / 30; col++) {
//			if (col > 252) {
//				break;
//			}
//			cell = sheet->Cell(row + 2, col + 4); if (!cell) break; cell->Set(val.Mid(col * 30, 30));
//		}
//
//
//		count++;
//		arr->RemoveAll();
//		delete arr;
//	} // while
//
//	CString newName = GetFileNameAppendExt(aFile, _T("Conv"), _T("xls"));
//	xls.SaveAs(newName);
//
//	//CString newName = GetFileNameAppendExt(aFile, _T("Conv"), _T("csv"));
//	//ofstream f(newName);
//	//sheet->Print(f, ',', '\"');
//	//f.close();
//	return newName;
//}


/*

	// My Name is Xiangxiong Jian ,I love Programming ,And I love the sharing code
#include "iostream"
#include "IniWriter.h"
#include "IniReader.h"
int main(int argc, char * argv[])
{
 CIniWriter iniWriter(".\\Logger.ini");
 iniWriter.WriteString("Setting", "Name", "jianxx");   
 iniWriter.WriteInteger("Setting", "Age", 27); 
 iniWriter.WriteFloat("Setting", "Height", 1.82f); 
 iniWriter.WriteBoolean("Setting", "Marriage", false);  
 CIniReader iniReader(".\\Logger.ini");
 char *szName = iniReader.ReadString("Setting", "Name", "");   
 int iAge = iniReader.ReadInteger("Setting", "Age", 25); 
 float fltHieght = iniReader.ReadFloat("Setting", "Height", 1.80f); 
 bool bMarriage = iniReader.ReadBoolean("Setting", "Marriage", true); 
 
 std::cout<<"Name:"<<szName<<std::endl
   <<"Age:"<<iAge<<std::endl 
   <<"Height:"<<fltHieght<<std::endl 
   <<"Marriage:"<<bMarriage<<std::endl; 
 delete szName;  
 return 1;   
}

*/