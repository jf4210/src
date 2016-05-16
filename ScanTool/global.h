#pragma once
#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <opencv2\opencv.hpp>

#include "Poco/Runnable.h"
#include "Poco/Exception.h"

#include "Poco/AutoPtr.h"  
#include "Poco/Logger.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"
#include "Poco/FileChannel.h"
#include "Poco/Message.h"

#include "Poco/DirectoryIterator.h"
#include "Poco/File.h"
#include "Poco/Path.h"

#include "Poco/JSON/JSON.h"
#include "Poco/JSON/Parser.h"
#include "Poco/Dynamic/Var.h"
#include "Poco/JSON/Object.h"

#include "Poco/AutoPtr.h"  
#include "Poco/Util/IniFileConfiguration.h" 

#include "Poco/Random.h"

#include "Poco/MD5Engine.h"
#include "Poco/DigestStream.h"
#include "Poco/StreamCopier.h"

#include "Poco/LocalDateTime.h"

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/URI.h"
#include "Poco/Net/NetException.h"
#include "Poco/Net/TCPServer.h"

#include "zip.h"
#include "unzip.h"
#include "MyCodeConvert.h"

#define  MSG_ERR_RECOG	(WM_USER + 110)

#define SOFT_VERSION	_T("1.0")
#define MAX_DLG_WIDTH	1024
#define MAX_DLG_HEIGHT	768

#define SAFE_RELEASE(pObj)	if(pObj){delete pObj; pObj = NULL;}

extern CString				g_strCurrentPath;
extern std::string			g_strPaperSavePath;
extern std::string			g_strModelSavePath;
extern Poco::Logger*		g_pLogger;
extern int					g_nExitFlag;
extern float				g_fSamePercent;		//判断校验区域是否填图百分比

typedef enum CPType
{
	UNKNOWN = 0,
	Fix_CP,			//定点，坐标计算用
	H_HEAD,			//水平同步头
	V_HEAD,			//垂直同步头
	ABMODEL,		//AB卷型
	COURSE,			//科目
	QK_CP,			//缺考
	GRAY_CP,		//灰度校验
	WHITE_CP		//白校验
};

typedef struct _RectInfo_
{
	CPType		eCPType;						//校验点类型
	int			nThresholdValue;				//此矩形识别时要求的标准值，即达到的灰度值的阀值
	float		fStandardValue;					//此矩形识别时测量值
	float		fRealValue;						//
	float		fStandardValuePercent;			//此矩形要求的标准值比例，即达到给定的灰度值的比例
	float		fRealValuePercent;				//此矩形实际的值(灰度值)
	int			nHItem;							//第几个水平同步头
	int			nVItem;							//第几个垂直同步头
//	cv::Point	ptFix;
	cv::Rect	rt;
//	cv::Rect	rtFix;
	_RectInfo_()
	{
		eCPType = UNKNOWN;
		nHItem = 0;
		nVItem = 0;
		nThresholdValue = 0;
		fRealValuePercent = 0.0;
		fStandardValuePercent = 0.0;
		fStandardValue = 0.0;
		fRealValue = 0.0;
//		ptFix = cv::Point(0, 0);
	}
}RECTINFO,*pRECTINFO;

typedef std::list<RECTINFO> RECTLIST;			//矩形位置列表

typedef struct _PaperModel_
{
	int			nPaper;					//标识此模板属于第几张试卷
	CString		strModelPicName;		//模板图片名称
	RECTLIST	lSelROI;				//选择的ROI的矩形列表
	RECTLIST	lCheckPoint;			//校验点矩形位置列表
	RECTLIST	lOMR;					//OMR识别的矩形位置列表
	RECTLIST	lFix;					//定点列表
	RECTLIST	lH_Head;				//水平校验点列表
	RECTLIST	lV_Head;				//垂直同步头列表
	RECTLIST	lABModel;				//卷型校验点
	RECTLIST	lCourse;				//科目校验点
	RECTLIST	lQK_CP;					//缺考校验点
	RECTLIST	lGray;					//灰度校验点
	RECTLIST	lWhite;					//空白校验点
}PAPERMODEL,*pPAPERMODEL;

typedef struct _Model_
{
	int			nEnableModify;			//模板是否可以修改
	int			nPicNum;				//图片数量
	int			nABModel;				//是否是AB卷模式
	int			nHasHead;				//是否有水平和垂直同步头
	CString		strModelName;			//模板名称

	std::vector<PAPERMODEL> vecPaperModel;	//存储每一页试卷的模板信息
	_Model_()
	{
		nPicNum = 0;
		nABModel = 0;
		nHasHead = 1;
	}
}MODEL, *pMODEL;
typedef std::list<pMODEL> MODELLIST;	//模板列表


