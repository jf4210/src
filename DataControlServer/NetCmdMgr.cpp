#include "NetCmdMgr.h"
#include "DCSDef.h"

#include "MissionHandleMgr.h"

CNetCmdMgr::CNetCmdMgr()
: m_pUserList(NULL)
, m_pTcpServer(NULL)
, m_bStop(TRUE)
{
}

CNetCmdMgr::~CNetCmdMgr()
{
	StopWork();
}

// 新连接
void CNetCmdMgr::OnAccept(ITcpContext* pTcpContext)
{
	if (m_bStop)
	{
		return;
	}

	CNetUser* pUser = new CNetUser(pTcpContext);

	CListUser::GetInstance()->AddUser(pUser);
	//投递接收操作
	pUser->OnRead(NULL, 0);
	std::cout << "new cmd user coming\n";
}

// 启动网络
bool CNetCmdMgr::StartWork(const char* pIP, WORD wPort)
{
	if (!m_pTcpServer)
	{
		m_pTcpServer = CreateTcpServer(*this, pIP, wPort);
	}

	if (!m_pTcpServer)
	{
		return FALSE;
	}
	m_bStop = FALSE;

	//创建任务处理线程
	CreateMissionHandleThread(2);
// 	//开启心跳监测
// 	if (SysSet.m_bOpenHeartMon)
// 	{
// 		StartHeartBeatThread();
// 	}

	return TRUE;
}
// 创建任务处理线程
void CNetCmdMgr::CreateMissionHandleThread(int nThreadNum)
{
	ReleaseMissionThread();
	for (int i = 0; i < nThreadNum; i++)
	{
		HANDLE hThread = CreateThread(NULL, 0, MissionHandleThread, this, 0, NULL);
		if (!hThread)
		{
			break;
		}
		m_vtMissionThread.push_back(hThread);
	}
}

DWORD WINAPI CNetCmdMgr::MissionHandleThread(LPVOID  lParam)
{
	CNetCmdMgr* pThis = (CNetCmdMgr*)lParam;
	//每个线程用一个数据库操作对象
	CMissionHandleMgr* pMissionHandler = new CMissionHandleMgr;
	while (!pThis->m_bStop)
	{
		pThis->HandleMission(pMissionHandler);
	}
	delete pMissionHandler;
	pMissionHandler = NULL;
	return 0;
}

void CNetCmdMgr::HandleMission(CMissionHandleMgr* pMissionHandler)
{
	CMission* pMission = CMissionFactory::GetInstance()->GetMission();
	if (!pMission)
	{//此处有时间优化成事件通知模式
		Sleep(10);
		return;
	}
// 	if (pMission->m_pMissionData)
// 	{
// 		ST_CMD_HEADER header = *(ST_CMD_HEADER*)pMission->m_pMissionData;
// 		if (header.wCmd == ACTIVE_PACKET)
// 		{
// 			//反馈心跳包
// 			pMission->m_pUser->SetSendData(pMission->m_pMissionData, pMission->m_dwDataSize);
// 			return;
// 		}
// 	}
	pMissionHandler->PassCmdMsg(pMission);
}

void CNetCmdMgr::ReleaseMissionThread(void)
{
	vector<HANDLE>::iterator it = m_vtMissionThread.begin();
	for (; it != m_vtMissionThread.end(); it++)
	{
		if (*it)
		{
			if (WaitForSingleObject(*it, 1000) != WAIT_OBJECT_0)
			{
				TerminateThread(*it, 0);
			}
			CloseHandle(*it);
		}
	}
	m_vtMissionThread.clear();
}

void CNetCmdMgr::StopWork(void)
{
	if (m_bStop)
	{
		return;
	}
	m_bStop = TRUE;

//	StopHeartBeatThread();

	if (m_pTcpServer)
	{
		m_pTcpServer->ReleaseConnections();
	}
	ReleaseMissionThread();
}



