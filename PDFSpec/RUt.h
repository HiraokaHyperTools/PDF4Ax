
#pragma once

class RUt {
public:
	static bool GetInt(LPCTSTR pszCom, LPCTSTR pszApp, LPCTSTR pszVal, DWORD& val, DWORD defval = 0) {
		CRegKey rk;
		LONG r;
		CString strKey; strKey.Format(
			_T("Software\\%s\\%s")
			, pszCom
			, pszApp
		);
		if (0 == (r = rk.Open(HKEY_CURRENT_USER, strKey, KEY_READ))) {
			if (0 == (r = rk.QueryDWORDValue(pszVal, val))) {
				return true;
			}
		}
		if (0 == (r = rk.Open(HKEY_LOCAL_MACHINE, strKey, KEY_READ))) {
			DWORD ddcompat = 0;
			if (0 == (r = rk.QueryDWORDValue(pszVal, val))) {
				return true;
			}
		}
		val = defval;
		return false;
	}

	static bool GetBool(LPCTSTR pszCom, LPCTSTR pszApp, LPCTSTR pszVal, BOOL& val, BOOL defval = FALSE) {
		DWORD temp;
		if (GetInt(pszCom, pszApp, pszVal, temp, defval)) {
			val = temp != 0;
			return true;
		}
		val = defval;
		return false;
	}

	static bool GetStr(LPCTSTR pszCom, LPCTSTR pszApp, LPCTSTR pszVal, CString& value, LPCTSTR defaultValue = NULL) {
		CRegKey rk;
		LONG r;
		CString strKey; strKey.Format(
			_T("Software\\%s\\%s")
			, pszCom
			, pszApp
		);
		for (int x = 0; x < 2; x++) {
			if (0 == (r = rk.Open((x == 0) ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE, strKey, KEY_READ))) {
				ULONG cchBuf = 1024;
				LPTSTR pszBuf = value.GetBuffer(cchBuf);
				if (0 == (r = rk.QueryStringValue(pszVal, pszBuf, &cchBuf))) {
					value.ReleaseBuffer(cchBuf);
					return true;
				}
				value.ReleaseBuffer(0);
			}
		}
		value = defaultValue;
		return false;
	}

	static bool SetBool(LPCTSTR pszCom, LPCTSTR pszApp, LPCTSTR pszVal, BOOL data) {
		CRegKey rk;
		LONG r;
		CString strKey;
		strKey.Format(
			_T("Software\\%s\\%s")
			, pszCom
			, pszApp
		);
		if (0 == (r = rk.Create(HKEY_CURRENT_USER, strKey))) {
			if (0 == (r = rk.SetDWORDValue(pszVal, data ? 1 : 0))) {
				return true;
			}
		}
		return false;
	}

	static bool SetStr(LPCTSTR pszCom, LPCTSTR pszApp, LPCTSTR pszVal, LPCTSTR data) {
		CRegKey rk;
		LONG r;
		CString strKey;
		strKey.Format(
			_T("Software\\%s\\%s")
			, pszCom
			, pszApp
		);
		if (0 == (r = rk.Create(HKEY_CURRENT_USER, strKey))) {
			if (0 == (r = rk.SetStringValue(pszVal, data))) {
				return true;
			}
		}
		return false;
	}
};