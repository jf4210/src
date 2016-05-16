#include "NetUser.h"
#include "ListUser.h"
#include "DCSDef.h"

CNetUser::CNetUser(ITcpContext* pTcpContext)
: m_bNoUse(FALSE)
, m_pTcpContext(pTcpContext)
, m_nRecvCount(0)
, m_nSendCount(0)
, m_wPort(0)
{
	m_pNetRecvBuffer = new CNetIoBuffer;
	m_pNetSendBuffer = new CNetIoSendBuffer;
	InitContext(pTcpContext);
}


CNetUser::~CNetUser()
{
	if (m_pNetRecvBuffer)
	{
		delete m_pNetRecvBuffer;
		m_pNetRecvBuffer = NULL;
	}

	if (m_pNetSendBuffer)
	{
		delete m_pNetSendBuffer;
		m_pNetSendBuffer = NULL;
	}
}


void CNetUser::InitContext(ITcpContext* pTcpContext)
{
	m_bNoUse = FALSE;
	m_pTcpContext = pTcpContext;
//	m_tLastPacketTime = CTime::GetCurrentTime();
	memset(m_Name, 0, LEN_NAME + 1);
	memset(m_Password, 0, LEN_PWD + 1);
	memset(m_pIPAddress, 0, LEN_IPADDRESS + 1);
	m_pNetRecvBuffer->ResetPosition();
	m_pNetSendBuffer->ResetPosition();
	if (m_pTcpContext)
	{
		m_pTcpContext->SetTcpContextNotify(this);
		m_pTcpContext->GetRemoteAddr(m_pIPAddress, m_wPort);
	}
}

void CNetUser::OnClose(void)
{
	std::cout << "net cmd user close" << std::endl;


	//构造退出消息
// 	if (m_nLoginSate == LOGIN_ON)
// 	{
// 		CMission* pMission = CMissionFactory::GetInstance()->CreateMission();
// 		if (!pMission)
// 		{
// 			TRACE0(" no memery\n");
// 			return;
// 		}
// 		pMission->SetNetUset(NULL);
// 
// 		char* pMsg = new char[HEAD_SIZE + sizeof(ST_CLOSEUSERINFO)];
// 		ST_CMD_HEADER *pHeader = (ST_CMD_HEADER*)pMsg;
// 		strcpy(pHeader->szVerify, CHECKCODE);
// 		pHeader->wCmd = EC_NOTIFY_CLOSECON;//对方关闭
// 		pHeader->wPackSize = sizeof(ST_CLOSEUSERINFO);
// 		pHeader->wResult = RESULT_SUCCESS;
// 
// 		ST_CLOSEUSERINFO* pStatus = (ST_CLOSEUSERINFO*)(pMsg + HEAD_SIZE);
// 		strcpy(pStatus->szUserNo, m_szUserNo);
// 		strcpy(pStatus->szSiteCode, m_pSiteCode);
// 		pStatus->nUserType = m_nUserType;
// 
// 		pMission->SetData(pMsg, HEAD_SIZE + sizeof(ST_CLOSEUSERINFO));
// 
// 		delete[]pMsg;
// 		pMsg = NULL;
// 
// 		CMissionFactory::GetInstance()->AddMission(pMission);
// 	}

	MAP_USER::iterator itFind = _mapUser_.find(m_Name);
	if (itFind != _mapUser_.end())
	{
		itFind->second = NULL;
	}

	CListUser::GetInstance()->RemoveUser(this);
//	CListUser::GetFreeInstance()->AddUser(this);
	m_bNoUse = TRUE;
	delete this;
}

void CNetUser::OnWrite(int nDataLen)
{
	LockSend();
	m_nSendCount--;
	UnLockSend();

	if (nDataLen > 0)
	{
		//把已发送的数据从缓冲区中移除
		m_pNetSendBuffer->FlushBuf(nDataLen);
	}
	NotifySendData();
}

