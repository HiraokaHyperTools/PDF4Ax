// PDF4AxPropPage.cpp : CPDF4AxPropPage プロパティ ページ クラスの実装

#include "stdafx.h"
#include "PDF4Ax.h"
#include "PDF4AxPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CPDF4AxPropPage, COlePropertyPage)



// メッセージ マップ

BEGIN_MESSAGE_MAP(CPDF4AxPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// クラス ファクトリおよび GUID を初期化します。

IMPLEMENT_OLECREATE_EX(CPDF4AxPropPage, "PDF4AX.PDF4AxPropPage.1",
	0xa6444baf, 0xab0d, 0x4ea1, 0xb3, 0xc5, 0xa0, 0x9b, 0x67, 0xa6, 0x46, 0xa4)



// CPDF4AxPropPage::CPDF4AxPropPageFactory::UpdateRegistry -
// CPDF4AxPropPage のシステム レジストリ エントリを追加または削除します。

BOOL CPDF4AxPropPage::CPDF4AxPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_PDF4AX_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CPDF4AxPropPage::CPDF4AxPropPage - コンストラクタ

CPDF4AxPropPage::CPDF4AxPropPage() :
	COlePropertyPage(IDD, IDS_PDF4AX_PPG_CAPTION)
{
}



// CPDF4AxPropPage::DoDataExchange - ページおよびプロパティ間でデータを移動します。

void CPDF4AxPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CPDF4AxPropPage メッセージ ハンドラ
