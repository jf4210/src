//#include "StdAfx.h"
#include "Mission.h"
#include "NetUser.h"
#include "DCSDef.h"

CMission::CMission()
: m_pUser(NULL)
, m_pMissionData(NULL)
, m_nDataSize(0)
, m_nTotalSize(0)
{
}

CMission::~CMission(void)
{
	ReleaseDataBuf();
}

void CMission::SetNetUset(CNetUser* pUser)
{
	m_pUser=pUser;
}


bool CMission::SetData(char* pData, int nDataSize)
{
	if (nDataSize>m_nTotalSize)
	{
		//��ԭ�����ڴ�ռ��ͷţ���������һ���Ŀռ�
		ReleaseDataBuf();
		int ret=CreateDataBuf(nDataSize);
		if (ret==0)
		{
//			g_Log.LogOut(LOG_ERROR,"CreateDataBufʧ��");
			g_Log.LogOut("CreateDataBufʧ��");
			return FALSE;
		}
	}

	memcpy(m_pMissionData,pData,nDataSize);
	m_nDataSize=nDataSize;
	return TRUE;
}


int CMission::CreateDataBuf(int nSize)
{
	m_pMissionData=new char[nSize];
	if (!m_pMissionData)
	{
		return 0;
	}
	return m_nTotalSize=nSize;
}


void CMission::ReleaseDataBuf(void)
{
	if (m_pMissionData)
	{
		delete []m_pMissionData;
		m_pMissionData=NULL;
	}
	m_nTotalSize=0;

}

