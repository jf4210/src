#include "SendToHttpThread.h"


CSendToHttpThread::CSendToHttpThread()
{
	std::cout << "HttpSendThread start.\n";
}


CSendToHttpThread::~CSendToHttpThread()
{
	std::cout << "HttpSendThread exit.\n";
}

void CSendToHttpThread::run()
{
	while (!g_nExitFlag)	//!g_exit.tryWait(0)
	{
		pSEND_HTTP_TASK pTask = NULL;
		try
		{
			g_fmHttpSend.lock();
			LIST_SEND_HTTP::iterator it = g_lHttpSend.begin();
			for (; it != g_lHttpSend.end();)
			{
				if (!(*it)->nSendFlag || ((*it)->nSendFlag > 0 && (*it)->nSendFlag < SysSet.m_nSendTimes && (*it)->sTime.elapsed() > SysSet.m_nIntervalTime * 1000000))
				{
					pTask = *it;
					it = g_lHttpSend.erase(it);
					break;
				}
				else if ((*it)->nSendFlag >= SysSet.m_nSendTimes)		//此任务连续发送5次，每次间隔5秒，都失败了，将它删除
				{
					std::string strEraseInfo = "Erase task: ";
					strEraseInfo.append((*it)->pPic->strFileName);
					g_Log.LogOutError(strEraseInfo);
					std::cout << strEraseInfo << std::endl;

					pTask->pPic->bUpLoadFlag = false;
					pTask->pPapers->fmNum.lock();
					pTask->pPapers->nUpLoadFail++;
					GenerateResult(pTask->pPapers, pTask);
					pTask->pPapers->fmNum.unlock();

					pTask = *it;
					it = g_lHttpSend.erase(it);
					delete pTask;
					pTask = NULL;
				}
				else
					it++;
			}
			g_fmHttpSend.unlock();

			if (pTask == NULL)
			{
				Poco::Thread::sleep(200);
				continue;
			}
			std::cout << "Get http upload task: " << pTask->pPic->strFileName << std::endl;

			Poco::URI uri(pTask->strUri);				//pTask->strUri
			Poco::Net::HTTPClientSession session;
			Poco::Net::HTTPRequest request;
			Poco::Net::HTTPResponse response;

			if (!doRequest(session, request, uri, pTask))
			{
				std::string strLog = "Open file fail: " + pTask->pPic->strFilePath;
				g_Log.LogOutError(strLog);
				std::cout << strLog << std::endl;

				pTask->pPic->bUpLoadFlag = false;
				pTask->pPapers->fmNum.lock();
				pTask->pPapers->nUpLoadFail++;
				GenerateResult(pTask->pPapers, pTask);
				pTask->pPapers->fmNum.unlock();

				delete pTask;
				pTask = NULL;
				continue;
			}

			std::istream& iStr = session.receiveResponse(response);  // get the response from server
//			std::cout << response.getStatus() << "\t" << response.getReason() << std::endl;

			if (response.getStatus() == Poco::Net::HTTPResponse::HTTP_OK)
			{
				std::string strResultStatus;
				std::string strContentEncoding;
				if (response.has("Content-Encoding"))
				{
					strContentEncoding = response.get("Content-Encoding");
				}
//				std::cout << strContentEncoding << std::endl;
				if (strContentEncoding == "gzip")
				{
					std::ostringstream outStringStream;
					Poco::InflatingInputStream inflater(iStr, Poco::InflatingStreamBuf::STREAM_GZIP);
					outStringStream << inflater.rdbuf();
					strResultStatus = outStringStream.str();
//					std::cout << strResultStatus << std::endl;
				}
				else
				{
					std::ostringstream outStringStream;
					outStringStream << iStr.rdbuf();
					strResultStatus = outStringStream.str();
//					std::cout << strResultStatus << std::endl;
				}

				if (pTask->nTaskType == 1)
				{
					if (!ParseResult(strResultStatus, pTask))
					{		//返回的数据解析失败
						pTask->pPic->bUpLoadFlag = false;
						pTask->pPapers->fmNum.lock();
						pTask->pPapers->nUpLoadFail++;
						GenerateResult(pTask->pPapers, pTask);
						pTask->pPapers->fmNum.unlock();
					}
					else
					{
						pTask->pPic->bUpLoadFlag = true;
						pTask->pPapers->fmNum.lock();
						pTask->pPapers->nUpLoadSuccess++;
						GenerateResult(pTask->pPapers, pTask);
						pTask->pPapers->fmNum.unlock();
					}
				}
				else if (pTask->nTaskType == 2)		//提交给后端
				{
					std::string strLog = "post papers result info success, papersName: " + pTask->pPapers->strPapersName + "\tdetail: " + pTask->strResult;
					g_Log.LogOut(strLog);
				}
			}
			else
			{
				std::string strLog;
				int nCode = response.getStatus();
				if (pTask->nTaskType == 1)
				{
					strLog = "send file fail: " + pTask->pPic->strFileName + "\tErrCode: " + response.getReason() + "\tPath: " + pTask->pPic->strFilePath;
					g_Log.LogOutError(strLog);
					std::cout << strLog << std::endl;

					pTask->pPic->bUpLoadFlag = false;
					pTask->pPapers->fmNum.lock();
					pTask->pPapers->nUpLoadFail++;
					GenerateResult(pTask->pPapers, pTask);
					pTask->pPapers->fmNum.unlock();
				}
				else if (pTask->nTaskType == 2)
				{
					strLog = "post papers result failed: " + pTask->pPapers->strPapersName + "\tErrCode: " + response.getReason() + "\tPath: " + pTask->pPapers->strPapersPath + "\nResult info: " + pTask->strResult;
					g_Log.LogOutError(strLog);
					std::cout << strLog << std::endl;
				}
			}
			delete pTask;
			pTask = NULL;
		}
		catch (Poco::Exception& exc)
		{
			std::string strErrorInfo;
			char szFailTimes[30] = { 0 };
			sprintf(szFailTimes, " --- Send fail times : %d", pTask->nSendFlag);

			strErrorInfo.append(exc.displayText());
			strErrorInfo.append(szFailTimes);
			if (pTask->nTaskType == 1)
			{
				strErrorInfo.append("\tFileInfo: " + pTask->pPic->strFileName);
				strErrorInfo.append("\tFilePath: " + pTask->pPic->strFilePath);
			}
			else if (pTask->nTaskType == 2)
			{
				strErrorInfo.append("\tPapersName: " + pTask->pPapers->strPapersName);
				strErrorInfo.append("\tPath: " + pTask->pPapers->strPapersPath);
			}			

			std::cout << "\n";
			std::cout << strErrorInfo << std::endl;
			g_Log.LogOutError(strErrorInfo);

			pTask->nSendFlag++;
			pTask->sTime.update();
			//发送失败，再放入队列
			g_fmHttpSend.lock();
			g_lHttpSend.push_back(pTask);
			g_fmHttpSend.unlock();
		}
		catch (...)
		{
			std::string strErrorInfo;
			char szFailTimes[5] = { 0 };
			sprintf(szFailTimes, "%d\n", pTask->nSendFlag);

			if (pTask->nTaskType == 1)
			{
				strErrorInfo.append("**** Unknown error ***\tFileInfo: " + pTask->pPic->strFileName + "\t---Send fail times: ");
				strErrorInfo.append(szFailTimes);
				strErrorInfo.append("\tFilePath: " + pTask->pPic->strFilePath);
			}
			else if (pTask->nTaskType == 2)
			{
				strErrorInfo.append("**** Unknown error ***\tPapersName: " + pTask->pPapers->strPapersName);
				strErrorInfo.append("\tPath: " + pTask->pPapers->strPapersPath);
			}			

			std::cout << "\n";
			std::cout << strErrorInfo << std::endl;
			g_Log.LogOutError(strErrorInfo);

			pTask->nSendFlag++;
			pTask->sTime.update();
			//发送失败，再放入队列
			g_fmHttpSend.lock();
			g_lHttpSend.push_back(pTask);
			g_fmHttpSend.unlock();
		}
	}
}

