#include "stdafx.h"
#include "MyCodeConvert.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include <iosfwd>
#include <string.h>
#include <stdarg.h>
#include <fstream>
#include <sstream>
#include <iomanip>  

#include <cassert>
#include<time.h>


#include <errno.h>


#ifdef WIN32   //window

#include <Windows.h>
#include <shlwapi.h>
#include <direct.h>

#include <tchar.h>
#pragma comment(lib,"Kernel32.lib")
#pragma comment(lib,"shlwapi.lib")

#elif __linux    //linux
#include <cassert>
#include <sys/types.h>
#include <iconv.h>
#include <stdio.h>

#endif


#include "Poco/Foundation.h"
#include "Poco/Mutex.h"

using Poco::FastMutex;
FastMutex g_mutexConverter;

#ifdef WIN32   //window


static std::string Gb32ToUtf8(const char * lpszGb32Text)
{
	int nUnicodeBufLen = MultiByteToWideChar(CP_ACP, 0, lpszGb32Text, -1, 0, 0);
	if (nUnicodeBufLen == 0)
		return "";

	WCHAR* pUnicodeBuf = new WCHAR[nUnicodeBufLen];
	if (pUnicodeBuf == 0)
		return "";

	MultiByteToWideChar(CP_ACP, 0, lpszGb32Text, -1, pUnicodeBuf, nUnicodeBufLen);

	int nUtf8BufLen = WideCharToMultiByte(CP_UTF8, 0, pUnicodeBuf, -1, 0, 0, NULL, NULL);
	if (nUtf8BufLen == 0)
	{
		delete[] pUnicodeBuf;
		return "";
	}

	char* pUft8Buf = new char[nUtf8BufLen];
	if (pUft8Buf == 0)
	{
		delete[] pUnicodeBuf;
		return "";
	}

	WideCharToMultiByte(CP_UTF8, 0, pUnicodeBuf, -1, pUft8Buf, nUtf8BufLen, NULL, NULL);

	std::string strUtf8 = pUft8Buf;

	delete[] pUnicodeBuf;
	delete[] pUft8Buf;

	return strUtf8;
}

static std::string Utf8ToGb32(const char * lpszUft8Text)
{
	int nUnicodeBufLen = MultiByteToWideChar(CP_UTF8, 0, lpszUft8Text, -1, 0, 0);
	if (nUnicodeBufLen == 0)
		return "";

	WCHAR* pUnicodeBuf = new WCHAR[nUnicodeBufLen];
	if (pUnicodeBuf == 0)
		return "";

	MultiByteToWideChar(CP_UTF8, 0, lpszUft8Text, -1, pUnicodeBuf, nUnicodeBufLen);

	int nGb32BufLen = WideCharToMultiByte(CP_ACP, 0, pUnicodeBuf, -1, 0, 0, NULL, NULL);
	if (nGb32BufLen == 0)
	{
		delete[] pUnicodeBuf;
		return "";
	}

	char* pGb32Buf = new char[nGb32BufLen];
	if (pGb32Buf == 0)
	{
		delete[] pUnicodeBuf;
		return "";
	}

	WideCharToMultiByte(CP_ACP, 0, pUnicodeBuf, -1, pGb32Buf, nGb32BufLen, NULL, NULL);

	std::string strGb32 = pGb32Buf;

	delete[] pUnicodeBuf;
	delete[] pGb32Buf;

	return strGb32;
}
#elif __linux    //linux

// 编码转换操作类->基于byte
//
class CodeConverterBase {

	//示例...
	//std::string strxxxxcvx=json;
	//CodeConverterBase cc2 = CodeConverterBase("gb2312","utf-8");
	//char outbuf[3000];
	//cc2.convert((char *)strxxxxcvx.c_str(),strxxxxcvx.size(),outbuf,1024);
	//strUtf8=outbuf;


private:
	iconv_t cd;
public:

	// 构造
	CodeConverterBase(const char *from_charset, const char *to_charset)
	{
		cd = iconv_open(to_charset, from_charset);
	}

