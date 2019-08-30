// AxFrm.cpp : CAxFrame クラスの実装
//

#include "stdafx.h"
#include "resource.h"
#include "AxFrm.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAxFrame

#define MY_WM_UPDATE_UI (WM_APP+0x100)

BEGIN_MESSAGE_MAP(CAxFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_WM_TIMER()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ステータス ライン インジケータ
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CAxFrame コンストラクション/デストラクション

CAxFrame::CAxFrame()
{

}

CAxFrame::~CAxFrame()
{

}

int CAxFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL)) {
		return -1;
	}
	
//	if (false
//		|| !m_wndToolBar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT)
//		|| !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)
//	) {
//		return -1;
//	}

//	if (false
//		|| !m_wndReBar.Create(this)
//		|| !m_wndReBar.AddBar(&m_wndToolBar)
//	) {
//		return -1;      // 作成できませんでした。
//	}

//	if (false
//		|| !m_wndStatusBar.Create(this)
//		|| !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT))
//	) {
//		return -1;      // 作成できませんでした。
//	}

//	SetTimer(0x0100, 100, NULL);

//	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	//m_wndView.LoadPDF(_T("C:\\Documents and Settings\\KU\\My Documents\\PDFs\\デジタルドルフィンズバージョンアップ.pdf.pdf"));

	return 0;
}

BOOL CAxFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	cs.hMenu = NULL;
	return TRUE;
}


// CAxFrame 診断

#ifdef _DEBUG
void CAxFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CAxFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CAxFrame メッセージ ハンドラ

void CAxFrame::OnSetFocus(CWnd* /*pOldWnd*/) {
	// ビュー ウィンドウにフォーカスを与えます。
	m_wndView.SetFocus();
}

BOOL CAxFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) {
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return true;
	if (CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return true;

	return false;
}

void CAxFrame::PostNcDestroy() {
}

void CAxFrame::OnAppAbout() {
	CAboutDlg wndDlg;
	wndDlg.DoModal();
}

#define WM_IDLEUPDATECMDUI  0x0363  // wParam == bDisableIfNoHandler

void CAxFrame::OnTimer(UINT_PTR nIDEvent) {
//	if (nIDEvent == 0x0100) {
//		m_wndToolBar.SendMessage(WM_IDLEUPDATECMDUI, 1);
//	}

	CFrameWnd::OnTimer(nIDEvent);
}

void CAxFrame::OnKillFocus(CWnd* pNewWnd) {
	CFrameWnd::OnKillFocus(pNewWnd);
	m_wndView.OnKillFocus(pNewWnd);
}
