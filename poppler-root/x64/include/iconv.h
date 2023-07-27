
#ifndef _ICONV_H_
#define _ICONV_H_

typedef int iconv_t;

extern "C" {

	iconv_t iconv_open(const char* tocode, const char* fromcode);

	size_t iconv(iconv_t cd,
		char** inbuf, size_t* inbytesleft,
		char** outbuf, size_t* outbytesleft);

	int iconv_close(iconv_t cd);

}

#endif // _ICONV_H_
