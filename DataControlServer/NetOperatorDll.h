#ifndef __NETOPERATOR_H__
#define __NETOPERATOR_H__


class ITcpContextNotify
{
public:
	//连接断开的通知
	virtual void OnClose()=0;
	//发送数据成功的通知，nDataLen已经发送的数据量
	virtual void  OnWrite(int nDataLen)=0;
	//接收数据成功的通知
	virtual void OnRead(char* pData,int nDataLen)=0;
};

class ITcpContext
{
public:
	virtual void ReleaseConnections(void)=0;
	virtual void SetTcpContextNotify(ITcpContextNotify* pTcpContextNotify)=0;
	//投递发送操作，返回值为-1说明投递失败
	virtual int	 SendData(const char* pData,int nDataLen)=0;
	//投递接收操作，返回值为-1说明投递失败
	virtual	int	 RecvData(char* pData,int nDataLen)=0;
	// 获取对方IP和端口
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