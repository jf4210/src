#pragma once
#include "MissionMgr.h"
#include "NetUser.h"

class CUserMgr :
	public CMissionMgr
{
public:
	CUserMgr();
	~CUserMgr();

	int HandleHeader(CMission* pMission);


};

