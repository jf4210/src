// MakeModelDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ScanTool.h"
#include "MakeModelDlg.h"
#include "afxdialogex.h"
#include "ModelSaveDlg.h"
#include <afxinet.h>
#include "ModelInfoDlg.h"

using namespace std;
using namespace cv;
// CMakeModelDlg 对话框

IMPLEMENT_DYNAMIC(CMakeModelDlg, CDialog)

CMakeModelDlg::CMakeModelDlg(pMODEL pModel /*= NULL*/, CWnd* pParent /*=NULL*/)
	: CDialog(CMakeModelDlg::IDD, pParent)
	, m_pModelPicShow(NULL), m_nGaussKernel(5), m_nSharpKernel(5), m_nThresholdKernel(150), m_nCannyKernel(90), m_nDelateKernel(6), m_nErodeKernel(2)
	, m_pModel(pModel), m_bNewModelFlag(false), m_nModelPicNums(2), m_nCurrTabSel(0), m_bSavedModelFlag(false), m_ncomboCurrentSel(0), m_eCurCPType(UNKNOWN)
	, m_nCurListCtrlSel(0), m_nThresholdVal(0), m_fThresholdValPercent(0.0)
	, m_nWhiteVal(225), m_nHeadVal(150), m_nABModelVal(150), m_nCourseVal(150), m_nQK_CPVal(150), m_nGrayVal(150), m_nFixVal(150)
	, m_fHeadThresholdPercent(0.75), m_fABModelThresholdPercent(0.75), m_fCourseThresholdPercent(0.75), m_fQK_CPThresholdPercent(0.75), m_fFixThresholdPercent(0.80)
	, m_fGrayThresholdPercent(0.75), m_fWhiteThresholdPercent(0.75)
	, m_pCurRectInfo(NULL), m_strCPTypeName(_T("")), m_ptFixCP(0,0)
{
}

CMakeModelDlg::~CMakeModelDlg()
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

	std::vector<pPaperModelInfo>::iterator itPaperModelInfo = m_vecPaperModelInfo.begin();
	for (; itPaperModelInfo != m_vecPaperModelInfo.end();)
	{
		pPaperModelInfo pPaperModel = *itPaperModelInfo;
		if (pPaperModel)
		{
			delete pPaperModel;
			pPaperModel = NULL;
		}
		itPaperModelInfo = m_vecPaperModelInfo.erase(itPaperModelInfo);
	}
}

void CMakeModelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_ModelPic, m_tabModelPicCtrl);
	DDX_Control(pDX, IDC_COMBO_CPType, m_comboCheckPointType);
	DDX_Control(pDX, IDC_LIST_CheckPoint, m_cpListCtrl);
	DDX_Text(pDX, IDC_EDIT_Threshold, m_nThresholdVal);
	DDX_Text(pDX, IDC_EDIT_ThresholdPercent, m_fThresholdValPercent);
	DDX_Text(pDX, IDC_EDIT_CPType, m_strCPTypeName);
}

BEGIN_MESSAGE_MAP(CMakeModelDlg, CDialog)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BTN_SelPic, &CMakeModelDlg::OnBnClickedBtnSelpic)
	ON_BN_CLICKED(IDC_BTN_RESET, &CMakeModelDlg::OnBnClickedBtnReset)
	ON_MESSAGE(WM_CV_LBTNUP, &CMakeModelDlg::RoiLBtnUp)
	ON_MESSAGE(WM_CV_LBTNDOWN, &CMakeModelDlg::RoiLBtnDown)
	ON_MESSAGE(WM_CV_RBTNUP, &CMakeModelDlg::RoiRBtnUp)
	ON_BN_CLICKED(IDC_BTN_SAVE, &CMakeModelDlg::OnBnClickedBtnSave)
	ON_BN_CLICKED(IDC_BTN_ExitModelDlg, &CMakeModelDlg::OnBnClickedBtnExitmodeldlg)
	ON_BN_CLICKED(IDC_BTN_New, &CMakeModelDlg::OnBnClickedBtnNew)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_ModelPic, &CMakeModelDlg::OnTcnSelchangeTabModelpic)
	ON_CBN_SELCHANGE(IDC_COMBO_CPType, &CMakeModelDlg::OnCbnSelchangeComboCptype)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CheckPoint, &CMakeModelDlg::OnNMRClickListCheckpoint)
	ON_COMMAND(ID_DelRecognition, &CMakeModelDlg::DeleteRectInfoOnList)
	ON_COMMAND(ID_DelPicRectRecog, &CMakeModelDlg::DelRectInfoOnPic)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CheckPoint, &CMakeModelDlg::OnNMDblclkListCheckpoint)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_CheckPoint, &CMakeModelDlg::OnLvnKeydownListCheckpoint)
	ON_BN_CLICKED(IDC_BTN_SaveRecogInfo, &CMakeModelDlg::OnBnClickedBtnSaverecoginfo)
END_MESSAGE_MAP()

// CMakeModelDlg 消息处理程序
BOOL CMakeModelDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	InitUI();
	InitConf();
	if (m_pModel)
	{
		m_vecPaperModelInfo.clear();

		for (int i = 0; i < m_pModel->nPicNum; i++)
		{
			CString strPicPath = g_strCurrentPath + _T("Model\\") + m_pModel->strModelName + _T("\\") + m_pModel->vecPaperModel[i].strModelPicName;

			pPaperModelInfo pPaperModel = new PaperModelInfo;
			m_vecPaperModelInfo.push_back(pPaperModel);
			pPaperModel->nPaper = i;
			pPaperModel->strModelPicPath = strPicPath;
			pPaperModel->strModelPicName = m_pModel->vecPaperModel[i].strModelPicName;

			pPaperModel->matSrcImg = imread((std::string)(CT2CA)strPicPath);
			pPaperModel->matDstImg = pPaperModel->matSrcImg;

			Mat src_img;
			src_img = m_vecPaperModelInfo[i]->matDstImg;
			m_vecPicShow[i]->ShowPic(src_img);
			
			RECTLIST::iterator itSelRoi = m_pModel->vecPaperModel[i].lSelROI.begin();
			for (; itSelRoi != m_pModel->vecPaperModel[i].lSelROI.end(); itSelRoi++)
			{
				pPaperModel->vecRtSel.push_back(*itSelRoi);
			}
			RECTLIST::iterator itCP = m_pModel->vecPaperModel[i].lCheckPoint.begin();
			for (; itCP != m_pModel->vecPaperModel[i].lCheckPoint.end(); itCP++)
			{
				pPaperModel->vecRtRecognition.push_back(*itCP);
			}
			RECTLIST::iterator itOmr = m_pModel->vecPaperModel[i].lOMR.begin();
			for (; itOmr != m_pModel->vecPaperModel[i].lOMR.end(); itOmr++)
			{
				pPaperModel->vecOmr.push_back(*itOmr);
			}
			RECTLIST::iterator itFix = m_pModel->vecPaperModel[i].lFix.begin();
			for (; itFix != m_pModel->vecPaperModel[i].lFix.end(); itFix++)
			{
				pPaperModel->vecRtFix.push_back(*itFix);
// 				m_ptFixCP.x = (*itFix).rt.x + (*itFix).rt.width / 2 + 0.5;
// 				m_ptFixCP.y = (*itFix).rt.y + (*itFix).rt.height / 2 + 0.5;
			}
			RECTLIST::iterator itHHead = m_pModel->vecPaperModel[i].lH_Head.begin();
			for (; itHHead != m_pModel->vecPaperModel[i].lH_Head.end(); itHHead++)
			{
				pPaperModel->vecH_Head.push_back(*itHHead);
			}
			RECTLIST::iterator itVHead = m_pModel->vecPaperModel[i].lV_Head.begin();
			for (; itVHead != m_pModel->vecPaperModel[i].lV_Head.end(); itVHead++)
			{
				pPaperModel->vecV_Head.push_back(*itVHead);
			}
			RECTLIST::iterator itABModel = m_pModel->vecPaperModel[i].lABModel.begin();
			for (; itABModel != m_pModel->vecPaperModel[i].lABModel.end(); itABModel++)
			{
				pPaperModel->vecABModel.push_back(*itABModel);
			}
			RECTLIST::iterator itCourse = m_pModel->vecPaperModel[i].lCourse.begin();
			for (; itCourse != m_pModel->vecPaperModel[i].lCourse.end(); itCourse++)
			{
				pPaperModel->vecCourse.push_back(*itCourse);
			}
			RECTLIST::iterator itQK = m_pModel->vecPaperModel[i].lQK_CP.begin();
			for (; itQK != m_pModel->vecPaperModel[i].lQK_CP.end(); itQK++)
			{
				pPaperModel->vecQK_CP.push_back(*itQK);
			}
			RECTLIST::iterator itGray = m_pModel->vecPaperModel[i].lGray.begin();
			for (; itGray != m_pModel->vecPaperModel[i].lGray.end(); itGray++)
			{
				pPaperModel->vecGray.push_back(*itGray);
			}
			RECTLIST::iterator itWhite = m_pModel->vecPaperModel[i].lWhite.begin();
			for (; itWhite != m_pModel->vecPaperModel[i].lWhite.end(); itWhite++)
			{
				pPaperModel->vecWhite.push_back(*itWhite);
			}
#if 1
			ShowRectByCPType(m_eCurCPType);
			UpdataCPList();
#else
			Mat tmp = m_vecPaperModelInfo[i]->matDstImg.clone();
			Mat tmp2 = m_vecPaperModelInfo[i]->matDstImg.clone();
			for (int j = 0; j < m_vecPaperModelInfo[i]->vecRtRecognition.size(); j++)
			{
				char szCP[20] = { 0 };
				sprintf_s(szCP, "CP%d", i);
				putText(tmp, szCP, Point(m_vecPaperModelInfo[i]->vecRtRecognition[j].rt.x, m_vecPaperModelInfo[i]->vecRtRecognition[j].rt.y + m_vecPaperModelInfo[i]->vecRtRecognition[j].rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
				rectangle(tmp, m_vecPaperModelInfo[i]->vecRtRecognition[j].rt, CV_RGB(255, 0, 0), 2);
				rectangle(tmp2, m_vecPaperModelInfo[i]->vecRtRecognition[j].rt, CV_RGB(255, 233, 10), -1);
			}
			cv::addWeighted(tmp, 0.5, tmp2, 0.5, 0, tmp);
			m_vecPicShow[i]->ShowPic(tmp);
#endif
		}
		CString strTitle = _T("");
		strTitle.Format(_T("模板名称: %s"), m_pModel->strModelName);
		SetWindowText(strTitle);
	}
	else
	{
		CString strTitle = _T("未保存模板");
		SetWindowText(strTitle);
	}

	return TRUE;
}
void CMakeModelDlg::OnTcnSelchangeTabModelpic(NMHDR *pNMHDR, LRESULT *pResult)
{	
	*pResult = 0;
	int nIndex		= m_tabModelPicCtrl.GetCurSel();
	m_nCurrTabSel	= nIndex;
	m_pModelPicShow = m_vecPicShow[nIndex];
	m_pModelPicShow->ShowWindow(SW_SHOW);
	for (int i = 0; i < m_vecPicShow.size(); i++)
	{
		if (i != nIndex)
			m_vecPicShow[i]->ShowWindow(SW_HIDE);
	}
	m_comboCheckPointType.SetCurSel(0);
	m_cpListCtrl.DeleteAllItems();

	m_ncomboCurrentSel = 0;
	m_eCurCPType = GetComboSelCpType();
	UpdataCPList();
	ShowRectByCPType(m_eCurCPType);	
	Invalidate();
}

void CMakeModelDlg::InitTab()
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
	m_tabModelPicCtrl.DeleteAllItems();

	USES_CONVERSION;
	CRect rtTab;
	m_tabModelPicCtrl.GetClientRect(&rtTab);
	for (int i = 0; i < m_nModelPicNums; i++)
	{
		char szTabHeadName[20] = { 0 };
		sprintf_s(szTabHeadName, "第%d页", i + 1);

		m_tabModelPicCtrl.InsertItem(i, A2T(szTabHeadName));

		CPicShow* pPicShow = new CPicShow(this);
		pPicShow->Create(CPicShow::IDD, &m_tabModelPicCtrl);
		pPicShow->ShowWindow(SW_HIDE);
		pPicShow->MoveWindow(&rtTab);
		m_vecPicShow.push_back(pPicShow);
	}
	m_tabModelPicCtrl.SetCurSel(0);
	if (m_vecPicShow.size())
	{
		m_vecPicShow[0]->ShowWindow(SW_SHOW);
		m_pModelPicShow = m_vecPicShow[0];
	}	

	if (m_tabModelPicCtrl.GetSafeHwnd())
	{
		CRect rtTab;
		m_tabModelPicCtrl.GetClientRect(&rtTab);
		int nTabHead_H = 25;		//tab控件头的高度
		CRect rtPic = rtTab;
		rtPic.top = rtPic.top + nTabHead_H;
		for (int i = 0; i < m_vecPicShow.size(); i++)
			m_vecPicShow[i]->MoveWindow(&rtPic);
	}
}

void CMakeModelDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	InitCtrlPosition();
}

void CMakeModelDlg::InitUI()
{
	USES_CONVERSION;
	if (m_pModel)
		m_nModelPicNums = m_pModel->nPicNum;

	m_cpListCtrl.SetExtendedStyle(m_cpListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_SHOWSELALWAYS);
	m_cpListCtrl.InsertColumn(0, _T("序号"), LVCFMT_CENTER, 36);
	m_cpListCtrl.InsertColumn(1, _T("位置信息"), LVCFMT_CENTER, 120);

	CRect rtTab;
	m_tabModelPicCtrl.GetClientRect(&rtTab);
	for (int i = 0; i < m_nModelPicNums; i++)
	{
		char szTabHeadName[20] = { 0 };
		sprintf_s(szTabHeadName, "第%d页", i + 1);

		m_tabModelPicCtrl.InsertItem(i, A2T(szTabHeadName));

		CPicShow* pPicShow = new CPicShow(this);
		pPicShow->Create(CPicShow::IDD, &m_tabModelPicCtrl);
		pPicShow->ShowWindow(SW_HIDE);
		pPicShow->MoveWindow(&rtTab);
		m_vecPicShow.push_back(pPicShow);
	}
	m_tabModelPicCtrl.SetCurSel(0);
	m_vecPicShow[0]->ShowWindow(SW_SHOW);
	m_pModelPicShow = m_vecPicShow[0];

	int sx = MAX_DLG_WIDTH;
	int sy = MAX_DLG_HEIGHT;
	MoveWindow(0, 0, sx, sy);
	CenterWindow();

	InitCtrlPosition();
}

void CMakeModelDlg::InitCtrlPosition()
{
	CRect rcClient;
	GetClientRect(&rcClient);
	int cx = rcClient.right;
	int cy = rcClient.bottom;

	int nGap = 5;	//控件的间隔
	int nTopGap = 2;	//距离上边边缘的间隔
	int nBottomGap = 2;	//距离下边边缘的间隔
	int nLeftGap = 2;	//距离左边边缘的间隔
	int nRightGap = 2;	//距离右边边缘的间隔
	int nBtnHeigh = 70;
	int	nBtnWidth = 70;
	int nLeftCtrlWidth = 170;	//左边控件栏的宽度
	int nStaticHeight = 20;		//校验点类型Static控件高度
	int nCommoboxHeight = 25;	//校验点combox的高度
	int nCPListCtrlHeight = 300;//校验点list控件的高度
	int nCurrentTop = nTopGap;
	if (GetDlgItem(IDC_STATIC_CPType)->GetSafeHwnd())
	{
		GetDlgItem(IDC_STATIC_CPType)->MoveWindow(nLeftGap, nCurrentTop, nLeftCtrlWidth, nStaticHeight);
		nCurrentTop = nCurrentTop + nStaticHeight + nGap;
	}
	if (m_comboCheckPointType.GetSafeHwnd())
	{
		m_comboCheckPointType.MoveWindow(nLeftGap, nCurrentTop, nLeftCtrlWidth, nCommoboxHeight);
		nCurrentTop = nCurrentTop + nCommoboxHeight + nGap;
	}
	if (GetDlgItem(IDC_STATIC_List)->GetSafeHwnd())
	{
		GetDlgItem(IDC_STATIC_List)->MoveWindow(nLeftGap, nCurrentTop, nLeftCtrlWidth, nStaticHeight);
		nCurrentTop = nCurrentTop + nStaticHeight + nGap;
	}
	if (m_cpListCtrl.GetSafeHwnd())
	{
		m_cpListCtrl.MoveWindow(nLeftGap, nCurrentTop, nLeftCtrlWidth, nCPListCtrlHeight);
		nCurrentTop = nCurrentTop + nCPListCtrlHeight + nGap;
	}
	int nGroupHeight = 100;
	int nGroupStaticHeight = 15;	//group文字的高度
	int nTopInGroup = nCurrentTop;
	if (GetDlgItem(IDC_STATIC_Group)->GetSafeHwnd())
	{
		GetDlgItem(IDC_STATIC_Group)->MoveWindow(nLeftGap, nTopInGroup, nLeftCtrlWidth, nGroupHeight);
		nTopInGroup = nTopInGroup + nGroupStaticHeight + nGap;		//加上一个group文字的高度
	}
	int nStaticWidthInGrop = (nLeftCtrlWidth - 3 * nGap) / 3;
	if (GetDlgItem(IDC_STATIC_CPTypeInGroup)->GetSafeHwnd())
	{
		GetDlgItem(IDC_STATIC_CPTypeInGroup)->MoveWindow(nLeftGap + nGap, nTopInGroup, nStaticWidthInGrop, nStaticHeight);
	}
	if (GetDlgItem(IDC_EDIT_CPType)->GetSafeHwnd())
	{
		GetDlgItem(IDC_EDIT_CPType)->MoveWindow(nLeftGap + nStaticWidthInGrop + nGap * 2, nTopInGroup, nStaticWidthInGrop * 2, nStaticHeight);
		nTopInGroup = nTopInGroup + nStaticHeight + nGap;
	}
	if (GetDlgItem(IDC_STATIC_Threshold)->GetSafeHwnd())
	{
		GetDlgItem(IDC_STATIC_Threshold)->MoveWindow(nLeftGap + nGap, nTopInGroup, nStaticWidthInGrop, nStaticHeight);
	}
	if (GetDlgItem(IDC_EDIT_Threshold)->GetSafeHwnd())
	{
		GetDlgItem(IDC_EDIT_Threshold)->MoveWindow(nLeftGap + nStaticWidthInGrop + nGap * 2, nTopInGroup, nStaticWidthInGrop, nStaticHeight);
		nTopInGroup = nTopInGroup + nStaticHeight + nGap;
	}
	if (GetDlgItem(IDC_STATIC_ThresholdPercent)->GetSafeHwnd())
	{
		GetDlgItem(IDC_STATIC_ThresholdPercent)->MoveWindow(nLeftGap + nGap, nTopInGroup, nStaticWidthInGrop, nStaticHeight);
	}
	if (GetDlgItem(IDC_EDIT_ThresholdPercent)->GetSafeHwnd())
	{
		GetDlgItem(IDC_EDIT_ThresholdPercent)->MoveWindow(nLeftGap + nStaticWidthInGrop + nGap * 2, nTopInGroup, nStaticWidthInGrop, nStaticHeight);
	}
	int nBtnInGroup_L = nLeftGap + nStaticWidthInGrop + nGap * 2 + nStaticWidthInGrop + nGap;
	int nBtnInGroup_W = nLeftCtrlWidth - nBtnInGroup_L - nGap;
	int nBtnInGroup_H = nStaticHeight * 2 + nGap;
	if (GetDlgItem(IDC_BTN_SaveRecogInfo)->GetSafeHwnd())
	{
		nTopInGroup = nCurrentTop + nGroupStaticHeight + nGap + nStaticHeight + nGap;
		GetDlgItem(IDC_BTN_SaveRecogInfo)->MoveWindow(nBtnInGroup_L, nTopInGroup, nBtnInGroup_W, nBtnInGroup_H);
	}

	nCurrentTop = nCurrentTop + nGroupHeight + nGap;
	if (GetDlgItem(IDC_BTN_New)->GetSafeHwnd())
	{
		GetDlgItem(IDC_BTN_New)->MoveWindow(nLeftGap, nCurrentTop, nBtnWidth, nBtnHeigh);
		nCurrentTop = nCurrentTop + nBtnHeigh + nGap;
	}
	if (GetDlgItem(IDC_BTN_SelPic)->GetSafeHwnd())
	{
		GetDlgItem(IDC_BTN_SelPic)->MoveWindow(nLeftGap, nCurrentTop, nBtnWidth, nBtnHeigh);
//		nCurrentTop = nCurrentTop + nBtnHeigh + nGap;
	}
	if (GetDlgItem(IDC_BTN_RESET)->GetSafeHwnd())
	{
		GetDlgItem(IDC_BTN_RESET)->MoveWindow(nLeftGap + nBtnWidth + nGap, nCurrentTop, nBtnWidth, nBtnHeigh);
		nCurrentTop = nCurrentTop + nBtnHeigh + nGap;
	}
	if (GetDlgItem(IDC_BTN_SAVE)->GetSafeHwnd())
	{
		GetDlgItem(IDC_BTN_SAVE)->MoveWindow(nLeftGap, nCurrentTop, nBtnWidth, nBtnHeigh);
//		nCurrentTop = nCurrentTop + nBtnHeigh + nGap;
	}
	if (GetDlgItem(IDC_BTN_ExitModelDlg)->GetSafeHwnd())
	{
		GetDlgItem(IDC_BTN_ExitModelDlg)->MoveWindow(nLeftGap + nBtnWidth + nGap, nCurrentTop, nBtnWidth, nBtnHeigh);
		nCurrentTop = nCurrentTop + nBtnHeigh + nGap;
	}
	if (m_tabModelPicCtrl.GetSafeHwnd())
	{
		m_tabModelPicCtrl.MoveWindow(nLeftGap + nLeftCtrlWidth + nGap, nTopGap, cx - nLeftGap - nLeftCtrlWidth - nGap - nRightGap, cy - nTopGap - nBottomGap);

		CRect rtTab;
		m_tabModelPicCtrl.GetClientRect(&rtTab);
		int nTabHead_H = 25;		//tab控件头的高度
		CRect rtPic = rtTab;
		rtPic.top = rtPic.top + nTabHead_H;
		for (int i = 0; i < m_vecPicShow.size(); i++)
			m_vecPicShow[i]->MoveWindow(&rtPic);
	}
}

