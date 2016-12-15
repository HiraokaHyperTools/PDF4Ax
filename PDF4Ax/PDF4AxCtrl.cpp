// PDF4AxCtrl.cpp :  CPDF4AxCtrl ActiveX コントロール クラスの実装

#include "stdafx.h"
#include "PDF4Ax.h"
#include "PDF4AxCtrl.h"
#include "PDF4AxPropPage.h"
#include "AboutDlg.h"
#include "TUt.h"

#pragma comment(lib, "Version.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CPDF4AxCtrl, COleControl)

BEGIN_INTERFACE_MAP(CPDF4AxCtrl, COleControl)
  INTERFACE_PART(CPDF4AxCtrl, IID_IObjectSafety, ObjectSafety)
  INTERFACE_PART(CPDF4AxCtrl, IID_IBindStatusCallback, BSC)
END_INTERFACE_MAP()

// メッセージ マップ

BEGIN_MESSAGE_MAP(CPDF4AxCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()



// ディスパッチ マップ

BEGIN_DISPATCH_MAP(CPDF4AxCtrl, COleControl)
	DISP_FUNCTION_ID(CPDF4AxCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX_ID(CPDF4AxCtrl, "src", dispidsrc, Getsrc, Setsrc, VT_BSTR)
	DISP_PROPERTY_NOTIFY_ID(CPDF4AxCtrl, "iv", dispidiv, m_iv, OnivChanged, VT_BSTR)
END_DISPATCH_MAP()



// イベント マップ

BEGIN_EVENT_MAP(CPDF4AxCtrl, COleControl)
END_EVENT_MAP()



// プロパティ ページ

// TODO: プロパティ ページを追加して、BEGIN 行の最後にあるカウントを増やしてください。
BEGIN_PROPPAGEIDS(CPDF4AxCtrl, 1)
	PROPPAGEID(CPDF4AxPropPage::guid)
END_PROPPAGEIDS(CPDF4AxCtrl)



// クラス ファクトリおよび GUID を初期化します。

IMPLEMENT_OLECREATE_EX(CPDF4AxCtrl, "PDF4Ax.PDFVw.1",
	0xfe687896, 0xf410, 0x4d10, 0x87, 0x40, 0xd5, 0x84, 0xda, 0x23, 0xc7, 0x4d)



// タイプ ライブラリ ID およびバージョン

IMPLEMENT_OLETYPELIB(CPDF4AxCtrl, _tlid, _wVerMajor, _wVerMinor)



// インターフェイス ID

const IID BASED_CODE IID_DPDF4Ax =
		{ 0x3FF92EA8, 0x9206, 0x48D0, { 0x90, 0xF2, 0x0, 0xF8, 0xA9, 0xA9, 0x8E, 0x16 } };
const IID BASED_CODE IID_DPDF4AxEvents =
		{ 0x7D43C1E0, 0xB4DF, 0x4E6F, { 0x9E, 0x34, 0x4D, 0xDB, 0xF5, 0x25, 0x89, 0x6 } };



// コントロールの種類の情報

static const DWORD BASED_CODE _dwPDF4AxOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CPDF4AxCtrl, IDS_PDF4AX, _dwPDF4AxOleMisc)



// CPDF4AxCtrl::CPDF4AxCtrlFactory::UpdateRegistry -
// CPDF4AxCtrl のシステム レジストリ エントリを追加または削除します。

BOOL CPDF4AxCtrl::CPDF4AxCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: コントロールが apartment モデルのスレッド処理の規則に従っていることを
	// 確認してください。詳細については MFC のテクニカル ノート 64 を参照してください。
	// アパートメント モデルのスレッド処理の規則に従わないコントロールの場合は、6 番目
	// のパラメータを以下のように変更してください。
	// afxRegApartmentThreading を 0 に設定します。

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_PDF4AX,
			IDB_PDF4AX,
			afxRegApartmentThreading,
			_dwPDF4AxOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CPDF4AxCtrl::CPDF4AxCtrl - コンストラクタ

CPDF4AxCtrl::CPDF4AxCtrl()
{
	InitializeIIDs(&IID_DPDF4Ax, &IID_DPDF4AxEvents);
	// ctor
	m_dwSafety = 0;
	m_fAsyncDownload = false;
	m_tickLastInv = 0;
}



// CPDF4AxCtrl::~CPDF4AxCtrl - デストラクタ

CPDF4AxCtrl::~CPDF4AxCtrl()
{
	// TODO: この位置にコントロールのインスタンス データの後処理用のコードを追加してください
}



// CPDF4AxCtrl::OnDraw - 描画関数

void CPDF4AxCtrl::OnDraw(CDC* pDC, const CRect& rcBounds, const CRect& rcInvalid)
{
	class Ut {
	public:
		static CString GetGoodSize(ULONG x) {
			CString str;
			if (x < 1024) {
				str.Format(_T("%u B"), x);
				return str;
			}
			x /= 1024;
			if (x < 1024) {
				str.Format(_T("%u KB"), x);
				return str;
			}
			x /= 1024;
			if (x < 1024) {
				str.Format(_T("%u MB"), x);
				return str;
			}
			x /= 1024;
			{
				str.Format(_T("%u GB"), x);
				return str;
			}
		}
	};

	if (!pDC)
		return;

	CDC &dc = *pDC;

	if (AmbientUserMode() || !m_fAsyncDownload) {
		// Design Mode -> m_frm is hidden.
		// Sync Download -> m_frm already displays.
		dc.SelectStockObject(WHITE_BRUSH);
		dc.SelectStockObject(BLACK_PEN);
		dc.SelectStockObject(DEFAULT_GUI_FONT);
		dc.Rectangle(rcBounds);
		dc.SetTextColor(RGB(0,0,0));
		dc.DrawText(_T("(PDF4Ax Control)"), CRect(rcBounds), DT_SINGLELINE|DT_VCENTER|DT_CENTER);
		return;
	}

	int bkMode = dc.SetBkMode(OPAQUE);
	COLORREF bkClr = dc.SetBkColor(GetSysColor(COLOR_3DDKSHADOW));
	COLORREF fgClr = dc.SetTextColor(GetSysColor(COLOR_3DLIGHT));
	int ta = dc.SetTextAlign(TA_CENTER|TA_BASELINE);

	CFont font;
	font.CreateStockObject(DEFAULT_GUI_FONT);
	LOGFONT lf;
	if (0 != font.GetLogFont(&lf)) {
		lf.lfHeight *= 3;
		font.DeleteObject();
		font.CreateFontIndirect(&lf);
	}

	CFont *oldFont = dc.SelectObject(&font);

	CSize size = dc.GetTextExtent(_T("1234567890"));

	CString str; str.Format(_T("読み込み中: %s / %s")
		, static_cast<LPCTSTR>(Ut::GetGoodSize(m_curPos))
		, static_cast<LPCTSTR>(Ut::GetGoodSize(m_maxPos))
		);
	CPoint pt = rcBounds.CenterPoint();
	ExtTextOut(dc, pt.x, pt.y, ETO_CLIPPED|ETO_OPAQUE, rcBounds, str, str.GetLength(), NULL);

	dc.SelectObject(oldFont);
	dc.SetTextAlign(ta);
	dc.SetTextColor(fgClr);
	dc.SetBkColor(bkClr);
	dc.SetBkMode(bkMode);
}



// CPDF4AxCtrl::DoPropExchange - 永続性のサポート

void CPDF4AxCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	CString &m_src = m_frm.m_wndView.m_strUrl;

	PX_String(pPX, _T("src"), m_src);
}