typedef struct _PicInfo_				//图片信息
{
	bool			bFindIssue;		//是否找到问题点
//	bool			bImgOpen;		//试卷图片是否已经打开，打开了就不需要再次打开
//	cv::Point		ptModelFix;		//模板的定点
//	cv::Point		ptFix;			//定点位置
	cv::Rect		rtFix;			//定点矩形
	std::string		strPicName;		//图片名称
	std::string		strPicPath;		//图片路径
	RECTLIST		lFix;			//定点列表
	RECTLIST		lNormalRect;	//识别出来的正常点位置
	RECTLIST		lIssueRect;		//识别出来的问题试卷的问题点位置，只要出现问题点就不进行下一页的识别
// 	cv::Mat			matSrc;
// 	cv::Mat			matDest;
	_PicInfo_()
	{
		bFindIssue = false;
//		ptFix = cv::Point(0, 0);
//		ptModelFix = cv::Point(0, 0);
//		bImgOpen = false;
	}
}ST_PicInfo, *pST_PicInfo;
typedef std::list<pST_PicInfo> PIC_LIST;	//图片列表定义

typedef struct _PaperInfo_
{
	bool		bIssuePaper;		//是否是问题试卷
	pMODEL		pModel;				//识别此学生试卷所用的模板
	void*		pPapers;			//所属的试卷袋信息
	void*		pSrcDlg;			//来源，来自哪个窗口，扫描or导入试卷窗口
	std::string strStudentInfo;		//学生信息	
	
	PIC_LIST	lPic;
	_PaperInfo_()
	{
		bIssuePaper = false;
		pModel = NULL;
		pPapers = NULL;
		pSrcDlg = NULL;
	}
	~_PaperInfo_()
	{
		PIC_LIST::iterator itPic = lPic.begin();
		for (; itPic != lPic.end();)
		{
			pST_PicInfo pPic = *itPic;
			SAFE_RELEASE(pPic);
			itPic = lPic.erase(itPic);
		}		
	}
}ST_PaperInfo, *pST_PaperInfo;		//试卷信息，一个学生对应一份试卷，一份试卷可以有多份图片
typedef std::list<pST_PaperInfo> PAPER_LIST;	//试卷列表

typedef struct _PapersInfo_				//试卷袋信息结构体
{
	int		nPaperCount;				//试卷袋中试卷总数量(学生数)
	int		nRecogErrCount;				//识别错误试卷数量
	Poco::FastMutex fmlPaper;			//对试卷列表读写锁
	Poco::FastMutex fmlIssue;			//对问题试卷列表读写锁
	std::string  strPapersName;			//试卷袋名称
	std::string	 strPapersDesc;			//试卷袋详细描述

	PAPER_LIST	lPaper;					//此试卷袋中试卷列表
	PAPER_LIST	lIssue;					//此试卷袋中识别有问题的试卷列表
	_PapersInfo_()
	{
		nPaperCount = 0;
		nRecogErrCount = 0;
	}
	~_PapersInfo_()
	{
		fmlPaper.lock();
		PAPER_LIST::iterator itPaper = lPaper.begin();
		for (; itPaper != lPaper.end();)
		{
			pST_PaperInfo pPaper = *itPaper;
			SAFE_RELEASE(pPaper);
			itPaper = lPaper.erase(itPaper);
		}
		fmlPaper.unlock();
		fmlIssue.lock();
		PAPER_LIST::iterator itIssuePaper = lIssue.begin();
		for (; itIssuePaper != lIssue.end();)
		{
			pST_PaperInfo pPaper = *itIssuePaper;
			SAFE_RELEASE(pPaper);
			itIssuePaper = lIssue.erase(itIssuePaper);
		}
		fmlIssue.unlock();
	}
}PAPERSINFO, *pPAPERSINFO;
typedef std::list<pPAPERSINFO> PAPERS_LIST;		//试卷袋列表

typedef struct _RecogTask_
{
	int		nPic;						//此试卷属于模板的第几张
	pMODEL pModel;						//识别用的模板
	std::string strPath;	
	pST_PaperInfo	pPaper;				//需要识别的试卷
}RECOGTASK, *pRECOGTASK;
typedef std::list<pRECOGTASK> RECOGTASKLIST;	//识别任务列表

extern Poco::FastMutex		g_fmRecog;		//识别线程获取任务锁
extern RECOGTASKLIST		g_lRecogTask;	//识别任务列表

extern Poco::FastMutex		g_fmPapers;		//操作试卷袋列表的任务锁
extern PAPERS_LIST			g_lPapers;		//所有的试卷袋信息


//文件上传任务
typedef struct _SendTask_
{
	std::string strFileName;
	std::string strPath;
}SENDTASK, *pSENDTASK;
typedef std::list<pSENDTASK> SENDTASKLIST;	//识别任务列表

extern Poco::FastMutex		g_fmSendLock;
extern SENDTASKLIST			g_lSendTask;


int		GetRectInfoByPoint(cv::Point pt, CPType eType, pPAPERMODEL pPaperModel, RECTINFO*& pRc);
bool	ZipFile(CString strSrcPath, CString strDstPath);
bool	UnZipFile(CString strZipPath);
pMODEL	LoadModelFile(CString strModelPath);		//加载模板文件
bool	SortByArea(cv::Rect& rt1, cv::Rect& rt2);		//按面积排序
bool	GetPosition(RECTLIST& lFix, RECTLIST& lModelFix, cv::Rect& rt, int nPicW = 0, int nPicH = 0);