void CMakeModelDlg::InitConf()
{
	if (m_pModel)
	{
		if (m_pModel->nABModel&&m_pModel->nHasHead)
		{
			m_comboCheckPointType.ResetContent();
			m_comboCheckPointType.AddString(_T(""));
			m_comboCheckPointType.AddString(_T("定点"));
			m_comboCheckPointType.AddString(_T("水平同步头"));
			m_comboCheckPointType.AddString(_T("垂直同步头"));
			m_comboCheckPointType.AddString(_T("卷形校验点"));
			m_comboCheckPointType.AddString(_T("科目校验点"));
			m_comboCheckPointType.AddString(_T("缺考校验点"));
			m_comboCheckPointType.AddString(_T("灰度校验点"));
			m_comboCheckPointType.AddString(_T("空白校验点"));
		}
		else if (m_pModel->nABModel&&!m_pModel->nHasHead)
		{
			m_comboCheckPointType.ResetContent();
			m_comboCheckPointType.AddString(_T(""));
			m_comboCheckPointType.AddString(_T("定点"));
			m_comboCheckPointType.AddString(_T("卷形校验点"));
			m_comboCheckPointType.AddString(_T("科目校验点"));
			m_comboCheckPointType.AddString(_T("缺考校验点"));
			m_comboCheckPointType.AddString(_T("灰度校验点"));
			m_comboCheckPointType.AddString(_T("空白校验点"));
		}
		else if (!m_pModel->nABModel&&m_pModel->nHasHead)
		{
			m_comboCheckPointType.ResetContent();
			m_comboCheckPointType.AddString(_T(""));
			m_comboCheckPointType.AddString(_T("定点"));
			m_comboCheckPointType.AddString(_T("水平同步头"));
			m_comboCheckPointType.AddString(_T("垂直同步头"));
			m_comboCheckPointType.AddString(_T("科目校验点"));
			m_comboCheckPointType.AddString(_T("缺考校验点"));
			m_comboCheckPointType.AddString(_T("灰度校验点"));
			m_comboCheckPointType.AddString(_T("空白校验点"));
		}
		else
		{
			m_comboCheckPointType.ResetContent();
			m_comboCheckPointType.AddString(_T(""));
			m_comboCheckPointType.AddString(_T("定点"));
			m_comboCheckPointType.AddString(_T("科目校验点"));
			m_comboCheckPointType.AddString(_T("缺考校验点"));
			m_comboCheckPointType.AddString(_T("灰度校验点"));
			m_comboCheckPointType.AddString(_T("空白校验点"));
		}
	}
	m_comboCheckPointType.SetCurSel(0);
}

LRESULT CMakeModelDlg::RoiRBtnUp(WPARAM wParam, LPARAM lParam)
{
	cv::Point pt = *(cv::Point*)(wParam);
	if (m_eCurCPType == UNKNOWN)
		return TRUE;

	int nFind = 0;
	RECTINFO* pRc = NULL;
	nFind = GetRectInfoByPoint(pt, m_eCurCPType, pRc);
	if (nFind < 0 || !pRc)
		return FALSE;

	//下面的这段代码, 不单单适应于ListCtrl  
	CMenu menu, *pPopup;
	menu.LoadMenu(IDR_MENU_RectInPic);
	pPopup = menu.GetSubMenu(0);
	CPoint myPoint;
	ClientToScreen(&myPoint);
	GetCursorPos(&myPoint); //鼠标位置  
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, myPoint.x, myPoint.y, this);//GetParent()
	m_ptRBtnUp = pt;
	return TRUE;
}

LRESULT CMakeModelDlg::RoiLBtnUp(WPARAM wParam, LPARAM lParam)
{
	Rect  Rt = *(Rect*)(wParam);
	Mat*  pShowMat = (Mat*)(lParam);

	if (m_eCurCPType == UNKNOWN)
		return FALSE;

	Recognise(Rt);
	SortRect();
	UpdataCPList();
//	PicRotate();
//	PaintRecognisedRect();
	return TRUE;
}

LRESULT CMakeModelDlg::RoiLBtnDown(WPARAM wParam, LPARAM lParam)
{
	cv::Point pt = *(cv::Point*)(wParam);
	ShowRectByPoint(pt);
	return TRUE;
}

void CMakeModelDlg::OnBnClickedBtnNew()
{
	USES_CONVERSION;
	m_vecPaperModelInfo.clear();

	CModelInfoDlg dlg;
	if (dlg.DoModal() != IDOK)
		return;

	GetDlgItem(IDC_BTN_New)->EnableWindow(FALSE);
	m_nModelPicNums = atoi(T2A(dlg.m_strPaperNum));
	InitTab();
	m_cpListCtrl.DeleteAllItems();

	m_bNewModelFlag = true;
	m_pModel = new MODEL;
	m_pModel->nABModel = dlg.m_bABPaperModel;
	m_pModel->nHasHead = dlg.m_bHasHead;
	InitConf();
}

