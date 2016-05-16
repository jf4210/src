
// ScanToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ScanTool.h"
#include "ScanToolDlg.h"
#include "afxdialogex.h"
#include "LoginDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
using namespace cv;

float				g_fSamePercent = 0.75;		//�ж�У�������Ƿ���ͼ�ٷֱ�
int					g_nExitFlag = 0;
CString g_strCurrentPath;
std::string			g_strPaperSavePath;	//�Ծ�ɨ��󱣴����·��
std::string			g_strModelSavePath;
Poco::Logger*		g_pLogger;
Poco::FastMutex		g_fmRecog;		//ʶ���̻߳�ȡ������
RECOGTASKLIST		g_lRecogTask;	//ʶ�������б�

Poco::FastMutex		g_fmPapers;		//�����Ծ���б��������
PAPERS_LIST			g_lPapers;		//���е��Ծ����Ϣ

Poco::FastMutex		g_fmSendLock;	//�ϴ��ļ��б����
SENDTASKLIST		g_lSendTask;	//�ϴ��ļ������б�
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CScanToolDlg �Ի���



CScanToolDlg::CScanToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScanToolDlg::IDD, pParent)
	, m_pModel(NULL), m_ncomboCurrentSel(0), m_pRecogThread(NULL), m_pCurrentPicShow(NULL), m_nModelPicNums(1)
	, m_bTwainInit(FALSE), m_nCurrTabSel(0), m_nScanCount(0), m_nScanStatus(0)
	, m_pPapersInfo(NULL), m_pPaper(NULL), m_colorStatus(RGB(0, 0, 255)), m_nStatusSize(35), m_pCurrentShowPaper(NULL)
	, m_pSendFileObj(NULL), m_SendFileThread(NULL), m_bLogin(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CScanToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_Picture, m_lcPicture);
	DDX_Control(pDX, IDC_LIST_Paper, m_lcPaper);
	DDX_Control(pDX, IDC_COMBO_Model, m_comboModel);
	DDX_Control(pDX, IDC_TAB_PicShow, m_tabPicShowCtrl);
}

BEGIN_MESSAGE_MAP(CScanToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_Scan, &CScanToolDlg::OnBnClickedBtnScan)
	ON_BN_CLICKED(IDC_BTN_ScanModule, &CScanToolDlg::OnBnClickedBtnScanmodule)
	ON_CBN_SELCHANGE(IDC_COMBO_Model, &CScanToolDlg::OnCbnSelchangeComboModel)
	ON_BN_CLICKED(IDC_BTN_InputPaper, &CScanToolDlg::OnBnClickedBtnInputpaper)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_PicShow, &CScanToolDlg::OnTcnSelchangeTabPicshow)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_Picture, &CScanToolDlg::OnNMDblclkListPicture)
	ON_MESSAGE(MSG_ERR_RECOG, &CScanToolDlg::MsgRecogErr)
	ON_MESSAGE(WM_CV_LBTNDOWN, &CScanToolDlg::RoiLBtnDown)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_UpLoadPapers, &CScanToolDlg::OnBnClickedBtnUploadpapers)
	ON_BN_CLICKED(IDC_BTN_Login, &CScanToolDlg::OnBnClickedBtnLogin)
END_MESSAGE_MAP()


// CScanToolDlg ��Ϣ�������

BOOL CScanToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	USES_CONVERSION;
	CString strTitle = _T("");
	strTitle.Format(_T("YKLX-ScanTool %s"), SOFT_VERSION);
	SetWindowText(strTitle);

	InitUI();
	InitConfig();

// 	Poco::LocalDateTime now;
// 	char szTime[50] = { 0 };
// 	sprintf_s(szTime, "%d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
// 	TRACE(szTime);

// 	CLoginDlg dlg(A2T(m_strCmdServerIP.c_str()), m_nCmdPort);
// 	if (dlg.DoModal() != IDOK)
// 		m_bLogin = FALSE;
// 	else 
// 		m_bLogin = TRUE;

	SearchModel();         
	m_comboModel.SetCurSel(0);
	if (m_comboModel.GetCount())
	{
		m_ncomboCurrentSel = m_comboModel.GetCurSel();
		CString strModelName;
		m_comboModel.GetLBText(m_comboModel.GetCurSel(), strModelName);
		CString strModelPath = g_strCurrentPath + _T("Model\\") + strModelName;
		CString strModelFullPath = strModelPath + _T(".zip");
		UnZipFile(strModelFullPath);
		m_pModel = LoadModelFile(strModelPath);

		if (m_pModel != NULL)
			m_nModelPicNums = m_pModel->nPicNum;
		InitTab();
	}

	// ����TWAIN ��ʼ��ɨ������
	ReleaseTwain();
	m_bTwainInit = FALSE;
	if (!m_bTwainInit)
	{
		m_bTwainInit = InitTwain(m_hWnd);
		if (!IsValidDriver())
		{
			AfxMessageBox(_T("Unable to load Twain Driver."));
		}
// 		if (CallTwainProc(&m_AppId, NULL, DG_CONTROL, DAT_IDENTITY, MSG_USERSELECT, &m_Source))
// 		{
// 			//AfxMessageBox("CallTwainProc true");
// 			m_bSourceSelected = TRUE;
// 		}

//			SelectDefaultSource();

			ScanSrcInit();
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CScanToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CScanToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CScanToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CScanToolDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	g_nExitFlag = 1;
	ReleaseTwain();

	std::vector<CPicShow*>::iterator itPic = m_vecPicShow.begin();
	for (; itPic != m_vecPicShow.end();)
	{
		CPicShow* pModelPicShow = *itPic;
		SAFE_RELEASE(pModelPicShow);
		itPic = m_vecPicShow.erase(itPic);
	}

	if (m_pModel)
	{
		delete m_pModel;
		m_pModel = NULL;
	}
	MODELLIST::iterator it = m_lModel.begin();
	for (; it != m_lModel.end();)
	{
		pMODEL pModel = *it;
		SAFE_RELEASE(pModel);
		it = m_lModel.erase(it);
	}

	g_fmRecog.lock();			//�ͷ�δ�������ʶ�������б�
	RECOGTASKLIST::iterator itRecog = g_lRecogTask.begin();
	for (; itRecog != g_lRecogTask.end();)
	{
		pRECOGTASK pRecogTask = *itRecog;
		SAFE_RELEASE(pRecogTask);
		itRecog = g_lRecogTask.erase(itRecog);
	}
	g_fmRecog.unlock();

	for (int i = 0; i < m_vecRecogThreadObj.size(); i++)
	{
		m_pRecogThread[i].join();
	}
	std::vector<CRecognizeThread*>::iterator itRecogObj = m_vecRecogThreadObj.begin();
	for (; itRecogObj != m_vecRecogThreadObj.end();)
	{
		CRecognizeThread* pObj = *itRecogObj;
		SAFE_RELEASE(pObj);
		itRecogObj = m_vecRecogThreadObj.erase(itRecogObj);
	}
	if (m_pRecogThread)
	{
		delete[] m_pRecogThread;
		m_pRecogThread = NULL;
	}
	m_SendFileThread->join();
	SAFE_RELEASE(m_pSendFileObj);
	SAFE_RELEASE(m_SendFileThread);
	

	g_fmPapers.lock();			//�ͷ��Ծ���б�
	PAPERS_LIST::iterator itPapers = g_lPapers.begin();
	for (; itPapers != g_lPapers.end();)
	{
		pPAPERSINFO pPapersTask = *itPapers;
		SAFE_RELEASE(pPapersTask);
		itPapers = g_lPapers.erase(itPapers);
	}
	g_fmPapers.unlock();

	SAFE_RELEASE(m_pPapersInfo);
}

