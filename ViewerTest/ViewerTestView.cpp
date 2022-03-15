
// ViewerTestView.cpp : CViewerTestView クラスの実装
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS は、プレビュー、縮小版、および検索フィルター ハンドラーを実装している ATL プロジェクトで定義でき、
// そのプロジェクトとのドキュメント コードの共有を可能にします。
#ifndef SHARED_HANDLERS
#include "ViewerTest.h"
#endif

#include "ViewerTestDoc.h"
#include "ViewerTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CViewerTestView

IMPLEMENT_DYNCREATE(CViewerTestView, CView)

BEGIN_MESSAGE_MAP(CViewerTestView, CView)
	// 標準印刷コマンド
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CViewerTestView コンストラクション/デストラクション

CViewerTestView::CViewerTestView() noexcept
{
	// TODO: 構築コードをここに追加します。

}

CViewerTestView::~CViewerTestView()
{
}

BOOL CViewerTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

	return CView::PreCreateWindow(cs);
}

// CViewerTestView 描画

void CViewerTestView::OnDraw(CDC* /*pDC*/)
{
	CViewerTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: この場所にネイティブ データ用の描画コードを追加します。
}


// CViewerTestView の印刷

BOOL CViewerTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 既定の印刷準備
	return DoPreparePrinting(pInfo);
}

void CViewerTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷前の特別な初期化処理を追加してください。
}

void CViewerTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 印刷後の後処理を追加してください。
}


// CViewerTestView の診断

#ifdef _DEBUG
void CViewerTestView::AssertValid() const
{
	CView::AssertValid();
}

void CViewerTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CViewerTestDoc* CViewerTestView::GetDocument() const // デバッグ以外のバージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CViewerTestDoc)));
	return (CViewerTestDoc*)m_pDocument;
}
#endif //_DEBUG


// CViewerTestView メッセージ ハンドラー


int CViewerTestView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1) {
		return -1;
	}

	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL)) {
		return -1;
	}

	return 0;
}


void CViewerTestView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	m_wndView.MoveWindow(0, 0, cx, cy);
}


BOOL CViewerTestView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) {
		return true;
	}

	return CView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CViewerTestView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
}


void CViewerTestView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* pHint)
{
	switch (lHint) {
	case 10001:
		m_wndView.LoadPDF(*reinterpret_cast<CString *>(pHint));
		m_wndView.Invalidate();
		break;
	case 10002:
		m_wndView.UnloadPDF();
		m_wndView.Invalidate();
		break;
	}
}
