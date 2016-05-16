#ifndef __NETOPERATOR_H__
#define __NETOPERATOR_H__


class ITcpContextNotify
{
public:
	//���ӶϿ���֪ͨ
	virtual void OnClose()=0;
	//�������ݳɹ���֪ͨ��nDataLen�Ѿ����͵�������
	virtual void  OnWrite(int nDataLen)=0;
	//�������ݳɹ���֪ͨ
	virtual void OnRead(char* pData,int nDataLen)=0;
};

class ITcpContext
{
public:
	virtual void ReleaseConnections(void)=0;
	virtual void SetTcpContextNotify(ITcpContextNotify* pTcpContextNotify)=0;
	//Ͷ�ݷ��Ͳ���������ֵΪ-1˵��Ͷ��ʧ��
	virtual int	 SendData(const char* pData,int nDataLen)=0;
	//Ͷ�ݽ��ղ���������ֵΪ-1˵��Ͷ��ʧ��
	virtual	int	 RecvData(char* pData,int nDataLen)=0;
	// ��ȡ�Է�IP�Ͷ˿�
	virtual void GetRemoteAddr(char* pIPAddress, unsigned short &port)=0;
};

class ITcpServerNotify
{
public:
	virtual void OnAccept(ITcpContext* pTcpContext)=0;

};

class ITcpServer
{
public:
	virtual void ReleaseConnections(void)=0;
	virtual bool RemoveTcpContext(ITcpContext* pTcpContext)=0;
};

extern "C" __declspec(dllexport) ITcpServer * CreateTcpServer(ITcpServerNotify& rNotify,const char* pIPAddress,unsigned short usPort);

extern "C" __declspec(dllexport) void SetLogFileName(char* pLogName);
#endif