#pragma once


// CModelInfoDlg �Ի���

class CModelInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CModelInfoDlg)

public:
	CModelInfoDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CModelInfoDlg();

// �Ի�������
	enum { IDD = IDD_MODELINFODLG };

public:
	CString		m_strPaperNum;
	bool		m_bABPaperModel;		//AB��ģʽ
	bool		m_bHasHead;				//�Ƿ���ͬ��ͷ
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedRadioNotabpaper();
	afx_msg void OnBnClickedRadioAbpaper();
	afx_msg void OnBnClickedRadioNohead();
	afx_msg void OnBnClickedRadioHashead();
};
