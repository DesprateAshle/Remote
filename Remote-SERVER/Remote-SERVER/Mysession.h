#pragma once
#include<iostream>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
struct Mysession
{
	SOCKET sclient;  //�ͻ����׽���

	sockaddr_in clientaddr; //�ͻ���ip

	DWORD clientlasttime;//�ͻ�������Ծʱ��
};
