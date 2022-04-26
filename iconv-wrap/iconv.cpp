
#include <windows.h>
#include "iconv.h"

#define CP_Unicode 1200

namespace {
	class Helper {
	public:
		static int GetCP(const char* name) {
			if (name != NULL) {
				if (_stricmp(name, "UTF-8") == 0) {
					return CP_UTF8;
				}
				if (_stricmp(name, "UTF-16LE") == 0) {
					return CP_Unicode;
				}
				if (_stricmp(name, "UTF-16BE") == 0) {
					return 1201;
				}
			}
			return -1;
		}
	};
}

iconv_t iconv_open(const char* tocode, const char* fromcode)
{
	int to = Helper::GetCP(tocode);
	int from = Helper::GetCP(fromcode);
	if (to >= 0 && from >= 0) {
		return MAKELONG(to, from);
	}
	return (iconv_t)-1;
}

size_t iconv(iconv_t cd,
	char** inbuf, size_t* inbytesleft,
	char** outbuf, size_t* outbytesleft)
{
	if (cd == (iconv_t)-1 || cd == 0) {
		return -1;
	}
	int to = LOWORD(cd);
	int from = HIWORD(cd);

	if (to == CP_Unicode) {
		int cchWritten = MultiByteToWideChar(
			from,
			MB_ERR_INVALID_CHARS,
			*inbuf,
			*inbytesleft,
			reinterpret_cast<LPWSTR>(*outbuf),
			*outbytesleft
		);
		if (cchWritten >= 1) {
			int cbRead = strlen(*inbuf);
			*inbuf += cbRead;
			*inbytesleft -= cbRead;
			*outbuf += sizeof(WCHAR) * cchWritten;
			*outbytesleft -= sizeof(WCHAR) * cchWritten;
			return 0;
		}
		else {
			_set_errno(EINVAL);
		}
	}
	else if (from == CP_Unicode) {
		BOOL usedDefaultChar;
		int cbWritten = WideCharToMultiByte(
			from,
			WC_ERR_INVALID_CHARS,
			reinterpret_cast<LPCWSTR>(*inbuf),
			*inbytesleft,
			*outbuf,
			*outbytesleft,
			NULL,
			&usedDefaultChar
		);
		if (cbWritten >= 1) {
			int cbRead = sizeof(WCHAR) * wcslen(reinterpret_cast<LPCWSTR>(*inbuf));
			*inbuf += cbRead;
			*inbytesleft -= cbRead;
			*outbuf += cbWritten;
			*outbytesleft -= cbWritten;
			return 0;
		}
		else {
			_set_errno(EINVAL);
		}
	}
	else {
		_set_errno(EINVAL);
	}

	return -1;
}

int iconv_close(iconv_t cd)
{
	_set_errno(0);
	return 0;
}
