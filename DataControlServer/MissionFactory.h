#pragma once
#include "Mission.h"
#include "ListMission.h"

class CMissionFactory
{

	CMissionFactory(void);
public:	
	virtual ~CMissionFactory(void);
	static CMissionFactory* m_pInstance;
	static CMissionFactory* GetInstance(void);
	CMission* CreateMission(void);
	void ReleaseMission(CMission* pMission);

private:
	//待处理任务列表
	CListMission		m_ListMission;
	//空闲任务列表
	CListMission		m_ListFreeMission;
	
public:
	CMission* GetMission(void);
	void ClearMission(void);
	void AddMission(CMission* pMission);
	static void ReleaseInstance(void);
};
