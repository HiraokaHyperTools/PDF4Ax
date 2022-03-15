#pragma once


// CPrintingNowDialog ダイアログ

class CPrintingNowDialog : public CDialog
{
	DECLARE_DYNAMIC(CPrintingNowDialog)

public:
	CPrintingNowDialog(CWnd* pParent = nullptr);   // 標準コンストラクター
	virtual ~CPrintingNowDialog();

	CString m_strPrintPos;

	// ダイアログ データ
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PRINTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV サポート

	DECLARE_MESSAGE_MAP()
};
