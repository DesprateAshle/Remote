#pragma once
#include<iostream>
#define SEVER_CMD_COMMAND 1001
#define CLIENT_CMD_BACK 1002
#define CLIENT_KEYBOARD_BACK 2001
struct DATA
{
	unsigned int type;
	unsigned int length;
	char reallydata[1];
};