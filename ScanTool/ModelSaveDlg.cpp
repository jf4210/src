// ModelSaveDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ScanTool.h"
#include "ModelSaveDlg.h"
#include "afxdialogex.h"


// CModelSaveDlg �Ի���

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


// CModelSaveDlg ��Ϣ�������


void CModelSaveDlg::OnBnClickedBtnSavemodeldlg()
{
	UpdateData(TRUE);

	OnOK();
}
