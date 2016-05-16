#pragma once
#include "global.h"
#include "PicShow.h"
#include "MyCodeConvert.h"
#include "FileUpLoad.h"
// CPaperInputDlg �Ի���

class CPaperInputDlg : public CDialog
{
	DECLARE_DYNAMIC(CPaperInputDlg)

public:
	CPaperInputDlg(pMODEL pModel, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPaperInputDlg();

// �Ի�������
	enum { IDD = IDD_PAPERINPUTDLG };

public:
	CListCtrl	m_lPapersCtrl;		//�Ծ���б�
	CListCtrl	m_lPaperCtrl;		//�Ծ���е��Ծ��б�
	CListCtrl	m_lIssuePaperCtrl;	//�����Ծ��б�
	CComboBox	m_comboModel;		//ģ�������б�
	CTabCtrl	m_tabPicShow;		//ͼƬ��ʾTab�ؼ�
	CButton		m_btnBroswer;		//�����ť

	CString		m_strPapersPath;	//�Ծ���ļ���·��
	int			m_nModelPicNums;	//ģ��ͼƬ������һ��ģ���ж���ͼƬ����Ӧ�����Ծ�

	CString		m_strModelName;		//���Ծ������ģ�������
	CString		m_strPapersName;	//���Ծ��������
	CString		m_strPapersDesc;	//���Ծ����������Ϣ

//	CFileUpLoad	m_fileUpLoad;

	pMODEL		m_pOldModel;				//��ģ����Ϣ���л�combox��ʱ���ܰ����ͷ�
	pMODEL		m_pModel;					//ģ����Ϣ
	CPicShow*	m_pCurrentPicShow;			//��ǰͼƬ��ʾ�ؼ�
	std::vector<CPicShow*>	m_vecPicShow;	//�洢ͼƬ��ʾ����ָ�룬�ж��ģ��ͼƬʱ����Ӧ����ͬ��tab�ؼ�ҳ��
	int						m_nCurrTabSel;	//��ǰTab�ؼ�ѡ���ҳ��

	int				m_nCurrItemPapers;		//�Ծ���б�ǰѡ�����
	int				m_nCurrItemPaper;		//�Ծ��б�ǰѡ�����
	int				m_ncomboCurrentSel;		//�����б�ǰѡ����
private:
	void	InitUI();
	void	InitCtrlPosition(); 
	void	InitTab();

	void	SeachModel();
// 	bool	UnZipModel(CString strZipPath);
// 	pMODEL	LoadModelFile(CString strModelPath);			//����ģ���ļ�

	void	PaintRecognisedRect(pST_PaperInfo pPaper);		//����ʶ��ľ���
	void	PaintIssueRect(pST_PaperInfo pPaper);			//������ʶ��������������λ��
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnBroswer();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnCbnSelchangeComboModellist();
	afx_msg void OnNMDblclkListPapers(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListPaper(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTcnSelchangeTabPicshow(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListIssuepaper(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSave();
};
