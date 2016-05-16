#pragma once
#include"DCSDef.h"

class CScanResquestHandler : public Poco::Runnable
{
public:
	CScanResquestHandler();
	~CScanResquestHandler();

	virtual void run();
	void	HandleTask(pSCAN_REQ_TASK pTask);

	bool	ParseResult(std::string& strInput);
};

