#pragma once
#include "DCSDef.h"
#include "MissionMgr.h"
#include "Log.h"
#include "UserMgr.h"


class CMissionHandleMgr
{
public:
	CMissionHandleMgr(void);
	~CMissionHandleMgr(void);
	void PassCmdMsg(CMission* pMission);


	
	void CreateHandler(void);
	void ReleaseHandler(void);

	CUserMgr*			m_pUserMgr;
	vector<CMissionMgr*> m_vtHandler;
};
