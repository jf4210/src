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
	// ����ͨѶ�ı���IP
	string m_sLocalIP;
	// ����ͨ�������˿�
	int m_nPaperUpLoadPort;
	// ָ��ͨ�������˿�
	int m_nCmdPort;

	int m_nDecompressThreads;
	int m_nSendHttpThreads;
	int m_nHttpTimeOut;		//�����ļ����ʱ��
	int m_nSendTimes;		//����ʧ�ܺ��ط�����
	int m_nIntervalTime;	//����ʧ�ܵļ��ʱ��

	//�������������λ��
	int m_nHeartPacketTime;
	string m_strCurrentDir;
	string m_strUpLoadPath;
	string m_strDecompressPath;
	string m_strUpLoadHttpUri;
	string m_strBackUri;
	string m_strScanReqUri;
};

#define SysSet CSysSet::GetRef()
