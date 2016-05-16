
// ScanToolDlg.h : 头文件
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

// CScanToolDlg 对话框
class CScanToolDlg : public CDialogEx, public CTwain
{
// 构造
public:
	CScanToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SCANTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

public:
	CListCtrl		m_lcPicture;			//图片列表控件
	CListCtrl		m_lcPaper;				//已扫试卷列表控件
	CComboBox		m_comboModel;			//模板下拉列表控件
	CTabCtrl		m_tabPicShowCtrl;		//图片显示控件

	int				m_nStatusSize;			//状态栏字体大小
	CFont			m_fontStatus;			//状态栏字体
	COLORREF		m_colorStatus;			//状态栏字体颜色

	pMODEL			m_pModel;				//扫描试卷时的校验模板
	MODELLIST		m_lModel;				//已经加载的模板列表

	int				m_ncomboCurrentSel;		//下拉列表当前选择项

	Poco::Thread*	m_pRecogThread;
	std::vector<CRecognizeThread*> m_vecRecogThreadObj;
	Poco::Thread*	m_SendFileThread;
	CSendFileThread* m_pSendFileObj;

	std::vector<CPicShow*>	m_vecPicShow;	//存储图片显示窗口指针，有多个模板图片时，对应到不同的tab控件页面
	int						m_nCurrTabSel;	//当前Tab控件选择的页面
	CPicShow*				m_pCurrentPicShow;		//当前图片显示控件
	int						m_nModelPicNums;		//模板图片数，即一份模板有多少图片，对应多少试卷

	int				m_nScanCount;			//已经扫描生成的图片数
	std::string		m_strCurrPicSavePath;	//当前扫描图像保存位置
	pPAPERSINFO		m_pPapersInfo;			//当前扫描的考场信息
	pST_PaperInfo	m_pPaper;				//当前扫描的学生信息
	pST_PaperInfo	m_pCurrentShowPaper;	//当前显示的学生信息 与m_pCurrentPicShow一起

	int				m_nScanStatus;			//当前扫描状态, 0-未扫描，1-扫描中，2-扫描异常，3-扫描结束

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
	void	SetStatusShowInfo(CString strMsg, BOOL bWarn = FALSE);	//设置状态栏显示的消息

	void	SearchModel();
//	bool	UnZipModel(CString strZipPath);
//	pMODEL	LoadModelFile(CString strModelPath);			//加载模板文件

	void	ShowRectByPoint(cv::Point pt, pST_PaperInfo pPaper);
	LRESULT RoiLBtnDown(WPARAM wParam, LPARAM lParam);		//鼠标左键按下的通知
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
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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
