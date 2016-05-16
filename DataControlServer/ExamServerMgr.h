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
	// 启动命令通道
	bool StartCmdChannel(void);
	// 启动文件传输通道
	bool StartFileChannel(void);
	void StopCmdChannel(void);
	void StopFileChannel(void);
	// 命令通道
	CNetCmdMgr* m_pNetCmdMgr;
	CPaperRecvMgr* m_pPaperRecvMgr;
};

