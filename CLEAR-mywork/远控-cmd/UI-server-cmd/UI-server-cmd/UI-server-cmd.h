
// UI-server-cmd.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号


// CUIservercmdApp:
// 有关此类的实现，请参阅 UI-server-cmd.cpp
//

class CUIservercmdApp : public CWinApp
{
public:

	 WORD wVersionRequested;
	 WSADATA wsaData;
	 int err;
	 int post;
	 sockaddr_in addr;
	 SOCKET s;
	 int length;

	 sockaddr_in clientaddr;
	 SOCKET sclient;
	 
	 static UINT ThreadFunc(LPVOID pParam);//线程函数声明
//自定义全局变量
public:
	CUIservercmdApp();

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CUIservercmdApp theApp;
