#ifndef _MY_CODE_CONVERT_H
#define  _MY_CODE_CONVERT_H


#include <iostream>


using namespace std;

///////////
//±àÂë×ª»»Àà
//
class CMyCodeConvert
{
public:

	CMyCodeConvert();

	~CMyCodeConvert();

	//
	static std::string Gb2312ToUtf8(std::string strSource);

	//
	static std::string Utf8ToGb2312(std::string strSource);

};



#endif