bool CSendToHttpThread::doRequest(Poco::Net::HTTPClientSession& session, Poco::Net::HTTPRequest& request, Poco::URI& uri, pSEND_HTTP_TASK pTask)
{
	std::string path(uri.getPathAndQuery());
	if (path.empty()) path = "/";

	request.setURI(path);
	request.setMethod(Poco::Net::HTTPRequest::HTTP_POST);
	request.setVersion(Poco::Net::HTTPMessage::HTTP_1_1);
	request.setKeepAlive(true);

	session.setHost(uri.getHost());
	session.setPort(uri.getPort());
	session.setKeepAlive(true);

	request.set("User-Agent", "Go-http-client/1.1");
	request.set("Accept-Encoding", "gzip");	//gzip	//text/html

	std::string strBody;
	if (pTask->nTaskType == 1)
	{
		std::ifstream fin(pTask->pPic->strFilePath, std::ifstream::binary);
		if (!fin)	return false;
		std::stringstream buffer;
		buffer << fin.rdbuf();
		strBody = buffer.str();
		fin.close();

		request.setContentType("jpeg");
		request.setContentLength(strBody.length());
	}
	else if (pTask->nTaskType == 2)		//提交给后端
	{
		strBody = pTask->strResult;
		request.setContentType("application/json");
		request.setContentLength(strBody.length());
	}

	std::ostream& ostr = session.sendRequest(request);
	ostr << strBody;

	return true;
}

