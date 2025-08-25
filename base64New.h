// #include <string>

    #pragma once
    #define DECODE_ERROR 0xffffffff

#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);


int CStringEncodeBase64(const CString &szInput, CString &szOutput);
int CStringDecodeBase64(const CString &szInput, CString &szOutput);
