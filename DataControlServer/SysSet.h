#pragma once
#include <string>

using namespace std;
class CSysSet
{
public:
	~CSysSet(void);
	static CSysSet&		GetRef();
private:
	CSysSet(void);
	static CSysSet m_SysSet;
public:
	bool Load(std::string& strConfPath);
	// 用于通讯的本地IP
	string m_sLocalIP;
	// 命令通道监听端口
	int m_nPaperUpLoadPort;
	// 指令通道监听端口
	int m_nCmdPort;

	int m_nDecompressThreads;
	int m_nSendHttpThreads;
	int m_nHttpTimeOut;		//发送文件间隔时间
	int m_nSendTimes;		//发送失败后重发次数
	int m_nIntervalTime;	//发送失败的间隔时间

	//心跳监测间隔，单位秒
	int m_nHeartPacketTime;
	string m_strCurrentDir;
	string m_strUpLoadPath;
	string m_strDecompressPath;
	string m_strUpLoadHttpUri;
	string m_strBackUri;
	string m_strScanReqUri;
};

#define SysSet CSysSet::GetRef()
