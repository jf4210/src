#pragma once
#include "global.h"

//发送环形缓冲区
class CNetIoSendBuffer
{
public:
	CNetIoSendBuffer(void);
	~CNetIoSendBuffer(void);

private:
	char	*m_buf;
	int		m_nReadPos;
	int		m_nWritePos;
	DWORD	m_dwBufTotalLength;
//	CRITICAL_SECTION	m_bufLock;
	Poco::FastMutex		m_bufLock;
public:
	int AddData(const char* pData, int nDataLen);
	int CopyData(char* pData, int nDataLen);
	void Lock(void);
	void UnLock(void);
	int m_nDataSize;
	void FlushBuf(int nDataLen);
	DWORD GetDataSize(void);
	void Flushtail(int nDataLen);
	// 重新分配大的缓冲区
	BOOL RecallocBuf(DWORD dwLen);
	void ResetPosition(void);
};
