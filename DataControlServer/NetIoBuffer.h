#pragma once
#include "global.h"

#define MAX_TCPBUF_LEN			102400		//100K
#define MAX_SENDPACK_LEN		4096		//ÿ�η���4K
#define MAX_SENBUF_LEN			10485760	//���ͻ��������Ŀռ�10M
class CNetIoBuffer
{
public:
	CNetIoBuffer(void);
	~CNetIoBuffer(void);


private:
	char				m_buf[MAX_TCPBUF_LEN];
	int					m_nUsed;
	CRITICAL_SECTION	m_Lock;
public:
	void	Lock(void);
	void	UnLock(void);
	BOOL	AddBuf(int nDataLen);
	BOOL	AddData(char* pData, int nDataLen);
	int		GetData(char* pData, int nDataLen);
	char*	GetBufPtr(void);
	char*	GetUnUsedBufPtr(void);
	int		GetDataSize(void);
	void	FlushBuf(int nDataLen);
	BOOL	GetMission(int& nMissionSize);
	int		GetUnUsedSize(void);
	// ���ݴӺ���ǰ�Ƴ�
	void Flushtail(int nDataLen);
	void ResetPosition(void);
};
