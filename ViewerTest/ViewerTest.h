
// ViewerTest.h : ViewerTest アプリケーションのメイン ヘッダー ファイル
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH に対してこのファイルをインクルードする前に 'pch.h' をインクルードしてください"
#endif

#include "resource.h"       // メイン シンボル


// CViewerTestApp:
// このクラスの実装については、ViewerTest.cpp を参照してください
//

class CViewerTestApp : public CWinApp
{
public:
	CViewerTestApp() noexcept;


// オーバーライド
public:
	virtual BOOL InitInstance();

// 実装
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CViewerTestApp theApp;