	// 析构
	~CodeConverterBase() {
		iconv_close(cd);
	}

	// 转换输出
	size_t convert(char *inbuf, size_t inlen, char *outbuf, size_t outlen)
	{
		char *pin = inbuf;
		char *pout = outbuf;
		memset(outbuf, 0, outlen);
		return iconv(cd, &pin, (size_t *)&inlen, &pout, (size_t *)&outlen);
	}
};

//字符串转换操作 ->基于string
class CodeConverterString
{
public:
	CodeConverterString()
	{
	}

	~CodeConverterString()
	{
	}

	static std::string Gb2312ToUtf8(std::string strSource)
	{
		std::string strRet = "";
		if (strSource.size() <= 0)
			return strRet;

		CodeConverterBase cc2 = CodeConverterBase("gbk", "utf-8//TRANSLIT//IGNORE");//  //TRANSLIT//IGNORE 转换失败找相似编码.

		size_t nInLen = (strSource.size()) + 1;
		char * szInBuf = (char *)malloc(nInLen);

		size_t nOutLen = (strSource.size()) * 4 + 4;
		char * szOutBuf = (char *)malloc(nOutLen);

		do
		{
			if (0 == szOutBuf || 0 == szInBuf)
				break;

			memset(szInBuf, 0, nInLen);
			memcpy(szInBuf, strSource.c_str(), strSource.size());

			size_t nError = cc2.convert((char *)szInBuf, nInLen, szOutBuf, nOutLen);

			if (nError < 0)
				std::cout << "Gb2312ToUtf8 ,nError=" << nError << ",errno =" << errno << std::endl;

			strRet = szOutBuf;

		} while (0);

		if (szOutBuf)
		{
			free(szOutBuf);
			szOutBuf = NULL;
		}

		if (szInBuf)
		{
			free(szInBuf);
			szInBuf = NULL;
		}

		return strRet;
	}

	static std::string Utf8ToGb2312(std::string strSource)
	{
		std::string strRet = "";

		if (strSource.size() <= 0)
			return strRet;

		CodeConverterBase cc2 = CodeConverterBase("utf-8", "gbk//TRANSLIT//IGNORE");//  //TRANSLIT//IGNORE 转换失败找相似编码.

		size_t nInLen = (strSource.size()) + 1;
		char * szInBuf = (char *)malloc(nInLen);

		size_t nOutLen = (strSource.size()) * 4 + 4;
		char * szOutBuf = (char *)malloc(nOutLen);

		do
		{
			if (0 == szOutBuf || 0 == szInBuf)
				break;

			memset(szInBuf, 0, nInLen);
			memcpy(szInBuf, strSource.c_str(), strSource.size());

			size_t nError = cc2.convert((char *)szInBuf, nInLen, szOutBuf, nOutLen);

			if (nError < 0)
				std::cout << "Utf8ToGb2312 ,nError=" << nError << ",errno =" << errno << std::endl;

			strRet = szOutBuf;

		} while (0);

		if (szOutBuf)
		{
			free(szOutBuf);
			szOutBuf = NULL;
		}

		if (szInBuf)
		{
			free(szInBuf);
			szInBuf = NULL;
		}
		return strRet;
	}
};
#endif



CMyCodeConvert::CMyCodeConvert()
{

}

CMyCodeConvert::~CMyCodeConvert()
{

}


std::string CMyCodeConvert::Gb2312ToUtf8(std::string strSource)
{
	FastMutex::ScopedLock lock(g_mutexConverter);
#ifdef WIN32   //window
	return Gb32ToUtf8(strSource.c_str());
#elif __linux    //linux
	return CodeConverterString::Gb2312ToUtf8(strSource);
#endif
}

//
std::string CMyCodeConvert::Utf8ToGb2312(std::string strSource)
{
	FastMutex::ScopedLock lock(g_mutexConverter);
#ifdef WIN32   //window
	return Utf8ToGb32(strSource.c_str());
#elif __linux    //linux
	return CodeConverterString::Utf8ToGb2312(strSource);
#endif
}