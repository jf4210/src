#include "SysSet.h"
#include "global.h"

CSysSet CSysSet::m_SysSet;
CSysSet::CSysSet(void)
{
}

CSysSet::~CSysSet(void)
{
}

CSysSet& CSysSet::GetRef()
{
	return m_SysSet;
}

bool CSysSet::Load(std::string& strConfPath)
{
	Poco::AutoPtr<Poco::Util::IniFileConfiguration> pConf(new Poco::Util::IniFileConfiguration(strConfPath));
	m_sLocalIP			= pConf->getString("Net.LocalIP", "127.0.0.1");
	m_nPaperUpLoadPort	= pConf->getInt("Net.FilePort", 19980);
	m_nCmdPort			= pConf->getInt("Net.CmdPort", 19981);
	m_strUpLoadPath		= pConf->getString("Sys.upLoadPath");
	m_nDecompressThreads = pConf->getInt("Sys.decompressThreads", 2);

	m_nSendHttpThreads	= pConf->getInt("UpHttp.sendThreads", 2);
	m_nHttpTimeOut		= pConf->getInt("UpHttp.sendTimeout", 60);
	m_nSendTimes		= pConf->getInt("UpHttp.sendTimes", 3);
	m_nIntervalTime		= pConf->getInt("UpHttp.intervalTime", 5);
	m_strUpLoadHttpUri	= pConf->getString("UpHttp.upUri");
	m_strBackUri		= pConf->getString("UpHttp.backUri");
	m_strScanReqUri		= pConf->getString("UpHttp.scanReqUri");
	return true;
}
