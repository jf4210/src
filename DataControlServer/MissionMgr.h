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

	// �����û��˺Ų����û�ID,nUserNoType:0-pUserNo�����֤���߽�ʦ�ţ�1-׼��֤
	CNetUser* GetUserID(char* pUserNo,int nUserNoType=0);
	

//	CDBManager* m_pDBManager;
};