void CMakeModelDlg::OnBnClickedBtnSelpic()
{
	CFileDialog dlg(true, _T("*.bmp"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
		_T("image file(*.bmp;*.png;*.tif;*.tiff;*.jpg)|*.bmp;*.png;*.tif;*.tiff;*.jpg|All Files(*.*)|*.*|"), NULL);
	dlg.m_ofn.lpstrTitle = _T("选择模板图像");
	if (dlg.DoModal() != IDOK)
		return;

	pPaperModelInfo paperMode = NULL;
	if (m_nCurrTabSel < m_vecPaperModelInfo.size())
	{
		paperMode = m_vecPaperModelInfo[m_nCurrTabSel];
	}
	else
	{
		paperMode = new PaperModelInfo;
		m_vecPaperModelInfo.push_back(paperMode);
	}

// 	pPaperModelInfo paperMode = new PaperModelInfo;
// 	m_vecPaperModelInfo.push_back(paperMode);
	paperMode->strModelPicName = dlg.GetFileName();
	paperMode->strModelPicPath = dlg.GetPathName();

	Mat src_img;
	paperMode->matSrcImg = imread((std::string)(CT2CA)paperMode->strModelPicPath);
	paperMode->matDstImg = paperMode->matSrcImg;
	src_img = paperMode->matDstImg;
	m_pModelPicShow->ShowPic(src_img);
//	PaintRecognisedRect();
	ShowRectByCPType(m_eCurCPType);

// 	if (m_vecPaperModelInfo.size() > 0)
// 	{
// 		CFileDialog dlg(true, _T("*.bmp"), NULL, OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY,
// 			_T("image file(*.bmp;*.png;*.tif;*.tiff;*.jpg)|*.bmp;*.png;*.tif;*.tiff;*.jpg|All Files(*.*)|*.*|"), NULL);
// 		dlg.m_ofn.lpstrTitle = _T("选择模板图像");
// 		if (dlg.DoModal() != IDOK)
// 			return;
// 		CString strPicPath = dlg.GetPathName();
// 		strPicPath.Replace(_T("//"), _T("////"));
// 		m_strModelPicPath = strPicPath;
// 		m_vecPaperModelInfo[m_nCurrTabSel].strModelPicName = dlg.GetFileName();
// 
// 		Mat src_img;
// 		m_vecPaperModelInfo[m_nCurrTabSel].matSrcImg = imread((std::string)(CT2CA)strPicPath);
// 		m_vecPaperModelInfo[m_nCurrTabSel].matDstImg = m_vecPaperModelInfo[m_nCurrTabSel].matSrcImg;
// 		m_pModelPicShow->ShowPic(src_img);
// 		PaintRecognisedRect();
// 	}
// 	else
// 	{
// 		AfxMessageBox(_T("模板为空，请新建模板！"));
// 	}
}

void CMakeModelDlg::OnBnClickedBtnReset()
{
	if (m_vecPaperModelInfo.size() <= 0 || m_vecPaperModelInfo.size() <= m_nCurrTabSel)
		return;

	cv::Mat displayImg = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg;
	m_pModelPicShow->ShowPic(displayImg);

	m_cpListCtrl.DeleteAllItems();
	switch (m_eCurCPType)
	{
	case UNKNOWN:
	case Fix_CP:
		if (m_eCurCPType == Fix_CP || m_eCurCPType == UNKNOWN)
		{
			m_vecPaperModelInfo[m_nCurrTabSel]->vecRtSel.clear();
			if (m_pModel && m_pModel->vecPaperModel.size()) m_pModel->vecPaperModel[m_nCurrTabSel].lFix.clear();
			if (m_pModel && m_pModel->vecPaperModel.size())m_pModel->vecPaperModel[m_nCurrTabSel].lSelROI.clear();
		}
	case H_HEAD:
		if (m_eCurCPType == H_HEAD || m_eCurCPType == UNKNOWN)
		{
			m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.clear();
			if (m_pModel && m_pModel->vecPaperModel.size()) m_pModel->vecPaperModel[m_nCurrTabSel].lH_Head.clear();
		}
	case V_HEAD:
		if (m_eCurCPType == V_HEAD || m_eCurCPType == UNKNOWN)
		{
			m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head.clear();
			if (m_pModel && m_pModel->vecPaperModel.size()) m_pModel->vecPaperModel[m_nCurrTabSel].lV_Head.clear();
		}
	case ABMODEL:
		if (m_eCurCPType == ABMODEL || m_eCurCPType == UNKNOWN)
		{
			m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel.clear();
			if (m_pModel && m_pModel->vecPaperModel.size()) m_pModel->vecPaperModel[m_nCurrTabSel].lABModel.clear();
		}
	case COURSE:
		if (m_eCurCPType == COURSE || m_eCurCPType == UNKNOWN)
		{
			m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse.clear();
			if (m_pModel && m_pModel->vecPaperModel.size()) m_pModel->vecPaperModel[m_nCurrTabSel].lCourse.clear();
		}
	case QK_CP:
		if (m_eCurCPType == QK_CP || m_eCurCPType == UNKNOWN)
		{
			m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP.clear();
			if (m_pModel && m_pModel->vecPaperModel.size()) m_pModel->vecPaperModel[m_nCurrTabSel].lQK_CP.clear();
		}
	case GRAY_CP:
		if (m_eCurCPType == GRAY_CP || m_eCurCPType == UNKNOWN)
		{
			m_vecPaperModelInfo[m_nCurrTabSel]->vecGray.clear();
			if (m_pModel && m_pModel->vecPaperModel.size()) m_pModel->vecPaperModel[m_nCurrTabSel].lGray.clear();
		}
	case WHITE_CP:
		if (m_eCurCPType == WHITE_CP || m_eCurCPType == UNKNOWN)
		{
			m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite.clear();
			if (m_pModel && m_pModel->vecPaperModel.size()) m_pModel->vecPaperModel[m_nCurrTabSel].lWhite.clear();
		}
	}
//	m_vecPaperModelInfo[m_nCurrTabSel]->vecRtSel.clear();
	m_vecPaperModelInfo[m_nCurrTabSel]->vecRtRecognition.clear();
	m_vecPaperModelInfo[m_nCurrTabSel]->vecOmr.clear();


	if (m_pModel)
	{
		if (m_pModel->vecPaperModel.size() > 0)
		{
			m_pModel->vecPaperModel[m_nCurrTabSel].lCheckPoint.clear();
//			m_pModel->vecPaperModel[m_nCurrTabSel].lSelROI.clear();
			m_pModel->vecPaperModel[m_nCurrTabSel].lOMR.clear();
		}
	}
}

bool CMakeModelDlg::RecogNewGrayValue(cv::Mat& matSrcRoi, RECTINFO& rc)
{
	cv::cvtColor(matSrcRoi, matSrcRoi, CV_BGR2GRAY);

	return RecogGrayValue(matSrcRoi, rc);
}

inline bool CMakeModelDlg::RecogGrayValue(cv::Mat& matSrcRoi, RECTINFO& rc)
{
	const int channels[1] = { 0 };
	const float* ranges[1];
	const int histSize[1] = { 1 };
	float hranges[2];
	if (rc.eCPType != WHITE_CP)
	{
		hranges[0] = 0;
		hranges[1] = rc.nThresholdValue;
		ranges[0] = hranges;
	}
	else
	{
		hranges[0] = rc.nThresholdValue;
		hranges[1] = 255;
		ranges[0] = hranges;
	}
	MatND src_hist;
	cv::calcHist(&matSrcRoi, 1, channels, Mat(), src_hist, 1, histSize, ranges, false);

	rc.fStandardValue = src_hist.at<float>(0);
	return true;
}

inline void CMakeModelDlg::GetThreshold(cv::Mat& matSrc, cv::Mat& matDst)
{
	switch (m_eCurCPType)
	{
	case Fix_CP:
	case H_HEAD:
	case V_HEAD:
	case ABMODEL:
	case COURSE:
	case QK_CP:		threshold(matSrc, matDst, 60, 255, THRESH_BINARY); break;
	case GRAY_CP: 
	case WHITE_CP:
	default:
		// 局部自适应阈值的图像二值化
		int blockSize = 25;		//25
		int constValue = 10;
		cv::Mat local;
		cv::adaptiveThreshold(matSrc, matDst, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, blockSize, constValue);
		break;
	}
}

bool CMakeModelDlg::Recognise(cv::Rect rtOri)
{
	clock_t start, end;
	start = clock();
	if (m_vecPaperModelInfo.size() <= m_nCurrTabSel)
		return false;

	Mat imgResult = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg(rtOri); 

	cvtColor(imgResult, imgResult, CV_BGR2GRAY);
	GaussianBlur(imgResult, imgResult, cv::Size(m_nGaussKernel, m_nGaussKernel), 0, 0);
	sharpenImage1(imgResult, imgResult);
#if 1
	GetThreshold(imgResult, imgResult);
#else
	// 局部自适应阈值的图像二值化
	int blockSize = 25;		//25
	int constValue = 10;
	cv::Mat local;
	cv::adaptiveThreshold(imgResult, imgResult, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, blockSize, constValue);
#endif
	cv::Canny(imgResult, imgResult, 0, m_nCannyKernel, 5);
	Mat element = getStructuringElement(MORPH_RECT, Size(m_nDelateKernel, m_nDelateKernel));	//Size(6, 6)	普通空白框可识别
	dilate(imgResult, imgResult, element);

 //	std::vector<std::vector<cv::Point> > vecContours;		//轮廓信息存储
	m_vecContours.clear();
	cv::findContours(imgResult.clone(), m_vecContours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);	//hsvRe.clone()		//CV_RETR_EXTERNAL	//CV_CHAIN_APPROX_SIMPLE

	bool bResult = false;
	std::vector<Rect>RectCompList;
	for (int i = 0; i < m_vecContours.size(); i++)
	{
		Rect rm = cv::boundingRect(cv::Mat(m_vecContours[i]));

		if (rm.width < 10 || rm.height < 7 || rm.width > 70 || rm.height > 50||rm.area() < 70)
		{
//			TRACE("*****Rect %d x = %d,y = %d, width = %d, high = %d \n", i, rm.x, rm.y, rm.width, rm.height);
			continue;
		}
		
		rm.x = rm.x + rtOri.x/* - m_ptFixCP.x*/;
		rm.y = rm.y + rtOri.y/* - m_ptFixCP.y*/;

		RectCompList.push_back(rm);

		RECTINFO rc;
		rc.rt = rm;
		rc.eCPType = m_eCurCPType;
		RECTINFO rcOri;
		rcOri.rt = rtOri;
		
		if (m_eCurCPType == H_HEAD)
		{
			rc.nThresholdValue = m_nHeadVal;
			rc.fStandardValuePercent = m_fHeadThresholdPercent;

			Rect rtTmp = rm;
			Mat matSrcModel = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg(rtTmp);
			RecogGrayValue(matSrcModel, rc);

			m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.push_back(rc);
		}
		else if (m_eCurCPType == V_HEAD)
		{
			rc.nThresholdValue = m_nHeadVal;
			rc.fStandardValuePercent = m_fHeadThresholdPercent;

			Rect rtTmp = rm;
			Mat matSrcModel = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg(rtTmp);
			RecogGrayValue(matSrcModel, rc);

			m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head.push_back(rc);
		}
		else if (m_eCurCPType == ABMODEL)
		{
			rc.nThresholdValue = m_nABModelVal;
			rc.fStandardValuePercent = m_fABModelThresholdPercent;

			Rect rtTmp = rm;
			Mat matSrcModel = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg(rtTmp);
			RecogGrayValue(matSrcModel, rc);

			m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel.push_back(rc);
		}
		else if (m_eCurCPType == COURSE)
		{
			rc.nThresholdValue = m_nCourseVal;
			rc.fStandardValuePercent = m_fCourseThresholdPercent;

			Rect rtTmp = rm;
			Mat matSrcModel = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg(rtTmp);
			RecogGrayValue(matSrcModel, rc);

			m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse.push_back(rc);
		}
		else if (m_eCurCPType == QK_CP)
		{
			rc.nThresholdValue = m_nQK_CPVal;
			rc.fStandardValuePercent = m_fQK_CPThresholdPercent;

			Rect rtTmp = rm;
			Mat matSrcModel = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg(rtTmp);
			RecogGrayValue(matSrcModel, rc);

			m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP.push_back(rc);
		}
		else if (m_eCurCPType == GRAY_CP)
		{
			rc.nThresholdValue = m_nGrayVal;
			rc.fStandardValuePercent = m_fGrayThresholdPercent;

			Rect rtTmp = rm;
			Mat matSrcModel = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg(rtTmp);
			RecogGrayValue(matSrcModel, rc);

			m_vecPaperModelInfo[m_nCurrTabSel]->vecGray.push_back(rc);
		}
		else if (m_eCurCPType == WHITE_CP)
		{
			rc.nThresholdValue = m_nWhiteVal;
			rc.fStandardValuePercent = m_fWhiteThresholdPercent;

			Rect rtTmp = rm;
			Mat matSrcModel = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg(rtTmp);
			RecogGrayValue(matSrcModel, rc);

			m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite.push_back(rc);
		}
// 		else if (m_eCurCPType == Fix_CP)
// 		{
// 			rc.eCPType = Fix_CP;				//把定位点选择的矩形框存入，在扫描匹配的时候根据这个框来识别定点坐标
// 			m_vecPaperModelInfo[m_nCurrTabSel]->vecRtSel.push_back(rcOri);
// 		}
//		m_vecPaperModelInfo[m_nCurrTabSel]->vecRtRecognition.push_back(rc);
//		m_vecPaperModelInfo[m_nCurrTabSel]->vecRtSel.push_back(rcOri);

		bResult = true;	
	}
	if (m_eCurCPType == Fix_CP)
	{
		if (RectCompList.size() > 0)
		{
			m_ptFixCP = cv::Point(0, 0);
			std::sort(RectCompList.begin(), RectCompList.end(), SortByArea);
			
			RECTINFO rcFixSel;					//把定位点选择的矩形框存入，在扫描匹配的时候根据这个框来识别定点坐标
			rcFixSel.eCPType = m_eCurCPType;
			rcFixSel.rt = rtOri;
			RECTINFO rcFixRt;					//把定位点选择的矩形框存入，在扫描匹配的时候根据这个框来识别定点坐标
			rcFixRt.eCPType = m_eCurCPType;
			rcFixRt.rt = RectCompList[0];

			Rect rtTmp = RectCompList[0];
// 			rtTmp.x = rtTmp.x + m_ptFixCP.x;
// 			rtTmp.y = rtTmp.y + m_ptFixCP.y;
			Mat matSrcModel = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg(rtTmp);
			rcFixRt.nThresholdValue = m_nFixVal;
			rcFixRt.fStandardValuePercent = m_fFixThresholdPercent;
			RecogGrayValue(matSrcModel, rcFixRt);

			m_vecPaperModelInfo[m_nCurrTabSel]->vecRtSel.push_back(rcFixSel);
			m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix.push_back(rcFixRt);
// 			m_ptFixCP.x = RectCompList[0].x + RectCompList[0].width / 2 + 0.5;
// 			m_ptFixCP.y = RectCompList[0].y + RectCompList[0].height / 2 + 0.5;
		}
	}
	end = clock();
//	PaintRecognisedRect();
	ShowRectByCPType(m_eCurCPType);

//	end = clock();
	char szLog[200] = { 0 };
	sprintf_s(szLog, "Recognise time: %d, find rect counts: %d.\n", end - start, RectCompList.size());
	g_pLogger->information(szLog);
	TRACE(szLog);
	return bResult;
}

void CMakeModelDlg::sharpenImage1(const cv::Mat &image, cv::Mat &result)
{
	//创建并初始化滤波模板
	cv::Mat kernel(3, 3, CV_32F, cv::Scalar(0));
	kernel.at<float>(1, 1) = m_nSharpKernel;
	kernel.at<float>(0, 1) = -1.0;
	kernel.at<float>(1, 0) = -1.0;
	kernel.at<float>(1, 2) = -1.0;
	kernel.at<float>(2, 1) = -1.0;

	result.create(image.size(), image.type());

	//对图像进行滤波
	cv::filter2D(image, result, image.depth(), kernel);
}

void CMakeModelDlg::PaintRecognisedRect()
{
	Mat tmp = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg.clone();
	Mat tmp2 = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg.clone();
	for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecRtRecognition.size(); i++)
	{
		char szCP[20] = { 0 };
		sprintf_s(szCP, "CP%d", i);
//		string words = szCP;
		putText(tmp, szCP, Point(m_vecPaperModelInfo[m_nCurrTabSel]->vecRtRecognition[i].rt.x, m_vecPaperModelInfo[m_nCurrTabSel]->vecRtRecognition[i].rt.y + m_vecPaperModelInfo[m_nCurrTabSel]->vecRtRecognition[i].rt.height / 2), CV_FONT_HERSHEY_PLAIN, 1, Scalar(255, 0, 0));	//CV_FONT_HERSHEY_COMPLEX
		rectangle(tmp, m_vecPaperModelInfo[m_nCurrTabSel]->vecRtRecognition[i].rt, CV_RGB(255, 0, 0), 2);
		rectangle(tmp2, m_vecPaperModelInfo[m_nCurrTabSel]->vecRtRecognition[i].rt, CV_RGB(255, 233, 10), -1);
	}
	cv::addWeighted(tmp, 0.5, tmp2, 0.5, 0, tmp);
	m_pModelPicShow->ShowPic(tmp);
}

void CMakeModelDlg::OnBnClickedBtnSave()
{
	if (!m_pModel || m_bNewModelFlag)
	{
		CModelSaveDlg dlg;
		if (dlg.DoModal() != IDOK)
			return;

		if (!m_bNewModelFlag)
			m_pModel = new MODEL;

		m_pModel->strModelName = dlg.m_strModelName;

		m_bSavedModelFlag = true;
		CString strTitle = _T("");
		strTitle.Format(_T("模板名称: %s"), m_pModel->strModelName);
		SetWindowText(strTitle);
	}

	if (!m_pModel)
		return;

	m_pModel->vecPaperModel.clear();
	m_pModel->nPicNum = m_vecPaperModelInfo.size();
	for (int i = 0; i < m_pModel->nPicNum; i++)
	{
		PAPERMODEL paperModel;
		paperModel.strModelPicName = m_vecPaperModelInfo[i]->strModelPicName;

		for (int j = 0; j < m_vecPaperModelInfo[i]->vecRtSel.size(); j++)
			paperModel.lSelROI.push_back(m_vecPaperModelInfo[i]->vecRtSel[j]);
		for (int j = 0; j < m_vecPaperModelInfo[i]->vecRtFix.size(); j++)
			paperModel.lFix.push_back(m_vecPaperModelInfo[i]->vecRtFix[j]);
		for (int j = 0; j < m_vecPaperModelInfo[i]->vecRtRecognition.size(); j++)
			paperModel.lCheckPoint.push_back(m_vecPaperModelInfo[i]->vecRtRecognition[j]);
		for (int j = 0; j < m_vecPaperModelInfo[i]->vecOmr.size(); j++)
			paperModel.lOMR.push_back(m_vecPaperModelInfo[i]->vecOmr[j]);
		for (int j = 0; j < m_vecPaperModelInfo[i]->vecH_Head.size(); j++)
			paperModel.lH_Head.push_back(m_vecPaperModelInfo[i]->vecH_Head[j]);
		for (int j = 0; j < m_vecPaperModelInfo[i]->vecV_Head.size(); j++)
			paperModel.lV_Head.push_back(m_vecPaperModelInfo[i]->vecV_Head[j]);
		for (int j = 0; j < m_vecPaperModelInfo[i]->vecABModel.size(); j++)
			paperModel.lABModel.push_back(m_vecPaperModelInfo[i]->vecABModel[j]);
		for (int j = 0; j < m_vecPaperModelInfo[i]->vecCourse.size(); j++)
			paperModel.lCourse.push_back(m_vecPaperModelInfo[i]->vecCourse[j]);
		for (int j = 0; j < m_vecPaperModelInfo[i]->vecQK_CP.size(); j++)
			paperModel.lQK_CP.push_back(m_vecPaperModelInfo[i]->vecQK_CP[j]);
		for (int j = 0; j < m_vecPaperModelInfo[i]->vecGray.size(); j++)
			paperModel.lGray.push_back(m_vecPaperModelInfo[i]->vecGray[j]);
		for (int j = 0; j < m_vecPaperModelInfo[i]->vecWhite.size(); j++)
			paperModel.lWhite.push_back(m_vecPaperModelInfo[i]->vecWhite[j]);

		m_pModel->vecPaperModel.push_back(paperModel);
	}
	USES_CONVERSION;
	CString modelPath = g_strCurrentPath + _T("Model");
	modelPath = modelPath + _T("\\") + m_pModel->strModelName;
	SaveModelFile(m_pModel);
	ZipFile(modelPath, modelPath);
	AfxMessageBox(_T("保存完成!"));
}

