// PrintingNowDialog.cpp : �����t�@�C��
//

#include "pch.h"
#include "res/PDFSpecResource.h"
#include "PrintingNowDialog.h"
#include "afxdialogex.h"


// CPrintingNowDialog �_�C�A���O

IMPLEMENT_DYNAMIC(CPrintingNowDialog, CDialog)

CPrintingNowDialog::CPrintingNowDialog(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_PRINTING, pParent)
{

}

CPrintingNowDialog::~CPrintingNowDialog()
{
}

void CPrintingNowDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PRINT_POS, m_strPrintPos);
}


BEGIN_MESSAGE_MAP(CPrintingNowDialog, CDialog)
END_MESSAGE_MAP()


// CPrintingNowDialog ���b�Z�[�W �n���h���[
