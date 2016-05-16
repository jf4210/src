// ModelSaveDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ScanTool.h"
#include "ModelSaveDlg.h"
#include "afxdialogex.h"


// CModelSaveDlg 对话框

IMPLEMENT_DYNAMIC(CModelSaveDlg, CDialog)

CModelSaveDlg::CModelSaveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModelSaveDlg::IDD, pParent)
{

}

CModelSaveDlg::~CModelSaveDlg()
{
}

void CModelSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ModelName, m_strModelName);
}


BEGIN_MESSAGE_MAP(CModelSaveDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_SaveModelDlg, &CModelSaveDlg::OnBnClickedBtnSavemodeldlg)
END_MESSAGE_MAP()


// CModelSaveDlg 消息处理程序


void CModelSaveDlg::OnBnClickedBtnSavemodeldlg()
{
	UpdateData(TRUE);

	OnOK();
}
