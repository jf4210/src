//#include "StdAfx.h"
#include "NetIoBuffer.h"
#include "DCSDef.h"

CNetIoBuffer::CNetIoBuffer(void)
: m_nUsed(0)
{
	ZeroMemory(m_buf,MAX_TCPBUF_LEN);
	InitializeCriticalSection(&m_Lock);
}

CNetIoBuffer::~CNetIoBuffer(void)
{
	DeleteCriticalSection(&m_Lock);
}

void CNetIoBuffer::Lock(void)
{
	EnterCriticalSection(&m_Lock);
}

void CNetIoBuffer::UnLock(void)
{
	LeaveCriticalSection(&m_Lock);
}

//网络接收到数据后直接更改指针
BOOL CNetIoBuffer::AddBuf(int nDataLen)
{
	Lock();
	if (m_nUsed+nDataLen>MAX_TCPBUF_LEN)
	{
		UnLock();
		return FALSE;
	}
	m_nUsed+=nDataLen;
	UnLock();
	return TRUE;
}

//向缓冲区中存放待发送数据
BOOL CNetIoBuffer::AddData(char* pData, int nDataLen)
{
	Lock();
	if (m_nUsed+nDataLen>MAX_TCPBUF_LEN)
	{
		UnLock();
		return FALSE;
	}
	memcpy(m_buf+m_nUsed,pData,nDataLen);
	m_nUsed+=nDataLen;
	UnLock();
	return TRUE;
}

//从缓冲区中取走数据
int CNetIoBuffer::GetData(char* pData, int nDataLen)
{
	Lock();
	if (nDataLen>m_nUsed)
	{
		nDataLen=m_nUsed;
	}
	memcpy(pData,m_buf,nDataLen);
	m_nUsed-=nDataLen;
	if (m_nUsed>0)
	{
		memmove(m_buf,m_buf+nDataLen,m_nUsed);
	}
	
	UnLock();
	return nDataLen;
}
//获取缓冲区开始地址
char* CNetIoBuffer::GetBufPtr(void)
{
	return m_buf;
}

char* CNetIoBuffer::GetUnUsedBufPtr(void)
{
	return (m_buf+m_nUsed);
}

int CNetIoBuffer::GetDataSize(void)
{
	return m_nUsed;
}

int CNetIoBuffer::GetUnUsedSize(void)
{
	Lock();
	int nDataLen=MAX_TCPBUF_LEN-m_nUsed;
	UnLock();
	return nDataLen;
}


void CNetIoBuffer::FlushBuf(int nDataLen)
{
	Lock();
	if (nDataLen>m_nUsed)
	{
		UnLock();
		return;
	}

	m_nUsed-=nDataLen;
	memmove(m_buf,m_buf+nDataLen,m_nUsed);
	UnLock();
}

//判断缓冲区中是否有完整的命令
BOOL CNetIoBuffer::GetMission(int& nMissionSize)
{
	Lock();
	if (m_nUsed<HEAD_SIZE)
	{
		UnLock();
		return FALSE;
	}

	ST_CMD_HEADER* header=(ST_CMD_HEADER*)m_buf;
	if (header->usVerifyCode != VERIFYCODE)
	{//说明接收的数据有问题
		std::cout << "data wrong\n";
	}
	else
	{
		if (m_nUsed<(header->uPackSize+HEAD_SIZE))
		{
			UnLock();
			return FALSE;
		}
		nMissionSize=HEAD_SIZE+header->uPackSize;
//		TRACE("get mission\n");
	}
	UnLock();
	return TRUE;
}


// 数据从后先前移除
void CNetIoBuffer::Flushtail(int nDataLen)
{
	Lock();
	if(m_nUsed<nDataLen)
	{
		UnLock();
		return;
	}
	m_nUsed-=nDataLen;
	UnLock();
}

void CNetIoBuffer::ResetPosition(void)
{
	Lock();
	m_nUsed=0;
	UnLock();
}