void CScanToolDlg::InitConfig()
{
	wchar_t szwPath[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, szwPath, MAX_PATH);
	char szPath[MAX_PATH] = { 0 };
	int nLen = WideCharToMultiByte(CP_ACP, 0, szwPath, -1, NULL, 0, NULL, NULL);
	char* pszDst = new char[nLen];
	WideCharToMultiByte(CP_ACP, 0, szwPath, -1, szPath, nLen, NULL, NULL);
	szPath[nLen - 1] = 0;
	delete[] pszDst;

	CString strFile(szPath);
	strFile = strFile.Left(strFile.ReverseFind('\\') + 1);
	g_strCurrentPath = strFile;
	
	USES_CONVERSION;

	std::string strLogPath = T2A(g_strCurrentPath + _T("ScanTool.log"));
	Poco::AutoPtr<Poco::PatternFormatter> pFormatter(new Poco::PatternFormatter("%L%Y-%m-%d %H:%M:%S.%F %q:%t"));
	Poco::AutoPtr<Poco::FormattingChannel> pFCFile(new Poco::FormattingChannel(pFormatter));
	Poco::AutoPtr<Poco::FileChannel> pFileChannel(new Poco::FileChannel(strLogPath));
	pFCFile->setChannel(pFileChannel);
	pFCFile->open();
	pFCFile->setProperty("rotation", "1 M");
	pFCFile->setProperty("archive", "timestamp");
	pFCFile->setProperty("compress", "true");
	pFCFile->setProperty("purgeCount", "5");
	Poco::Logger& appLogger = Poco::Logger::create("ScanTool", pFCFile, Poco::Message::PRIO_INFORMATION);
	g_pLogger = &appLogger;

	g_strPaperSavePath = T2A(g_strCurrentPath + _T("Paper\\"));	//���ɨ���Ծ��·��
	Poco::File filePaperPath(g_strPaperSavePath);
	filePaperPath.createDirectories();

	strFile.Append(_T("config.ini"));
	Poco::AutoPtr<Poco::Util::IniFileConfiguration> pConf(new Poco::Util::IniFileConfiguration(T2A(strFile)));
	int nRecogThreads = pConf->getInt("Recog.threads", 2);
	std::string strFileServerIP	= pConf->getString("Server.fileIP");
	int			nFileServerPort	= pConf->getInt("Server.filePort", 19980);
	m_strCmdServerIP				= pConf->getString("Server.cmdIP");
	m_nCmdPort						= pConf->getInt("Server.cmdPort", 19980);

	m_pRecogThread = new Poco::Thread[nRecogThreads];
	for (int i = 0; i < nRecogThreads; i++)
	{
		CRecognizeThread* pRecogObj = new CRecognizeThread;
		m_pRecogThread[i].start(*pRecogObj);

		m_vecRecogThreadObj.push_back(pRecogObj);
	}
	m_SendFileThread = new Poco::Thread;
	m_pSendFileObj = new CSendFileThread(strFileServerIP, nFileServerPort);
	m_SendFileThread->start(*m_pSendFileObj);
}

void CScanToolDlg::InitTab()
{
	if (m_pModel)
	{
		std::vector<CPicShow*>::iterator itPic = m_vecPicShow.begin();
		for (; itPic != m_vecPicShow.end();)
		{
			CPicShow* pModelPicShow = *itPic;
			if (pModelPicShow)
			{
				delete pModelPicShow;
				pModelPicShow = NULL;
			}
			itPic = m_vecPicShow.erase(itPic);
		}
	}
	m_tabPicShowCtrl.DeleteAllItems();

	USES_CONVERSION;
	CRect rtTab;
	m_tabPicShowCtrl.GetClientRect(&rtTab);
	for (int i = 0; i < m_nModelPicNums; i++)
	{
		char szTabHeadName[20] = { 0 };
		sprintf_s(szTabHeadName, "��%dҳ", i + 1);

		m_tabPicShowCtrl.InsertItem(i, A2T(szTabHeadName));

		CPicShow* pPicShow = new CPicShow(this);
		pPicShow->Create(CPicShow::IDD, &m_tabPicShowCtrl);
		pPicShow->ShowWindow(SW_HIDE);
		pPicShow->MoveWindow(&rtTab);
		m_vecPicShow.push_back(pPicShow);
	}
	m_tabPicShowCtrl.SetCurSel(0);
	if (m_vecPicShow.size())
	{
		m_vecPicShow[0]->ShowWindow(SW_SHOW);
		m_pCurrentPicShow = m_vecPicShow[0];
	}	

	if (m_tabPicShowCtrl.GetSafeHwnd())
	{
		CRect rtTab;
		m_tabPicShowCtrl.GetClientRect(&rtTab);
		int nTabHead_H = 25;		//tab�ؼ�ͷ�ĸ߶�
		CRect rtPic = rtTab;
		rtPic.top = rtPic.top + nTabHead_H;
		for (int i = 0; i < m_vecPicShow.size(); i++)
			m_vecPicShow[i]->MoveWindow(&rtPic);
	}
}

void CScanToolDlg::InitUI()
{
	m_lcPicture.SetExtendedStyle(m_lcPicture.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_SHOWSELALWAYS);
	m_lcPicture.InsertColumn(0, _T("���"), LVCFMT_CENTER, 40);
	m_lcPicture.InsertColumn(1, _T("ѧ����Ϣ"), LVCFMT_CENTER, 100);

	m_lcPaper.SetExtendedStyle(m_lcPaper.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_SHOWSELALWAYS);
	m_lcPaper.InsertColumn(0, _T("�Ծ���"), LVCFMT_CENTER, 80);
	m_lcPaper.InsertColumn(1, _T("�ϴ�״̬"), LVCFMT_CENTER, 70);

	SetFontSize(m_nStatusSize);

	USES_CONVERSION;
	CRect rtTab;
	m_tabPicShowCtrl.GetClientRect(&rtTab);
	for (int i = 0; i < m_nModelPicNums; i++)
	{
		char szTabHeadName[20] = { 0 };
		sprintf_s(szTabHeadName, "��%dҳ", i + 1);

		m_tabPicShowCtrl.InsertItem(i, A2T(szTabHeadName));

		CPicShow* pPicShow = new CPicShow(this);
		pPicShow->Create(CPicShow::IDD, &m_tabPicShowCtrl);
		pPicShow->ShowWindow(SW_HIDE);
		pPicShow->MoveWindow(&rtTab);
		m_vecPicShow.push_back(pPicShow);
	}
	m_tabPicShowCtrl.SetCurSel(0);
	m_vecPicShow[0]->ShowWindow(SW_SHOW);
	m_pCurrentPicShow = m_vecPicShow[0];
	m_nCurrTabSel = 0;

// 	int sx = GetSystemMetrics(SM_CXFULLSCREEN);
// 	int sy = GetSystemMetrics(SM_CYFULLSCREEN);
#if 1
	CRect rc;
	::SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
	int sx = rc.Width();
	int sy = rc.Height();
#else
	int sx = MAX_DLG_WIDTH;
	int sy = MAX_DLG_HEIGHT;
#endif
	MoveWindow(0, 0, sx, sy);

	CenterWindow();
	InitCtrlPosition();
}

void CScanToolDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	InitCtrlPosition();
}

HBRUSH CScanToolDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);

	UINT CurID = pWnd->GetDlgCtrlID();

	if (IDC_STATIC_STATUS == CurID)
	{
		pDC->SetTextColor(m_colorStatus);
// 		HBRUSH hbrsh = (HBRUSH)GetStockObject(NULL_BRUSH);
// 		pDC->SetBkMode(TRANSPARENT);

		return hbr;	// hbrsh;
	}
	return hbr;
}

void CScanToolDlg::SetFontSize(int nSize)
{
	m_fontStatus.DeleteObject();
	m_fontStatus.CreateFont(nSize, 0, 0, 0,
		FW_BOLD, FALSE, FALSE, 0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_SWISS,
		_T("Arial"));
	GetDlgItem(IDC_STATIC_STATUS)->SetFont(&m_fontStatus);
	GetDlgItem(IDC_STATIC_SCANCOUNT)->SetFont(&m_fontStatus);
}

void CScanToolDlg::InitCtrlPosition()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	int cx = rcClient.right;
	int cy = rcClient.bottom;

	int nTopGap = cy / 10;	//�ϱߵļ��������������
	if (nTopGap > 90)
		nTopGap = 90;
	else if (nTopGap < 50)
		nTopGap = 50;

	const int nLeftGap = 2;		//��ߵĿհ׼��
	const int nBottomGap = 2;	//�±ߵĿհ׼��
	const int nRightGap = 2;	//�ұߵĿհ׼��
	const int nGap = 2;			//��ͨ�ؼ��ļ��

	int nListCtrlWidth = 200;	//ͼƬ�б�ؼ����
	int nStaticTip = 15;		//�б���ʾstatic�ؼ��߶�
	int nStatusHeight = nStaticTip * 2;		//�ײ�״̬��ʾ���ĸ߶�
	int nComboBoxHeith = 25;	//ģ�������б�ؼ��ĸ߶�
	int nPaperListHeigth = cy / 3;	//�Ծ���б�Ŀؼ��߶�
	if (nPaperListHeigth > 300)
		nPaperListHeigth = 300;
	int nPicListHeight = cy - nTopGap - nStaticTip - nGap - nComboBoxHeith - nGap - nStaticTip - nGap - nGap - nStaticTip - nGap - nPaperListHeigth - nGap - nStatusHeight - nBottomGap;		//ͼƬ�б�ؼ��߶�

	int nCurrentTop = 0;
	if (GetDlgItem(IDC_STATIC_PicList)->GetSafeHwnd())
	{
		GetDlgItem(IDC_STATIC_Model)->MoveWindow(nLeftGap, nTopGap, nListCtrlWidth, nStaticTip);
		nCurrentTop = nTopGap + nStaticTip + nGap;
	}
	if (m_comboModel.GetSafeHwnd())
	{
		m_comboModel.MoveWindow(nLeftGap, nCurrentTop, nListCtrlWidth, nComboBoxHeith);
		nCurrentTop = nCurrentTop + nComboBoxHeith + nGap;
	}
	if (GetDlgItem(IDC_STATIC_PicList)->GetSafeHwnd())
	{
		GetDlgItem(IDC_STATIC_PicList)->MoveWindow(nLeftGap, nCurrentTop, nListCtrlWidth, nStaticTip);
		nCurrentTop = nCurrentTop + nStaticTip + nGap;
	}
	if (m_lcPicture.GetSafeHwnd())
	{
		m_lcPicture.MoveWindow(nLeftGap, nCurrentTop, nListCtrlWidth, nPicListHeight);
		nCurrentTop = nCurrentTop + nPicListHeight + nGap;
	}
	if (GetDlgItem(IDC_STATIC_PicList)->GetSafeHwnd())
	{
		GetDlgItem(IDC_STATIC_PaperList)->MoveWindow(nLeftGap, nCurrentTop, nListCtrlWidth, nStaticTip);
		nCurrentTop = nCurrentTop + nStaticTip + nGap;
	}
	if (m_lcPaper.GetSafeHwnd())
	{
		m_lcPaper.MoveWindow(nLeftGap, nCurrentTop, nListCtrlWidth, nPaperListHeigth);
		nCurrentTop = nCurrentTop + nPaperListHeigth + nGap;
	}
	if (GetDlgItem(IDC_STATIC_SCANCOUNT)->GetSafeHwnd())
	{
		GetDlgItem(IDC_STATIC_SCANCOUNT)->MoveWindow(nLeftGap, nCurrentTop, 150, nStatusHeight);
	}
	if (GetDlgItem(IDC_STATIC_STATUS)->GetSafeHwnd())
	{
		GetDlgItem(IDC_STATIC_STATUS)->MoveWindow(nLeftGap + 150, nCurrentTop, cx - nLeftGap - nRightGap, nStatusHeight);
	}

	int nPicShowTabCtrlWidth = cx - nLeftGap - nRightGap - nListCtrlWidth - nGap - nGap;
	if (m_tabPicShowCtrl.GetSafeHwnd())
	{
		m_tabPicShowCtrl.MoveWindow(nLeftGap + nListCtrlWidth + nGap, nTopGap, nPicShowTabCtrlWidth, cy - nTopGap - nGap - nStatusHeight - nBottomGap);

		CRect rtTab;
		m_tabPicShowCtrl.GetClientRect(&rtTab);
		int nTabHead_H = 25;		//tab�ؼ�ͷ�ĸ߶�
		CRect rtPic = rtTab;
		rtPic.top = rtPic.top + nTabHead_H;
		for (int i = 0; i < m_vecPicShow.size(); i++)
			m_vecPicShow[i]->MoveWindow(&rtPic);
	}

	//��������ťλ��
	int nBtnWidth = nTopGap - nGap - nGap;
	int nBtnCurrLeft = nLeftGap;
	if (GetDlgItem(IDC_BTN_Login)->GetSafeHwnd())
	{
		GetDlgItem(IDC_BTN_Login)->MoveWindow(nBtnCurrLeft, nGap, nBtnWidth, nTopGap - nGap - nGap);
		nBtnCurrLeft = nBtnCurrLeft + nBtnWidth + nGap;
	}
	if (GetDlgItem(IDC_BTN_Scan)->GetSafeHwnd())
	{
		GetDlgItem(IDC_BTN_Scan)->MoveWindow(nBtnCurrLeft, nGap, nBtnWidth, nTopGap - nGap - nGap);
		nBtnCurrLeft = nBtnCurrLeft + nBtnWidth + nGap;
	}
	if (GetDlgItem(IDC_BTN_ScanModule)->GetSafeHwnd())
	{
		GetDlgItem(IDC_BTN_ScanModule)->MoveWindow(nBtnCurrLeft, nGap, nBtnWidth, nTopGap - nGap - nGap);
		nBtnCurrLeft = nBtnCurrLeft + nBtnWidth + nGap;
	}
	if (GetDlgItem(IDC_BTN_InputPaper)->GetSafeHwnd())
	{
		GetDlgItem(IDC_BTN_InputPaper)->MoveWindow(nBtnCurrLeft, nGap, nBtnWidth, nTopGap - nGap - nGap);
		nBtnCurrLeft = nBtnCurrLeft + nBtnWidth + nGap;
	}
	if (GetDlgItem(IDC_BTN_UpLoadPapers)->GetSafeHwnd())
	{
		GetDlgItem(IDC_BTN_UpLoadPapers)->MoveWindow(nBtnCurrLeft, nGap, nBtnWidth, nTopGap - nGap - nGap);
	}
}

