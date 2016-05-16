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

//������յ����ݺ�ֱ�Ӹ���ָ��
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

//�򻺳����д�Ŵ���������
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

//�ӻ�������ȡ������
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
//��ȡ��������ʼ��ַ
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

//�жϻ��������Ƿ�������������
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
	{//˵�����յ�����������
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


// ���ݴӺ���ǰ�Ƴ�
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
