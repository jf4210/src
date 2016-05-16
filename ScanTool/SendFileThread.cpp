#include "stdafx.h"
#include "SendFileThread.h"

CSendFileThread::CSendFileThread(std::string& strIP, int nPort)
: _strIp(strIP), _nPort(nPort), m_upLoad(*this)
{
	g_pLogger->information("CSendFileThread start.");
	TRACE("CSendFileThread start.\n");
}

CSendFileThread::~CSendFileThread()
{
	g_pLogger->information("CSendFileThread exit.");
	TRACE("CSendFileThread exit.\n");
}

void CSendFileThread::run()
{
	USES_CONVERSION;
	m_upLoad.InitUpLoadTcp(A2T(_strIp.c_str()), _nPort);

	while (!g_nExitFlag)
	{
		pSENDTASK pTask = NULL;
		g_fmSendLock.lock();
		SENDTASKLIST::iterator it = g_lSendTask.begin();
		for (; it != g_lSendTask.end();)
		{
			pTask = *it;
			it = g_lSendTask.erase(it);
			break;
		}
		g_fmSendLock.unlock();
		if (NULL == pTask)
		{
			Poco::Thread::sleep(500);
			continue;
		}

		HandleTask(pTask);

		delete pTask;
		pTask = NULL;
	}
}

void CSendFileThread::HandleTask(pSENDTASK pTask)
{
	USES_CONVERSION;
	
	Poco::File fileSrcPath(pTask->strPath);
	if (!fileSrcPath.exists())
	{
		char szLog[300] = { 0 };
		sprintf_s(szLog, "发送文件(%s)失败,路径不存在: %s", pTask->strFileName, pTask->strPath);
		g_pLogger->information(szLog);
		TRACE(szLog);
		return;
	}

	m_upLoad.SendAnsFile(A2T(pTask->strPath.c_str()), A2T(pTask->strFileName.c_str()));

}

void CSendFileThread::SendFileComplete(char* pName, char* pSrcPath)
{
	char szLog[300] = { 0 };
	sprintf_s(szLog, "发送文件(%s)完成.", pName);
	g_pLogger->information(szLog);
}


