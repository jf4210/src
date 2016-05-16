#include "stdafx.h"
#include "FileUpLoad.h"
#include "Net_Cmd_Protocol.h"
#include "SendFileThread.h"

CFileUpLoad::CFileUpLoad(CSendFileThread& rNotify)
 :m_pITcpClient(NULL)
,m_uThreadType(0)
,m_bConnect(FALSE)
,m_bUpLoad(TRUE)
,m_bSendOK(FALSE)
,m_bReadyOK(FALSE)
, m_rNotify(rNotify)
{
	ZeroMemory(m_szSendBuf, FILE_BUFF);
	m_hAddAnsEvent=CreateEventW(NULL,TRUE,FALSE,NULL);
	m_hSendReadyEvent=CreateEventW(NULL,TRUE,FALSE,NULL);
	m_hSendDoneEvent=CreateEventW(NULL,TRUE,FALSE,NULL);
	staticTimer = 0; //计时
}

CFileUpLoad::~CFileUpLoad(void)
{
	UnInit();
}
#if 1
void CFileUpLoad::OnTcpClientNotifyReceivedData( const char* pData,int nLen )
{
	//接受到文件服务器的答案上传成功指令
	//通知主程序文件标志更改
	ST_CMD_HEADER stHead;
	memcpy(&stHead,pData,HEAD_SIZE);
	switch(stHead.usCmd)
	{
	case RESPONSE_UPLOADANS:
		ProcessUpLoadResponse(stHead.usResult == RESULT_SUCCESS);
		break;
	case NOTIFY_RECVANSWERFIN:
		ProcessUpLoadDone(stHead.usResult == RESULT_SUCCESS);
		break;
	}
}

void CFileUpLoad::OnTcpClientNotifyDisconnect( UINT uSocket )
{
	TRACE0("file server disconnect!\n");
	m_uThreadType = 1;
	m_bConnect = FALSE;
}

BOOL CFileUpLoad::SendAnsFile(CString strFilePath, CString strFileName)
{
	stUpLoadAns* stAns = new stUpLoadAns();
	stAns->strAnsName = strFileName;
	stAns->strPath = strFilePath;
	stAns->bUpload = FALSE;
#if 1
	m_listFile.push_back(stAns);
#else
	m_VecAns.push_back(stAns);
#endif

	TRACE0("add ans file %s\n", strFileName);
	SetEvent(m_hAddAnsEvent);
	return TRUE;
	
}

