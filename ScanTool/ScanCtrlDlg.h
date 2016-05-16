#pragma once
#include "TwainCpp.h"

typedef CArray<TW_IDENTITY, TW_IDENTITY> SCANSRCARRY;

// CScanCtrlDlg 对话框

class CScanCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CScanCtrlDlg)

public:
	CScanCtrlDlg(SCANSRCARRY& srcArry, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CScanCtrlDlg();

// 对话框数据
	enum { IDD = IDD_SCANCTRLDLG };

	bool	ScanSrcInit();
	bool	InitUI();
public:
	CComboBox	m_comboScanSrc;		//扫描仪源
	CComboBox	m_comboDuplex;		//单双面下拉
	int		m_nStudentNum;		//学生数量

	SCANSRCARRY& sourceArry;
	int			m_nCurrScanSrc;		//当前选择的扫描仪源
	int			m_nCurrDuplex;		//当前选择的单双面
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboScansrc();
	afx_msg void OnCbnSelchangeComboDuplex();
	afx_msg void OnBnClickedBtnScan();
};
