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

	//���ӶϿ���֪ͨ
	void OnClose(void);
	// ������ɵ�֪ͨ
	void OnWrite(int nDataLen);
	// ������ɵ�֪ͨ
	void OnRead(char* pData, int nDataLen);

	void SetAnswerInfo(ST_FILE_INFO info);
	bool SendResult(unsigned short usCmd, int nResultCode);

	int WriteAnswerFile(char* pData, int nDataLen);

	// ���ļ���MD5
	char			m_szAnswerMd5[LEN_MD5];
	//�ļ���
	char			m_szFileName[256];
	char			m_szFilePath[256];
	// �ļ����ܳ���
	unsigned int	m_dwTotalFileSize;
	// �Ѿ����յ��ļ�����
	DWORD			m_dwRecvFileSize;
	// ���յĴ��ļ��Ƿ����0-�޴���1-�д���
	int				m_nAnswerPacketError;
	ITcpContext*			m_pTcpContext;
	CListPaperUser			m_PaperUserList;

	char			m_PacketBuf[ANSWERPACK_LEN];
	char			m_ResponseBuf[512];

	bool CheckAnswerFile(void);
	void ClearAnswerInfo(void);
};

