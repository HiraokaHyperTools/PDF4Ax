
#include "pch.h"
#include "GlobalParams.h"
#include "PDFSpecExt.h"

namespace Ext {
	void CreateGlobalParams(const char *customPopplerDataDir) {
		GlobalParamsIniter::setCustomDataDir(customPopplerDataDir);
		//if (globalParams == NULL)
		//	globalParams = new GlobalParams(customPopplerDataDir);
	}
	void DestroyGlobalParams() {
		//if (globalParams != NULL) {
		//	delete globalParams;
		//	globalParams = NULL;
		//}
	}
}
