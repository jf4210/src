#include "ListPaperUser.h"
#include "PaperUser.h"

Poco::FastMutex g_fmTest;

CListPaperUser::CListPaperUser()
{
}


CListPaperUser::~CListPaperUser()
{
}
void CListPaperUser::AddUser(CPaperUser* pUser)
{
	if (!pUser)
	{
		return;
	}
	Lock();
	m_UserList.push_back(pUser);
	UnLock();
}

void CListPaperUser::RemoveUser(CPaperUser* pUser)
{
	if (!pUser)
	{
		return;
	}
	Lock();
	list<CPaperUser*>::iterator it = m_UserList.begin();
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

void CListPaperUser::Lock(void)
{
//	EnterCriticalSection(&m_listLock);
//	m_fmListLock.lock();
	g_fmTest.lock();
}

void CListPaperUser::UnLock(void)
{
//	LeaveCriticalSection(&m_listLock);
//	m_fmListLock.unlock();
	g_fmTest.unlock();
}

void CListPaperUser::ClearUser(void)
{
	Lock();
	list<CPaperUser*>::iterator it = m_UserList.begin();
	for (; it != m_UserList.end(); it++)
	{
		if (*it)
		{
			delete *it;
			*it = NULL;
		}
	}
	m_UserList.clear();
	UnLock();
}