#pragma comment(lib, "shlwapi.lib")

void CPDF4AxCtrl::LoadFromMoniker(LPBC pibc, LPMONIKER pimkDL) {
	HRESULT hr;

	bool ok = false;

	if (pimkDL != NULL) {
		if (!ok) {
			// ファイル名が出た→Setsrc
			// URLが出た→今は何もしない
			LPOLESTR lpcw = NULL;
			if (SUCCEEDED(hr = pimkDL->GetDisplayName(pibc, NULL, &lpcw))) {
				WCHAR wcfp[1000] = {0};
				DWORD cchfp = 256;
				if (!ok && UrlIsFileUrl(lpcw)) {
					if (SUCCEEDED(hr = PathCreateFromUrl(lpcw, wcfp, &cchfp, NULL))) {
						Setsrc(CW2T(wcfp));
						ok = true;
					}
				}
				if (!ok && PathIsURL(lpcw)) {

				}
				CoTaskMemFree(lpcw);
			}
		}
		if (!ok) {
			// URL等のモニカ読み込み(asynchronous moniker)
			CComPtr<IBindCtx> pibcAsync;
			if (SUCCEEDED(hr = CreateAsyncBindCtx(0, &m_xBSC, NULL, &pibcAsync))) {
				CComPtr<IStream> pSt;
				hr = pimkDL->BindToStorage(pibcAsync, NULL, IID_IStream, reinterpret_cast<void **>(&pSt));
				if (hr == S_OK) {
					if (LoadSyncSt(pSt))
						ok = true;
				}
				else if (hr == MK_S_ASYNCHRONOUS) {
					if (m_frm.m_hWnd != NULL)
						m_frm.ShowWindow(SW_HIDE);
					m_fAsyncDownload = true;
					m_curPos = m_maxPos = 0;
					m_statusText.Empty();
					m_pStAsync.Release();
					ok = true;
				}
			}
		}
		if (!ok) {
			// URL等のモニカ読み込み(not asynchronous moniker)
			CComPtr<IStream> pSt;
			if (SUCCEEDED(hr = pimkDL->BindToStorage(pibc, NULL, IID_IStream, reinterpret_cast<void **>(&pSt)))) {
				if (LoadSyncSt(pSt))
					return;
			}
		}
		if (ok)
			return;
	}
	AfxThrowFileException(CFileException::fileNotFound);
}


