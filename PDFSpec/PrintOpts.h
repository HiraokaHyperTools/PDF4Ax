#pragma once

#include "RUt.h"

class PrintOpts {
public:
	PrintOpts()
		: m_bCentering(false), m_bIgnoreMargin(false), m_bAutoPaperSize(false)
	{

	}

	BOOL m_bCentering;
	BOOL m_bIgnoreMargin;
	BOOL m_bAutoPaperSize;

	void LoadFromReg() {
		RUt::GetBool(_T("HIRAOKA HYPERS TOOLS, Inc."), _T("AxTIF5"), _T("centering"), m_bCentering, false);
		RUt::GetBool(_T("HIRAOKA HYPERS TOOLS, Inc."), _T("AxTIF5"), _T("ignoremargin"), m_bIgnoreMargin, false);
		RUt::GetBool(_T("HIRAOKA HYPERS TOOLS, Inc."), _T("AxTIF5"), _T("autopapersize"), m_bAutoPaperSize, false);
	}

	void SaveToReg() {
		RUt::SetBool(_T("HIRAOKA HYPERS TOOLS, Inc."), _T("AxTIF5"), _T("centering"), m_bCentering);
		RUt::SetBool(_T("HIRAOKA HYPERS TOOLS, Inc."), _T("AxTIF5"), _T("ignoremargin"), m_bIgnoreMargin);
		RUt::SetBool(_T("HIRAOKA HYPERS TOOLS, Inc."), _T("AxTIF5"), _T("autopapersize"), m_bAutoPaperSize);
	}
};
