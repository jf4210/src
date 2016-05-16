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
	//�û���
	char				m_Name[LEN_NAME + 1];
	//����
	char				m_Password[LEN_PWD + 1];
	char				m_pIPAddress[LEN_IPADDRESS + 1];
	WORD				m_wPort;

	ITcpContext*		m_pTcpContext;
	//����Ľ��ղ�������
	int					m_nRecvCount;
	//����ķ��Ͳ�������
	int					m_nSendCount;
	//����������ջ�����
	CNetIoBuffer		*m_pNetRecvBuffer;
	//���緢�ͻ�����
	CNetIoSendBuffer	*m_pNetSendBuffer;

	char				m_SendPacketBuf[MAX_SENDPACK_LEN];
	Poco::FastMutex		m_LockSend;
	Poco::FastMutex		m_LockRecv;

	//���ӶϿ���֪ͨ
	void OnClose(void);
	// ������ɵ�֪ͨ
	void OnWrite(int nDataLen);
	// ������ɵ�֪ͨ
	void OnRead(char* pData, int nDataLen);

	// �����Ҫ���͵�����
	int SetSendData(char* pData, int nDataLen);

	BOOL SendResult(WORD dwCmd, int nResultCode);
	BOOL SendResponesInfo(WORD dwCmd, char* pInfoData, DWORD dwInfoSize);

	void LockSend(void);
	void LockRecv(void);
	void UnLockSend(void);
	void UnLockRecv(void);

	BOOL NotifySendData(void);

	// ��¼�ɹ�����»�����Ϣ
	void UpdateLogonInfo(char* pUserName, char* pPwd);

	void InitContext(ITcpContext* pTcpContext);

private:
	// �ö����Ƿ���ʹ��
	BOOL m_bNoUse;
};

