
// ScanToolDlg.h : ͷ�ļ�
//

#pragma once
#include "global.h"
#include "PicShow.h"
#include "MakeModelDlg.h"
#include "RecognizeThread.h"
#include "PaperInputDlg.h"
#include "TwainCpp.h"
#include "DIB.h"
#include "bmp2ipl.h"
#include "ScanCtrlDlg.h"
#include "PapersInfoSaveDlg.h"
#include "SendFileThread.h"

// CScanToolDlg �Ի���
class CScanToolDlg : public CDialogEx, public CTwain
{
// ����
public:
	CScanToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_SCANTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	CListCtrl		m_lcPicture;			//ͼƬ�б�ؼ�
	CListCtrl		m_lcPaper;				//��ɨ�Ծ��б�ؼ�
	CComboBox		m_comboModel;			//ģ�������б�ؼ�
	CTabCtrl		m_tabPicShowCtrl;		//ͼƬ��ʾ�ؼ�

	int				m_nStatusSize;			//״̬�������С
	CFont			m_fontStatus;			//״̬������
	COLORREF		m_colorStatus;			//״̬��������ɫ

	pMODEL			m_pModel;				//ɨ���Ծ�ʱ��У��ģ��
	MODELLIST		m_lModel;				//�Ѿ����ص�ģ���б�

	int				m_ncomboCurrentSel;		//�����б�ǰѡ����

	Poco::Thread*	m_pRecogThread;
	std::vector<CRecognizeThread*> m_vecRecogThreadObj;
	Poco::Thread*	m_SendFileThread;
	CSendFileThread* m_pSendFileObj;

	std::vector<CPicShow*>	m_vecPicShow;	//�洢ͼƬ��ʾ����ָ�룬�ж��ģ��ͼƬʱ����Ӧ����ͬ��tab�ؼ�ҳ��
	int						m_nCurrTabSel;	//��ǰTab�ؼ�ѡ���ҳ��
	CPicShow*				m_pCurrentPicShow;		//��ǰͼƬ��ʾ�ؼ�
	int						m_nModelPicNums;		//ģ��ͼƬ������һ��ģ���ж���ͼƬ����Ӧ�����Ծ�

	int				m_nScanCount;			//�Ѿ�ɨ�����ɵ�ͼƬ��
	std::string		m_strCurrPicSavePath;	//��ǰɨ��ͼ�񱣴�λ��
	pPAPERSINFO		m_pPapersInfo;			//��ǰɨ��Ŀ�����Ϣ
	pST_PaperInfo	m_pPaper;				//��ǰɨ���ѧ����Ϣ
	pST_PaperInfo	m_pCurrentShowPaper;	//��ǰ��ʾ��ѧ����Ϣ ��m_pCurrentPicShowһ��

	int				m_nScanStatus;			//��ǰɨ��״̬, 0-δɨ�裬1-ɨ���У�2-ɨ���쳣��3-ɨ�����

	std::string		m_strCmdServerIP;
	int				m_nCmdPort;

	BOOL			m_bLogin;
	CString			m_strUserName;
	CString			m_strPwd;
public:
	void	InitUI();
	void	InitTab();
	void	InitConfig();
	void	InitCtrlPosition();
	void	SetFontSize(int nSize);
	void	SetStatusShowInfo(CString strMsg, BOOL bWarn = FALSE);	//����״̬����ʾ����Ϣ

	void	SearchModel();
//	bool	UnZipModel(CString strZipPath);
//	pMODEL	LoadModelFile(CString strModelPath);			//����ģ���ļ�

	void	ShowRectByPoint(cv::Point pt, pST_PaperInfo pPaper);
	LRESULT RoiLBtnDown(WPARAM wParam, LPARAM lParam);		//���������µ�֪ͨ
	LRESULT MsgRecogErr(WPARAM wParam, LPARAM lParam);
	void	PaintRecognisedRect(pST_PaperInfo pPaper);
	int		PaintIssueRect(pST_PaperInfo pPaper);

	int GetRectInfoByPoint(cv::Point pt, pST_PicInfo pPic, RECTINFO*& pRc);

	bool	ScanSrcInit();
// 	BOOL	InitTwain(HWND hWnd);
// 
// 	HWND	m_hMessageWnd;
// 	HINSTANCE m_hTwainDLL;
// 	TW_IDENTITY m_AppId;

	BOOL m_bTwainInit;

	CArray<TW_IDENTITY, TW_IDENTITY> m_scanSourceArry;
	void CopyImage(HANDLE hBitmap, TW_IMAGEINFO& info);
	void SetImage(HANDLE hBitmap, int bits);
	void ScanDone(int nStatus);
// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnScan();
	afx_msg void OnBnClickedBtnScanmodule();
	afx_msg void OnCbnSelchangeComboModel();
	afx_msg void OnBnClickedBtnInputpaper();
	afx_msg void OnTcnSelchangeTabPicshow(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	afx_msg void OnNMDblclkListPicture(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedBtnUploadpapers();
	afx_msg void OnBnClickedBtnLogin();
};
