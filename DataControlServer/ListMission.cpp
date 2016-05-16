//#include "StdAfx.h"
#include "ListMission.h"



CListMission::CListMission(void)
{
	InitializeCriticalSection(&m_ListLock);
}

CListMission::~CListMission(void)
{
	DeleteCriticalSection(&m_ListLock);
}


void CListMission::Lock(void)
{
	EnterCriticalSection(&m_ListLock);
}

void CListMission::UnLock(void)
{
	LeaveCriticalSection(&m_ListLock);
}

BOOL CListMission::AddMission(CMission* pMission)
{
	Lock();
	m_listMission.push_back(pMission);
//	TRACE("AddMission\n");
	UnLock();
	return TRUE;
}

CMission* CListMission::GetMission(void)
{
	Lock();
	if (m_listMission.size()==0)
	{
		UnLock();
		return NULL;
	}
	CMission* pMission=m_listMission.front();
	m_listMission.pop_front();
//	TRACE("GetMission\n");
	UnLock();
	
	return pMission;
}

//Çå³ýmission
void CListMission::ReleaseMission(void)
{
	Lock();
	list<CMission*>::iterator it=m_listMission.begin();
	for (;it!=m_listMission.end();it++)
	{
		if (*it)
		{
			delete *it;
			*it=NULL;
		}
	}
	m_listMission.clear();
	UnLock();
}
