#pragma once
#include "global.h"
#include "Poco/Mutex.h"

using namespace std;
class CPaperUser;
class CListPaperUser
{
public:
	CListPaperUser();
	~CListPaperUser();
	void AddUser(CPaperUser* pUser);
	void RemoveUser(CPaperUser* pUser);
	list<CPaperUser*> m_UserList;
	void Lock(void);
	void UnLock(void);
//	CRITICAL_SECTION	m_listLock;
	void ClearUser(void);
public:
//	Poco::FastMutex		m_fmListLock;
};

