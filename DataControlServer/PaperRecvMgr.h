#pragma once
#include "NetOperatorDll.h"
#include "ListPaperUser.h"
#include "PaperUser.h"
#include <list>

using namespace std;
class CPaperRecvMgr :
	public ITcpServerNotify
{
public:
	CPaperRecvMgr();
	~CPaperRecvMgr();
	void OnAccept(ITcpContext* pTcpContext);

private:
	ITcpServer* m_pTcpServer;
	CListPaperUser	m_listPaperUser;

public:
	bool StartWork(const char* pLocalIP, WORD wPort);
	void StopWork(void);
	bool m_bStop;
};

