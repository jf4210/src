#include "UserMgr.h"
#include "DCSDef.h"

CUserMgr::CUserMgr()
{
}


CUserMgr::~CUserMgr()
{
}

int CUserMgr::HandleHeader(CMission* pMission)
{
	ST_CMD_HEADER header = *(ST_CMD_HEADER*)pMission->m_pMissionData;
	CNetUser* pUser = pMission->m_pUser;

	BOOL bFind = TRUE; //该命令是否被处理
	switch (header.usCmd)
	{
	case USER_LOGIN:
		{
			ST_LOGIN_INFO  LoginInfo = *(pStLoginInfo)(pMission->m_pMissionData + HEAD_SIZE);
			std::cout << "login info: " << LoginInfo.szUserNo << ", pwd: " << LoginInfo.szPWD << std::endl;

#if 0
			Poco::JSON::Object jsnData;
			jsnData.set("msg", "login");
			jsnData.set("user", LoginInfo.szUserNo);
			jsnData.set("pwd", LoginInfo.szPWD);
			std::stringstream jsnString;
			jsnData.stringify(jsnString);

			pSCAN_REQ_TASK pTask = new SCAN_REQ_TASK;
			pTask->strRequest = jsnString.str();
			pTask->strUri	  = SysSet.m_strScanReqUri;
			g_fmScanReq.lock();
			g_lScanReq.push_back(pTask);
			g_fmScanReq.unlock();
#else
			//++	应该在scanResquestHandler收到结果后再调用此过程返回结果
			int ret = RESULT_SUCCESS;
			if (ret == RESULT_SUCCESS)
			{
				pUser->UpdateLogonInfo(LoginInfo.szUserNo, LoginInfo.szPWD);
			}
			pUser->SendResult(USER_RESPONSE_LOGIN, ret);

			MAP_USER::iterator itFind = _mapUser_.find(LoginInfo.szUserNo);
			if (itFind == _mapUser_.end())
				_mapUser_.insert(MAP_USER::value_type(LoginInfo.szUserNo, pUser));
			else
				itFind->second = pUser;
			//--
#endif
		}
		break;
	default:
		bFind = FALSE;
		break;
	}
	if (bFind)
	{
		return 1;
	}
	return 0;
}