// CPDF4AxCtrl::GetControlFlags -
// MFC の ActiveX コントロールの実装のカスタマイズ用フラグです。
//
//DWORD CPDF4AxCtrl::GetControlFlags()
//{
//	DWORD dwFlags = COleControl::GetControlFlags();
//
//
//	// コントロールはウィンドウを作成せずにアクティベート可能です。
//	// TODO: コントロールのメッセージ ハンドラを作成する場合、m_hWnd
//	//		m_hWnd メンバ変数の値が NULL 以外であることを最初に確認
//	//		してから使用してください。
//	dwFlags |= 0;
//	return dwFlags;
//}



// CPDF4AxCtrl::OnResetState - コントロールを既定の状態にリセットします。

void CPDF4AxCtrl::OnResetState()
{
	COleControl::OnResetState();  // DoPropExchange を呼び出して既定値にリセット

	// TODO: この位置にコントロールの状態をリセットする処理を追加してください
}



// CPDF4AxCtrl::AboutBox - "バージョン情報" ボックスをユーザーに表示します。

void CPDF4AxCtrl::AboutBox() {
	CAboutDlg wndDlg;
	wndDlg.DoModal();
}



// CPDF4AxCtrl メッセージ ハンドラ

BSTR CPDF4AxCtrl::Getsrc(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult = m_frm.m_wndView.m_strUrl;

	return strResult.AllocSysString();
}

void CPDF4AxCtrl::Setsrc(LPCTSTR newVal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HRESULT hr;
	if (FAILED(hr = m_frm.m_wndView.LoadPDF(newVal)))
		AfxThrowOleException(hr);

	if (!m_frm.IsWindowVisible())
		m_frm.ShowWindow(SW_SHOW);
}

int CPDF4AxCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_frm.LoadFrame(IDR_MAINFRAME, WS_CHILD, this))
		return -1;

	m_frm.InitialUpdateFrame(NULL, true);

	CString &m_src = m_frm.m_wndView.m_strUrl;
	CString m_title;

	if (m_src.IsEmpty())
		return 0;

	HRESULT hr;

	CComPtr<IBindCtx> pibc;
	if (SUCCEEDED(hr = CreateBindCtx(0, &pibc))) {
		IOleClientSite *pClientSite = GetClientSite();
		if (pClientSite != NULL) {
			CComPtr<IMoniker> pimkDL;
			if (pimkDL == NULL) {
				CComQIPtr<IBindHost> pBindHost = pClientSite;
				if (pBindHost != NULL) {
					if (SUCCEEDED(hr = pBindHost->CreateMoniker(CT2W(m_src), pibc, &pimkDL, 0))) {

					}
				}
			}
			if (pimkDL == NULL) {
				CComPtr<IMoniker> pimkName;
				if (SUCCEEDED(hr = pClientSite->GetMoniker(OLEGETMONIKER_FORCEASSIGN, OLEWHICHMK_CONTAINER, &pimkName))) {
					CComPtr<IBindCtx> pibc;
					CreateBindCtx(0, &pibc);
					ULONG chEaten = 0;
					if (SUCCEEDED(hr = pimkName->ParseDisplayName(pibc, pimkName, CT2W(m_src), &chEaten, &pimkDL))) {

					}
				}
			}

			if (pimkDL != NULL) {
				LPOLESTR pszDisplayName = NULL;
				if (SUCCEEDED(hr = pimkDL->GetDisplayName(pibc, NULL, &pszDisplayName))) {
					m_title = pszDisplayName;
					CoTaskMemFree(pszDisplayName);
				}
			}

			m_pimkDL = pimkDL;

			LoadFromMoniker(pibc, pimkDL);
		}
	}
	return 0;
}

