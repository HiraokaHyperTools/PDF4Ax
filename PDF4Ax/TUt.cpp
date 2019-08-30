
#include "StdAfx.h"
#include "TUt.h"

namespace TUt {
	bool GetTempPathName(TCHAR tctmp[MAX_PATH], LPCTSTR pszPrefix) {
		TCHAR tcdir[MAX_PATH] = {0};
		GetTempPath(256, tcdir);
		ZeroMemory(tctmp, sizeof(tctmp));
		if (GetTempFileName(tcdir, pszPrefix, 0, tctmp))
			return true;
		return false;
	}
};