bool CMakeModelDlg::SaveModelFile(pMODEL pModel)
{
	USES_CONVERSION;
	CString modelPath = g_strCurrentPath + _T("Model");
	DWORD dwAttr = GetFileAttributesA(T2A(modelPath));
	if (dwAttr == 0xFFFFFFFF)
	{
		CreateDirectoryA(T2A(modelPath), NULL);
	}
	modelPath = modelPath + _T("\\") + pModel->strModelName;
	dwAttr = GetFileAttributesA(T2A(modelPath));
	if (dwAttr == 0xFFFFFFFF)
	{
		CreateDirectoryA(T2A(modelPath), NULL);
	}

	Poco::JSON::Object jsnModel;
	Poco::JSON::Array  jsnPicModel;
	Poco::JSON::Array  jsnPaperModel;				//卷形模板，AB卷用
	for (int i = 0; i < pModel->nPicNum; i++)
	{
		Poco::JSON::Object jsnPaperObj;
		CString strOldPath = m_vecPaperModelInfo[i]->strModelPicPath;
		try
		{
			std::string strUtf8OldPath = CMyCodeConvert::Gb2312ToUtf8(T2A(strOldPath));
			std::string strUtf8ModelPath = CMyCodeConvert::Gb2312ToUtf8(T2A(modelPath));

			Poco::File modelPicPath(strUtf8OldPath);	//T2A(strOldPath)
			modelPicPath.copyTo(strUtf8ModelPath);	//T2A(modelPath)
		}
		catch (Poco::Exception &exc)
		{
			std::string strLog;
			strLog.append("file cope error: " + exc.displayText());
			g_pLogger->information(strLog);
			TRACE("file cope error: %s\n", exc.displayText().c_str());
		}

		CString strPicName = pModel->vecPaperModel[i].strModelPicName;

		Poco::JSON::Array jsnSelRoiArry;
		Poco::JSON::Array jsnSelCPArry;
		Poco::JSON::Array jsnOMRArry;
		Poco::JSON::Array jsnFixCPArry;
		Poco::JSON::Array jsnHHeadArry;
		Poco::JSON::Array jsnVHeadArry;
		Poco::JSON::Array jsnABModelArry;
		Poco::JSON::Array jsnCourseArry;
		Poco::JSON::Array jsnQKArry;
		Poco::JSON::Array jsnGrayCPArry;
		Poco::JSON::Array jsnWhiteCPArry;
		RECTLIST::iterator itFix = pModel->vecPaperModel[i].lFix.begin();
		for (; itFix != pModel->vecPaperModel[i].lFix.end(); itFix++)
		{
			Poco::JSON::Object jsnObj;
			jsnObj.set("eType", (int)itFix->eCPType);
			jsnObj.set("left", itFix->rt.x);
			jsnObj.set("top", itFix->rt.y);
			jsnObj.set("width", itFix->rt.width);
			jsnObj.set("height", itFix->rt.height);
			jsnObj.set("thresholdValue", itFix->nThresholdValue);
			jsnObj.set("standardValPercent", itFix->fStandardValuePercent);
			jsnObj.set("standardVal", itFix->fStandardValue);
			jsnFixCPArry.add(jsnObj);
		}
		RECTLIST::iterator itHHead = pModel->vecPaperModel[i].lH_Head.begin();
		for (; itHHead != pModel->vecPaperModel[i].lH_Head.end(); itHHead++)
		{
			Poco::JSON::Object jsnObj;
			jsnObj.set("eType", (int)itHHead->eCPType);
			jsnObj.set("left", itHHead->rt.x);
			jsnObj.set("top", itHHead->rt.y);
			jsnObj.set("width", itHHead->rt.width);
			jsnObj.set("height", itHHead->rt.height);
			jsnObj.set("thresholdValue", itHHead->nThresholdValue);
			jsnObj.set("standardValPercent", itHHead->fStandardValuePercent);
			jsnObj.set("standardVal", itHHead->fStandardValue);
			jsnHHeadArry.add(jsnObj);
		}
		RECTLIST::iterator itVHead = pModel->vecPaperModel[i].lV_Head.begin();
		for (; itVHead != pModel->vecPaperModel[i].lV_Head.end(); itVHead++)
		{
			Poco::JSON::Object jsnObj;
			jsnObj.set("eType", (int)itVHead->eCPType);
			jsnObj.set("left", itVHead->rt.x);
			jsnObj.set("top", itVHead->rt.y);
			jsnObj.set("width", itVHead->rt.width);
			jsnObj.set("height", itVHead->rt.height);
			jsnObj.set("thresholdValue", itVHead->nThresholdValue);
			jsnObj.set("standardValPercent", itVHead->fStandardValuePercent);
			jsnObj.set("standardVal", itVHead->fStandardValue);
			jsnVHeadArry.add(jsnObj);
		}
		RECTLIST::iterator itABModel = pModel->vecPaperModel[i].lABModel.begin();
		for (; itABModel != pModel->vecPaperModel[i].lABModel.end(); itABModel++)
		{
			Poco::JSON::Object jsnObj;
			jsnObj.set("eType", (int)itABModel->eCPType);
			jsnObj.set("left", itABModel->rt.x);
			jsnObj.set("top", itABModel->rt.y);
			jsnObj.set("width", itABModel->rt.width);
			jsnObj.set("height", itABModel->rt.height);
			jsnObj.set("hHeadItem", itABModel->nHItem);
			jsnObj.set("vHeadItem", itABModel->nVItem);
			jsnObj.set("thresholdValue", itABModel->nThresholdValue);
			jsnObj.set("standardValPercent", itABModel->fStandardValuePercent);
			jsnObj.set("standardVal", itABModel->fStandardValue);
			jsnABModelArry.add(jsnObj);
		}
		RECTLIST::iterator itCourse = pModel->vecPaperModel[i].lCourse.begin();
		for (; itCourse != pModel->vecPaperModel[i].lCourse.end(); itCourse++)
		{
			Poco::JSON::Object jsnObj;
			jsnObj.set("eType", (int)itCourse->eCPType);
			jsnObj.set("left", itCourse->rt.x);
			jsnObj.set("top", itCourse->rt.y);
			jsnObj.set("width", itCourse->rt.width);
			jsnObj.set("height", itCourse->rt.height);
			jsnObj.set("hHeadItem", itCourse->nHItem);
			jsnObj.set("vHeadItem", itCourse->nVItem);
			jsnObj.set("thresholdValue", itCourse->nThresholdValue);
			jsnObj.set("standardValPercent", itCourse->fStandardValuePercent);
			jsnObj.set("standardVal", itCourse->fStandardValue);
			jsnCourseArry.add(jsnObj);
		}
		RECTLIST::iterator itQKCP = pModel->vecPaperModel[i].lQK_CP.begin();
		for (; itQKCP != pModel->vecPaperModel[i].lQK_CP.end(); itQKCP++)
		{
			Poco::JSON::Object jsnObj;
			jsnObj.set("eType", (int)itQKCP->eCPType);
			jsnObj.set("left", itQKCP->rt.x);
			jsnObj.set("top", itQKCP->rt.y);
			jsnObj.set("width", itQKCP->rt.width);
			jsnObj.set("height", itQKCP->rt.height);
			jsnObj.set("hHeadItem", itQKCP->nHItem);
			jsnObj.set("vHeadItem", itQKCP->nVItem);
			jsnObj.set("thresholdValue", itQKCP->nThresholdValue);
			jsnObj.set("standardValPercent", itQKCP->fStandardValuePercent);
			jsnObj.set("standardVal", itQKCP->fStandardValue);
			jsnQKArry.add(jsnObj);
		}
		RECTLIST::iterator itGrayCP = pModel->vecPaperModel[i].lGray.begin();
		for (; itGrayCP != pModel->vecPaperModel[i].lGray.end(); itGrayCP++)
		{
			Poco::JSON::Object jsnObj;
			jsnObj.set("eType", (int)itGrayCP->eCPType);
			jsnObj.set("left", itGrayCP->rt.x);
			jsnObj.set("top", itGrayCP->rt.y);
			jsnObj.set("width", itGrayCP->rt.width);
			jsnObj.set("height", itGrayCP->rt.height);
			jsnObj.set("hHeadItem", itGrayCP->nHItem);
			jsnObj.set("vHeadItem", itGrayCP->nVItem);
			jsnObj.set("thresholdValue", itGrayCP->nThresholdValue);
			jsnObj.set("standardValPercent", itGrayCP->fStandardValuePercent);
			jsnObj.set("standardVal", itGrayCP->fStandardValue);
			jsnGrayCPArry.add(jsnObj);
		}
		RECTLIST::iterator itWhiteCP = pModel->vecPaperModel[i].lWhite.begin();
		for (; itWhiteCP != pModel->vecPaperModel[i].lWhite.end(); itWhiteCP++)
		{
			Poco::JSON::Object jsnObj;
			jsnObj.set("eType", (int)itWhiteCP->eCPType);
			jsnObj.set("left", itWhiteCP->rt.x);
			jsnObj.set("top", itWhiteCP->rt.y);
			jsnObj.set("width", itWhiteCP->rt.width);
			jsnObj.set("height", itWhiteCP->rt.height);
			jsnObj.set("hHeadItem", itWhiteCP->nHItem);
			jsnObj.set("vHeadItem", itWhiteCP->nVItem);
			jsnObj.set("thresholdValue", itWhiteCP->nThresholdValue);
			jsnObj.set("standardValPercent", itWhiteCP->fStandardValuePercent);
			jsnObj.set("standardVal", itWhiteCP->fStandardValue);
			jsnWhiteCPArry.add(jsnObj);
		}
		RECTLIST::iterator itSelRoi = pModel->vecPaperModel[i].lSelROI.begin();
		for (; itSelRoi != pModel->vecPaperModel[i].lSelROI.end(); itSelRoi++)
		{
			Poco::JSON::Object jsnObj;
			jsnObj.set("eType", (int)itSelRoi->eCPType);
			jsnObj.set("left", itSelRoi->rt.x);
			jsnObj.set("top", itSelRoi->rt.y);
			jsnObj.set("width", itSelRoi->rt.width);
			jsnObj.set("height", itSelRoi->rt.height);
			jsnObj.set("thresholdValue", itSelRoi->nThresholdValue);
			jsnObj.set("standardValPercent", itSelRoi->fStandardValuePercent);
//			jsnObj.set("standardVal", itSelRoi->fStandardValue);
			jsnSelRoiArry.add(jsnObj);
		}
		RECTLIST::iterator itCP = pModel->vecPaperModel[i].lCheckPoint.begin();
		for (; itCP != pModel->vecPaperModel[i].lCheckPoint.end(); itCP++)
		{
			Poco::JSON::Object jsnObj;
			jsnObj.set("eType", (int)itCP->eCPType);
			jsnObj.set("left", itCP->rt.x);
			jsnObj.set("top", itCP->rt.y);
			jsnObj.set("width", itCP->rt.width);
			jsnObj.set("height", itCP->rt.height);
			jsnObj.set("thresholdValue", itCP->nThresholdValue);
			jsnObj.set("standardValPercent", itCP->fStandardValuePercent);
			jsnObj.set("standardVal", itCP->fStandardValue);
			jsnSelCPArry.add(jsnObj);
		}
		RECTLIST::iterator itOmr = pModel->vecPaperModel[i].lOMR.begin();
		for (; itOmr != pModel->vecPaperModel[i].lOMR.end(); itSelRoi++)
		{
			Poco::JSON::Object jsnObj;
			jsnObj.set("eType", (int)itOmr->eCPType);
			jsnObj.set("left", itOmr->rt.x);
			jsnObj.set("top", itOmr->rt.y);
			jsnObj.set("width", itOmr->rt.width);
			jsnObj.set("height", itOmr->rt.height);
			jsnObj.set("hHeadItem", itOmr->nHItem);
			jsnObj.set("vHeadItem", itOmr->nVItem);
			jsnObj.set("thresholdValue", itOmr->nThresholdValue);
			jsnObj.set("standardValPercent", itOmr->fStandardValuePercent);
			jsnObj.set("standardVal", itOmr->fStandardValue);
			jsnOMRArry.add(jsnObj);
		}
		jsnPaperObj.set("paperNum", i);
		jsnPaperObj.set("modelPicName", CMyCodeConvert::Gb2312ToUtf8(T2A(strPicName)));		//CMyCodeConvert::Gb2312ToUtf8(T2A(strPicName))
		jsnPaperObj.set("FixCP", jsnFixCPArry);
		jsnPaperObj.set("H_Head", jsnHHeadArry);
		jsnPaperObj.set("V_Head", jsnVHeadArry);
		jsnPaperObj.set("ABModel", jsnABModelArry);
		jsnPaperObj.set("Course", jsnCourseArry);
		jsnPaperObj.set("QKCP", jsnQKArry);
		jsnPaperObj.set("GrayCP", jsnGrayCPArry);
		jsnPaperObj.set("WhiteCP", jsnWhiteCPArry);
		jsnPaperObj.set("selRoiRect", jsnSelRoiArry);
		jsnPaperObj.set("selCPRect", jsnSelCPArry);
		jsnPaperObj.set("selOmrRect", jsnOMRArry);
		jsnPicModel.add(jsnPaperObj);
	}
	
	jsnModel.set("modelName", CMyCodeConvert::Gb2312ToUtf8(T2A(pModel->strModelName)));		//CMyCodeConvert::Gb2312ToUtf8(T2A(pModel->strModelName))
	jsnModel.set("paperModelCount", pModel->nPicNum);			//此模板有几页试卷(图片)
	jsnModel.set("enableModify", 1);							//是否可以修改标识
	jsnModel.set("abPaper", m_pModel->nABModel);				//是否是AB卷					*************	暂时没加入AB卷的模板	**************
	jsnModel.set("hasHead", m_pModel->nHasHead);				//是否有同步头
	jsnModel.set("paperInfo", jsnPicModel);

	std::stringstream jsnString;
	jsnModel.stringify(jsnString, 0);

	std::string strJsnFile = T2A(modelPath);
	strJsnFile += "\\model.dat";
	ofstream out(strJsnFile);
	out << jsnString.str().c_str();
	out.close();
	
	return true;
}

