#pragma once
//#include "DBManager.h"
#include "Mission.h"
#include<vector>

using namespace std;
class CNetUser;
class CMissionMgr
{
public:
	CMissionMgr();
	virtual ~CMissionMgr(void);
	virtual int HandleHeader(CMission* pMission) = 0;

	
	virtual void GetAdmUser(char* pSiteCode, vector<CNetUser*>& vtAdmUser);
	void GetStuUser(char* pSiteCode, vector<CNetUser*>& vtStudent, int nFlag);

	// 根据用户账号查找用户ID,nUserNoType:0-pUserNo是身份证或者教师号，1-准考证
	CNetUser* GetUserID(char* pUserNo,int nUserNoType=0);
	

//	CDBManager* m_pDBManager;
};