bool CScanToolDlg::ScanSrcInit()
{
	USES_CONVERSION;
	if (CallTwainProc(&m_AppId, NULL, DG_CONTROL, DAT_IDENTITY, MSG_GETFIRST, &m_Source))
	{
		TW_IDENTITY temp_Source = m_Source;
		m_scanSourceArry.Add(temp_Source);
		while (CallTwainProc(&m_AppId, NULL, DG_CONTROL, DAT_IDENTITY, MSG_GETNEXT, &m_Source))
		{
			TW_IDENTITY temp_Source = m_Source;
			m_scanSourceArry.Add(temp_Source);
		}
		m_bSourceSelected = TRUE;
	}
	else
	{
		m_bSourceSelected = FALSE;
	}
	return m_bSourceSelected;
}

void CScanToolDlg::OnTcnSelchangeTabPicshow(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	int nIndex = m_tabPicShowCtrl.GetCurSel();
	m_nCurrTabSel = nIndex;
	m_pCurrentPicShow = m_vecPicShow[nIndex];
	m_pCurrentPicShow->ShowWindow(SW_SHOW);
	for (int i = 0; i < m_vecPicShow.size(); i++)
	{
		if (i != nIndex)
			m_vecPicShow[i]->ShowWindow(SW_HIDE);
	}
}
void CScanToolDlg::OnBnClickedBtnLogin()
{
	USES_CONVERSION;
	if (!m_bLogin)
	{
		CLoginDlg dlg(A2T(m_strCmdServerIP.c_str()), m_nCmdPort);
		if (dlg.DoModal() != IDOK)
		{
			m_bLogin = FALSE;
			m_strUserName = _T("");
			m_strPwd = _T("");
			GetDlgItem(IDC_BTN_Login)->SetWindowTextW(_T("��¼"));
		}
		else
		{
			m_bLogin = TRUE;
			m_strUserName = dlg.m_strUserName;
			m_strPwd = dlg.m_strPwd;
			GetDlgItem(IDC_BTN_Login)->SetWindowTextW(_T("�˳�"));
		}
	}
	else
	{
		m_bLogin = FALSE;
		m_strUserName = _T("");
		m_strPwd = _T("");
		GetDlgItem(IDC_BTN_Login)->SetWindowTextW(_T("��¼"));
	}	
}
void CScanToolDlg::OnBnClickedBtnScan()
{
	if (!m_bTwainInit)
		return;

	if (m_nScanStatus == 1)	//ɨ���У����ܲ���
		return;

	CScanCtrlDlg dlg(m_scanSourceArry);
	if (dlg.DoModal() != IDOK)
		return;

	m_comboModel.EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_Scan)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_ScanModule)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_InputPaper)->EnableWindow(FALSE);
	GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(_T(""));

	Poco::Random rnd;
	rnd.seed();
	USES_CONVERSION;
	char szPicTmpPath[MAX_PATH] = { 0 };
#if 1
	if (m_nScanStatus == 2)		//ɨ���쳣������ɾ��ǰһ�ݵ��Ծ����Ϣ
	{
		SAFE_RELEASE(m_pPapersInfo);
	}
	sprintf_s(szPicTmpPath, "%sPaper\\Tmp", T2A(g_strCurrentPath));
	if (!m_pPapersInfo)
	{
		m_lcPicture.DeleteAllItems();
		GetDlgItem(IDC_STATIC_SCANCOUNT)->SetWindowText(_T(""));

		Poco::File tmpPath(szPicTmpPath);
		if (tmpPath.exists())
			tmpPath.remove(true);

		Poco::File tmpPath1(szPicTmpPath);
		tmpPath1.createDirectories();

		m_strCurrPicSavePath = szPicTmpPath;
		m_pPapersInfo = new PAPERSINFO();
		m_nScanCount = 0;					//��ɨ��������0
	}
	m_nScanStatus = 1;
#else
	sprintf_s(szPicTmpPath, "%sPaper\\Tmp%05d", T2A(g_strCurrentPath), rnd.next(99999));

	Poco::File tmpPath(szPicTmpPath);
	tmpPath.createDirectories();

	m_strCurrPicSavePath = szPicTmpPath;
	m_pPapersInfo = new PAPERSINFO();
// 	g_fmPapers.lock();
// 	g_lPapers.push_back(m_pPapersInfo);
// 	g_fmPapers.unlock();
#endif

	m_Source = m_scanSourceArry.GetAt(dlg.m_nCurrScanSrc);
	int nDuplex = dlg.m_nCurrDuplex;		//��˫��,0-����,1-˫��
	int nSize = 1;							//1-A4
	int nPixel = 2;							//0-�ڰף�1-�Ҷȣ�2-��ɫ
	int nResolution = 200;					//dpi: 72, 150, 200, 300

	int nNum = dlg.m_nStudentNum;

	if (nDuplex == 1)
		nNum *= 2;

	if (nNum == 0)
		nNum = TWCPP_ANYCOUNT;
	if (!Acquire(nNum, nDuplex, nSize, nPixel, nResolution))	//TWCPP_ANYCOUNT
	{
		m_comboModel.EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_Scan)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_ScanModule)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_InputPaper)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(_T(""));
		m_nScanStatus = 2;
	}
}

void CScanToolDlg::OnBnClickedBtnScanmodule()
{
	int nCoutn = m_comboModel.GetCount();
	if (nCoutn == 0)
		m_pModel = NULL;

	CMakeModelDlg dlg(m_pModel);
	if (dlg.DoModal() != IDOK)
		return;

	if (!m_pModel)	//���ģ�岻Ϊ�գ�˵��֮ǰ�Ѿ���ģ���ˣ�����Ҫʹ����ģ��
		m_pModel = dlg.m_pModel;

	if (m_pModel)
	{
		m_comboModel.ResetContent();
		int n = m_comboModel.GetCount();
		SearchModel();
		for (int i = 0; i < m_comboModel.GetCount(); i++)
		{
			CString strItemName;
			m_comboModel.GetLBText(i, strItemName);
			if (strItemName == m_pModel->strModelName)
			{
				m_comboModel.SetCurSel(i);
				m_ncomboCurrentSel = i;
				break;
			}
		}

		m_nModelPicNums = m_pModel->nPicNum;
	}
	
	InitTab();
}

void CScanToolDlg::SearchModel()
{
	USES_CONVERSION;
	std::string strModelPath = T2A(g_strCurrentPath + _T("Model"));
	g_strModelSavePath = strModelPath;

	std::string strLog;
	try
	{
		Poco::DirectoryIterator it(strModelPath);
		Poco::DirectoryIterator end;
		while (it != end)
		{
			Poco::Path p(it->path());
			if (it->isFile() && p.getExtension() == "zip")
			{
//				std::string strModelName = p.getBaseName();
				std::string strModelName = CMyCodeConvert::Utf8ToGb2312(p.getBaseName());
				m_comboModel.AddString(A2T(strModelName.c_str()));
			}
			it++;
		}
//		m_comboModel.SetCurSel(0);
	}
	catch (Poco::FileException& exc)
	{
		strLog = "����ģ��ʧ��: " + exc.displayText();
	}
	catch (Poco::Exception& exc)
	{
		strLog = "����ģ��ʧ��2: " + exc.displayText();
	}
	g_pLogger->information(strLog);
}

