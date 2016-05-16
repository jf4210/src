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
	//�����������б�
	CListMission		m_ListMission;
	//���������б�
	CListMission		m_ListFreeMission;
	
public:
	CMission* GetMission(void);
	void ClearMission(void);
	void AddMission(CMission* pMission);
	static void ReleaseInstance(void);
};
