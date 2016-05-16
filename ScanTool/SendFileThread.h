#pragma once
#include "global.h"
#include "FileUpLoad.h"

class CSendFileThread :
	public Poco::Runnable
{
public:
	CSendFileThread(std::string& strIP, int nPort);
	~CSendFileThread();


	virtual void run();

	void HandleTask(pSENDTASK pTask);
	void SendFileComplete(char* pName, char* pSrcPath);

	CFileUpLoad		m_upLoad;

	std::string _strIp;
	int		_nPort;
};

