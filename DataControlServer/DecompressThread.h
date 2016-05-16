#pragma once
#include "DCSDef.h"


class CDecompressThread : public Poco::Runnable
{
public:
	CDecompressThread();
	~CDecompressThread();

	virtual void run();

	void HandleTask(pDECOMPRESSTASK pTask);

};

