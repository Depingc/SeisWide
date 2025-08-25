#ifndef CSTRINGEX_H
#define CSTRINGEX_H

class CStringEx: public CString
{
	public:

		enum Options
		{
			NO_OPTIONS=0,
			COMMA_DELIMIT=1,
		};

	public:

		CStringEx(void) {};
		CStringEx(const char* s) : CString(s) {}
		CStringEx(const char c) {char s[2]={'\0', '\0'}; s[0]=c; CString::operator=(s);}
		CStringEx(int i, const char* format="%d", DWORD options=NO_OPTIONS);
		CStringEx(double d, const char* format="%02lf", DWORD options=NO_OPTIONS);
		virtual ~CStringEx() {};

		CStringEx Left(char c, int n=1) const;						// everything to the left of the nth occurance of <c>
		CStringEx Right(char c, int n=1) const;						// everything to the right of the nth occurance of <c>
		CStringEx Rightmost(char c) const;							// everything to the right of the last occurance of <c>
		CStringEx LeftOfRightmost(char c) const;					// everything to the left of the last occurance of <c>
		CStringEx SubStr(int begin, int len) const;					// substring from s[begin] to s[begin+len]

		void Trim(void) {TrimLeft(); TrimRight();};					// trims both left and right sides

		CStringEx& operator=(const CString& s) {CString::operator=(s); return *this;}

		CStringEx CommaDelimitNumber(const char* s);

};

#endif