void CPDF4AxCtrl::OnSize(UINT nType, int cx, int cy) {
	COleControl::OnSize(nType, cx, cy);

	if (m_frm.m_hWnd != NULL)
		m_frm.MoveWindow(0, 0, cx, cy);
}

bool CPDF4AxCtrl::LoadSyncSt(IStream *pSt) {
	TRY
		TCHAR tctmp[MAX_PATH] = {0};
		TUt::GetTempPathName(tctmp);

		COleStreamFile fIn(pSt);
		CFile fOut;
		if (fOut.Open(tctmp, CFile::modeCreate|CFile::modeReadWrite|CFile::shareDenyWrite)) {
			BYTE buff[4000];
			while (true) {
				UINT r = fIn.Read(buff, 4000);
				if (r == 0)
					break;
				fOut.Write(buff, r);
			}
			fOut.Close();
			Setsrc(CW2T(tctmp));
			return true;
		}
	CATCH_ALL(e) 
	END_CATCH_ALL
	return false;
}

void CPDF4AxCtrl::OnSetFocus(CWnd* pOldWnd) {
	COleControl::OnSetFocus(pOldWnd);
	if (m_frm.m_hWnd != NULL)
		m_frm.SetFocus();
}

void CPDF4AxCtrl::OnKillFocus(CWnd* pNewWnd) {
	COleControl::OnKillFocus(pNewWnd);
	m_frm.OnKillFocus(pNewWnd);
}

void CPDF4AxCtrl::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) {
	COleControl::OnActivate(nState, pWndOther, bMinimized);
	if (nState == WA_ACTIVE || nState == WA_CLICKACTIVE)
		m_frm.OnSetFocus(pWndOther);
	else if (nState == WA_INACTIVE)
		m_frm.OnKillFocus(NULL);
}

void CPDF4AxCtrl::OnAmbientPropertyChange(DISPID dispid) {
	COleControl::OnAmbientPropertyChange(dispid);

	if (dispid == DISPID_AMBIENT_UIDEAD) {
		if (AmbientUIDead()) {
			m_frm.ShowWindow(SW_HIDE);
		}
		else {
			m_frm.ShowWindow(SW_SHOW);
		}
	}
}

void CPDF4AxCtrl::OnivChanged(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
}

// -*- -*- -*-
// CPDF4AxCtrl::XObjectSafety
// -*- -*- -*-

