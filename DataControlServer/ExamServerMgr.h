#pragma once

class CNetCmdMgr;
class CPaperRecvMgr;
class CExamServerMgr
{
public:
	CExamServerMgr();
	~CExamServerMgr();

	bool Start(void);
	void Stop(void);
	void SetLogDir(void);
	// ��������ͨ��
	bool StartCmdChannel(void);
	// �����ļ�����ͨ��
	bool StartFileChannel(void);
	void StopCmdChannel(void);
	void StopFileChannel(void);
	// ����ͨ��
	CNetCmdMgr* m_pNetCmdMgr;
	CPaperRecvMgr* m_pPaperRecvMgr;
};

