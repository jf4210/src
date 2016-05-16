#pragma once


// CModelInfoDlg 对话框

class CModelInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CModelInfoDlg)

public:
	CModelInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CModelInfoDlg();

// 对话框数据
	enum { IDD = IDD_MODELINFODLG };

public:
	CString		m_strPaperNum;
	bool		m_bABPaperModel;		//AB卷模式
	bool		m_bHasHead;				//是否有同步头
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedRadioNotabpaper();
	afx_msg void OnBnClickedRadioAbpaper();
	afx_msg void OnBnClickedRadioNohead();
	afx_msg void OnBnClickedRadioHashead();
};
