#pragma once
#include "global.h"
#include "PicShow.h"
#include "MyCodeConvert.h"

// CMakeModelDlg 对话框


typedef struct 
{
	int nPaper;			//第几张试卷
	CString		strModelPicName;	//模板图片的名称
	CString		strModelPicPath;	//模板图片路径
	cv::Mat		matSrcImg;			//原始图像的Mat
	cv::Mat		matDstImg;			//显示图像的Mat
	std::vector<RECTINFO> vecRtSel;				//存储选择的矩形,框的大矩形
	std::vector<RECTINFO> vecRtFix;				//存储定点矩形
	std::vector<RECTINFO> vecRtRecognition;		//存储识别出来的校验点矩形
	std::vector<RECTINFO> vecOmr;				//存储识别出来的答案矩形
	std::vector<RECTINFO>	vecH_Head;				//水平校验点列表
	std::vector<RECTINFO>	vecV_Head;				//垂直同步头列表
	std::vector<RECTINFO>	vecABModel;				//卷型校验点
	std::vector<RECTINFO>	vecCourse;				//科目校验点
	std::vector<RECTINFO>	vecQK_CP;				//缺考校验点
	std::vector<RECTINFO>	vecGray;				//灰度校验点
	std::vector<RECTINFO>	vecWhite;				//空白校验点
}PaperModelInfo, *pPaperModelInfo;

class CMakeModelDlg : public CDialog
{
	DECLARE_DYNAMIC(CMakeModelDlg)

public:
	CMakeModelDlg(pMODEL pModel = NULL, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMakeModelDlg();

// 对话框数据
	enum { IDD = IDD_MAKEMODELDLG };
public:
	int			m_nModelPicNums;		//模板图片数，即一份模板有多少图片，对应多少试卷
	CTabCtrl	m_tabModelPicCtrl;		//模板图片显示Tab控件
	pMODEL		m_pModel;				//模板信息
	CPicShow*	m_pModelPicShow;		//图片显示控件
	cv::Mat		m_matSrcImg;			//原始图像的Mat
	cv::Mat		m_matDstImg;			//显示图像的Mat
	CComboBox	m_comboCheckPointType;	//校验点类型
	CListCtrl	m_cpListCtrl;			//校验点列表控件

	CString		m_strCPTypeName;		//校验点的类型名
	int			m_nThresholdVal;		//校验点的识别阀值
	float		m_fThresholdValPercent;	//校验点达到阀值的比例
	const int	m_nFixVal;
	const int	m_nHeadVal;				//水平垂直同步头的阀值
	const int	m_nABModelVal;
	const int	m_nCourseVal;
	const int	m_nQK_CPVal;
	const int	m_nGrayVal;
	const int	m_nWhiteVal;
	const float m_fFixThresholdPercent;
	const float m_fHeadThresholdPercent;	//同步头达到阀值的比例
	const float m_fABModelThresholdPercent;
	const float m_fCourseThresholdPercent;
	const float m_fQK_CPThresholdPercent;
	const float m_fGrayThresholdPercent;
	const float m_fWhiteThresholdPercent;

	std::vector<CPicShow*>	m_vecPicShow;	//存储图片显示窗口指针，有多个模板图片时，对应到不同的tab控件页面
	int						m_nCurrTabSel;	//当前Tab控件选择的页面

	std::vector<pPaperModelInfo> m_vecPaperModelInfo;	//存储每张试卷的模板信息

	int	m_nGaussKernel;			//高斯变换核因子
	int m_nSharpKernel;			//锐化核因子
	int m_nThresholdKernel;		//二值化核因子
	int m_nCannyKernel;			//轮廓化核因子
	int m_nDelateKernel;		//膨胀核因子
	int m_nErodeKernel;			//腐蚀核因子

	CString m_strModelPicPath;	
	HZIP hz;					//压缩

	cv::Point	m_ptRBtnUp;			//右键抬起坐标，用于右键删除图像显示区选中的识别点

	bool		m_bNewModelFlag;	//是否是新创建的模板标识
	bool		m_bSavedModelFlag;	//是否新模板是否已经保存
	CPType		m_eCurCPType;		//当前校验点类型
	int			m_ncomboCurrentSel; //当前校验点的所选项
	int			m_nCurListCtrlSel;	//当前List列表所选的项
	pRECTINFO	m_pCurRectInfo;		//当前点击时选择的识别点
	cv::Point	m_ptFixCP;			//定点的坐标
	std::vector<std::vector<cv::Point> > m_vecContours;
private:
	void InitUI();
	void InitTab();
	void InitCtrlPosition();
	void InitConf();
	void UpdataCPList();
	CPType GetComboSelCpType();
	bool Recognise(cv::Rect rtOri);				//识别操作
	inline bool RecogGrayValue(cv::Mat& matSrcRoi, RECTINFO& rc);						//识别灰度值
	bool RecogNewGrayValue(cv::Mat& matSrcRoi, RECTINFO& rc);							//在修改阀值后重新计算矩形区的灰度值
	bool PicRotate();							//图像偏移操作
	void PaintRecognisedRect();					//画出所有已经识别出来的矩形
	void sharpenImage1(const cv::Mat &image, cv::Mat &result);		//锐化

	inline void GetThreshold(cv::Mat& matSrc, cv::Mat& matDst);			//二值化计算
	inline void ShowDetailRectInfo();
	inline int GetStandardVal(CPType eType);
	inline int GetRectInfoByPoint(cv::Point pt, CPType eType, RECTINFO*& pRc);
	void ShowRectByPoint(cv::Point pt);
	void ShowRectByItem(int nItem);
	void ShowRectByCPType(CPType eType);
	void SortRect();							//将识别出来的矩形进行坐标排序
	void ShowRectTracker();						//显示橡皮筋类矩形
	LRESULT RoiLBtnUp(WPARAM wParam, LPARAM lParam);
	LRESULT RoiLBtnDown(WPARAM wParam, LPARAM lParam);
	LRESULT RoiRBtnUp(WPARAM wParam, LPARAM lParam);
	
	bool SaveModelFile(pMODEL pModel);			//保存模板到文件

	void DelRectInfoOnPic();					//删除图像控件上的选中识别点
	void DeleteRectInfoOnList();				//删除List列表选中的识别点
	BOOL DeleteRectInfo(CPType eType, int nItem);				//删除选中的识别点
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog(); 
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedBtnSelpic();
	afx_msg void OnBnClickedBtnReset();
	afx_msg void OnBnClickedBtnSave();
	afx_msg void OnBnClickedBtnExitmodeldlg();
	afx_msg void OnBnClickedBtnNew();
	afx_msg void OnTcnSelchangeTabModelpic(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboCptype();
	afx_msg void OnNMRClickListCheckpoint(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListCheckpoint(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownListCheckpoint(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnSaverecoginfo();
};
