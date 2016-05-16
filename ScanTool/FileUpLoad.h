#pragma once

#include "Thread.h"
#include <vector>
#include <list>
#include "NetCtrl.h"
#include "md5.h"

#define FILE_BUFF	1024*10

//文件上传结构体
typedef struct ST_UPLOAD_ANS
{
	CString strAnsName;
	CString strPath;
	BOOL	bUpload;
	ST_UPLOAD_ANS()
	{
		strAnsName = _T("");
		strPath = _T("");
		bUpload = FALSE;
	}
}stUpLoadAns, *pStUpLoadAns;

class CSendFileThread;
class CFileUpLoad : public ITcpClientNotify,public CThread
{
public:
	CFileUpLoad(CSendFileThread& rNotify);
	~CFileUpLoad(void);

	void		OnTcpClientNotifyReceivedData(const char* pData,int nLen);
	void		OnTcpClientNotifyDisconnect(UINT uSocket);
	void		OnTcpClientNotifyTime(DWORD dwTime){};
	void		ThreadProcMain();

	BOOL		InitUpLoadTcp(CString strAddr,USHORT usPort);
	void		UnInit();
	BOOL		SendAnsFile(CString strFilePath, CString strFileName);
	void		sendData(char * szBuff, DWORD nLen);
	
	void		ProcessUpLoadResponse(BOOL bFlag);
	void		ProcessUpLoadDone(BOOL bFlag);
	BOOL		CheckUpLoadFile();

	std::list<stUpLoadAns*>		m_listFile; 
	std::vector<stUpLoadAns*> m_VecAns;

	ITcpClient	*m_pITcpClient;
	UINT		m_uThreadType;
	CString		m_strAddr;
	USHORT		m_usPort;
	char		m_szSendBuf[FILE_BUFF];		//发送缓存
 	CRITICAL_SECTION m_csCrit;
	BOOL		m_bConnect;
	BOOL		m_bUpLoad;
	HANDLE		m_hAddAnsEvent;
	HANDLE		m_hSendReadyEvent;
	HANDLE		m_hSendDoneEvent;
	BOOL		m_bSendOK;
	BOOL		m_bReadyOK;
	DWORD       staticTimer; //计时
	CSendFileThread& m_rNotify;
};
