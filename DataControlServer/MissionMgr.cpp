//#include "StdAfx.h"
#include "MissionMgr.h"
#include "NetUser.h"

CMissionMgr::CMissionMgr()
{

}

CMissionMgr::~CMissionMgr(void)
{
	
}


// ��ȡָ�������ļ࿼�Ϳ�������ϵͳUSERID
void CMissionMgr::GetAdmUser(char* pSiteCode, vector<CNetUser*>& vtAdmUser)
{
//	CListUser::GetInstance()->GetAdmUser(pSiteCode,vtAdmUser);
}


void CMissionMgr::GetStuUser(char* pSiteCode, vector<CNetUser*>& vtStudent, int nFlag)
{
//	CListUser::GetInstance()->GetStuUser(pSiteCode,vtStudent,nFlag);
}

// �����û��˺Ų����û�ID
CNetUser* CMissionMgr::GetUserID(char* pUserNo,int nUserNoType)
{
//	CNetUser* pUser=CListUser::GetInstance()->FindUser(pUserNo,nUserNoType);

//	return pUser;
	return NULL;
}


