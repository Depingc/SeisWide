/* 
   base64.cpp and base64.h

   Copyright (C) 2004-2008 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

*/

#include "stdafx.h"
#include "base64.h"
#include "ealloc_d.h"

static char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//static char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

//static const std::string base64_chars = 
//             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//             "abcdefghijklmnopqrstuvwxyz"
//             "0123456789+/";


size_t CStringEncodeBase64(const CString &szInput, CString &szOutput)
{
	//return 0;

	size_t nRet = 0;
	size_t nLeng = 0;
    nLeng    = (szInput.GetLength() + 1)*sizeof(TCHAR);
    
//	char *pInput = new char[nLeng];
	// char *pOutput = new char[nLeng];  
	char *pInput = (char*)alloc1(nLeng, sizeof(char));
	char *pOutput = (char *)alloc1(nLeng, sizeof(char));
    ZeroMemory(pInput, nLeng);
    ZeroMemory(pOutput, nLeng);

	strcpy_s(pInput, nLeng, CT2A(szInput));
    nRet = base64_encode((void *)pInput, strlen(pInput), &pOutput);
    szOutput = CA2T(pOutput);
	free1(pInput);
	free1(pOutput);
    //delete []pInput;
    //delete []pOutput;

    return nRet;
}
 
size_t CStringDecodeBase64(const CString &szInput, CString &szOutput)
{
	size_t nRet = 0;
	size_t nLeng = 0;
    nLeng    = (szInput.GetLength() + 1)*sizeof(TCHAR);

	//char *pInput = new char[nLeng];
	//char *pOutput = new char[nLeng];
	char *pInput = (char*)alloc1(nLeng, sizeof(char));
	char *pOutput = (char *)alloc1(nLeng, sizeof(char));
	ZeroMemory(pInput, nLeng);
	ZeroMemory(pOutput, nLeng);

    strcpy_s(pInput, nLeng, CT2A(szInput));
    nRet = base64_decode(pInput, pOutput);
    szOutput = CA2T(pOutput);
    //delete [] pInput;
    //delete [] pOutput;
	free1(pInput);
//    delete pInput; // testing
	free1(pOutput);
//    delete pOutput;
	return nRet;
}
 
size_t base64_encode(const void *data, const size_t size, char **str)
{
    char *s, *p;
	size_t i;
    int c;
    const unsigned char *q;
 
    p = s = (char *) alloc1(size * 4 / 3 + 4, sizeof(char));
    if (p == NULL)
    {
        return -1;
    }
    q = (const unsigned char *) data;
    i = 0;
    for (i = 0; i < size;)
    {
        c = q[i++];
        c *= 256;
        if (i < size)
        {
            c += q[i];
        }
        i++;
        c *= 256;
        if (i < size)
        {
            c += q[i];
        }
        i++;
        p[0] = base64_chars[(c & 0x00fc0000) >> 18];
        p[1] = base64_chars[(c & 0x0003f000) >> 12];
        p[2] = base64_chars[(c & 0x00000fc0) >> 6];
        p[3] = base64_chars[(c & 0x0000003f) >> 0];
        if (i > size)
        {
            p[3] = '=';
        }
        if (i > size + 1)
        {
            p[2] = '=';
        }
        p += 4;
    }
 
    *p = 0;
    *str = s;
    return strlen(s);
}
 
 
static size_t pos(const char c)
{
    char *p;
    for (p = base64_chars; *p; p++)
    {
        if (*p == c)
        {
            return p - base64_chars;
        }
    }
    return -1;
}
 
static size_t token_decode(const char *token)
{
    int i;
    unsigned int val = 0;
    int marker = 0;
    if (strlen(token) < 4)
    {
        return DECODE_ERROR;
    }
    for (i = 0; i < 4; i++)
    {
        val *= 64;
        if (token[i] == '=')
        {
            marker++;
        }
        else
        {
            if (marker > 0)
            {
                return DECODE_ERROR;
            }
            else
            {
				val += (unsigned int)pos(token[i]);
            }
        }            
    }
    if (marker > 2)
    {
        return DECODE_ERROR;
    }
    return (marker << 24) | val;
}
 
int base64_decode(const char *str, char *data)
{
    const char *p;
    unsigned char *q;
 
    q = (byte *)data;
    for (p = str; *p && (*p == '=' || strchr(base64_chars, *p)); p += 4)
    {
		unsigned int val = (unsigned int)token_decode(p);
        unsigned int marker = (val >> 24) & 0xff;
        if (val == DECODE_ERROR)
        {
            return -1;
        }
        *q++ = (val >> 16) & 0xff;
        if (marker < 2)
        {
            *q++ = (val >> 8) & 0xff;
        }
        if (marker < 1)
        {
            *q++ = val & 0xff;
        }
    }
//	CString tstr = CString(data);

    return (int)(q - (unsigned char *) data);
}
