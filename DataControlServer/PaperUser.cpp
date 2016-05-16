#include "PaperUser.h"
#include "DCSDef.h"

CPaperUser::CPaperUser(ITcpContext* pTcpContext, CListPaperUser& PaperUserList)
: m_dwTotalFileSize(0)
, m_dwRecvFileSize(0)
, m_nAnswerPacketError(0)
, m_pTcpContext(pTcpContext)
, m_PaperUserList(PaperUserList)
{
	ZeroMemory(m_szFileName, sizeof(m_szFileName));
	ZeroMemory(m_szFilePath, sizeof(m_szFilePath));
	if (m_pTcpContext)
	{
		m_pTcpContext->SetTcpContextNotify(this);
	}
}

CPaperUser::~CPaperUser()
{
}

void CPaperUser::OnClose(void)
{
	std::cout << "file sender close" << std::endl;
	g_Log.LogOut("file sender close");
	delete this;
}

void CPaperUser::OnWrite(int nDataLen)
{
}

void CPaperUser::OnRead(char* pData, int nDataLen)
{
	if (nDataLen == 0)
	{
		m_pTcpContext->RecvData(m_PacketBuf, HEAD_SIZE + sizeof(ST_FILE_INFO));
	}
	else
	{
		if (m_nAnswerPacketError == 1 || m_dwTotalFileSize == 0)
		{
			//头信息
			ST_CMD_HEADER header = *(ST_CMD_HEADER*)m_PacketBuf;
			if (header.usVerifyCode != VERIFYCODE)
				return;

			if (header.usCmd != REQUEST_UPLOADANS)
				return;

			ST_FILE_INFO AnswerInfo = *(ST_FILE_INFO*)(m_PacketBuf + HEAD_SIZE);
			strcpy_s(m_szFileName, AnswerInfo.szFileName);
			char szLog[300] = { 0 };
			sprintf(szLog, "start recv file: %s", AnswerInfo.szFileName);
			g_Log.LogOut(szLog);
			std::cout << szLog << std::endl;

			SetAnswerInfo(AnswerInfo);
			SendResult(RESPONSE_UPLOADANS, RESULT_SUCCESS);
			m_nAnswerPacketError = 0;
			int nWantDataLen = m_dwTotalFileSize - m_dwRecvFileSize;
			if (nWantDataLen > ANSWERPACK_LEN)
			{
				nWantDataLen = ANSWERPACK_LEN;
			}
			m_pTcpContext->RecvData(m_PacketBuf, nWantDataLen);
		}
		else
		{
			int nWantDataSize = m_dwTotalFileSize - m_dwRecvFileSize;
			if (nDataLen > nWantDataSize)
			{
			}
			else
			{
				if (WriteAnswerFile(m_PacketBuf, nDataLen) == 0)
				{
					m_nAnswerPacketError = 1;
					ClearAnswerInfo();
					SendResult(RESPONSE_UPLOADANS, RESULT_ERROR_FILEIO);
					return;
				}

				if (m_dwRecvFileSize == m_dwTotalFileSize)
				{
					//文件接收完成
					if (CheckAnswerFile())
					{
						SendResult(NOTIFY_RECVANSWERFIN, RESULT_SUCCESS);
						char szLog[300] = { 0 };
						sprintf(szLog, "recv file: %s completed.", m_szFilePath);
						g_Log.LogOut(szLog);
						std::cout << szLog << std::endl;

						pDECOMPRESSTASK pDecompressTask = new DECOMPRESSTASK;
						pDecompressTask->strFilePath = m_szFilePath;
						pDecompressTask->strFileName = m_szFileName;
						pDecompressTask->strFileName = pDecompressTask->strFileName.substr(0, pDecompressTask->strFileName.length() - 4);
						g_fmDecompressLock.lock();
						g_lDecompressTask.push_back(pDecompressTask);
						g_fmDecompressLock.unlock();
					}
					else
					{
						m_nAnswerPacketError = 1;
						SendResult(NOTIFY_RECVANSWERFIN, RESULT_ERROR_CHECKMD5);
					}
					ClearAnswerInfo();
					m_pTcpContext->RecvData(m_PacketBuf, HEAD_SIZE + sizeof(ST_FILE_INFO));
				}
				else
				{
					int nWantDataLen = m_dwTotalFileSize - m_dwRecvFileSize;
					if (nWantDataLen > ANSWERPACK_LEN)
					{
						nWantDataLen = ANSWERPACK_LEN;
					}
					m_pTcpContext->RecvData(m_PacketBuf, nWantDataLen);
				}
			}
		}
	}
}

