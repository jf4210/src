#pragma once
#include "global.h"

// CPapersInfoSaveDlg 对话框

class CPapersInfoSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CPapersInfoSaveDlg)

public:
	CPapersInfoSaveDlg(pPAPERSINFO pPapers, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPapersInfoSaveDlg();

// 对话框数据
	enum { IDD = IDD_PAPERSINFOSAVEDLG };

public:
	CString		m_strPapersName;
	int			m_nPaperCount;
	CString		m_strPapersDetail;
private:
	pPAPERSINFO m_pPapers;
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
