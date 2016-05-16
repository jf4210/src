#include "ExamServerMgr.h"
#include "PaperRecvMgr.h"
#include "NetCmdMgr.h"
#include "DCSDef.h"

CExamServerMgr::CExamServerMgr()
: m_pNetCmdMgr(NULL)
, m_pPaperRecvMgr(NULL)
{
}

CExamServerMgr::~CExamServerMgr()
{
	Stop();
}

bool CExamServerMgr::Start(void)
{
	Stop();
	//以下两行只是创建相应的实例
// 	CMissionFactory::GetInstance();
// 	CListUser::GetInstance();

	SetLogDir();
	
	StartCmdChannel();
	StartFileChannel();
	return true;
}

void CExamServerMgr::Stop(void)
{
	StopCmdChannel();
	StopFileChannel();
// 	CMissionFactory::ReleaseInstance();
// 	CListUser::ReleaseInstance();
// 	CListUser::ReleaseFreeInstance();
}

void CExamServerMgr::SetLogDir(void)
{
}

// 启动命令通道
bool CExamServerMgr::StartCmdChannel(void)
{
	if (!m_pNetCmdMgr)
	{
		m_pNetCmdMgr = new CNetCmdMgr;
	}

	return m_pNetCmdMgr->StartWork((char*)SysSet.m_sLocalIP.c_str(), SysSet.m_nCmdPort);
}


void CExamServerMgr::StopCmdChannel(void)
{
	if (m_pNetCmdMgr)
	{
		m_pNetCmdMgr->StopWork();
		delete m_pNetCmdMgr;
		m_pNetCmdMgr = NULL;
	}
}


// 启动文件传输通道
bool CExamServerMgr::StartFileChannel(void)
{
	if (!m_pPaperRecvMgr)
	{
		m_pPaperRecvMgr = new CPaperRecvMgr();
	}
	return m_pPaperRecvMgr->StartWork((char*)SysSet.m_sLocalIP.c_str(), SysSet.m_nPaperUpLoadPort);
}

void CExamServerMgr::StopFileChannel(void)
{
	if (m_pPaperRecvMgr)
	{
		m_pPaperRecvMgr->StopWork();
		delete m_pPaperRecvMgr;
		m_pPaperRecvMgr = NULL;
	}
}
