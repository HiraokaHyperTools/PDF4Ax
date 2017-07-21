
#include "GlobalParams.h"
#include "Ext.h"

namespace Ext {
	void CreateGlobalParams(const char *customPopplerDataDir) {
		if (globalParams == NULL)
			globalParams = new GlobalParams(customPopplerDataDir);
	}
	void DestroyGlobalParams() {
		if (globalParams != NULL) {
			delete globalParams;
			globalParams = NULL;
		}
	}
}
