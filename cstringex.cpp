#include "stdafx.h"
#include <stdio.h>
#include <string.h>

#include "cstringex.h"


CStringEx::CStringEx(int i, const char* format, DWORD options)
{
	char s[64];							// Eeek, a hard coded limit on integer sizes!
	sprintf(s, "%d", i);
	if (options & COMMA_DELIMIT)
	{
		CString::operator=(CommaDelimitNumber(s));
	}
	else
	{
		CString::operator=(s);
	}
}


CStringEx::CStringEx(double d, const char* format, DWORD options)
{
	char s[64];							// Eeek, a hard coded limit on double sizes!
	sprintf(s, format, d);
	if (options & COMMA_DELIMIT)
	{
		CString::operator=(CommaDelimitNumber(s));
	}
	else
	{
		CString::operator=(s);
	}
}


// extracts string to the left of c, or entire string if c does not exist in str
CStringEx CStringEx::Left(char c, int q) const
{
	int n=0;
	while (q)
	{
		n=Find(c, n);
		if (n != -1)
		{
			++n;
		}
		else
		{
			return *this;
		}
		--q;
	}

	return CString::Left(n-1);
}


// extracts string to the right of c, or returns empty string if c does not exist in str
CStringEx CStringEx::Right(char c, int q) const
{
	int n=0;
	while (q)
	{
		n=Find(c, n);
		if (n == -1)
		{
			return "";
		}
		--q;
		++n;
	}
	return CString::Right(GetLength()-n);
}


// return the string to the right of the rightmost instance of c, or an empty string if c does not exist in str
CStringEx CStringEx::Rightmost(char c) const
{
	int n=ReverseFind(c);
	if (n == -1)
	{
		return "";
	}
	return CString::Right(GetLength()-n-1);
}


// return the string to the left of the rightmost instance of c, or an empty string if c does not exist in str
CStringEx CStringEx::LeftOfRightmost(char c) const
{
	int n=ReverseFind(c);
	if (n == -1)
	{
		return "";
	}
	return CString::Left(n);
}


CStringEx CStringEx::SubStr(int begin, int len) const
{
	return CString::Mid(begin, len);
}


CStringEx CStringEx::CommaDelimitNumber(const char* s)
{
	CStringEx s2=s;										// convert to CStringEx
	CStringEx q=s2.Left('.');								// get everything to the left of the first decimal point
	char posNegChar=q[0];									// get the first char
	bool posNeg=!isdigit(posNegChar);							// if not digit, then assume + or -
	if (posNeg)											// if so, strip off
	{
		q=q.SubStr(1, strlen(q)-1);
	}
	CStringEx dp=s2.Right('.');								// remember everything to the right of the decimal point
	CStringEx q2;											// working string
	while (strlen(q) > 3)									// if more than three digits...
	{
		CStringEx s3=CStringEx(",")+q.SubStr(strlen(q)-3, 3);		// insert a comma before the last three digits (100's)
		q2=s3+q2;											// append this to our working string
		q=q.SubStr(0, strlen(q)-3);							// get everything except the last three digits
	}
	q2=q+q2;												// prepend remainder to the working string
	if (dp != "")											// if we have decimal point...
	{
		q2=q2+"."+dp;										// append it and the digits
	}
	if (posNeg)											// if we stripped off a +/- ...
	{
		q2=CStringEx(posNegChar)+q2;							// add it back in
	}
	return q2;											// this is our final comma delimited string
}