void CMakeModelDlg::OnBnClickedBtnExitmodeldlg()
{
	if (m_bNewModelFlag && !m_bSavedModelFlag)
		SAFE_RELEASE(m_pModel);
	OnOK();
}

void CMakeModelDlg::ShowRectTracker()
{
	if (m_vecPaperModelInfo.size() <= m_nCurrTabSel)
		return;

	Rect rt;
	if (m_eCurCPType == H_HEAD)
	{
		cv::Point pt1 = cv::Point(m_pModelPicShow->m_picShow.m_RectTrackerH.m_rect.left, m_pModelPicShow->m_picShow.m_RectTrackerH.m_rect.top);
		cv::Point pt2 = cv::Point(m_pModelPicShow->m_picShow.m_RectTrackerH.m_rect.right, m_pModelPicShow->m_picShow.m_RectTrackerH.m_rect.bottom);
		rt = cv::Rect(pt1, pt2);
	}
	else if (m_eCurCPType == V_HEAD)
	{
		cv::Point pt1 = cv::Point(m_pModelPicShow->m_picShow.m_RectTrackerV.m_rect.left, m_pModelPicShow->m_picShow.m_RectTrackerV.m_rect.top);
		cv::Point pt2 = cv::Point(m_pModelPicShow->m_picShow.m_RectTrackerV.m_rect.right, m_pModelPicShow->m_picShow.m_RectTrackerV.m_rect.bottom);
		rt = cv::Rect(pt1, pt2);
	}

	Mat tmp = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg.clone();
	Mat tmp2 = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg.clone();

	rectangle(tmp, rt, CV_RGB(255, 120, 150), 2);
	rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);

	cv::addWeighted(tmp, 0.5, tmp2, 0.5, 0, tmp);
	m_pModelPicShow->ShowPic(tmp);
}
void CMakeModelDlg::ShowRectByPoint(cv::Point pt)
{
	int  nFind = 0;

	m_pCurRectInfo = NULL;
	nFind = GetRectInfoByPoint(pt, m_eCurCPType, m_pCurRectInfo);
	if(nFind < 0)
		return;

	if (!m_pCurRectInfo)
		return;

	ShowDetailRectInfo();

	Rect rt = m_pCurRectInfo->rt;
	if (m_pCurRectInfo->eCPType != Fix_CP)
	{
// 		rt.x = rt.x + m_ptFixCP.x;
// 		rt.y = rt.y + m_ptFixCP.y;
	}
	Mat tmp = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg.clone();
	Mat tmp2 = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg.clone();
	
	rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
	rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
	
	cv::addWeighted(tmp, 0.5, tmp2, 0.5, 0, tmp);
	m_pModelPicShow->ShowPic(tmp);
}

void CMakeModelDlg::ShowRectByItem(int nItem)
{
	if (m_vecPaperModelInfo.size() <= m_nCurrTabSel)
		return;

	cv::Rect rt;
	switch (m_eCurCPType)
	{
	case Fix_CP:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix.size() < nItem)
			return;
		rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix[nItem].rt;
		m_pCurRectInfo = &m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix[nItem];
		break;
	case H_HEAD:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.size() < nItem)
			return;
		rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head[nItem].rt;
		m_pCurRectInfo = &m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head[nItem];
		break;
	case V_HEAD:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head.size() < nItem)
			return;
		rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head[nItem].rt;
		m_pCurRectInfo = &m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head[nItem];
		break;
	case ABMODEL:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel.size() < nItem)
			return;
		rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel[nItem].rt;
		m_pCurRectInfo = &m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel[nItem];
		break;
	case COURSE:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse.size() < nItem)
			return;
		rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse[nItem].rt;
		m_pCurRectInfo = &m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse[nItem];
		break;
	case QK_CP:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP.size() < nItem)
			return;
		rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP[nItem].rt;
		m_pCurRectInfo = &m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP[nItem];
		break;
	case GRAY_CP:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecGray.size() < nItem)
			return;
		rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecGray[nItem].rt;
		m_pCurRectInfo = &m_vecPaperModelInfo[m_nCurrTabSel]->vecGray[nItem];
		break;
	case WHITE_CP:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite.size() < nItem)
			return;
		rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite[nItem].rt;
		m_pCurRectInfo = &m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite[nItem];
		break;
	default: return;
	}
	if (m_eCurCPType != Fix_CP)
	{
// 		rt.x = rt.x + m_ptFixCP.x;
// 		rt.y = rt.y + m_ptFixCP.y;
	}
	ShowDetailRectInfo();

	Mat tmp = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg.clone();
	Mat tmp2 = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg.clone();

	rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
	rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);

	cv::addWeighted(tmp, 0.5, tmp2, 0.5, 0, tmp);
	m_pModelPicShow->ShowPic(tmp);
}

void CMakeModelDlg::ShowRectByCPType(CPType eType)
{
	if (!m_pModel || m_vecPaperModelInfo.size() <= m_nCurrTabSel)
		return;

	cv::Rect rt;
	cv::Point ptFix = cv::Point(0, 0);
	if (m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix.size())
	{
		cv::Rect rtFix = m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix[0].rt;
		ptFix = m_ptFixCP;
	}	
	Mat tmp = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg.clone();
	Mat tmp2 = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg.clone();
	switch (eType)
	{
	case UNKNOWN:
	case Fix_CP:
		if (eType == Fix_CP || eType == UNKNOWN)
		{
			for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix.size(); i++)
			{
				rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix[i].rt;
				rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
				rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
			}
		}
	case H_HEAD:
		if (eType == H_HEAD || eType == UNKNOWN)
		{
			for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.size(); i++)
			{
				rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head[i].rt;
// 				rt.x = rt.x + ptFix.x;
// 				rt.y = rt.y + ptFix.y;
				rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
				rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
			}
		}
	case V_HEAD:
		if (eType == V_HEAD || eType == UNKNOWN)
		{
			for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head.size(); i++)
			{
				rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head[i].rt;
// 				rt.x = rt.x + ptFix.x;
// 				rt.y = rt.y + ptFix.y;
				rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
				rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
			}
		}
	case ABMODEL:
		if (eType == ABMODEL || eType == UNKNOWN)
		{
			for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel.size(); i++)
			{
				rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel[i].rt;
// 				rt.x = rt.x + ptFix.x;
// 				rt.y = rt.y + ptFix.y;
				rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
				rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
			}
		}
	case COURSE:
		if (eType == COURSE || eType == UNKNOWN)
		{
			for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse.size(); i++)
			{
				rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse[i].rt;
// 				rt.x = rt.x + ptFix.x;
// 				rt.y = rt.y + ptFix.y;
				rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
				rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
			}
		}
	case QK_CP:
		if (eType == QK_CP || eType == UNKNOWN)
		{
			for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP.size(); i++)
			{
				rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP[i].rt;
// 				rt.x = rt.x + ptFix.x;
// 				rt.y = rt.y + ptFix.y;
				rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
				rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
			}
		}
	case GRAY_CP:
		if (eType == GRAY_CP || eType == UNKNOWN)
		{
			for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecGray.size(); i++)
			{
				rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecGray[i].rt;
// 				rt.x = rt.x + ptFix.x;
// 				rt.y = rt.y + ptFix.y;
				rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
				rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
			}
		}
	case WHITE_CP:
		if (eType == WHITE_CP || eType == UNKNOWN)
		{
			for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite.size(); i++)
			{
				rt = m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite[i].rt;
// 				rt.x = rt.x + ptFix.x;
// 				rt.y = rt.y + ptFix.y;
				rectangle(tmp, rt, CV_RGB(255, 0, 0), 2);
				rectangle(tmp2, rt, CV_RGB(255, 233, 10), -1);
			}
		}
		break;
	default: return;
	}
	cv::addWeighted(tmp, 0.5, tmp2, 0.5, 0, tmp);
	m_pModelPicShow->ShowPic(tmp);
}

CPType CMakeModelDlg::GetComboSelCpType()
{
	CPType eType = UNKNOWN;
	CString strCheckPoint = _T("");
	m_comboCheckPointType.GetLBText(m_ncomboCurrentSel, strCheckPoint);
	if (strCheckPoint == "")
		eType = UNKNOWN;
	else if (strCheckPoint == "定点")
		eType = Fix_CP;
	else if (strCheckPoint == "水平同步头")
		eType = H_HEAD;
	else if (strCheckPoint == "垂直同步头")
		eType = V_HEAD;
	else if (strCheckPoint == "卷形校验点")
		eType = ABMODEL;
	else if (strCheckPoint == "科目校验点")
		eType = COURSE;
	else if (strCheckPoint == "缺考校验点")
		eType = QK_CP;
	else if (strCheckPoint == "灰度校验点")
		eType = GRAY_CP;
	else if (strCheckPoint == "空白校验点")
		eType = WHITE_CP;
	return eType;
}

