#pragma once
#include <list>
#include "Mission.h"
#include "global.h"

using namespace std;
class CListMission
{
	
public:

	CListMission(void);
	
	virtual ~CListMission(void);

	list<CMission*>		m_listMission;

	CRITICAL_SECTION	 m_ListLock;
	void Lock(void);
	void UnLock(void);

	BOOL AddMission(CMission* pMission);
	CMission* GetMission(void);
	void ReleaseMission(void);
};
