
#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>

#include "GlobalParams.h"
#include "PDFSpec.h"

#include "PDFDoc.h"

namespace PDFSpec {
	void InitPopplerDataDir(const char* customPopplerDataDir) {
		GlobalParamsIniter::setCustomDataDir(customPopplerDataDir);
	}
}
