#pragma once

class CThread
{
public:
	CThread();
	virtual ~CThread();
public:
	/*
	启动线程，成功返回true,失败返回false,只有在该进程创建的线程数到达系统限制时才返回false
	*/
	bool StartThread(void);
	/*
	等待线程退出，调用此函数前必须调用方法，以确保从主线程函数中退出，否则调用此函数会阻塞
	*/
	void WaitForStop(void);

	/*
	此函数只供类内部调用
	*/
	static DWORD WINAPI InitThreadProc(PVOID pObj){
		return	((CThread*)pObj)->ThreadProc();
	}
	/*
	此函数只供类内部调用
	*/
	unsigned long ThreadProc(void);

protected:
	/*
	主线程函数，您可以在此函数中实现线程要做的事
	*/
	virtual void ThreadProcMain(void)=0;
protected:
	DWORD	m_dwThreadID;		// 线程标识
	HANDLE	m_hThread;			// 线程句柄
	HANDLE	m_evStop;
};