void CMakeModelDlg::OnCbnSelchangeComboCptype()
{
	if (m_ncomboCurrentSel == m_comboCheckPointType.GetCurSel())
		return;

	m_ncomboCurrentSel	= m_comboCheckPointType.GetCurSel();
	m_eCurCPType		= GetComboSelCpType();
	UpdataCPList();
	ShowRectByCPType(m_eCurCPType);

//	ShowRectTracker();
}

void CMakeModelDlg::UpdataCPList()
{
	if (!m_pModel || m_vecPaperModelInfo.size() <= m_nCurrTabSel)
		return;

	USES_CONVERSION;
	int nCount = 0;
	m_cpListCtrl.DeleteAllItems();
	if (m_eCurCPType == H_HEAD)
	{
		m_pModelPicShow->SetShowTracker(true, false);
		cv::Point pt1 = cv::Point(0, 0);
		cv::Point pt2 = cv::Point(m_vecPaperModelInfo[m_nCurrTabSel]->matSrcImg.cols, 50);
		m_pModelPicShow->m_picShow.setHTrackerPosition(pt1, pt2);
	}
	else if (m_eCurCPType == V_HEAD)
	{
		m_pModelPicShow->SetShowTracker(false, true);
		cv::Point pt1 = cv::Point(0, 0);
		cv::Point pt2 = cv::Point(m_vecPaperModelInfo[m_nCurrTabSel]->matSrcImg.cols - 50, m_vecPaperModelInfo[m_nCurrTabSel]->matSrcImg.rows);
		m_pModelPicShow->m_picShow.setHTrackerPosition(pt1, pt2);
	}
	else
	{
		m_pModelPicShow->SetShowTracker(false, false);
	}

	if (m_eCurCPType == Fix_CP || m_eCurCPType == UNKNOWN)			//当当前类型为UNKNOWN时，显示所有的校验点
	{
		for (int i = nCount; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix.size() + nCount; i++)
		{
			RECTINFO rcInfo = m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix[i - nCount];
			char szPosition[50] = { 0 };
			sprintf_s(szPosition, "(%d,%d,%d,%d)", rcInfo.rt.x, rcInfo.rt.y, rcInfo.rt.width, rcInfo.rt.height);
			char szCount[10] = { 0 };
			sprintf_s(szCount, "%d", i + 1);
			m_cpListCtrl.InsertItem(i, NULL);
			m_cpListCtrl.SetItemText(i, 0, (LPCTSTR)A2T(szCount));
			m_cpListCtrl.SetItemText(i, 1, (LPCTSTR)A2T(szPosition));
		}
		if (m_eCurCPType == UNKNOWN)
			nCount += m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix.size();
	}
	if (m_eCurCPType == H_HEAD || m_eCurCPType == UNKNOWN)			//当当前类型为UNKNOWN时，显示所有的校验点
	{ 
		for (int i = nCount; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.size() + nCount; i++)
		{
			RECTINFO rcInfo = m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head[i - nCount];
			char szPosition[50] = { 0 };
			sprintf_s(szPosition, "(%d,%d,%d,%d)", rcInfo.rt.x, rcInfo.rt.y, rcInfo.rt.width, rcInfo.rt.height);
			char szCount[10] = { 0 };
			sprintf_s(szCount, "%d", i + 1);
			m_cpListCtrl.InsertItem(i, NULL);
			m_cpListCtrl.SetItemText(i, 0, (LPCTSTR)A2T(szCount));
			m_cpListCtrl.SetItemText(i, 1, (LPCTSTR)A2T(szPosition));
		}
		if (m_eCurCPType == UNKNOWN)
			nCount += m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.size();
	}
	if (m_eCurCPType == V_HEAD || m_eCurCPType == UNKNOWN)
	{
		for (int i = nCount; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head.size() + nCount; i++)
		{
			RECTINFO rcInfo = m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head[i - nCount];
			char szPosition[50] = { 0 };
			sprintf_s(szPosition, "(%d,%d,%d,%d)", rcInfo.rt.x, rcInfo.rt.y, rcInfo.rt.width, rcInfo.rt.height);
			char szCount[10] = { 0 };
			sprintf_s(szCount, "%d", i + 1);
			m_cpListCtrl.InsertItem(i, NULL);
			m_cpListCtrl.SetItemText(i, 0, (LPCTSTR)A2T(szCount));
			m_cpListCtrl.SetItemText(i, 1, (LPCTSTR)A2T(szPosition));
		}
		if (m_eCurCPType == UNKNOWN)
			nCount += m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head.size();
	}
	if (m_eCurCPType == ABMODEL || m_eCurCPType == UNKNOWN)
	{
		for (int i = nCount; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel.size() + nCount; i++)
		{
			RECTINFO rcInfo = m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel[i - nCount];
			char szPosition[50] = { 0 };
			sprintf_s(szPosition, "(%d,%d,%d,%d)", rcInfo.rt.x, rcInfo.rt.y, rcInfo.rt.width, rcInfo.rt.height);
			char szCount[10] = { 0 };
			sprintf_s(szCount, "%d", i + 1);
			m_cpListCtrl.InsertItem(i, NULL);
			m_cpListCtrl.SetItemText(i, 0, (LPCTSTR)A2T(szCount));
			m_cpListCtrl.SetItemText(i, 1, (LPCTSTR)A2T(szPosition));
		}
		if (m_eCurCPType == UNKNOWN)
			nCount += m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel.size();
	}
	if (m_eCurCPType == COURSE || m_eCurCPType == UNKNOWN)
	{
		for (int i = nCount; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse.size() + nCount; i++)
		{
			RECTINFO rcInfo = m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse[i - nCount];
			char szPosition[50] = { 0 };
			sprintf_s(szPosition, "(%d,%d,%d,%d)", rcInfo.rt.x, rcInfo.rt.y, rcInfo.rt.width, rcInfo.rt.height);
			char szCount[10] = { 0 };
			sprintf_s(szCount, "%d", i + 1);
			m_cpListCtrl.InsertItem(i, NULL);
			m_cpListCtrl.SetItemText(i, 0, (LPCTSTR)A2T(szCount));
			m_cpListCtrl.SetItemText(i, 1, (LPCTSTR)A2T(szPosition));
		}
		if (m_eCurCPType == UNKNOWN)
			nCount += m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse.size();
	}
	if (m_eCurCPType == QK_CP || m_eCurCPType == UNKNOWN)
	{
		for (int i = nCount; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP.size() + nCount; i++)
		{
			RECTINFO rcInfo = m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP[i - nCount];
			char szPosition[50] = { 0 };
			sprintf_s(szPosition, "(%d,%d,%d,%d)", rcInfo.rt.x, rcInfo.rt.y, rcInfo.rt.width, rcInfo.rt.height);
			char szCount[10] = { 0 };
			sprintf_s(szCount, "%d", i + 1);
			m_cpListCtrl.InsertItem(i, NULL);
			m_cpListCtrl.SetItemText(i, 0, (LPCTSTR)A2T(szCount));
			m_cpListCtrl.SetItemText(i, 1, (LPCTSTR)A2T(szPosition));
		}
		if (m_eCurCPType == UNKNOWN)
			nCount += m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP.size();
	}
	if (m_eCurCPType == GRAY_CP || m_eCurCPType == UNKNOWN)
	{
		for (int i = nCount; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecGray.size() + nCount; i++)
		{
			RECTINFO rcInfo = m_vecPaperModelInfo[m_nCurrTabSel]->vecGray[i - nCount];
			char szPosition[50] = { 0 };
			sprintf_s(szPosition, "(%d,%d,%d,%d)", rcInfo.rt.x, rcInfo.rt.y, rcInfo.rt.width, rcInfo.rt.height);
			char szCount[10] = { 0 };
			sprintf_s(szCount, "%d", i + 1);
			m_cpListCtrl.InsertItem(i, NULL);
			m_cpListCtrl.SetItemText(i, 0, (LPCTSTR)A2T(szCount));
			m_cpListCtrl.SetItemText(i, 1, (LPCTSTR)A2T(szPosition));
		}
		if (m_eCurCPType == UNKNOWN)
			nCount += m_vecPaperModelInfo[m_nCurrTabSel]->vecGray.size();
	}
	if (m_eCurCPType == WHITE_CP || m_eCurCPType == UNKNOWN)
	{
		for (int i = nCount; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite.size() + nCount; i++)
		{
			RECTINFO rcInfo = m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite[i - nCount];
			char szPosition[50] = { 0 };
			sprintf_s(szPosition, "(%d,%d,%d,%d)", rcInfo.rt.x, rcInfo.rt.y, rcInfo.rt.width, rcInfo.rt.height);
			char szCount[10] = { 0 };
			sprintf_s(szCount, "%d", i + 1);
			m_cpListCtrl.InsertItem(i, NULL);
			m_cpListCtrl.SetItemText(i, 0, (LPCTSTR)A2T(szCount));
			m_cpListCtrl.SetItemText(i, 1, (LPCTSTR)A2T(szPosition));
		}
		if (m_eCurCPType == UNKNOWN)
			nCount += m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite.size();
	}
}

void CMakeModelDlg::OnNMRClickListCheckpoint(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	if (m_cpListCtrl.GetSelectedCount() <= 0)
		return;

	if (m_eCurCPType == UNKNOWN)
		return;
	m_nCurListCtrlSel = pNMItemActivate->iItem;
	//下面的这段代码, 不单单适应于ListCtrl  
	CMenu menu, *pPopup;
	menu.LoadMenu(IDR_MENU_RectRecognition);
	pPopup = menu.GetSubMenu(0);
	CPoint myPoint;
	ClientToScreen(&myPoint);
	GetCursorPos(&myPoint); //鼠标位置  
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, myPoint.x, myPoint.y, this);//GetParent()
}

void CMakeModelDlg::DeleteRectInfoOnList()
{
	if (DeleteRectInfo(m_eCurCPType, m_nCurListCtrlSel))
	{
		UpdataCPList();
		ShowRectByCPType(m_eCurCPType);
	}	
}
void CMakeModelDlg::DelRectInfoOnPic()
{
	RECTINFO* pRc = NULL;
	int nFind = 0;
	nFind = GetRectInfoByPoint(m_ptRBtnUp, m_eCurCPType, pRc);
	if (nFind < 0)
		return;
	
	if (pRc && pRc->eCPType != UNKNOWN)
	{
		if (DeleteRectInfo(pRc->eCPType, nFind))
		{
			UpdataCPList();
			ShowRectByCPType(m_eCurCPType);
		}
	}	
}
BOOL CMakeModelDlg::DeleteRectInfo(CPType eType, int nItem)
{
	if (m_vecPaperModelInfo.size() <= m_nCurrTabSel)
		return FALSE;

	std::vector<RECTINFO>::iterator it;
	switch (eType)
	{
	case Fix_CP:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix.size() < 0)
			return FALSE;
		it = m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix.begin() + nItem;
		if (it != m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix.end())
			m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix.erase(it);

		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecRtSel.size() < 0)
			return FALSE;
		it = m_vecPaperModelInfo[m_nCurrTabSel]->vecRtSel.begin() + nItem;
		if (it != m_vecPaperModelInfo[m_nCurrTabSel]->vecRtSel.end())
			m_vecPaperModelInfo[m_nCurrTabSel]->vecRtSel.erase(it);
		break;
	case H_HEAD:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.size() < 0)
			return FALSE;
		it = m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.begin() + nItem;
		if (it != m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.end())
			m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.erase(it);
		break;
	case V_HEAD:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head.size() < 0)
			return FALSE;
		it = m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head.begin() + nItem;
		if (it != m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head.end())
			m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head.erase(it);
		break;
	case ABMODEL:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel.size() < 0)
			return FALSE;
		it = m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel.begin() + nItem;
		if (it != m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel.end())
			m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel.erase(it);
		break;
	case COURSE:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse.size() < 0)
			return FALSE;
		it = m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse.begin() + nItem;
		if (it != m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse.end())
			m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse.erase(it);
		break;
	case QK_CP:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP.size() < 0)
			return FALSE;
		it = m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP.begin() + nItem;
		if (it != m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP.end())
			m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP.erase(it);
		break;
	case GRAY_CP:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecGray.size() < 0)
			return FALSE;
		it = m_vecPaperModelInfo[m_nCurrTabSel]->vecGray.begin() + nItem;
		if (it != m_vecPaperModelInfo[m_nCurrTabSel]->vecGray.end())
			m_vecPaperModelInfo[m_nCurrTabSel]->vecGray.erase(it);
		break;
	case WHITE_CP:
		if (m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite.size() < 0)
			return FALSE;
		it = m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite.begin() + nItem;
		if (it != m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite.end())
			m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite.erase(it);
		break;
	default: return FALSE;
	}
	return TRUE;
}

