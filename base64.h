// #include <string>

    #pragma once
    #define DECODE_ERROR 0xffffffff

size_t CStringEncodeBase64(const CString &szInput, CString &szOutput);
size_t CStringDecodeBase64(const CString &szInput, CString &szOutput);
     
	size_t base64_encode(const void *data, const size_t size, char **str);
	static size_t pos(const char c);
	static size_t token_decode(const char *token);
    int base64_decode(const char *str, char *data);

