//#include "StdAfx.h"
#include "NetIoSendBuffer.h"
#include "DCSDef.h"

CNetIoSendBuffer::CNetIoSendBuffer(void)
: m_nReadPos(0)
, m_nWritePos(0)
, m_nDataSize(0)
, m_dwBufTotalLength(MAX_TCPBUF_LEN)
{
	m_buf=new char[MAX_TCPBUF_LEN];
//	InitializeCriticalSection(&m_bufLock);
}

CNetIoSendBuffer::~CNetIoSendBuffer(void)
{
	if (m_buf)
	{
		delete []m_buf;
		m_buf=NULL;
	}
//	DeleteCriticalSection(&m_bufLock);
}

int CNetIoSendBuffer::AddData(const char* pData, int nDataLen)
{
	Lock();
	if (nDataLen==0)
	{
		UnLock();
		return 0;
	}

	DWORD dwLen=nDataLen+m_nDataSize;
	if (dwLen>MAX_SENBUF_LEN)
	{

		UnLock();
//		g_Log.LogOut(LOG_SERIOUS,"待发送数据太多");
		g_Log.LogOut("待发送数据太多");
		return 0;
	}
	if (nDataLen+m_nDataSize>m_dwBufTotalLength)
	{
		if(!RecallocBuf(dwLen*1.5))
		{
			UnLock();
			return 0;
		}
	}

	if (m_nWritePos+nDataLen>m_dwBufTotalLength)
	{
		int nLeft=m_dwBufTotalLength-m_nWritePos;
		memcpy(m_buf+m_nWritePos,pData,nLeft);
		memcpy(m_buf,pData+nLeft,nDataLen-nLeft);
	}
	else
	{
		memcpy(m_buf+m_nWritePos,pData,nDataLen);
	}
	
	m_nWritePos=(m_nWritePos+nDataLen)%m_dwBufTotalLength;

	m_nDataSize+=nDataLen;
	UnLock();
	return nDataLen;
}

int CNetIoSendBuffer::CopyData(char* pData, int nDataLen)
{
	if (nDataLen>m_nDataSize)
	{
		nDataLen=m_nDataSize;
	}

	if(nDataLen==0)
		return 0;

	if (m_nReadPos+nDataLen>m_dwBufTotalLength)
	{
		int nLeft=m_dwBufTotalLength-m_nReadPos;
		memcpy(pData,m_buf+m_nReadPos,nLeft);
		memcpy(pData+nLeft,m_buf,nDataLen-nLeft);
	}
	else
	{
		memcpy(pData,m_buf+m_nReadPos,nDataLen);
	}
	return nDataLen;
}

void CNetIoSendBuffer::Lock(void)
{
//	EnterCriticalSection(&m_bufLock);
	m_bufLock.lock();
}

void CNetIoSendBuffer::UnLock(void)
{
//	LeaveCriticalSection(&m_bufLock);
	m_bufLock.unlock();
}

void CNetIoSendBuffer::FlushBuf(int nDataLen)
{
	
	if (nDataLen>m_nDataSize)
	{
		nDataLen=m_nDataSize;
	}

	if(nDataLen==0)
		return ;

	Lock();
	m_nReadPos=(m_nReadPos+nDataLen)%m_dwBufTotalLength;
	m_nDataSize-=nDataLen;
	UnLock();
}

DWORD CNetIoSendBuffer::GetDataSize(void)
{
	return m_nDataSize;
}

void CNetIoSendBuffer::Flushtail(int nDataLen)
{
	Lock();
	m_nWritePos=(m_nWritePos-nDataLen)%m_dwBufTotalLength;

	m_nDataSize-=nDataLen;
	UnLock();
}

// 重新分配大的缓冲区
BOOL CNetIoSendBuffer::RecallocBuf(DWORD dwLen)
{
	char* pBuf=new char[dwLen];
	if (!pBuf)
	{
		g_Log.LogOut("RecallocBuf-err:内存不足");
		return FALSE;
	}
	CopyData(pBuf,m_nDataSize);
	delete []m_buf;
	m_buf=pBuf;
	m_nReadPos=0;
	m_nWritePos=m_nDataSize;
	m_dwBufTotalLength=dwLen;
	return TRUE;
}

void CNetIoSendBuffer::ResetPosition(void)
{
	Lock();
	m_nReadPos=0;
	m_nWritePos=0;
	UnLock();
}
