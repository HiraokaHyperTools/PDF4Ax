// Frm.cpp : アプリケーションのクラス動作を定義します。
//

#include "stdafx.h"
#include "Frm.h"
#include "AxFrm.h"
#include "Ext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CFrmApp

BEGIN_MESSAGE_MAP(CFrmApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CFrmApp::OnAppAbout)
END_MESSAGE_MAP()


// CFrmApp コンストラクション

CFrmApp::CFrmApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
}


// 唯一の CFrmApp オブジェクトです。

CFrmApp theApp;


// CFrmApp 初期化

BOOL CFrmApp::InitInstance()
{
	CWinApp::InitInstance();

	// 標準初期化
	// これらの機能を使わずに最終的な実行可能ファイルの
	// サイズを縮小したい場合は、以下から不要な初期化
	// ルーチンを削除してください。
	// 設定が格納されているレジストリ キーを変更します。
	// TODO: 会社名または組織名などの適切な文字列に
	// この文字列を変更してください。

	//_CrtSetBreakAlloc(1601);

	TCHAR tcMe[MAX_PATH] = {0};
	GetModuleFileName(AfxGetInstanceHandle(), tcMe, MAX_PATH);
	PathRemoveFileSpec(tcMe);
	TCHAR tcDir[MAX_PATH] = {0};
	PathCombine(tcDir, tcMe, _T("share\\poppler"));
	Ext::CreateGlobalParams(CT2A(tcDir));

	SetRegistryKey(_T("アプリケーション ウィザードで生成されたローカル アプリケーション"));
	// メイン ウィンドウを作成するとき、このコードは新しいフレーム ウィンドウ オブジェクトを作成し、
	// それをアプリケーションのメイン ウィンドウにセットします
	CAxFrame* pFrame = new CAxFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// フレームをリソースからロードして作成します
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);





	// メイン ウィンドウが初期化されたので、表示と更新を行います。
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// 接尾辞が存在する場合にのみ DragAcceptFiles を呼び出してください。
	//  SDI アプリケーションでは、ProcessShellCommand の直後にこの呼び出しが発生しなければなりません。
	return TRUE;
}


// CFrmApp メッセージ ハンドラ




// アプリケーションのバージョン情報に使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// 実装
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// ダイアログを実行するためのアプリケーション コマンド
void CFrmApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CFrmApp メッセージ ハンドラ


int CFrmApp::ExitInstance() {
	Ext::DestroyGlobalParams();

	return CWinApp::ExitInstance();
}

int _tmain() {
	if (!AfxWinInit(GetModuleHandle(NULL), 0, GetCommandLine(), SW_SHOW)) {
		return 1;
	}
	if (AfxGetApp()->InitApplication()) {
		AfxGetApp()->InitInstance();
		AfxGetApp()->Run();
	}
	AfxWinTerm();
	return 0;
}
