// PapersInfoSaveDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ScanTool.h"
#include "PapersInfoSaveDlg.h"
#include "afxdialogex.h"


// CPapersInfoSaveDlg 对话框

IMPLEMENT_DYNAMIC(CPapersInfoSaveDlg, CDialog)

CPapersInfoSaveDlg::CPapersInfoSaveDlg(pPAPERSINFO pPapers, CWnd* pParent /*=NULL*/)
	: CDialog(CPapersInfoSaveDlg::IDD, pParent)
	, m_nPaperCount(0), m_strPapersName(_T("")), m_strPapersDetail(_T("")), m_pPapers(pPapers)
{

}

CPapersInfoSaveDlg::~CPapersInfoSaveDlg()
{
}

void CPapersInfoSaveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PapersName, m_strPapersName);
	DDX_Text(pDX, IDC_EDIT_PaperCount, m_nPaperCount);
	DDX_Text(pDX, IDC_EDIT_PapersDetail, m_strPapersDetail);
}


BEGIN_MESSAGE_MAP(CPapersInfoSaveDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CPapersInfoSaveDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CPapersInfoSaveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	USES_CONVERSION;
	m_nPaperCount = m_pPapers->nPaperCount;
	m_strPapersDetail = m_pPapers->strPapersDesc.c_str();
	m_strPapersName = m_pPapers->strPapersName.c_str();
	UpdateData(FALSE);
	return TRUE;
}


// CPapersInfoSaveDlg 消息处理程序


void CPapersInfoSaveDlg::OnBnClickedOk()
{
	UpdateData(TRUE);
	USES_CONVERSION;
	m_pPapers->strPapersName = T2A(m_strPapersName);
	m_pPapers->strPapersDesc = T2A(m_strPapersDetail);

	CDialog::OnOK();
}
