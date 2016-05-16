//#include "StdAfx.h"
#include "MissionMgr.h"
#include "NetUser.h"

CMissionMgr::CMissionMgr()
{

}

CMissionMgr::~CMissionMgr(void)
{
	
}


// 获取指定考场的监考和考场管理系统USERID
void CMissionMgr::GetAdmUser(char* pSiteCode, vector<CNetUser*>& vtAdmUser)
{
//	CListUser::GetInstance()->GetAdmUser(pSiteCode,vtAdmUser);
}


void CMissionMgr::GetStuUser(char* pSiteCode, vector<CNetUser*>& vtStudent, int nFlag)
{
//	CListUser::GetInstance()->GetStuUser(pSiteCode,vtStudent,nFlag);
}

// 根据用户账号查找用户ID
CNetUser* CMissionMgr::GetUserID(char* pUserNo,int nUserNoType)
{
//	CNetUser* pUser=CListUser::GetInstance()->FindUser(pUserNo,nUserNoType);

//	return pUser;
	return NULL;
}


