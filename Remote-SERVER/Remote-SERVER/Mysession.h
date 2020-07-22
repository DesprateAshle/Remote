#pragma once
#include<iostream>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include"CscreenDlg.h"
#include"Ccmddlg.h"
#include"Ckeyboarddlg.h"
#include"Cprocessdlg.h"
struct Mysession
{
	SOCKET sclient;  //客户端套接字

	sockaddr_in clientaddr; //客户端ip

	DWORD clientlasttime;//客户端最后活跃时间

	CscreenDlg *pscreendlg;

	Ccmddlg *pcmddlg;

	Ckeyboarddlg *pkeyboarddlg;
	
	Cprocessdlg *pprocessdlg;

	Mysession() {
		pscreendlg = NULL;
		pcmddlg = NULL;
		pkeyboarddlg = NULL;
		pprocessdlg = NULL;
	}
};
