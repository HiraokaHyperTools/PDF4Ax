
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>

#include "GlobalParams.h"
#include "PDFSpec.h"

#include "PDFDoc.h"

namespace PDFSpec {
	void CreateGlobalParams(const char* customPopplerDataDir) {
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
