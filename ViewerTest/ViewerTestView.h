
// ViewerTestView.h : CViewerTestView クラスのインターフェイス
//

#pragma once

#include "AxVw.h"

class CViewerTestView : public CView
{
	CAxVw m_wndView;

protected: // シリアル化からのみ作成します。
	CViewerTestView() noexcept;
	DECLARE_DYNCREATE(CViewerTestView)

// 属性
public:
	CViewerTestDoc* GetDocument() const;

// 操作
public:

// オーバーライド
public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画するためにオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 実装
public:
	virtual ~CViewerTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
};

#ifndef _DEBUG  // ViewerTestView.cpp のデバッグ バージョン
inline CViewerTestDoc* CViewerTestView::GetDocument() const
   { return reinterpret_cast<CViewerTestDoc*>(m_pDocument); }
#endif

