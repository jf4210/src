
// ScanTool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CScanToolApp: 
// �йش����ʵ�֣������ ScanTool.cpp
//

class CScanToolApp : public CWinApp
{
public:
	CScanToolApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CScanToolApp theApp;