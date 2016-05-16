#pragma once
#include "global.h"

enum LOGLEVEL{LOG_DEBUG=0,LOG_SERIOUS,LOG_ERROR} ;
class CLog
{
public:
	CLog(char* pFileName=NULL);
	~CLog(void);
	void UnLock();
	void Lock(void);
	void LogOut(char* pInfo);
	void LogOut(std::string& strInfo);
	void LogOutError(std::string& strInfo);
	void SetFileName(char* pFileName);
	void SetFileName(std::string& strLogFileName);
public:
	Poco::FastMutex		m_fmLogLock;
	Poco::Logger* m_pLogger;
//	Poco::Logger m_Logger;
	char m_pFileName[255];
};
