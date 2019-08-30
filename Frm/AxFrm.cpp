// AxFrm.cpp : CAxFrame クラスの実装
//

#include "stdafx.h"
#include "resource.h"
#include "AxFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAxFrame

IMPLEMENT_DYNAMIC(CAxFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CAxFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEACTIVATE()
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
	AfxPostQuitMessage(0);
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

//	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	//m_wndView.LoadPDF(_T("C:\\Proj\\TestConvPdfs2\\JP.pdf"));
	m_wndView.LoadPDF(_T("C:\\A\\A.pdf"));
	//m_wndView.LoadPDF(_T("E:\\DL\\saitama.pdf"));
	//m_wndView.LoadPDF(_T("E:\\DL\\map_tokyo.pdf"));
	//m_wndView.LoadPDF(_T("H:\\DL\\OpenGL\\GDC2003_OGL_ARBFragmentProgram.pdf"));
	//m_wndView.LoadPDF(_T("E:\\DL\\TIFF6.pdf"));

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

BOOL CAxFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// ビューに最初にコマンドを処理する機会を与えます。
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// それ以外の場合は、既定の処理を行います。
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

int CAxFrame::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) {
//	if (nHitTest == HTCLIENT) {
//		return MA_ACTIVATEANDEAT;
//	}

	return CFrameWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
