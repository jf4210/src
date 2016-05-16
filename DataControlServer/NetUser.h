#pragma once
#include "NetOperatorDll.h"
//#include "ListUser.h"
#include "Net_Cmd_Protocol.h"
#include "NetIoBuffer.h"
#include "NetIoSendBuffer.h"

class CListUser;
class CNetUser :
	public ITcpContextNotify
{
public:
	CNetUser(ITcpContext* pTcpContext);
	~CNetUser();
	//用户名
	char				m_Name[LEN_NAME + 1];
	//密码
	char				m_Password[LEN_PWD + 1];
	char				m_pIPAddress[LEN_IPADDRESS + 1];
	WORD				m_wPort;

	ITcpContext*		m_pTcpContext;
	//挂起的接收操作数量
	int					m_nRecvCount;
	//挂起的发送操作数量
	int					m_nSendCount;
	//网络命令接收缓冲区
	CNetIoBuffer		*m_pNetRecvBuffer;
	//网络发送缓冲区
	CNetIoSendBuffer	*m_pNetSendBuffer;

	char				m_SendPacketBuf[MAX_SENDPACK_LEN];
	Poco::FastMutex		m_LockSend;
	Poco::FastMutex		m_LockRecv;

	//连接断开的通知
	void OnClose(void);
	// 发送完成的通知
	void OnWrite(int nDataLen);
	// 接收完成的通知
	void OnRead(char* pData, int nDataLen);

	// 添加需要发送的数据
	int SetSendData(char* pData, int nDataLen);

	BOOL SendResult(WORD dwCmd, int nResultCode);
	BOOL SendResponesInfo(WORD dwCmd, char* pInfoData, DWORD dwInfoSize);

	void LockSend(void);
	void LockRecv(void);
	void UnLockSend(void);
	void UnLockRecv(void);

	BOOL NotifySendData(void);

	// 登录成功后更新基本信息
	void UpdateLogonInfo(char* pUserName, char* pPwd);

	void InitContext(ITcpContext* pTcpContext);

private:
	// 该对象是否在使用
	BOOL m_bNoUse;
};