bool CSendToHttpThread::ParseResult(std::string& strInput, pSEND_HTTP_TASK pTask)
{
	bool bResult = false;
	Poco::JSON::Parser parser;
	Poco::Dynamic::Var result;
	try
	{
		result = parser.parse(strInput);
		Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

		bResult = object->get("ret").convert<bool>();
		if (bResult)
		{
			Poco::JSON::Object::Ptr objData = object->getObject("info");
			std::string strMd5 = objData->get("md5").convert<std::string>();
			int nSize = objData->get("size").convert<int>();
			pTask->pPic->strHashVal = strMd5;
		}
		else
		{
			Poco::JSON::Object::Ptr objData = object->getObject("error");

			std::stringstream jsnString;
			objData->stringify(jsnString, 0);

			std::string strErrInfo;
			strErrInfo.append("Error when upload file: " + pTask->pPic->strFileName + "\tDetail: " + jsnString.str());
			g_Log.LogOutError(strErrInfo);
			std::cout << strErrInfo << std::endl;
		}
	}
	catch (Poco::JSON::JSONException& jsone)
	{
		std::string strErrInfo;
		strErrInfo.append("Error when parse json: ");
		strErrInfo.append(jsone.message() + "\tData:" + strInput);
		g_Log.LogOutError(strErrInfo);
		std::cout << strErrInfo << std::endl;
		return false;
	}
	catch (Poco::Exception& exc)
	{
		std::string strErrInfo;
		strErrInfo.append("Error: ");
		strErrInfo.append(exc.message() + "\tData:" + strInput);
		g_Log.LogOutError(strErrInfo);
		std::cout << strErrInfo << std::endl;
		return false;
	}
	catch (...)
	{
		std::string strErrInfo;
		strErrInfo.append("Unknown error.\tData:" + strInput);
		g_Log.LogOutError(strErrInfo);
		std::cout << strErrInfo << std::endl;
		return false;
	}

	return bResult;
}

bool CSendToHttpThread::GenerateResult(pPAPERS_DETAIL pPapers, pSEND_HTTP_TASK pTask)
{
	if (pPapers->nUpLoadSuccess + pPapers->nUpLoadFail != pPapers->nTotalPics)
		return false;

	Poco::JSON::Object jsnPapers;
	jsnPapers.set("papers", CMyCodeConvert::Gb2312ToUtf8(pPapers->strPapersName));
	jsnPapers.set("papersDesc", CMyCodeConvert::Gb2312ToUtf8(pPapers->strDesc));
	jsnPapers.set("totalNum", pPapers->nTotalPaper);
	jsnPapers.set("qkNum", pPapers->nQk);
	Poco::JSON::Array  paperArry;

	LIST_PAPER_INFO::iterator it2 = pPapers->lPaper.begin();
	for (; it2 != pPapers->lPaper.end(); it2++)
	{
		pPAPER_INFO pPaper = *it2;

		Poco::JSON::Object jsnPaper;
		jsnPaper.set("paperName", pPaper->strName);
		Poco::JSON::Array  picArry;

		std::cout << "---- " << pPaper->strName << " ----" << std::endl;
		LIST_PIC_DETAIL::iterator itPic = pPaper->lPic.begin();
		for (; itPic != pPaper->lPic.end(); itPic++)
		{
			pPIC_DETAIL pPic = *itPic;
			int nPos1 = pPic->strFileName.rfind("_");
			int nPos2 = pPic->strFileName.rfind(".");
			std::string strNum = pPic->strFileName.substr(nPos1 + 1, nPos2 - nPos1 - 1);
			
			char szTmp[20] = { 0 };
			sprintf(szTmp, "%s", strNum.c_str());
			char szLetter[20] = { 0 };
			sscanf(szTmp, "%[A-Za-z]", szLetter);
			int nLenLetter = strlen(szLetter);
			char szNum[15] = { 0 };
			memmove(szNum, szTmp + nLenLetter, strlen(szTmp) - nLenLetter);
			int nSN = atoi(szNum);

			Poco::JSON::Object jsnPic;
			jsnPic.set("picSN", nSN);
			jsnPic.set("picName", pPic->strFileName);
			jsnPic.set("upLoadFlag", (int)pPic->bUpLoadFlag);
			jsnPic.set("hashVal", pPic->strHashVal);
			picArry.add(jsnPic);

			std::cout << "pic: " << (*itPic)->strFileName << "\tupLoadFlag: "<< (*itPic)->bUpLoadFlag << "\tMD5: "<< (*itPic)->strHashVal << std::endl;
		}
		std::cout << "---------------" << std::endl;

		jsnPaper.set("picList", picArry);
		paperArry.add(jsnPaper);
	}
	jsnPapers.set("paperList", paperArry);
	
	std::stringstream jsnString;
	jsnPapers.stringify(jsnString, 0);

// 	std::cout << "********** 提交给后端数据: **********\n" << std::endl;
// 	std::cout << jsnString.str() << std::endl;
// 	std::cout << "****************************************" << std::endl;

#if 0
	pSEND_HTTP_TASK pNewTask = new SEND_HTTP_TASK;
	pNewTask->nTaskType = 2;
	pNewTask->strResult = jsnString.str();
	pNewTask->pPapers	= pPapers;
	pNewTask->strUri	= SysSet.m_strBackUri;
	g_fmHttpSend.lock();
	g_lHttpSend.push_back(pNewTask);
	g_fmHttpSend.unlock();
#endif

	return true;
}

