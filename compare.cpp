// compare.cpp
// this namespace is not used ..... Deping
//#include "compare.h"
#include "stdafx.h"

namespace metasharp
{
	inline int GetExpoBase2(double d)
	{
		int i = 0;
		((short *)(&i))[0] = (((short *)(&d))[3] & (short)32752); // _123456789ab____ & 0111111111110000
		return (i >> 4) - 1023;
	}

	bool	Equals(double d1, double d2)
	{
		if (d1 == d2)
			return true;
		int e1 = GetExpoBase2(d1);
		int e2 = GetExpoBase2(d2);
		int e3 = GetExpoBase2(d1 - d2);
		if ((e3 - e2 < -48) && (e3 - e1 < -48))
			return true;
		return false;
	}

	bool	Equals(float d1, float d2)
	{
		if (d1 == d2)
			return true;
		int e1 = GetExpoBase2(d1);
		int e2 = GetExpoBase2(d2);
		int e3 = GetExpoBase2(d1 - d2);
		if ((e3 - e2 < -48) && (e3 - e1 < -48))
			return true;
		return false;
	}

	int 	Compare(double d1, double d2)
	{
		if (Equals(d1, d2) == true)
			return 0;
		if (d1 > d2)
			return 1;
		return -1;
	}

	bool	Greater(double d1, double d2)
	{
		if (Equals(d1, d2) == true)
			return false;
		if (d1 > d2)
			return true;
		return false;
	}

	bool	GreaterOrEqual(double d1, double d2)
	{
		if (Equals(d1, d2) == true)
			return true;
		if (d1 > d2)
			return true;
		return false;
	}

	bool	Less(double d1, double d2)
	{
		if (Equals(d1, d2) == true)
			return false;
		if (d1 < d2)
			return true;
		return false;
	}

	bool	Less(float d1f, float d2f)
	{
		return Less((double)d1f, (double)d2f);
	}

	bool	LessOrEqual(double d1, double d2)
	{
		if (Equals(d1, d2) == true)
			return true;
		if (d1 < d2)
			return true;
		return false;
	}
}