STDMETHODIMP CPDF4AxCtrl::XObjectSafety::GetInterfaceSafetyOptions(
            REFIID riid, 
            DWORD __RPC_FAR *pdwSupportedOptions, 
            DWORD __RPC_FAR *pdwEnabledOptions)
{
    METHOD_PROLOGUE_EX(CPDF4AxCtrl, ObjectSafety)

	if (!pdwSupportedOptions || !pdwEnabledOptions)
		return E_FAIL;

	LPUNKNOWN pUnk = NULL;
	if (QueryInterface(riid, reinterpret_cast<LPVOID *>(&pUnk)) == E_NOINTERFACE) {
		return E_NOINTERFACE;
	}
	else {
		pUnk->Release();
		pUnk = NULL;
	}

	DWORD &m_dwSafety = pThis->m_dwSafety;

	if (riid == IID_IDispatch) {
		// IDispatch is an interface used for scripting. If your
		// control supports other IDispatch or Dual interfaces, you
		// may decide to add them here as well. Client wants to know
		// if object is safe for scripting. Only indicate safe for
		// scripting when the interface is safe.
		*pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_CALLER;
		*pdwEnabledOptions = m_dwSafety & INTERFACESAFE_FOR_UNTRUSTED_CALLER;
		return S_OK;
	}
	else if ((riid == IID_IPersistStreamInit) || (riid == IID_IPersistStorage) || (riid == IID_IPersistPropertyBag)) {
		// IID_IPersistStreamInit and IID_IPersistStorage are
		// interfaces used for Initialization. If your control
		// supports other Persistence interfaces, you may decide to
		// add them here as well. Client wants to know if object is
		// safe for initializing. Only indicate safe for initializing
		// when the interface is safe.
		*pdwSupportedOptions = INTERFACESAFE_FOR_UNTRUSTED_DATA;
		*pdwEnabledOptions = m_dwSafety & INTERFACESAFE_FOR_UNTRUSTED_DATA;
		return S_OK;
	}
	else {
		// We are saying that no other interfaces in this control are
		// safe for initializing or scripting.
		*pdwSupportedOptions = 0;
		*pdwEnabledOptions = 0;
		return E_FAIL;
	}
}

STDMETHODIMP CPDF4AxCtrl::XObjectSafety::SetInterfaceSafetyOptions(
        REFIID riid, 
        DWORD dwOptionSetMask, 
        DWORD dwEnabledOptions)
{
    METHOD_PROLOGUE_EX(CPDF4AxCtrl, ObjectSafety)
	
	// http://support.microsoft.com/kb/168371/ja

	if (!dwOptionSetMask && !dwEnabledOptions) return E_FAIL;

	LPUNKNOWN pUnk = NULL;
	if (QueryInterface(riid, reinterpret_cast<LPVOID *>(&pUnk)) == E_NOINTERFACE) {
		return E_NOINTERFACE;
	}
	else if (pUnk) {
		pUnk->Release();
		pUnk = NULL;
	}

	DWORD &m_dwSafety = pThis->m_dwSafety;

	m_dwSafety |= dwEnabledOptions & dwOptionSetMask;

	if ((riid == IID_IDispatch) && (m_dwSafety & INTERFACESAFE_FOR_UNTRUSTED_CALLER) != 0) {
	   // Client wants us to disable any functionality that would
	   // make the control unsafe for scripting. The same applies to
	   // any other IDispatch or Dual interfaces your control may
	   // support. Because our control is safe for scripting by
	   // default we just return S_OK.
		return S_OK;
	}
	else if ((riid == IID_IPersistStreamInit || riid == IID_IPersistStorage || riid == IID_IPersistPropertyBag) && (m_dwSafety & INTERFACESAFE_FOR_UNTRUSTED_DATA) != 0) {
	   // Client wants us to make the control safe for initializing
	   // from persistent data. For these interfaces, this control
	   // is safe so we return S_OK. For Any interfaces that are not
	   // safe, we would return E_FAIL.
		return S_OK;
	}

    return E_FAIL;
}

