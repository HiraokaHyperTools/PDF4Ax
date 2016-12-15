#pragma once

// PDF4AxPropPage.h : CPDF4AxPropPage プロパティ ページ クラスの宣言です。


// CPDF4AxPropPage : 実装に関しては PDF4AxPropPage.cpp を参照してください。

class CPDF4AxPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CPDF4AxPropPage)
	DECLARE_OLECREATE_EX(CPDF4AxPropPage)

// コンストラクタ
public:
	CPDF4AxPropPage();

// ダイアログ データ
	enum { IDD = IDD_PROPPAGE_PDF4AX };

// 実装
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

// メッセージ マップ
protected:
	DECLARE_MESSAGE_MAP()
};

