#pragma once
#include<iostream>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include"CscreenDlg.h"
struct Mysession
{
	SOCKET sclient;  //�ͻ����׽���

	sockaddr_in clientaddr; //�ͻ���ip

	DWORD clientlasttime;//�ͻ�������Ծʱ��

	CscreenDlg* pscreendlg;

	Mysession() {
		pscreendlg = NULL;
	}
};
