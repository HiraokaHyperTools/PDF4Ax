#pragma once


// CPrintingNowDialog �_�C�A���O

class CPrintingNowDialog : public CDialog
{
	DECLARE_DYNAMIC(CPrintingNowDialog)

public:
	CPrintingNowDialog(CWnd* pParent = nullptr);   // �W���R���X�g���N�^�[
	virtual ~CPrintingNowDialog();

	CString m_strPrintPos;

	// �_�C�A���O �f�[�^
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PRINTING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �T�|�[�g

	DECLARE_MESSAGE_MAP()
};
