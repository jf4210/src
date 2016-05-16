#pragma once
#include "global.h"
#include "NetUser.h"
#include "Poco/Mutex.h"

using namespace std;

class CListUser
{
public:
	CListUser();
	~CListUser();
	int AddUser(CNetUser* pUser);
	void RemoveUser(CNetUser* pUser);
	std::list<CNetUser*> m_UserList;
	void Lock(void);
	void UnLock(void);
//	CRITICAL_SECTION	m_listLock;
	Poco::FastMutex		m_listLock;
	void ClearUser(void);

	CNetUser* FindUser(char* pUserNo);

public:
	static CListUser* GetInstance(void);
	static void ReleaseInstance(void);
	static CListUser* m_pInstance;
};

