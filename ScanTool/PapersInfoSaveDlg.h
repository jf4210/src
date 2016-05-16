#pragma once
#include "global.h"

// CPapersInfoSaveDlg �Ի���

class CPapersInfoSaveDlg : public CDialog
{
	DECLARE_DYNAMIC(CPapersInfoSaveDlg)

public:
	CPapersInfoSaveDlg(pPAPERSINFO pPapers, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPapersInfoSaveDlg();

// �Ի�������
	enum { IDD = IDD_PAPERSINFOSAVEDLG };

public:
	CString		m_strPapersName;
	int			m_nPaperCount;
	CString		m_strPapersDetail;
private:
	pPAPERSINFO m_pPapers;
protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
