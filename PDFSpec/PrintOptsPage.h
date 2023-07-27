
#pragma once
#include "PrintOpts.h"

class CPrintOptsPage : public CPropertyPage, public PrintOpts {
	DECLARE_DYNAMIC(CPrintOptsPage);
	DECLARE_MESSAGE_MAP();

public:
	enum { IDD = IDD_PRINT_OPTS };

	CPrintOptsPage()
		: CPropertyPage(IDD, IDS_PDF4AX_PRINT_PROP_CAPTION)
	{
	}

	virtual BOOL OnInitDialog() {
		__super::OnInitDialog();

		UpdateData(false);
		return true;
	}

	void DoDataExchange(CDataExchange* pDX) {
		__super::DoDataExchange(pDX);
		DDX_Check(pDX, IDC_CENTERING, m_bCentering);
		DDX_Check(pDX, IDC_IGNORE_MARGIN, m_bIgnoreMargin);
		DDX_Check(pDX, IDC_AUTO_PAPERSIZE, m_bAutoPaperSize);
		DDX_Check(pDX, IDC_PRINT_ANNOTATION, m_bPrintAnnotation);
	}
};
