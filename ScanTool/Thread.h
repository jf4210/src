#pragma once

class CThread
{
public:
	CThread();
	virtual ~CThread();
public:
	/*
	�����̣߳��ɹ�����true,ʧ�ܷ���false,ֻ���ڸý��̴������߳�������ϵͳ����ʱ�ŷ���false
	*/
	bool StartThread(void);
	/*
	�ȴ��߳��˳������ô˺���ǰ������÷�������ȷ�������̺߳������˳���������ô˺���������
	*/
	void WaitForStop(void);

	/*
	�˺���ֻ�����ڲ�����
	*/
	static DWORD WINAPI InitThreadProc(PVOID pObj){
		return	((CThread*)pObj)->ThreadProc();
	}
	/*
	�˺���ֻ�����ڲ�����
	*/
	unsigned long ThreadProc(void);

protected:
	/*
	���̺߳������������ڴ˺�����ʵ���߳�Ҫ������
	*/
	virtual void ThreadProcMain(void)=0;
protected:
	DWORD	m_dwThreadID;		// �̱߳�ʶ
	HANDLE	m_hThread;			// �߳̾��
	HANDLE	m_evStop;
};

