#include "ListUser.h"

CListUser* CListUser::m_pInstance = NULL;

CListUser::CListUser(void)
{
//	InitializeCriticalSection(&m_Lock);
}

CListUser::~CListUser(void)
{
	ClearUser();
//	DeleteCriticalSection(&m_Lock);
}

void CListUser::Lock(void)
{
//	EnterCriticalSection(&m_Lock);
	m_listLock.lock();
}

void CListUser::UnLock(void)
{
//	LeaveCriticalSection(&m_Lock);
	m_listLock.unlock();
}


int CListUser::AddUser(CNetUser* pUser)
{
	if (!pUser)
	{
		return -1;
	}
	Lock();
	m_UserList.push_back(pUser);
	UnLock();
	return 0;
}

void CListUser::RemoveUser(CNetUser* pUser)
{
	if (!pUser)
	{
		return;
	}
	Lock();
	list<CNetUser*>::iterator it = m_UserList.begin();
	for (; it != m_UserList.end(); it++)
	{
		if ((*it) == pUser)
		{
			m_UserList.erase(it);
			break;
		}
	}

	UnLock();
}

CListUser* CListUser::GetInstance(void)
{
	if (!m_pInstance)
	{
		m_pInstance = new CListUser();
	}
	return m_pInstance;
}

void CListUser::ReleaseInstance(void)
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

void CListUser::ClearUser(void)
{
	Lock();
	list<CNetUser*>::iterator it = m_UserList.begin();
	for (; it != m_UserList.end(); it++)
	{
		if (*it)
		{
			delete *it;
		}
	}
	m_UserList.clear();
	UnLock();
}

CNetUser* CListUser::FindUser(char* pUserNo)
{
	Lock();
	list<CNetUser*>::iterator it = m_UserList.begin();
	for (; it != m_UserList.end(); it++)
	{
		CNetUser* p = *it;

		if (!strcmp(p->m_Name, pUserNo))
		{
			UnLock();
			return p;
		}
	}
	UnLock();

	return NULL;
}
