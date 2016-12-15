// AxFrm.h : CAxFrame クラスのインターフェイス
//


#pragma once

#include "AxVw.h"

class CAxFrame : public CFrameWnd
{
	
public:
	CAxFrame();
protected: 

// 属性
public:

// 操作
public:

// オーバーライド
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

// 実装
public:
	virtual ~CAxFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // コントロール バー用メンバ
//	CStatusBar  m_wndStatusBar;
//	CReBar      m_wndReBar;
//	CToolBar    m_wndToolBar;

public:
	CAxVw    m_wndView;

// 生成された、メッセージ割り当て関数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	virtual void PostNcDestroy();
	afx_msg void OnAppAbout();
};