void CScanToolDlg::OnCbnSelchangeComboModel()
{
	if (m_ncomboCurrentSel == m_comboModel.GetCurSel())
		return;

	CString strModelName;
	m_comboModel.GetLBText(m_comboModel.GetCurSel(), strModelName);
	CString strModelPath = g_strCurrentPath + _T("Model\\") + strModelName;
	CString strModelFullPath = strModelPath + _T(".zip");
	UnZipFile(strModelFullPath);
	if (m_pModel)
	{
		delete m_pModel;
		m_pModel = NULL;
	}
	m_pModel = LoadModelFile(strModelPath);
	if (!m_pModel)
		return;
	m_ncomboCurrentSel = m_comboModel.GetCurSel();

	m_nModelPicNums = m_pModel->nPicNum;
	InitTab();

	//ģ���л���֮ǰ��ɨ���Ծ���Ҫ���
	m_pCurrentShowPaper = NULL;
	m_lcPicture.DeleteAllItems();
	SAFE_RELEASE(m_pPapersInfo);
}

void CScanToolDlg::OnBnClickedBtnInputpaper()
{
	CPaperInputDlg dlg(m_pModel);
	if (dlg.DoModal() != IDOK)
		return;
}


void CopyData(char *dest, const char *src, int dataByteSize, bool isConvert, int height) 
{
	char * p = dest;
	if (!isConvert) 
	{
		memcpy(dest, src, dataByteSize);
		return;
	}
	if (height <= 0) return;
	//int height = dataByteSize/rowByteSize;
	int rowByteSize = dataByteSize / height;
	src = src + dataByteSize - rowByteSize;
	for (int i = 0; i < height; i++) 
	{
		memcpy(dest, src, rowByteSize);
		dest += rowByteSize;
		src -= rowByteSize;
	}
}

IplImage* CBitmap2IplImage(CBitmap* pBitmap)
{
	HBITMAP   hBmp = (HBITMAP)(*pBitmap);//
	BITMAP bmp;
	::GetBitmapBits(hBmp, sizeof(BITMAP), &bmp);


	int nChannels = 1;
	int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	IplImage* pImg = cvCreateImage(cvSize(bmp.bmWidth, bmp.bmHeight), depth, nChannels);

//	pImg->origin = IPL_ORIGIN_TL;

	BYTE *pBuffer = new BYTE[bmp.bmHeight*bmp.bmWidth*nChannels];//���������� 
	GetBitmapBits(hBmp, bmp.bmHeight*bmp.bmWidth*nChannels, pBuffer);//��λͼ��Ϣ���Ƶ�������
	memcpy(pImg->imageData, pBuffer, bmp.bmHeight*bmp.bmWidth*nChannels);//����������Ϣ���Ƹ�IplImage
	delete[] pBuffer;
	return pImg;
}

IplImage* DIB2IplImage(CDIB& m_dib)
{
	/////////////////////////////////////////////////////////// DIB2CBitmap
// 	unsigned char *pBits = (unsigned char *)malloc(m_dib.height*m_dib.bytes);
// 	memcpy(pBits, m_dib.m_pBits, m_dib.height*m_dib.bytes);
	CBitmap* bitMap = new CBitmap;
	bitMap->CreateBitmap(m_dib.width, m_dib.height, 1, m_dib.m_nBits, m_dib.m_pVoid);	//m_dib.m_pVoid
//	free(pBits);
	//////////////////////////////////////////////////////
	IplImage* img = CBitmap2IplImage(bitMap);
	delete[] bitMap;

//	free(pBits2);

	return img;
}

IplImage* CreateImgFormData(BYTE* pImgData, int nWidth, int nHeight, int nChannel)
{
	IplImage *pImg = cvCreateImage(cvSize(nWidth, nHeight), 8, nChannel);
	if (!pImg)
	{
		return NULL;
	}

	for (int h = 0; h < nHeight; h++)
	{
		BYTE* pRowData = pImgData + nWidth * nChannel * (nHeight - 1 - h); //���ͼ������µߵ��� ��(nHeight-1-h) ��Ϊ h �Ϳ��� 

		memcpy(pImg->imageData + pImg->widthStep * nChannel * h, pRowData, nWidth);
	}

	return pImg;
}


void CScanToolDlg::CopyImage(HANDLE hBitmap, TW_IMAGEINFO& info)
{
	if (_bTwainContinue == FALSE)	//����Ѿ�������ɨ��ֹͣ��ʶ���򲻽��е�ǰͼ��Ĵ���
		return;

	SetImage(hBitmap, info.BitsPerPixel);
}

void CScanToolDlg::SetImage(HANDLE hBitmap, int bits)
{
	clock_t start, end;
	start = clock();

	CDIB dib;
	dib.CreateFromHandle(hBitmap, bits);

	BITMAPFILEHEADER bFile;
	::ZeroMemory(&bFile, sizeof(bFile));
	memcpy((void *)&bFile.bfType, "BM", 2);
	bFile.bfSize = dib.GetDIBSize() + sizeof(bFile);
	bFile.bfOffBits = sizeof(BITMAPINFOHEADER) + dib.GetPaletteSize()*sizeof(RGBQUAD) + sizeof(BITMAPFILEHEADER);
	unsigned char *pBits = (unsigned char *)malloc(bFile.bfSize);
	memcpy(pBits, &bFile, sizeof(BITMAPFILEHEADER));
	memcpy(pBits + sizeof(BITMAPFILEHEADER), dib.m_pVoid, dib.GetDIBSize());

	BYTE *p = pBits;
	BITMAPFILEHEADER fheader;
	memcpy(&fheader, p, sizeof(BITMAPFILEHEADER));
	BITMAPINFOHEADER bmphdr;
	p += sizeof(BITMAPFILEHEADER);
	memcpy(&bmphdr, p, sizeof(BITMAPINFOHEADER));
	int w = bmphdr.biWidth;
	int h = bmphdr.biHeight;
	p = pBits + fheader.bfOffBits;

	int nChannel = (bmphdr.biBitCount == 1) ? 1 : bmphdr.biBitCount / 8;
	int depth = (bmphdr.biBitCount == 1) ? IPL_DEPTH_1U : IPL_DEPTH_8U;
	IplImage *pIpl2 = cvCreateImage(cvSize(w, h), depth, nChannel);

	int height;
	bool isLowerLeft = bmphdr.biHeight > 0;
	height = (bmphdr.biHeight > 0) ? bmphdr.biHeight : -bmphdr.biHeight;
	CopyData(pIpl2->imageData, (char*)p, bmphdr.biSizeImage, isLowerLeft, height);
	free(pBits);
	pBits = NULL;

// 	IplImage* pIpl = DIB2IplImage(dib);
// 	cv::Mat matTest = cv::cvarrToMat(pIpl);
	USES_CONVERSION;
	
	int nStudentId = m_nScanCount / m_nModelPicNums + 1;
	int nOrder = m_nScanCount % m_nModelPicNums + 1;

	char szPicName[50] = { 0 };
	char szPicPath[MAX_PATH] = { 0 };
	sprintf_s(szPicName, "S%d_%d.jpg", nStudentId, nOrder);
	sprintf_s(szPicPath, "%s\\S%d_%d.jpg", m_strCurrPicSavePath.c_str(), nStudentId, nOrder);

	m_nScanCount++;

	cv::Mat matTest2 = cv::cvarrToMat(pIpl2);

	m_pCurrentPicShow->ShowPic(matTest2);
	m_pCurrentShowPaper = m_pPaper;

	std::string strPicName = szPicPath;
	imwrite(strPicName, matTest2);

	cvReleaseImage(&pIpl2);

	pST_PicInfo pPic = new ST_PicInfo;
	pPic->strPicName = szPicName;
	pPic->strPicPath = szPicPath;
	//�Ծ��б���ʾɨ���Ծ�
	if (nOrder == 1)	//��һҳ��ʱ�򴴽��µ��Ծ���Ϣ
	{
		char szStudentName[30] = { 0 };
		sprintf_s(szStudentName, "S%d", nStudentId);
		m_pPaper = new ST_PaperInfo;
		m_pPaper->strStudentInfo = szStudentName;
		m_pPaper->pModel = m_pModel;
		m_pPaper->pPapers = m_pPapersInfo;
		m_pPaper->pSrcDlg = this;
		m_pPaper->lPic.push_back(pPic);

		m_pPapersInfo->fmlPaper.lock();
		m_pPapersInfo->lPaper.push_back(m_pPaper);
		m_pPapersInfo->fmlPaper.unlock();
		
		//��ӽ��Ծ��б�ؼ�
		int nCount = m_lcPicture.GetItemCount();
		char szCount[10] = { 0 };
		sprintf_s(szCount, "%d", nCount + 1);
		m_lcPicture.InsertItem(nCount, NULL);
		m_lcPicture.SetItemText(nCount, 0, (LPCTSTR)A2T(szCount));
		m_lcPicture.SetItemText(nCount, 1, (LPCTSTR)A2T(szStudentName));
		m_lcPicture.SetItemData(nCount, (DWORD_PTR)m_pPaper);

		//��ӵ�ʶ�������б�
		if (m_pModel)
		{
			pRECOGTASK pTask = new RECOGTASK;
			pTask->pPaper = m_pPaper;
			g_lRecogTask.push_back(pTask);
		}
	}
	else
	{
		m_pPaper->lPic.push_back(pPic);
	}
	CString strMsg = _T("");
	strMsg.Format(_T("��ɨ��%d��"), m_nScanCount);
	GetDlgItem(IDC_STATIC_SCANCOUNT)->SetWindowText(strMsg);
}

