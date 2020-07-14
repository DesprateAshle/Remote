#pragma once
#include<iostream>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
struct Mysession
{
	SOCKET sclient;  //客户端套接字

	sockaddr_in clientaddr; //客户端ip

	DWORD clientlasttime;//客户端最后活跃时间
};