BOOL CMakeModelDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}
void CMakeModelDlg::OnLvnKeydownListCheckpoint(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	*pResult = 0;
	if (pLVKeyDow->wVKey == VK_UP)
	{
		m_nCurListCtrlSel--;
		if (m_nCurListCtrlSel <= 0)
			m_nCurListCtrlSel = 0;
		ShowRectByItem(m_nCurListCtrlSel);
	}
	else if (pLVKeyDow->wVKey == VK_DOWN)
	{
		m_nCurListCtrlSel++;
		if (m_nCurListCtrlSel >= m_cpListCtrl.GetItemCount() - 1)
			m_nCurListCtrlSel = m_cpListCtrl.GetItemCount() - 1;
		ShowRectByItem(m_nCurListCtrlSel);
	}
}

void CMakeModelDlg::OnNMDblclkListCheckpoint(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	m_nCurListCtrlSel = pNMItemActivate->iItem;
	ShowRectByItem(m_nCurListCtrlSel);
}
bool SortByPosition(RECTINFO& rc1, RECTINFO& rc2)
{
	bool bResult = true;
	bResult = rc1.rt.x < rc2.rt.x ? true : false;
	if (!bResult)
	{
		if (rc1.rt.x == rc2.rt.x)
			bResult = rc1.rt.y < rc2.rt.y ? true : false;
	}
	return bResult;
}
void CMakeModelDlg::SortRect()
{
	if (!m_pModel || m_vecPaperModelInfo.size() == 0)
		return;

	switch (m_eCurCPType)
	{
	case H_HEAD:
		std::sort(m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.begin(), m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.end(), SortByPosition);
		break;
	case V_HEAD:
		std::sort(m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head.begin(), m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head.end(), SortByPosition);
		break;
	case ABMODEL:
		std::sort(m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel.begin(), m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel.end(), SortByPosition);
		break;
	case COURSE:
		std::sort(m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse.begin(), m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse.end(), SortByPosition);
		break;
	case QK_CP:
		std::sort(m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP.begin(), m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP.end(), SortByPosition);
		break;
	case GRAY_CP:
		std::sort(m_vecPaperModelInfo[m_nCurrTabSel]->vecGray.begin(), m_vecPaperModelInfo[m_nCurrTabSel]->vecGray.end(), SortByPosition);
		break;
	case WHITE_CP:
		std::sort(m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite.begin(), m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite.end(), SortByPosition);
		break;
	default: return;
	}
}

inline int CMakeModelDlg::GetRectInfoByPoint(cv::Point pt, CPType eType, RECTINFO*& pRc)
{
	int  nFind = -1;
	if (m_vecPaperModelInfo.size() <= m_nCurrTabSel)
		return nFind;
	
	switch (eType)
	{
	case UNKNOWN:
	case Fix_CP:
		if (eType == Fix_CP || eType == UNKNOWN)
		{
			for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix.size(); i++)
			{
				if (m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix[i].rt.contains(pt))
				{
					nFind = i;
					pRc = &m_vecPaperModelInfo[m_nCurrTabSel]->vecRtFix[i];
					break;
				}
			}
		}
	case H_HEAD:
// 		pt.x = pt.x - m_ptFixCP.x;
// 		pt.y = pt.y - m_ptFixCP.y;
		if (eType == H_HEAD || eType == UNKNOWN)
		{
			for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.size(); i++)
			{
				if (m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head[i].rt.contains(pt))
				{
					nFind = i;
					pRc = &m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head[i];
					break;
				}
			}
		}
	case V_HEAD:
		if (eType == V_HEAD || eType == UNKNOWN)
		{
			if (nFind < 0)
			{
				for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head.size(); i++)
				{
					if (m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head[i].rt.contains(pt))
					{
						nFind = i;
						pRc = &m_vecPaperModelInfo[m_nCurrTabSel]->vecV_Head[i];
						break;
					}
				}
			}
		}
	case ABMODEL:
		if (eType == ABMODEL || eType == UNKNOWN)
		{
			if (nFind < 0)
			{
				for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel.size(); i++)
				{
					if (m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel[i].rt.contains(pt))
					{
						nFind = i;
						pRc = &m_vecPaperModelInfo[m_nCurrTabSel]->vecABModel[i];
						break;
					}
				}
			}
		}
	case COURSE:
		if (eType == COURSE || eType == UNKNOWN)
		{
			if (nFind < 0)
			{
				for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse.size(); i++)
				{
					if (m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse[i].rt.contains(pt))
					{
						nFind = i;
						pRc = &m_vecPaperModelInfo[m_nCurrTabSel]->vecCourse[i];
						break;
					}
				}
			}
		}
	case QK_CP:
		if (eType == QK_CP || eType == UNKNOWN)
		{
			if (nFind < 0)
			{
				for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP.size(); i++)
				{
					if (m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP[i].rt.contains(pt))
					{
						nFind = i;
						pRc = &m_vecPaperModelInfo[m_nCurrTabSel]->vecQK_CP[i];
						break;
					}
				}
			}
		}
	case GRAY_CP:
		if (eType == GRAY_CP || eType == UNKNOWN)
		{
			if (nFind < 0)
			{
				for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecGray.size(); i++)
				{
					if (m_vecPaperModelInfo[m_nCurrTabSel]->vecGray[i].rt.contains(pt))
					{
						nFind = i;
						pRc = &m_vecPaperModelInfo[m_nCurrTabSel]->vecGray[i];
						break;
					}
				}
			}
		}
	case WHITE_CP:
		if (eType == WHITE_CP || eType == UNKNOWN)
		{
			if (nFind < 0)
			{
				for (int i = 0; i < m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite.size(); i++)
				{
					if (m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite[i].rt.contains(pt))
					{
						nFind = i;
						pRc = &m_vecPaperModelInfo[m_nCurrTabSel]->vecWhite[i];
						break;
					}
				}
			}
		}
	}

	return nFind;
}

int CMakeModelDlg::GetStandardVal(CPType eType)
{
	int  nFind = -1;
	if (m_vecPaperModelInfo.size() <= m_nCurrTabSel)
		return nFind;

	switch (eType)
	{
	case Fix_CP:
		m_nThresholdVal			= m_nFixVal;
		m_fThresholdValPercent	= m_fFixThresholdPercent * 100;
		break;
	case H_HEAD:
	case V_HEAD:
		m_nThresholdVal			= m_nHeadVal;
		m_fThresholdValPercent	= m_fHeadThresholdPercent * 100;
		break;
	case ABMODEL:
		m_nThresholdVal			= m_nABModelVal;
		m_fThresholdValPercent	= m_fABModelThresholdPercent * 100;
		break;
	case COURSE:
		m_nThresholdVal			= m_nCourseVal;
		m_fThresholdValPercent	= m_fCourseThresholdPercent * 100;
		break;
	case QK_CP:
		m_nThresholdVal			= m_nQK_CPVal;
		m_fThresholdValPercent	= m_fQK_CPThresholdPercent * 100;
		break;
	case GRAY_CP:
		m_nThresholdVal			= m_nGrayVal;
		m_fThresholdValPercent	= m_fGrayThresholdPercent * 100;
		break;
	case WHITE_CP:
		m_nThresholdVal			= m_nWhiteVal;
		m_fThresholdValPercent	= m_fWhiteThresholdPercent * 100;
		break;
	default:
		return 0;
	}
	return 1;
}

void CMakeModelDlg::OnBnClickedBtnSaverecoginfo()
{
	if (!m_pCurRectInfo)
		return;

	int nOldThresholdVal = m_nThresholdVal;
	UpdateData(TRUE);
	if (m_nThresholdVal > 255)	m_nThresholdVal = 255;
	if (m_nThresholdVal < 0)	m_nThresholdVal = 0;
	if (m_fThresholdValPercent > 100.0)	m_fThresholdValPercent = 100.0;
	if (m_fThresholdValPercent < 0.0)	m_fThresholdValPercent = 0.0;

	m_pCurRectInfo->nThresholdValue = m_nThresholdVal;
	m_pCurRectInfo->fStandardValuePercent = m_fThresholdValPercent / 100;

	if (nOldThresholdVal != m_nThresholdVal)
	{
		Rect rm = m_pCurRectInfo->rt;
		if (m_pCurRectInfo->eCPType != Fix_CP)
		{
// 			rm.x = rm.x + m_ptFixCP.x;
// 			rm.y = rm.y + m_ptFixCP.y;
		}
		Mat imgResult = m_vecPaperModelInfo[m_nCurrTabSel]->matDstImg(rm);
		RecogNewGrayValue(imgResult, *m_pCurRectInfo);
	}
}

void CMakeModelDlg::ShowDetailRectInfo()
{
	if (!m_pCurRectInfo)
		return;
	//显示此校验点的详细信息
	switch (m_pCurRectInfo->eCPType)
	{
	case Fix_CP: m_strCPTypeName = _T("定点"); break;
	case H_HEAD: m_strCPTypeName = _T("水平同步头"); break;
	case V_HEAD: m_strCPTypeName = _T("垂直同步头"); break;
	case ABMODEL: m_strCPTypeName = _T("卷型点"); break;
	case COURSE: m_strCPTypeName = _T("科目点"); break;
	case QK_CP: m_strCPTypeName = _T("缺考点"); break;
	case GRAY_CP: m_strCPTypeName = _T("灰度校验点"); break;
	case WHITE_CP: m_strCPTypeName = _T("空白校验点"); break;
	default:
		m_strCPTypeName = _T(""); break;
	}
	m_nThresholdVal = m_pCurRectInfo->nThresholdValue;
	m_fThresholdValPercent = m_pCurRectInfo->fStandardValuePercent * 100;
	UpdateData(FALSE);
}

bool CMakeModelDlg::PicRotate()
{
	if (m_vecPaperModelInfo.size() < m_nCurrTabSel)
		return false;

	if (m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.size() <= 1)
		return false;

	int nMaxItem = m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head.size() - 1;
	cv::Point pt1, pt2;
	pt1.x = m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head[0].rt.x + m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head[0].rt.width / 2 + 0.5;
	pt1.y = m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head[0].rt.y + m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head[0].rt.height / 2 + 0.5;

	pt2.x = m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head[nMaxItem].rt.x + m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head[nMaxItem].rt.width / 2 + 0.5;
	pt2.y = m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head[nMaxItem].rt.y + m_vecPaperModelInfo[m_nCurrTabSel]->vecH_Head[nMaxItem].rt.height / 2 + 0.5;

	float k = (float)(pt2.y - pt1.y) / (pt2.x - pt1.x);
	float fAngle = atan(k) / CV_PI * 180;
	TRACE("pt1(%d,%d), pt2(%d,%d), 斜率: %f, 旋转角度: %f\n", pt1.x, pt1.y, pt2.x, pt2.y, k, fAngle);

	Point2f center(pt2.x, pt2.y);
	Mat affine_matrix = getRotationMatrix2D(center, fAngle, 1.0);

	Mat inputImg = m_vecPaperModelInfo[m_nCurrTabSel]->matSrcImg;
	Mat tmpImg;
	warpAffine(inputImg, tmpImg, affine_matrix, inputImg.size());
	namedWindow("warpAffine", 0);
	imshow("warpAffine", tmpImg);
	m_pModelPicShow->ShowPic(tmpImg);

	return true;
}