void CScanToolDlg::ScanDone(int nStatus)
{
	m_pPapersInfo->nPaperCount = m_nScanCount / m_pModel->nPicNum;	//����ɨ���Ծ�����

	m_comboModel.EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_Scan)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_ScanModule)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_InputPaper)->EnableWindow(TRUE);

	bool bWarn = false;
	if (nStatus != 1)
		bWarn = true;

	USES_CONVERSION;
	CString strMsg = _T("");
	if (nStatus == 1)
	{
		strMsg = _T("ɨ�����");
		m_nScanStatus = 3;
	}
	else
	{
		strMsg = _T("ɨ���쳣����");
//		SAFE_RELEASE(m_pPapersInfo);
		m_nScanCount = 0;
		m_nScanStatus = 2;
	}
	SetStatusShowInfo(strMsg, bWarn);
}

void CScanToolDlg::SetStatusShowInfo(CString strMsg, BOOL bWarn /*= FALSE*/)
{
	if (bWarn)
		m_colorStatus = RGB(255, 0, 0);
	else
		m_colorStatus = RGB(0, 0, 255);
	GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(strMsg);
	TRACE("\n----------------\n");
	TRACE(strMsg);
	TRACE("\n----------------\n");
}

BOOL CScanToolDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_SYSCHAR)		//WM_SYSCHAR
	{
		switch (pMsg->wParam)
		{
		case 's':
		case 'S':
			OnBnClickedBtnScan();
			return TRUE;
		}
	}
	ProcessMessage(*pMsg);
//	return CDialogEx::PreTranslateMessage(pMsg);
	return CDialog::PreTranslateMessage(pMsg);
}

void CScanToolDlg::OnNMDblclkListPicture(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	pST_PaperInfo pPaper = (pST_PaperInfo)m_lcPicture.GetItemData(pNMItemActivate->iItem);
//	m_nCurrItemPaper = pNMItemActivate->iItem;

	m_pCurrentShowPaper = pPaper;
	if (pPaper->bIssuePaper)
		PaintIssueRect(pPaper);
	else
		PaintRecognisedRect(pPaper);

	m_tabPicShowCtrl.SetCurSel(0);
	m_pCurrentPicShow = m_vecPicShow[0];
	m_pCurrentPicShow->ShowWindow(SW_SHOW);
	for (int i = 0; i < m_vecPicShow.size(); i++)
	{
		if (i != 0)
			m_vecPicShow[i]->ShowWindow(SW_HIDE);
	}
}

void CScanToolDlg::PaintRecognisedRect(pST_PaperInfo pPaper)
{
	PIC_LIST::iterator itPic = pPaper->lPic.begin();
	for (int i = 0; itPic != pPaper->lPic.end(); itPic++, i++)
	{
		Mat matSrc = imread((*itPic)->strPicPath);
		Mat tmp = matSrc.clone();
		Mat tmp2 = matSrc.clone();
#if 1
		RECTLIST::iterator itSelRoi = pPaper->pModel->vecPaperModel[i].lSelROI.begin();													//��ʾʶ�𶨵��ѡ����
		for (int j = 0; itSelRoi != pPaper->pModel->vecPaperModel[i].lSelROI.end(); itSelRoi++, j++)
		{
			cv::Rect rt = (*itSelRoi).rt;
			//				GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);

			char szCP[20] = { 0 };
			// 				sprintf_s(szCP, "FIX%d", j);
			// 				putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 0));	//CV_FONT_HERSHEY_COMPLEX
			rectangle(tmp, rt, CV_RGB(0, 0, 255), 2);
			rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
		}

		RECTLIST::iterator itPicFix = (*itPic)->lFix.begin();														//��ʾʶ������Ķ���
		for (int j = 0; itPicFix != (*itPic)->lFix.end(); itPicFix++, j++)
		{
			cv::Rect rt = (*itPicFix).rt;
//			GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);

			char szCP[20] = { 0 };
			sprintf_s(szCP, "FIX%d", j);
			putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 0));	//CV_FONT_HERSHEY_COMPLEX
			rectangle(tmp, rt, CV_RGB(0, 255, 0), 2);
			rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
		}
		RECTLIST::iterator itFixRect = pPaper->pModel->vecPaperModel[i].lFix.begin();								//��ʾģ���ϵĶ����Ӧ�����Ծ��ϵ��¶���
		for (int j = 0; itFixRect != pPaper->pModel->vecPaperModel[i].lFix.end(); itFixRect++, j++)
		{
			cv::Rect rt = (*itFixRect).rt;
			GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);

			char szCP[20] = { 0 };
			sprintf_s(szCP, "FIX%d", j);
			putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
			rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
			rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
		}
		RECTLIST::iterator itHRect = pPaper->pModel->vecPaperModel[i].lH_Head.begin();
		for (int j = 0; itHRect != pPaper->pModel->vecPaperModel[i].lH_Head.end(); itHRect++, j++)
		{
			cv::Rect rt = (*itHRect).rt;
			GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);

			char szCP[20] = { 0 };
			sprintf_s(szCP, "H%d", j);
			putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
			rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
			rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
		}
		RECTLIST::iterator itVRect = pPaper->pModel->vecPaperModel[i].lV_Head.begin();
		for (int j = 0; itVRect != pPaper->pModel->vecPaperModel[i].lV_Head.end(); itVRect++, j++)
		{
			cv::Rect rt = (*itVRect).rt;
			GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);

			char szCP[20] = { 0 };
			sprintf_s(szCP, "V%d", j);
			putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
			rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
			rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
		}
		RECTLIST::iterator itABRect = pPaper->pModel->vecPaperModel[i].lABModel.begin();
		for (int j = 0; itABRect != pPaper->pModel->vecPaperModel[i].lABModel.end(); itABRect++, j++)
		{
			cv::Rect rt = (*itABRect).rt;
			GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);

			char szCP[20] = { 0 };
			sprintf_s(szCP, "AB%d", j);
			putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
			rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
			rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
		}
		RECTLIST::iterator itCourseRect = pPaper->pModel->vecPaperModel[i].lCourse.begin();
		for (int j = 0; itCourseRect != pPaper->pModel->vecPaperModel[i].lCourse.end(); itCourseRect++, j++)
		{
			cv::Rect rt = (*itCourseRect).rt;
			GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);

			char szCP[20] = { 0 };
			sprintf_s(szCP, "C%d", j);
			putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
			rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
			rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
		}
		RECTLIST::iterator itQKRect = pPaper->pModel->vecPaperModel[i].lQK_CP.begin();
		for (int j = 0; itQKRect != pPaper->pModel->vecPaperModel[i].lQK_CP.end(); itQKRect++, j++)
		{
			cv::Rect rt = (*itQKRect).rt;
			GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);

			char szCP[20] = { 0 };
			sprintf_s(szCP, "QK%d", j);
			putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
			rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
			rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
		}
		RECTLIST::iterator itGrayRect = pPaper->pModel->vecPaperModel[i].lGray.begin();
		for (int j = 0; itGrayRect != pPaper->pModel->vecPaperModel[i].lGray.end(); itGrayRect++, j++)
		{
			cv::Rect rt = (*itGrayRect).rt;
			GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);

			char szCP[20] = { 0 };
			sprintf_s(szCP, "G%d", j);
			putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
			rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
			rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
		}
		RECTLIST::iterator itWhiteRect = pPaper->pModel->vecPaperModel[i].lWhite.begin();
		for (int j = 0; itWhiteRect != pPaper->pModel->vecPaperModel[i].lWhite.end(); itWhiteRect++, j++)
		{
			cv::Rect rt = (*itWhiteRect).rt;
			GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);

			char szCP[20] = { 0 };
			sprintf_s(szCP, "W%d", j);
			putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
			rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
			rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
		}
