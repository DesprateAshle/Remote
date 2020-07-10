#include<iostream>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

typedef struct MyData
{
    SOCKET server;
    bool isclose;
    //char key[32];
}MYDATA;

DWORD WINAPI sendthread(LPVOID lpParam)
{
    MyData* s = (MyData*)lpParam;
    int getdate;
    while (true)
    {
        char rbuf[256] = { 0 };
        getdate = recv(s->server, rbuf, 256, 0);
        if (getdate > 0)
        {
            cout << "-----------------------------来自服务器的数据-----------------------------" << endl;
            cout << endl << "获取到服务器发送了一个数据" << endl;
            cout << endl << rbuf << endl << endl;
            cout << "-----------------------------来自服务器的数据-----------------------------" << endl;
        }
        else if (getdate == 0)
        {
            cout << endl << endl << "服务器已关闭,请稍后重试!!" << endl;

            cout << "断开连接 正在尝试重新连接-----(请输入任意值并回车进行重连)" << endl;
            break;
        }
        else
        {
            cout << endl << endl << "连接断开,请检查网络!" << endl;
            cout << "断开连接 正在尝试重新连接-----(请输入任意值并回车进行重连)" << endl;
            break;
        }
    }
    return 0;
}

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

    while (true)
    {
        //1.socket
        SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
        //2.connet
        sockaddr_in addr;
        int length = sizeof(sockaddr_in);
        addr.sin_family = AF_INET;
        addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
        addr.sin_port = htons(10087);

        cout << "----------------正在等待服务器响应------------" << endl;

    
        while (true)
        {
            int isconncet = connect(s, (sockaddr*)&addr, length);
            if (isconncet == 0) break;
            Sleep(300);
        }
        cout <<endl<< "---------连接成功!!!!-------------" << endl;
        Sleep(1000);
        system("cls");

        //4.send/recv
        
        MYDATA data;
        data.server = s;
        data.isclose = false;
        CreateThread(NULL, 0, sendthread, &data, 0, NULL);


        while (true)
        {
            char wbuf[256] = { 0 };

            cout << endl << "请输入要发送的信息" << endl;
            gets_s(wbuf);
            int getdate = send(s, wbuf, strlen(wbuf) + 1, 0);
            if (getdate > 0)
            {
                cout << endl << "成功发送了数据!!" << endl;
            }
            else if (getdate == SOCKET_ERROR)
            {
                cout << endl << "发送失败!!!!!!!" << endl;
                Sleep(1000);
                cout << "客户端断开连接 正在等待重新连接-----" << endl;
                data.isclose = true;
                break;
            }

        }
    }


    //5.closesocket



    WSACleanup();

    return 0;
}