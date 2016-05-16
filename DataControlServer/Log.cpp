#include "Log.h"

CLog::CLog(char* pFileName)
: m_pLogger(NULL)
{
	memset(m_pFileName,0,255);
	if (pFileName)
	{
		strcpy(m_pFileName,pFileName);
	}
}

CLog::~CLog(void)
{
}

void CLog::Lock(void)
{
	m_fmLogLock.lock();
}

void CLog::UnLock()
{
	m_fmLogLock.unlock();
}

void CLog::LogOut(std::string& strInfo)
{
	if (!m_pLogger)
	{
		return;
	}
	Lock();
	m_pLogger->information(strInfo);
	UnLock();
}

void CLog::LogOut(char* pInfo)
{
	if (!m_pLogger)
	{
		return;
	}
	Lock();
	m_pLogger->information(pInfo);
	UnLock();
}

void CLog::LogOutError(std::string& strInfo)
{
	if (!m_pLogger)
	{
		return;
	}
	Lock();
	m_pLogger->error(strInfo);
	UnLock();
}

void CLog::SetFileName(char* pFileName)
{
	strcpy(m_pFileName,pFileName);
}

void CLog::SetFileName(std::string& strLogFileName)
{
	if (m_pLogger)
		return;

	Poco::AutoPtr<Poco::PatternFormatter> pFormatter(new Poco::PatternFormatter("%L%Y-%m-%d %H:%M:%S.%F %q:%t"));
	Poco::AutoPtr<Poco::FormattingChannel> pFCFile(new Poco::FormattingChannel(pFormatter));
	Poco::AutoPtr<Poco::FileChannel> pFileChannel(new Poco::FileChannel(strLogFileName));
	pFCFile->setChannel(pFileChannel);
	pFCFile->open();
	pFCFile->setProperty("rotation", "1 M");
	pFCFile->setProperty("archive", "timestamp");
	pFCFile->setProperty("compress", "true");
	pFCFile->setProperty("purgeCount", "5");
	//	Poco::Logger& appLogger = Poco::Logger::create("ScanTool", pFCFile, Poco::Message::PRIO_INFORMATION);
	m_pLogger = &(Poco::Logger::create("DCS", pFCFile, Poco::Message::PRIO_INFORMATION));
}
