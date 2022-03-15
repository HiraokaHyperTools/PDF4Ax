
// ViewerTestDoc.cpp : CViewerTestDoc クラスの実装
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS は、プレビュー、縮小版、および検索フィルター ハンドラーを実装している ATL プロジェクトで定義でき、
// そのプロジェクトとのドキュメント コードの共有を可能にします。
#ifndef SHARED_HANDLERS
#include "ViewerTest.h"
#endif

#include "ViewerTestDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CViewerTestDoc

IMPLEMENT_DYNCREATE(CViewerTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CViewerTestDoc, CDocument)
END_MESSAGE_MAP()


// CViewerTestDoc コンストラクション/デストラクション

CViewerTestDoc::CViewerTestDoc() noexcept
{
	// TODO: この位置に 1 度だけ呼ばれる構築用のコードを追加してください。

}

CViewerTestDoc::~CViewerTestDoc()
{
}

BOOL CViewerTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	UpdateAllViews(NULL, 10002);

	return TRUE;
}




// CViewerTestDoc のシリアル化

void CViewerTestDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 格納するコードをここに追加してください。
	}
	else
	{
		// TODO: 読み込むコードをここに追加してください。
	}
}

#ifdef SHARED_HANDLERS

//縮小版のサポート
void CViewerTestDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// このコードを変更してドキュメントのデータを描画します
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 検索ハンドラーのサポート
void CViewerTestDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ドキュメントのデータから検索コンテンツを設定します。
	// コンテンツの各部分は ";" で区切る必要があります

	// 例:      strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CViewerTestDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CViewerTestDoc の診断

#ifdef _DEBUG
void CViewerTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CViewerTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CViewerTestDoc コマンド


BOOL CViewerTestDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	CString filePath(lpszPathName);
	UpdateAllViews(NULL, 10001, reinterpret_cast<CObject*>(&filePath));
	SetModifiedFlag(false);

	return TRUE;
}
