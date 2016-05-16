#pragma once
#include "NetOperatorDll.h"
#include "ListPaperUser.h"
#include "Net_Cmd_Protocol.h"

#define ANSWERPACK_LEN		4096
class CPaperUser :
	public ITcpContextNotify
{
public:
	CPaperUser(ITcpContext* pTcpContext, CListPaperUser& PaperUserList);
	~CPaperUser();

	//连接断开的通知
	void OnClose(void);
	// 发送完成的通知
	void OnWrite(int nDataLen);
	// 接收完成的通知
	void OnRead(char* pData, int nDataLen);

	void SetAnswerInfo(ST_FILE_INFO info);
	bool SendResult(unsigned short usCmd, int nResultCode);

	int WriteAnswerFile(char* pData, int nDataLen);

	// 答案文件的MD5
	char			m_szAnswerMd5[LEN_MD5];
	//文件名
	char			m_szFileName[256];
	char			m_szFilePath[256];
	// 文件的总长度
	unsigned int	m_dwTotalFileSize;
	// 已经接收的文件长度
	DWORD			m_dwRecvFileSize;
	// 接收的答案文件是否出错0-无错误，1-有错误
	int				m_nAnswerPacketError;
	ITcpContext*			m_pTcpContext;
	CListPaperUser			m_PaperUserList;

	char			m_PacketBuf[ANSWERPACK_LEN];
	char			m_ResponseBuf[512];

	bool CheckAnswerFile(void);
	void ClearAnswerInfo(void);
};

