#pragma once
#include "global.h"
#include "Log.h"
#include "SysSet.h"
#include "MyCodeConvert.h"
#include "NetIoBuffer.h"
#include "MissionFactory.h"
#include "Net_Cmd_Protocol.h"
#include "NetUser.h"

//#define HEAD_SIZE	sizeof(ST_CMD_HEADER)
#define SAFE_RELEASE(pObj)	if(pObj){delete pObj; pObj = NULL;}

extern CLog g_Log;
extern int	g_nExitFlag;


typedef std::map<std::string, CNetUser*>	MAP_USER;
extern MAP_USER		_mapUser_;					//用户映射


typedef struct _DecompressTask_
{
	std::string strFileName;
	std::string strFilePath;
}DECOMPRESSTASK, *pDECOMPRESSTASK;
typedef std::list<pDECOMPRESSTASK> DECOMPRESSTASKLIST;	//识别任务列表

extern Poco::FastMutex			g_fmDecompressLock;		//解压文件列表锁
extern DECOMPRESSTASKLIST		g_lDecompressTask;		//解压文件列表


//图片文件
typedef struct _Pic_
{
	bool		bUpLoadFlag;		//上传http服务器成功标志
	std::string strFileName;
	std::string strFilePath;
	std::string strHashVal;			//上传http后返回的hash
	_Pic_()
	{
		bUpLoadFlag = false;
	}
}PIC_DETAIL, *pPIC_DETAIL;
typedef std::list<pPIC_DETAIL> LIST_PIC_DETAIL;

//试卷,针对考生
typedef struct _Paper_
{
	std::string strName;	//识别出来的考生序列号、准考证号
	LIST_PIC_DETAIL lPic;

	~_Paper_()
	{
		LIST_PIC_DETAIL::iterator it = lPic.begin();
		for (; it != lPic.end();)
		{
			pPIC_DETAIL pObj = *it;
			SAFE_RELEASE(pObj);
			it = lPic.erase(it);
		}
	}
}PAPER_INFO, *pPAPER_INFO;
typedef std::list<pPAPER_INFO> LIST_PAPER_INFO;
//试卷袋
typedef struct _Papers_
{
	int			nTotalPics;			//总的图片数
	int			nUpLoadSuccess;
	int			nUpLoadFail;
	int			nTotalPaper;		//总学生数量，从试卷袋文件夹读取
	int			nQk;				//缺考学生数量，从试卷袋文件夹读取
	std::string strDesc;			//从试卷袋文件夹读取
	std::string strPapersName;
	std::string strPapersPath;
	Poco::FastMutex	fmNum;			//对上传结果的计数操作的锁
	LIST_PAPER_INFO lPaper;
	_Papers_()
	{
		nTotalPics = 0;
		nUpLoadFail = 0;
		nUpLoadSuccess = 0;
		nTotalPaper = 0;
		nQk = 0;
	}
	~_Papers_()
	{
		LIST_PAPER_INFO::iterator it = lPaper.begin();
		for (; it != lPaper.end();)
		{
			pPAPER_INFO pObj = *it;
			SAFE_RELEASE(pObj);
			it = lPaper.erase(it);
		}
	}
}PAPERS_DETAIL, *pPAPERS_DETAIL;
typedef std::list<pPAPERS_DETAIL> LIST_PAPERS_DETAIL;

extern Poco::FastMutex		g_fmPapers;		
extern LIST_PAPERS_DETAIL	g_lPapers;		//试卷袋列表


typedef struct _SendHttpTask_
{
	int			nTaskType;			//任务类型: 1-给img服务器提交图片，2-给后端提交数据
	int			nSendFlag;			//发送标示，1：发送失败1次，2：发送失败2次...
	Poco::Timestamp sTime;			//创建任务时间，用于发送失败时延时发送
	pPIC_DETAIL pPic;
	pPAPERS_DETAIL pPapers;
	std::string strUri;
	std::string strResult;			//发送考场信息给后端服务器，当nTaskType = 2时有用
	_SendHttpTask_()
	{
		nTaskType = 0;
		nSendFlag = 0;
		pPic	= NULL;
		pPapers = NULL;
	}
}SEND_HTTP_TASK, *pSEND_HTTP_TASK;
typedef std::list<pSEND_HTTP_TASK> LIST_SEND_HTTP;

extern Poco::FastMutex		g_fmHttpSend;
extern LIST_SEND_HTTP		g_lHttpSend;		//发送HTTP任务列表

typedef struct _ScanReqTask_
{
	std::string strRequest;
	std::string strUri;
}SCAN_REQ_TASK, *pSCAN_REQ_TASK;
typedef std::list<pSCAN_REQ_TASK> LIST_SCAN_REQ;

extern Poco::FastMutex		g_fmScanReq;
extern LIST_SCAN_REQ		g_lScanReq;		//扫描端请求任务列表