bool CPaperUser::SendResult(unsigned short usCmd, int nResultCode)
{
	ST_CMD_HEADER* resultCmd = (ST_CMD_HEADER*)m_ResponseBuf;
	resultCmd->usCmd	= VERIFYCODE;
	resultCmd->usCmd	= usCmd;
	resultCmd->uPackSize	= 0;
	resultCmd->usResult		= nResultCode;
	m_pTcpContext->SendData(m_ResponseBuf, HEAD_SIZE);
	return true;
}

void CPaperUser::SetAnswerInfo(ST_FILE_INFO info)
{
	string strFilePath = SysSet.m_strUpLoadPath + "\\";
	strFilePath.append(info.szFileName);

	try
	{
		Poco::File upLoadPath(SysSet.m_strUpLoadPath);
		if (!upLoadPath.exists())
			upLoadPath.createDirectories();

		Poco::File filePath(strFilePath);
		if (filePath.exists())
			filePath.remove(true);
	}
	catch (Poco::Exception &exc)
	{
		std::string strLog;
		strLog.append("upLoadPath createDirectories or filePath remove error: " + exc.displayText());
		g_Log.LogOut(strLog);
		std::cout << strLog << std::endl;
	}

	strcpy(m_szAnswerMd5, info.szMD5);
	sprintf(m_szFilePath, "%s", strFilePath.c_str());
	m_dwTotalFileSize = info.dwFileLen;
	m_dwRecvFileSize = 0;
}

bool CPaperUser::CheckAnswerFile(void)
{
	try
	{
		Poco::MD5Engine md5;
		Poco::DigestOutputStream dos(md5);

		std::ifstream istr(m_szFilePath, std::ios::binary);
		if (!istr)
		{
			string strLog = "calc MD5 failed 1: ";
			strLog.append(m_szFilePath);
			g_Log.LogOutError(strLog);
			std::cout << strLog << std::endl;
			return false;
		}
		Poco::StreamCopier::copyStream(istr, dos);
		dos.close();

		string strMd5 = Poco::DigestEngine::digestToHex(md5.digest());
		if (strcmp(strMd5.c_str(), m_szAnswerMd5))
		{
			string strLog = "calc MD5 failed 2: ";
			strLog.append(m_szFilePath);
			g_Log.LogOutError(strLog);
			std::cout << strLog << std::endl;
			return false;
		}
	}
	catch (...)
	{
		string strLog = "calc MD5 failed 3: ";
		strLog.append(m_szFilePath);
		g_Log.LogOutError(strLog);
		std::cout << strLog << std::endl;
		return false;
	}

	return true;
}

void CPaperUser::ClearAnswerInfo(void)
{
	memset(m_szAnswerMd5, 0, LEN_MD5);
	memset(m_szFilePath, 0, 255);
	m_dwTotalFileSize = 0;
	m_dwRecvFileSize = 0;
}

int CPaperUser::WriteAnswerFile(char* pData, int nDataLen)
{
	try
	{
		FILE *pf = NULL;
		if (m_dwRecvFileSize == 0)
		{
			pf = fopen(m_szFilePath, "wb");
		}
		else
			pf = fopen(m_szFilePath, "ab");
		if (!pf)
		{
			return 0;
		}

		int ret = fwrite(pData, 1, nDataLen, pf);
		if (ret <= 0)
		{
			fclose(pf);
			return ret;
		}
		fclose(pf);
		m_dwRecvFileSize += nDataLen;
	}
	catch (...)
	{
		return 0;
	}

	return nDataLen;
}
