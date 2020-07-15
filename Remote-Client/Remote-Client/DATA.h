#pragma once
#include<iostream>
#define SEVER_CMD_COMMAND 1001
#define CLIENT_CMD_BACK 1002
#define CLIENT_KEYBOARD_BACK 2001
#define SERVER_SCREEN_COMMAND 3001
#define CLIENT_SCREEN_BACK 3002
#pragma pack(push)
#pragma pack(1)
struct DATA
{
	unsigned int type;
	unsigned int length;
	char reallydata[];
};
#pragma pack(pop) 

inline bool recvdata(SOCKET s, char* pbuf, int bufsize)
{
	if (bufsize <= 0 || pbuf == NULL || s == INVALID_SOCKET)
	{
		return false;
	}

	int recvedsize = 0;

	while (recvedsize<bufsize)
	{
		int ret = recv(s, pbuf + recvedsize, bufsize - recvedsize, 0);
		if (ret <= 0) return false;
		else recvedsize += ret;
	}
	return true;
}

inline bool senddata(SOCKET s, unsigned int type, char* pbuf, int bufsize)
{
	if (bufsize <= 0 || pbuf == NULL || s == INVALID_SOCKET)
	{
		return false;
	}
	DATA pkt;
	pkt.length = bufsize;
	pkt.type = type;


	send(s, (char*)&pkt, sizeof(unsigned int) * 2, 0);
	send(s, pbuf, bufsize, 0);

	return true;
}