void CNetUser::OnRead(char* pData, int nDataLen)
{
	LockRecv();
	m_nRecvCount--;
	if (m_nRecvCount<0)
	{
		m_nRecvCount = 0;
	}
	UnLockRecv();
	if (nDataLen>0)
	{
// 		UpdateLastPacketTime();
 		m_pNetRecvBuffer->AddBuf(nDataLen);
		std::cout << "OnRead-" << nDataLen << std::endl;
	}
	int nMissionSize = 0;
	if (m_pNetRecvBuffer->GetMission(nMissionSize))
	{
		//有一条完整的命令
		CMission* pMission = CMissionFactory::GetInstance()->CreateMission();
		if (!pMission)
		{
			std::cout << " no memery\n";
			return;
		}
		pMission->SetNetUset(this);
		pMission->SetData(m_pNetRecvBuffer->GetBufPtr(), nMissionSize);
		m_pNetRecvBuffer->FlushBuf(nMissionSize);
		CMissionFactory::GetInstance()->AddMission(pMission);
		if (!m_bNoUse)
		{//该连接已经关闭则不接收数据
			m_pTcpContext->RecvData(m_pNetRecvBuffer->GetUnUsedBufPtr(), m_pNetRecvBuffer->GetUnUsedSize());
		}
	}
	else
	{
		int nLen = m_pNetRecvBuffer->GetUnUsedSize();
		if (nLen <= 0)
		{//没有空间,暂时不处理
		}
		else
		{
			if (!m_bNoUse)
			{//该连接已经关闭则不接收数据
				if (m_pTcpContext)
				{
					m_pTcpContext->RecvData(m_pNetRecvBuffer->GetUnUsedBufPtr(), nLen);
				}
			}
		}
	}
}

int CNetUser::SetSendData(char* pData, int nDataLen)
{
	if (m_bNoUse)
		return 0;
	if (!m_pNetSendBuffer->AddData(pData, nDataLen))
	{
		return 0;
	}
	if (!NotifySendData())
		return 0;
	return  nDataLen;
}

void CNetUser::LockSend(void)
{
	m_LockSend.lock();
}

void CNetUser::UnLockSend(void)
{
	m_LockSend.unlock();
}

void CNetUser::LockRecv(void)
{
	m_LockRecv.lock();
}

void CNetUser::UnLockRecv(void)
{
	m_LockRecv.unlock();
}

BOOL CNetUser::NotifySendData(void)
{
	LockSend();
	if (m_nSendCount == 0)
	{
		if (m_pTcpContext)
		{
			DWORD dwCount = m_pNetSendBuffer->GetDataSize();
			if (dwCount == 0)
			{
				UnLockSend();
				return TRUE;
			}
			if (dwCount > MAX_SENDPACK_LEN)
			{
				dwCount = MAX_SENDPACK_LEN;
			}
			m_pNetSendBuffer->CopyData(m_SendPacketBuf, dwCount);
			int ret = m_pTcpContext->SendData(m_SendPacketBuf, dwCount);
			if (ret == -1)
			{//投递失败
				UnLockSend();
				return FALSE;
			}
		}
		m_nSendCount++;
	}

	UnLockSend();
	return TRUE;
}

// 发送结果
BOOL CNetUser::SendResult(WORD dwCmd, int nResultCode)
{
	if (m_bNoUse)
	{
		return FALSE;
	}
	ST_CMD_HEADER resultCmd;
	resultCmd.usVerifyCode = VERIFYCODE;
	resultCmd.usCmd		= dwCmd;
	resultCmd.uPackSize = 0;
	resultCmd.usResult	= nResultCode;

	return SetSendData((char*)&resultCmd, HEAD_SIZE);

}

// 发送反馈信息
BOOL CNetUser::SendResponesInfo(WORD dwCmd, char* pInfoData, DWORD dwInfoSize)
{

	if (m_bNoUse)
	{
		return FALSE;
	}
	ST_CMD_HEADER resultCmd;
	resultCmd.usVerifyCode = VERIFYCODE;
	resultCmd.usCmd = dwCmd;
	resultCmd.uPackSize = dwInfoSize;
	resultCmd.usResult = RESULT_SUCCESS;

	if (!m_pNetSendBuffer->AddData((char*)&resultCmd, HEAD_SIZE))
	{
		return FALSE;
	}
	if (dwInfoSize > 0 && pInfoData)
	{
		if (!m_pNetSendBuffer->AddData(pInfoData, dwInfoSize))
		{//把先前加入发送队列的头取出
			m_pNetSendBuffer->Flushtail(HEAD_SIZE);
			return FALSE;
		}
	}

	if (!NotifySendData())
		return FALSE;

	return TRUE;
}

void CNetUser::UpdateLogonInfo(char* pUserName, char* pPwd)
{
	strcpy(m_Name, pUserName);
	strcpy(m_Password, pPwd);
}

