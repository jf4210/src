#pragma once
#include "TwainCpp.h"

typedef CArray<TW_IDENTITY, TW_IDENTITY> SCANSRCARRY;

// CScanCtrlDlg �Ի���

class CScanCtrlDlg : public CDialog
{
	DECLARE_DYNAMIC(CScanCtrlDlg)

public:
	CScanCtrlDlg(SCANSRCARRY& srcArry, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CScanCtrlDlg();

// �Ի�������
	enum { IDD = IDD_SCANCTRLDLG };

	bool	ScanSrcInit();
	bool	InitUI();
public:
	CComboBox	m_comboScanSrc;		//ɨ����Դ
	CComboBox	m_comboDuplex;		//��˫������
	int		m_nStudentNum;		//ѧ������

	SCANSRCARRY& sourceArry;
	int			m_nCurrScanSrc;		//��ǰѡ���ɨ����Դ
	int			m_nCurrDuplex;		//��ǰѡ��ĵ�˫��
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnCbnSelchangeComboScansrc();
	afx_msg void OnCbnSelchangeComboDuplex();
	afx_msg void OnBnClickedBtnScan();
};
