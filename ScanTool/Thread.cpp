// Thread.cpp: implementation of the CThread class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "Thread.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CThread::CThread()
{
	m_dwThreadID=0;
	m_hThread=NULL;
	m_evStop = CreateEvent(NULL,TRUE,TRUE,NULL);
	SetEvent(m_evStop);
}

CThread::~CThread()
{
	if(m_evStop)
	{
		CloseHandle(m_evStop);
		m_evStop = NULL;
	}
}

bool CThread::StartThread()
{
	//����߳��Ѿ�����������Ҫ�ٴ���
	if (!m_hThread){ 
		//�����������̺߳���
		m_hThread = CreateThread(
					NULL,			//the handle cannot be inherited. 
                    0,				//the default Thread Stack Size is set
                    InitThreadProc,	//�̺߳���
                    this,			//�̺߳����Ĳ���
                    0,				//ʹ�̺߳������������������
                    &m_dwThreadID	//receives the thread identifier
					);
                
        }//end if (!m_hThread...

	if (m_hThread)
		ResetEvent(m_evStop);

	return	m_hThread != NULL;
}

void CThread::WaitForStop()
{		
	WaitForSingleObject(m_evStop,INFINITE);

	// �����߳̾��
	HANDLE hThread = (HANDLE)InterlockedExchange((LONG *)&m_hThread, 0);
	if (hThread) {
		// �ȴ��߳���ֹ
		WaitForSingleObject(hThread, INFINITE);
		// �ر��߳̾��
		CloseHandle(hThread);
	}// end if (hThread...
}

unsigned long  CThread::ThreadProc()
{
	ThreadProcMain();
	SetEvent(m_evStop);
	return 0;
}

