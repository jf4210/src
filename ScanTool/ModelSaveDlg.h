#pragma once


// CModelSaveDlg 对话框

class CModelSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CModelSaveDlg)

public:
	CModelSaveDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CModelSaveDlg();

// 对话框数据
	enum { IDD = IDD_MODELSAVEDLG };
public:
	CString m_strModelName;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnSavemodeldlg();
};