#else
		RECTLIST::iterator itCPRect = pPaper->pModel->vecPaperModel[i].lCheckPoint.begin();
		for (int j = 0; itCPRect != pPaper->pModel->vecPaperModel[i].lCheckPoint.end(); itCPRect++, j++)
		{
			cv::Rect rt = (*itCPRect).rt;
			rt.x = rt.x + (*itPic)->ptFix.x;
			rt.y = rt.y + (*itPic)->ptFix.y;

			char szCP[20] = { 0 };
			sprintf_s(szCP, "CP%d", j);
#if 1
			putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
			rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
			rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
#else
			putText(tmp, szCP, Point((*itCPRect).rt.x, (*itCPRect).rt.y + (*itCPRect).rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
			rectangle(tmp, (*itCPRect).rt, CV_RGB(255, 0, 0), 2);
			rectangle(tmp2, (*itCPRect).rt, CV_RGB(255, 233, 10), -1);
#endif
		}
#endif
		addWeighted(tmp, 0.5, tmp2, 0.5, 0, tmp);
		m_vecPicShow[i]->ShowPic(tmp);
	}
}

int CScanToolDlg::PaintIssueRect(pST_PaperInfo pPaper)
{
	int nResult = -1;
	PIC_LIST::iterator itPic = pPaper->lPic.begin();
	for (int i = 0; itPic != pPaper->lPic.end(); itPic++, i++)
	{
		if ((*itPic)->bFindIssue)
		{
			Point pt(0, 0);
			Mat matSrc = imread((*itPic)->strPicPath);
			Mat tmp = matSrc.clone();
			Mat tmp2 = matSrc.clone();

			RECTLIST::iterator itSelRoi = pPaper->pModel->vecPaperModel[i].lSelROI.begin();													//��ʾʶ�𶨵��ѡ����
			for (int j = 0; itSelRoi != pPaper->pModel->vecPaperModel[i].lSelROI.end(); itSelRoi++, j++)
			{
				cv::Rect rt = (*itSelRoi).rt;
//				GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);

				char szCP[20] = { 0 };
// 				sprintf_s(szCP, "FIX%d", j);
// 				putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 0));	//CV_FONT_HERSHEY_COMPLEX
				rectangle(tmp, rt, CV_RGB(0, 0, 255), 2);
				rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
			}

			RECTLIST::iterator itPicFix = (*itPic)->lFix.begin();													//��ʾʶ������Ķ���
			for (int j = 0; itPicFix != (*itPic)->lFix.end(); itPicFix++, j++)
			{
				cv::Rect rt = (*itPicFix).rt;
//				GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);

				char szCP[20] = { 0 };
				sprintf_s(szCP, "FIX%d", j);
				putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 0));	//CV_FONT_HERSHEY_COMPLEX
				rectangle(tmp, rt, CV_RGB(0, 255, 0), 2);
				rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
			}
			RECTLIST::iterator itFixRect = pPaper->pModel->vecPaperModel[i].lFix.begin();							//��ʾģ���ϵĶ����Ӧ�����Ծ��ϵ��¶���
			for (int j = 0; itFixRect != pPaper->pModel->vecPaperModel[i].lFix.end(); itFixRect++, j++)
			{
				cv::Rect rt = (*itFixRect).rt;
				GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);

				char szCP[20] = { 0 };
				sprintf_s(szCP, "FIX%d", j);
				putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
				rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
				rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
			}

			nResult = i;
			RECTLIST::iterator itIssueRect = (*itPic)->lIssueRect.begin();
			for (int j = 0; itIssueRect != (*itPic)->lIssueRect.end(); itIssueRect++, j++)
			{
				cv::Rect rt = (*itIssueRect).rt;			
//				GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);
				if (j == 0)
				{
					pt = (*itIssueRect).rt.tl();	// +(*itPic)->ptFix;		//(*itIssueRect).rt.tl()
					pt.x = pt.x - 100;
					pt.y = pt.y - 100;
				}
				char szCP[20] = { 0 };
				sprintf_s(szCP, "Err%d", j);
#if 1
				putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
				rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
				rectangle(tmp2, rt, CV_RGB(255, 200, 100), -1);
#else
				putText(tmp, szCP, Point((*itIssueRect).rt.x, (*itIssueRect).rt.y + (*itIssueRect).rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
				rectangle(tmp, (*itIssueRect).rt, CV_RGB(255, 0, 0), 2);
				rectangle(tmp2, (*itIssueRect).rt, CV_RGB(255, 200, 100), -1);
#endif
			}

			RECTLIST::iterator itNormal = (*itPic)->lNormalRect.begin();													//��ʾʶ�𶨵��ѡ����
			for (int j = 0; itNormal != (*itPic)->lNormalRect.end(); itNormal++, j++)
			{
				cv::Rect rt = (*itNormal).rt;
				//				GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[i].lFix, rt);

				char szCP[20] = { 0 };
				// 				sprintf_s(szCP, "FIX%d", j);
				// 				putText(tmp, szCP, Point(rt.x, rt.y + rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(0, 255, 0));	//CV_FONT_HERSHEY_COMPLEX
				rectangle(tmp, rt, CV_RGB(50, 255, 55), 2);
				rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
			}

			addWeighted(tmp, 0.5, tmp2, 0.5, 0, tmp);
			m_vecPicShow[i]->ShowPic(tmp, pt);
		}
	}
	return nResult;
}

