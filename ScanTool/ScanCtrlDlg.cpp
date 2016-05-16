// ScanCtrlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ScanTool.h"
#include "ScanCtrlDlg.h"
#include "afxdialogex.h"


// CScanCtrlDlg 对话框

IMPLEMENT_DYNAMIC(CScanCtrlDlg, CDialog)

CScanCtrlDlg::CScanCtrlDlg(SCANSRCARRY& srcArry, CWnd* pParent /*=NULL*/)
: CDialog(CScanCtrlDlg::IDD, pParent)
, m_nStudentNum(0), sourceArry(srcArry), m_nCurrScanSrc(0), m_nCurrDuplex(0)
{

}

CScanCtrlDlg::~CScanCtrlDlg()
{
}

void CScanCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DUPLEX, m_comboDuplex);
	DDX_Control(pDX, IDC_COMBO_SCANSRC, m_comboScanSrc);
	DDX_Text(pDX, IDC_EDIT_TiKaNum, m_nStudentNum);
}

BEGIN_MESSAGE_MAP(CScanCtrlDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_SCANSRC, &CScanCtrlDlg::OnCbnSelchangeComboScansrc)
	ON_CBN_SELCHANGE(IDC_COMBO_DUPLEX, &CScanCtrlDlg::OnCbnSelchangeComboDuplex)
	ON_BN_CLICKED(IDC_BTN_Scan, &CScanCtrlDlg::OnBnClickedBtnScan)
END_MESSAGE_MAP()

BOOL CScanCtrlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	InitUI();

	return TRUE;
}

bool CScanCtrlDlg::ScanSrcInit()
{
	USES_CONVERSION;
	for (int i = 0; i < sourceArry.GetCount(); i++)
	{
		TW_IDENTITY temp_Source = sourceArry.GetAt(i);
		m_comboScanSrc.AddString(A2T(temp_Source.ProductName));
	}

	if (sourceArry.GetCount() == 0)
	{
		m_comboScanSrc.AddString(_T("没有可用的打印机"));
		return false;
	}
	return true;
}

bool CScanCtrlDlg::InitUI()
{
	ScanSrcInit();

	USES_CONVERSION;
	m_comboScanSrc.SetCurSel(0);

	m_comboDuplex.AddString(_T("单面打印"));
	m_comboDuplex.AddString(_T("双面打印"));
	m_comboDuplex.SetCurSel(1);
	m_nCurrDuplex = 1;

	UpdateData(FALSE);
	return true;
}


// CScanCtrlDlg 消息处理程序


void CScanCtrlDlg::OnCbnSelchangeComboScansrc()
{
	if (sourceArry.GetCount())
	{
		m_nCurrScanSrc = m_comboScanSrc.GetCurSel();
	}
}

void CScanCtrlDlg::OnCbnSelchangeComboDuplex()
{
	m_nCurrDuplex = m_comboDuplex.GetCurSel();
}

void CScanCtrlDlg::OnBnClickedBtnScan()
{
	UpdateData(TRUE);
	OnOK();
}
