#pragma once
#include "NetOperatorDll.h"
#include <list>
#include "ListUser.h"

using namespace std;

class CMissionHandleMgr;
class CNetCmdMgr :
	public ITcpServerNotify
{
public:
	CNetCmdMgr();
	~CNetCmdMgr();

	void OnAccept(ITcpContext* pTcpContext);


	// 创建任务处理线程
	void CreateMissionHandleThread(int nThreadNum);
	void ReleaseMissionThread(void);
	static DWORD WINAPI MissionHandleThread(LPVOID  lParam);

	void HandleMission(CMissionHandleMgr* pMissionHandler);
private:
	ITcpServer* m_pTcpServer;
	CListUser*	m_pUserList;

	//任务处理线程句柄容器
	vector<HANDLE>		m_vtMissionThread;
public:
	bool StartWork(const char* pLocalIP, WORD wPort);
	void StopWork(void);
	bool m_bStop;
};