LRESULT CScanToolDlg::MsgRecogErr(WPARAM wParam, LPARAM lParam)
{
	pST_PaperInfo pPaper	= (pST_PaperInfo)wParam;
	pPAPERSINFO   pPapers	= (pPAPERSINFO)lParam;

	_bTwainContinue = FALSE;

	int nIssuePaper = 0;
	int nCount = m_lcPicture.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		pST_PaperInfo pPaperData = (pST_PaperInfo)m_lcPicture.GetItemData(i);
		if (pPaper == pPaperData)
		{
			nIssuePaper = PaintIssueRect(pPaperData);

			m_lcPicture.SetItemState(i, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			m_lcPicture.SetSelectionMark(i);
			m_lcPicture.SetFocus();
			break;
		}
	}
	m_tabPicShowCtrl.SetCurSel(nIssuePaper);
	m_pCurrentPicShow = m_vecPicShow[nIssuePaper];
	m_pCurrentPicShow->ShowWindow(SW_SHOW);
	for (int i = 0; i < m_vecPicShow.size(); i++)
	{
		if (i != nIssuePaper)
			m_vecPicShow[i]->ShowWindow(SW_HIDE);
	}
	m_pCurrentShowPaper = pPaper;
	m_nScanStatus = 2;
	USES_CONVERSION;
	char szErrInfo[200] = { 0 };
	sprintf_s(szErrInfo, "���� %s ��%dҳʶ�������У���", pPaper->strStudentInfo.c_str(), nIssuePaper + 1);
	SetStatusShowInfo(A2T(szErrInfo));
	return TRUE;
}

void CScanToolDlg::OnBnClickedBtnUploadpapers()
{
	if (!m_pPapersInfo)
		return;

	if (!m_bLogin)
	{
		AfxMessageBox(_T("û�е�¼�������ϴ������ȵ�¼!"));
		return;
	}

	CPapersInfoSaveDlg dlg(m_pPapersInfo);
	if (dlg.DoModal() != IDOK)
		return;

	clock_t start, end;
	start = clock();

	//�Ծ��ѹ��
	USES_CONVERSION;
	char szPapersSavePath[MAX_PATH] = { 0 };
	char szZipName[50] = { 0 };
	if (m_bLogin)
	{
		Poco::LocalDateTime now;
		char szTime[50] = { 0 };
		sprintf_s(szTime, "%d%02d%02d%02d%02d%02d", now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());

		sprintf_s(szPapersSavePath, "%sPaper\\%s_%s", T2A(g_strCurrentPath), T2A(m_strUserName), szTime);
		sprintf_s(szZipName, "%s_%s.zip", T2A(m_strUserName), szTime);
	}
	else
	{
		sprintf_s(szPapersSavePath, "%sPaper\\%s", T2A(g_strCurrentPath), m_pPapersInfo->strPapersName.c_str());
		sprintf_s(szZipName, "%s.zip", m_pPapersInfo->strPapersName.c_str());
	}
	CString strInfo;
	bool bWarn = false;
	strInfo.Format(_T("���򱣴�%s..."), A2T(szZipName));
	SetStatusShowInfo(strInfo, bWarn);
	if (!ZipFile(A2T(m_strCurrPicSavePath.c_str()), A2T(szPapersSavePath)))
	{
		bWarn = true;
		strInfo.Format(_T("����%sʧ��"), A2T(szZipName));
	}
	else
	{
		end = clock();
		strInfo.Format(_T("����%s�ɹ�,�Ծ��ѹ��ʱ��: %dms"), A2T(szZipName), end - start);
		SAFE_RELEASE(m_pPapersInfo);
		m_lcPaper.DeleteAllItems();
		m_pCurrentShowPaper = NULL;
	}
	SetStatusShowInfo(strInfo, bWarn);

	//����ϴ��б�	******************		��Ҫ���м�Ȩ����	***************	
	char szFileFullPath[300] = { 0 };
	sprintf_s(szFileFullPath, "%s.zip", szPapersSavePath);
	pSENDTASK pTask = new SENDTASK;
	pTask->strFileName	= szZipName;
	pTask->strPath = szFileFullPath;
	g_fmSendLock.lock();
	g_lSendTask.push_back(pTask);
	g_fmSendLock.unlock();
}

LRESULT CScanToolDlg::RoiLBtnDown(WPARAM wParam, LPARAM lParam)
{
	cv::Point pt = *(cv::Point*)(wParam);
	ShowRectByPoint(pt, m_pCurrentShowPaper);
	return TRUE;
}

void CScanToolDlg::ShowRectByPoint(cv::Point pt, pST_PaperInfo pPaper)
{
	if (!pPaper || !pPaper->pModel || pPaper->pModel->vecPaperModel.size() < m_nCurrTabSel)
		return;

// 	if (!pPaper->bIssuePaper)		//��ǰ����û������㣬��������ʾ
// 		return;

	int nFind = -1;
	RECTINFO* pRc = NULL;
	PIC_LIST::iterator itPic = pPaper->lPic.begin();
	for (int i = 0; itPic != pPaper->lPic.end(); itPic++, i++)
	{
		if (i == m_nCurrTabSel)
			break;
	}
// 	pt.x = pt.x - (*itPic)->ptFix.x;
// 	pt.y = pt.y - (*itPic)->ptFix.y;
	cv::Rect rt;
	rt.x = pt.x;
	rt.y = pt.y;
//	GetPosition((*itPic)->lFix, pPaper->pModel->vecPaperModel[m_nCurrTabSel].lFix, rt);
// 	GetPosition(pPaper->pModel->vecPaperModel[m_nCurrTabSel].lFix, (*itPic)->lFix, rt);
// 	pt.x = rt.x;
// 	pt.y = rt.y;
	nFind = GetRectInfoByPoint(pt, *itPic, pRc);
	if (nFind < 0)
		return;

	if (!pRc)
		return;

	CString strInfo;
	strInfo.Format(_T("��ֵ: %d, Ҫ�����: %f, ʵ��: %f"), pRc->nThresholdValue, pRc->fStandardValuePercent, pRc->fRealValuePercent);
	if (pPaper->bIssuePaper)
		SetStatusShowInfo(strInfo, TRUE);
	else
		SetStatusShowInfo(strInfo);
}

int CScanToolDlg::GetRectInfoByPoint(cv::Point pt, pST_PicInfo pPic, RECTINFO*& pRc)
{
	int  nFind = -1;
	RECTLIST::iterator itRectInfo = pPic->lNormalRect.begin();
	for (int i = 0; itRectInfo != pPic->lNormalRect.end(); itRectInfo++, i++)
	{
		if (itRectInfo->rt.contains(pt))
		{
			nFind = i;
			pRc = &(*itRectInfo);
			break;
		}
	}
	if (nFind < 0)
	{
		RECTLIST::iterator itIssueRectInfo = pPic->lIssueRect.begin();
		for (int i = 0; itIssueRectInfo != pPic->lIssueRect.end(); itIssueRectInfo++, i++)
		{
			if (itIssueRectInfo->rt.contains(pt))
			{
				nFind = i;
				pRc = &(*itIssueRectInfo);
				break;
			}
		}
	}
	return nFind;
}



