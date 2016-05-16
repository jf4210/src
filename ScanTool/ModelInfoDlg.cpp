// ModelInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ScanTool.h"
#include "ModelInfoDlg.h"
#include "afxdialogex.h"


// CModelInfoDlg 对话框

IMPLEMENT_DYNAMIC(CModelInfoDlg, CDialog)

CModelInfoDlg::CModelInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModelInfoDlg::IDD, pParent)
	, m_strPaperNum(_T("2")), m_bABPaperModel(FALSE), m_bHasHead(TRUE)
{

}

CModelInfoDlg::~CModelInfoDlg()
{
}

void CModelInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_Papers, m_strPaperNum);
}


BEGIN_MESSAGE_MAP(CModelInfoDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_OK, &CModelInfoDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_RADIO_NotABPaper, &CModelInfoDlg::OnBnClickedRadioNotabpaper)
	ON_BN_CLICKED(IDC_RADIO_ABPaper, &CModelInfoDlg::OnBnClickedRadioAbpaper)
	ON_BN_CLICKED(IDC_RADIO_NoHead, &CModelInfoDlg::OnBnClickedRadioNohead)
	ON_BN_CLICKED(IDC_RADIO_HasHead, &CModelInfoDlg::OnBnClickedRadioHashead)
END_MESSAGE_MAP()

BOOL CModelInfoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	UpdateData(FALSE);

	((CButton*)GetDlgItem(IDC_RADIO_NotABPaper))->SetCheck(!m_bABPaperModel);
	((CButton*)GetDlgItem(IDC_RADIO_ABPaper))->SetCheck(m_bABPaperModel);

	((CButton*)GetDlgItem(IDC_RADIO_HasHead))->SetCheck(m_bHasHead);
	((CButton*)GetDlgItem(IDC_RADIO_NoHead))->SetCheck(!m_bHasHead);

	return TRUE;
}


// CModelInfoDlg 消息处理程序


void CModelInfoDlg::OnBnClickedBtnOk()
{
	UpdateData(TRUE);
	m_bABPaperModel = ((CButton*)GetDlgItem(IDC_RADIO_ABPaper))->GetCheck();
	m_bHasHead = ((CButton*)GetDlgItem(IDC_RADIO_HasHead))->GetCheck();
	OnOK();
}


void CModelInfoDlg::OnBnClickedRadioNotabpaper()
{
	if (BST_UNCHECKED == ((CButton*)GetDlgItem(IDC_RADIO_NotABPaper))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_RADIO_NotABPaper))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_ABPaper))->SetCheck(BST_UNCHECKED);
	}
}


void CModelInfoDlg::OnBnClickedRadioAbpaper()
{
	if (BST_UNCHECKED == ((CButton*)GetDlgItem(IDC_RADIO_ABPaper))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_RADIO_NotABPaper))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_ABPaper))->SetCheck(BST_CHECKED);
	}
}


void CModelInfoDlg::OnBnClickedRadioNohead()
{
	if (BST_UNCHECKED == ((CButton*)GetDlgItem(IDC_RADIO_NoHead))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_RADIO_HasHead))->SetCheck(BST_UNCHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_NoHead))->SetCheck(BST_CHECKED);
	}
}


void CModelInfoDlg::OnBnClickedRadioHashead()
{
	if (BST_UNCHECKED == ((CButton*)GetDlgItem(IDC_RADIO_HasHead))->GetCheck())
	{
		((CButton*)GetDlgItem(IDC_RADIO_HasHead))->SetCheck(BST_CHECKED);
		((CButton*)GetDlgItem(IDC_RADIO_NoHead))->SetCheck(BST_UNCHECKED);
	}
}