void CFileUpLoad::ThreadProcMain()
{
RESTART:
	if (m_uThreadType == 1)
	{
		while(m_bConnect == FALSE)
		{
			DWORD dwTimeLatest=0;
			DWORD dwTimeWait=0;
			dwTimeLatest = GetTickCount();
			dwTimeWait = dwTimeLatest - staticTimer;
			if (dwTimeWait > 3000)
			{
				staticTimer = dwTimeLatest;
				USES_CONVERSION;
				m_pITcpClient = CreateTcpClient(*this,T2A(m_strAddr),m_usPort);
				if (m_pITcpClient == NULL)
				{
					TRACE0("\nconect to File Server failed!\n");
					Sleep(100);
					continue;
				}
				else
				{
					TRACE0("\nconect to File Server Success!\n");
					m_bConnect=TRUE;
					m_uThreadType=2;
					goto RESTART;
				}
			}
		}
	}
	else if (m_uThreadType == 2)
	{

#if 1
		while (m_bUpLoad)
		{
			if (!m_bConnect) goto RESTART;
			bool bFindTask = false;
			std::list<stUpLoadAns*>::iterator it = m_listFile.begin();
			for (; it != m_listFile.end(); it++)
			{
				stUpLoadAns* pTask = *it;

				if (pTask->bUpload == FALSE)
				{
					bFindTask = true;
					CFile MyFileSend(pTask->strPath, CFile::modeRead);
					DWORD Length = MyFileSend.GetLength();
					char	*szFileBuff = new char[Length];
					MyFileSend.Seek(0, CFile::begin);
					MyFileSend.Read(szFileBuff, Length);
					MyFileSend.Close();

					TRACE0("start send ans file\n");
					//上传文件
					ST_CMD_HEADER stHead;
					stHead.usCmd = REQUEST_UPLOADANS;
					stHead.uPackSize = sizeof(ST_FILE_INFO)+Length;
					ST_FILE_INFO stAnsInfo;
					USES_CONVERSION;
					strcpy(stAnsInfo.szFileName, T2A(pTask->strAnsName));

					char *pMd5 = MD5File(T2A(pTask->strPath));
					memcpy(stAnsInfo.szMD5, pMd5, LEN_MD5);
					stAnsInfo.dwFileLen = Length;
					memcpy(m_szSendBuf, &stHead, HEAD_SIZE);
					memcpy(m_szSendBuf + HEAD_SIZE, &stAnsInfo, sizeof(ST_FILE_INFO));
				RESENDHEAD:
					sendData(m_szSendBuf, HEAD_SIZE + sizeof(ST_FILE_INFO));
					WaitForSingleObject(m_hSendReadyEvent, INFINITE);
					ResetEvent(m_hSendReadyEvent);
					if (m_bReadyOK == FALSE)
					{
						goto RESENDHEAD;
					}
					sendData(szFileBuff, Length);
					WaitForSingleObject(m_hSendDoneEvent, INFINITE);
					ResetEvent(m_hSendDoneEvent);
					if (m_bSendOK == FALSE)
					{
						goto RESENDHEAD;
					}
					pTask->bUpload = TRUE;
					delete szFileBuff;
					delete pMd5;
					TRACE0("end send ans file\n");
					m_rNotify.SendFileComplete(stAnsInfo.szFileName, T2A(pTask->strPath));
				}
			}
			if (!bFindTask)	Sleep(1000);
		}
#else
		while(m_bUpLoad)
		{
			WaitForSingleObject(m_hAddAnsEvent,INFINITE);
			std::vector<stUpLoadAns*>::iterator it = m_VecAns.begin();
			for (;it!=m_VecAns.end();it++)
			{
				if ((*it)->bUpload == FALSE)
				{
					stUpLoadAns* pTask = *it;

					CFile MyFileSend(pTask->strPath, CFile::modeRead);
					DWORD Length = MyFileSend.GetLength();
					char	*szFileBuff = new char[Length];
					MyFileSend.Seek(0,CFile::begin);
					MyFileSend.Read(szFileBuff,Length);
					MyFileSend.Close();

					TRACE0("start send ans file\n");
					//上传文件
					ST_CMD_HEADER stHead;
					stHead.usCmd = REQUEST_UPLOADANS;
					stHead.uPackSize = sizeof(ST_FILE_INFO)+Length;
					ST_FILE_INFO stAnsInfo;
					USES_CONVERSION;
					strcpy(stAnsInfo.szFileName, T2A(pTask->strAnsName));

					char *pMd5 = MD5File(T2A(pTask->strPath));
					memcpy(stAnsInfo.szMD5,pMd5,LEN_MD5);
					stAnsInfo.dwFileLen = Length;
					memcpy(m_szSendBuf, &stHead, HEAD_SIZE);
					memcpy(m_szSendBuf + HEAD_SIZE, &stAnsInfo, sizeof(ST_FILE_INFO));
				RESENDHEAD:
					sendData(m_szSendBuf, HEAD_SIZE + sizeof(ST_FILE_INFO));
					WaitForSingleObject(m_hSendReadyEvent,INFINITE);
					ResetEvent(m_hSendReadyEvent);
					if (m_bReadyOK==FALSE)
					{
						goto RESENDHEAD;
					}
					sendData(szFileBuff,Length);
					WaitForSingleObject(m_hSendDoneEvent,INFINITE);
					ResetEvent(m_hSendDoneEvent);
					if (m_bSendOK==FALSE)
					{
						goto RESENDHEAD;
					}
					pTask->bUpload = TRUE;
					delete szFileBuff;
					delete pMd5;
					TRACE0("end send ans file\n");
					m_rNotify.SendFileComplete(stAnsInfo.szFileName, T2A(pTask->strPath));
				}
			}
			ResetEvent(m_hAddAnsEvent);
		}
#endif

	}
}

BOOL CFileUpLoad::InitUpLoadTcp(CString strAddr,USHORT usPort)
{
	m_strAddr = strAddr;
	m_usPort  = usPort;
	m_uThreadType=1;
	return CThread::StartThread();
}

void CFileUpLoad::sendData( char * szBuff, DWORD nLen)
{
	ULONG uOffset = 0;
	if(m_pITcpClient == NULL)
		return;

	while(uOffset < nLen)
	{
		if ((nLen-uOffset)<TCP_PACKET_MAXSIZE)
		{
			m_pITcpClient->SendData(szBuff + uOffset, nLen-uOffset);
			uOffset=nLen;
		}
		else
		{
			m_pITcpClient->SendData(szBuff + uOffset,TCP_PACKET_MAXSIZE);
			uOffset+=TCP_PACKET_MAXSIZE;
		}
	}
}

//反馈结果成功则记录，失败则重发
void CFileUpLoad::ProcessUpLoadResponse(BOOL bFlag )
{
	m_bReadyOK=bFlag;
	SetEvent(m_hSendReadyEvent);
}

BOOL CFileUpLoad::CheckUpLoadFile()
{
	if (m_VecAns.size() < 5)
	{
		return FALSE;
	}
	std::vector<stUpLoadAns*>::iterator it = m_VecAns.begin();
	for (;it != m_VecAns.end();it++)
	{
		stUpLoadAns *stAns = (stUpLoadAns*)*it;
		if (stAns->bUpload == FALSE)
		{
			return FALSE;
		}
	}
	return TRUE;
}

void CFileUpLoad::ProcessUpLoadDone( BOOL bFlag )
{	
	m_bSendOK = bFlag;
	SetEvent(m_hSendDoneEvent);
}

void CFileUpLoad::UnInit()
{
	m_bConnect=TRUE;
	
	m_bUpLoad = FALSE;

	if (m_pITcpClient)
	{
		m_pITcpClient->ReleaseConnections();
		m_pITcpClient=NULL;
	}
	// 	m_VecAns.clear();
	std::vector<stUpLoadAns*>::iterator it = m_VecAns.begin();
	for (;it!=m_VecAns.end();it++)
	{
		if (*it)
		{
			delete (*it);
			(*it)=NULL;
		}

	}
}
#endif