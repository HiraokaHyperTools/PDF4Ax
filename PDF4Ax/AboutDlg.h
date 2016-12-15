
#pragma once

class CAboutDlg : public CDialog {
public:
	CAboutDlg(): CDialog(IDD_ABOUTBOX_PDF4AX) {
	}

	BOOL OnInitDialog() {
		CWnd *pWnd = GetDlgItem(IDC_STATIC_VER);
		if (pWnd != NULL) {
			HMODULE hMod = AfxGetResourceHandle();
			HRSRC hVeri = FindResource(hMod, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
			PVOID pvVeri = LockResource(LoadResource(hMod, hVeri));
			DWORD cbVeri = SizeofResource(hMod, hVeri);
			if (pvVeri != NULL && cbVeri != 0) {
				VS_FIXEDFILEINFO *pffi = NULL;
				UINT cb = 0;
				if (VerQueryValue(pvVeri, _T("\\"), reinterpret_cast<LPVOID *>(&pffi), &cb)) {
					if (pffi->dwSignature == 0xFEEF04BD) {
						CString strText;
						pWnd->GetWindowText(strText);
						CString strVer;
						strVer.Format(_T("%u.%u.%u")
							, 0U +(WORD)(pffi->dwFileVersionMS >> 16)
							, 0U +(WORD)(pffi->dwFileVersionMS >> 0)
							, 0U +(WORD)(pffi->dwFileVersionLS >> 16)
							, 0U +(WORD)(pffi->dwFileVersionLS >> 0)
							);
						strText.Replace(_T("x.x.x"), strVer);
						pWnd->SetWindowText(strText);
					}
				}
			}
		}
		return true;
	}
};

