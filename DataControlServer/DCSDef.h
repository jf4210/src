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
extern MAP_USER		_mapUser_;					//�û�ӳ��


typedef struct _DecompressTask_
{
	std::string strFileName;
	std::string strFilePath;
}DECOMPRESSTASK, *pDECOMPRESSTASK;
typedef std::list<pDECOMPRESSTASK> DECOMPRESSTASKLIST;	//ʶ�������б�

extern Poco::FastMutex			g_fmDecompressLock;		//��ѹ�ļ��б���
extern DECOMPRESSTASKLIST		g_lDecompressTask;		//��ѹ�ļ��б�


//ͼƬ�ļ�
typedef struct _Pic_
{
	bool		bUpLoadFlag;		//�ϴ�http�������ɹ���־
	std::string strFileName;
	std::string strFilePath;
	std::string strHashVal;			//�ϴ�http�󷵻ص�hash
	_Pic_()
	{
		bUpLoadFlag = false;
	}
}PIC_DETAIL, *pPIC_DETAIL;
typedef std::list<pPIC_DETAIL> LIST_PIC_DETAIL;

//�Ծ�,��Կ���
typedef struct _Paper_
{
	std::string strName;	//ʶ������Ŀ������кš�׼��֤��
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
//�Ծ��
typedef struct _Papers_
{
	int			nTotalPics;			//�ܵ�ͼƬ��
	int			nUpLoadSuccess;
	int			nUpLoadFail;
	int			nTotalPaper;		//��ѧ�����������Ծ���ļ��ж�ȡ
	int			nQk;				//ȱ��ѧ�����������Ծ���ļ��ж�ȡ
	std::string strDesc;			//���Ծ���ļ��ж�ȡ
	std::string strPapersName;
	std::string strPapersPath;
	Poco::FastMutex	fmNum;			//���ϴ�����ļ�����������
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
extern LIST_PAPERS_DETAIL	g_lPapers;		//�Ծ���б�


typedef struct _SendHttpTask_
{
	int			nTaskType;			//��������: 1-��img�������ύͼƬ��2-������ύ����
	int			nSendFlag;			//���ͱ�ʾ��1������ʧ��1�Σ�2������ʧ��2��...
	Poco::Timestamp sTime;			//��������ʱ�䣬���ڷ���ʧ��ʱ��ʱ����
	pPIC_DETAIL pPic;
	pPAPERS_DETAIL pPapers;
	std::string strUri;
	std::string strResult;			//���Ϳ�����Ϣ����˷���������nTaskType = 2ʱ����
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
extern LIST_SEND_HTTP		g_lHttpSend;		//����HTTP�����б�

typedef struct _ScanReqTask_
{
	std::string strRequest;
	std::string strUri;
}SCAN_REQ_TASK, *pSCAN_REQ_TASK;
typedef std::list<pSCAN_REQ_TASK> LIST_SCAN_REQ;

extern Poco::FastMutex		g_fmScanReq;
extern LIST_SCAN_REQ		g_lScanReq;		//ɨ������������б�

