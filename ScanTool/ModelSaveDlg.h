#pragma once


// CModelSaveDlg �Ի���

class CModelSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CModelSaveDlg)

public:
	CModelSaveDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CModelSaveDlg();

// �Ի�������
	enum { IDD = IDD_MODELSAVEDLG };
public:
	CString m_strModelName;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSavemodeldlg();
};
