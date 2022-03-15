
// ViewerTestDoc.h : CViewerTestDoc クラスのインターフェイス
//


#pragma once


class CViewerTestDoc : public CDocument
{
protected: // シリアル化からのみ作成します。
	CViewerTestDoc() noexcept;
	DECLARE_DYNCREATE(CViewerTestDoc)

// 属性
public:

// 操作
public:

// オーバーライド
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 実装
public:
	virtual ~CViewerTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 検索ハンドラーの検索コンテンツを設定するヘルパー関数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
};