STDMETHODIMP_(ULONG) CPDF4AxCtrl::XObjectSafety::AddRef()
{
    METHOD_PROLOGUE_EX_(CPDF4AxCtrl, ObjectSafety);
    return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CPDF4AxCtrl::XObjectSafety::Release()
{
    METHOD_PROLOGUE_EX_(CPDF4AxCtrl, ObjectSafety);
    return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CPDF4AxCtrl::XObjectSafety::QueryInterface(
    REFIID iid, LPVOID* ppvObj)
{
    METHOD_PROLOGUE_EX_(CPDF4AxCtrl, ObjectSafety);
    return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

BOOL CPDF4AxCtrl::PreCreateWindow(CREATESTRUCT& cs) {
	if (!COleControl::PreCreateWindow(cs))
		return false;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	return true;
}

// -*- -*- -*-
// CPDF4AxCtrl::XBSC
// -*- -*- -*-

STDMETHODIMP_(ULONG) CPDF4AxCtrl::XBSC::AddRef()
{
    METHOD_PROLOGUE_EX_(CPDF4AxCtrl, BSC);
    return (ULONG)pThis->ExternalAddRef();
}

STDMETHODIMP_(ULONG) CPDF4AxCtrl::XBSC::Release()
{
    METHOD_PROLOGUE_EX_(CPDF4AxCtrl, BSC);
    return (ULONG)pThis->ExternalRelease();
}

STDMETHODIMP CPDF4AxCtrl::XBSC::QueryInterface(
    REFIID iid, LPVOID* ppvObj)
{
    METHOD_PROLOGUE_EX_(CPDF4AxCtrl, BSC);
    return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

STDMETHODIMP CPDF4AxCtrl::XBSC::OnStartBinding(/* [in] */ DWORD dwReserved,/* [in] */ __RPC__in_opt IBinding *pib) {
    METHOD_PROLOGUE_EX_(CPDF4AxCtrl, BSC);
	pThis->m_binding = pib;
	return S_OK;
}
STDMETHODIMP CPDF4AxCtrl::XBSC::GetPriority(/* [out] */ __RPC__out LONG *pnPriority) {
	return S_OK;
}
STDMETHODIMP CPDF4AxCtrl::XBSC::OnLowResource(/* [in] */ DWORD reserved) {
	return S_OK;
}
STDMETHODIMP CPDF4AxCtrl::XBSC::OnProgress(/* [in] */ ULONG ulProgress,/* [in] */ ULONG ulProgressMax,/* [in] */ ULONG ulStatusCode,/* [unique][in] */ __RPC__in_opt LPCWSTR szStatusText) {
    METHOD_PROLOGUE_EX_(CPDF4AxCtrl, BSC);

	pThis->m_statusText = szStatusText;
	pThis->m_curPos = ulProgress;
	pThis->m_maxPos = ulProgressMax;

	DWORD t = GetTickCount();
	if (pThis->m_hWnd != NULL && (ulProgress == ulProgressMax || pThis->m_tickLastInv + 500 < t)) {
		pThis->Invalidate();
		pThis->m_tickLastInv = t;
	}
	return S_OK;
}
STDMETHODIMP CPDF4AxCtrl::XBSC::OnStopBinding(/* [in] */ HRESULT hresult,/* [unique][in] */ __RPC__in_opt LPCWSTR szError) {
    METHOD_PROLOGUE_EX_(CPDF4AxCtrl, BSC);

	HRESULT hr;
	if (pThis->m_binding != NULL) {
		CComQIPtr<IWinInetHttpInfo> http = pThis->m_binding;
		if (http != NULL) {
			CHAR cBuf[1000] = {"X-iv"};
			DWORD cbBuf = 1000;
			DWORD flags = 0;
			if (S_OK == (hr = http->QueryInfo(HTTP_QUERY_CUSTOM, cBuf, &cbBuf, &flags, NULL))) {
				TRY {
					pThis->m_iv = cBuf;
					pThis->OnivChanged();
				}
				CATCH_ALL(e) {

				}
				END_CATCH_ALL;
			}
		}
	}
	pThis->m_binding.Release();

	if (hresult == S_OK && pThis->m_pStAsync != NULL) {
		pThis->LoadSyncSt(pThis->m_pStAsync);
	}
	return S_OK;
}
STDMETHODIMP CPDF4AxCtrl::XBSC::GetBindInfo(/* [out] */ DWORD *grfBINDF,/* [unique][out][in] */ BINDINFO *pbindinfo) {
	if (grfBINDF == NULL)
		return E_POINTER;
	*grfBINDF |= BINDF_ASYNCHRONOUS | BINDF_NEEDFILE;
	return S_OK;
}
STDMETHODIMP CPDF4AxCtrl::XBSC::OnDataAvailable(/* [in] */ DWORD grfBSCF,/* [in] */ DWORD dwSize,/* [in] */ FORMATETC *pformatetc,/* [in] */ STGMEDIUM *pstgmed) {
    METHOD_PROLOGUE_EX_(CPDF4AxCtrl, BSC);

	if (0 != (grfBSCF & (BSCF_DATAFULLYAVAILABLE|BSCF_LASTDATANOTIFICATION)) && pstgmed->tymed == TYMED_ISTREAM) {
		pThis->m_pStAsync = pstgmed->pstm;
	}

	ReleaseStgMedium(pstgmed);
	return S_OK;
}
STDMETHODIMP CPDF4AxCtrl::XBSC::OnObjectAvailable(/* [in] */ __RPC__in REFIID riid,/* [iid_is][in] */ __RPC__in_opt IUnknown *punk) {
	return S_OK;
}
