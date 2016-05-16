//#include "StdAfx.h"
#include "MissionHandleMgr.h"

CMissionHandleMgr::CMissionHandleMgr(void)
: m_pUserMgr(NULL)
{
	CreateHandler();
}

CMissionHandleMgr::~CMissionHandleMgr(void)
{
	ReleaseHandler();
}

void CMissionHandleMgr::PassCmdMsg(CMission* pMission)
{
	vector<CMissionMgr*>::iterator it=m_vtHandler.begin();
	for (;it!=m_vtHandler.end();it++)
	{
		CMissionMgr* pHandler=(*it);
		if (pHandler->HandleHeader(pMission)!=0)//已经被处理
		{
			break;
		}
	}
	CMissionFactory::GetInstance()->ReleaseMission(pMission);
}

void CMissionHandleMgr::CreateHandler(void)
{

	m_vtHandler.clear();
	if (!m_pUserMgr)
	{
		m_pUserMgr = new CUserMgr;
	}
	m_vtHandler.push_back(m_pUserMgr);
}

void CMissionHandleMgr::ReleaseHandler(void)
{
	SAFE_RELEASE(m_pUserMgr);
	m_vtHandler.clear();
}
