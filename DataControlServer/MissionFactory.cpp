//#include "StdAfx.h"
#include "MissionFactory.h"

CMissionFactory* CMissionFactory::m_pInstance=NULL;

CMissionFactory::CMissionFactory(void)
{
}

CMissionFactory::~CMissionFactory(void)
{
	ClearMission();
}

CMission* CMissionFactory::CreateMission()
{
	CMission* pMission= m_ListFreeMission.GetMission();
	if (!pMission)
	{
		pMission=new CMission();
	}

	return pMission;
}

void CMissionFactory::ReleaseMission(CMission* pMission)
{
	m_ListFreeMission.AddMission(pMission);
}

CMissionFactory* CMissionFactory::GetInstance(void)
{
	if (!m_pInstance)
	{
		m_pInstance=new CMissionFactory();
	}
	
	return m_pInstance;
}

void CMissionFactory::ReleaseInstance(void)
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance=NULL;
	}
}

CMission* CMissionFactory::GetMission(void)
{
	return m_ListMission.GetMission();
}

void CMissionFactory::ClearMission(void)
{
	m_ListMission.ReleaseMission();
	m_ListFreeMission.ReleaseMission();
}

void CMissionFactory::AddMission(CMission* pMission)
{
	m_ListMission.AddMission(pMission);
}

