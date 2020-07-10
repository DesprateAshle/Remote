#include<iostream>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include<map>
using namespace std;

typedef struct MyData
{
    SOCKET sclient;
    bool isclose;
    map<int, SOCKET> *map;
    sockaddr_in addr;
    int addrlen;
    int id;
    //char key[32];
}MYDATA;

DWORD WINAPI recvthread(LPVOID lpParam)
{
    MyData* s = (MyData*)lpParam;
    int getdate;
    while (true)
    {
        char rbuf[256] = { 0 };
        getdate = recv(s->sclient, rbuf, 256, 0);
        if (getdate > 0)
        {
            cout << "-----------------------------来自客户端的数据----------------------" << endl;
            cout << endl << "获取到客户端"<< s->id <<"发送了一个数据" << endl;
            cout << endl << rbuf << endl << endl;
            cout << "-----------------------------来自客户端的数据----------------------" << endl;
            char temp[256];
            strcpy(temp, rbuf);
            sprintf_s(rbuf, "%d%s%s", s->id, "号网友说:-----", temp);

            for (map<int, SOCKET>::iterator it = s->map->begin(); it != s->map->end(); it++)
            {
                SOCKET temp = it->second;

                send(temp, rbuf, sizeof(rbuf), 0);
            }
        }
        else
        {
            break;
        }


        
    }
    return 0;
}

DWORD WINAPI serverthread(LPVOID lpParam)
{
    MyData* s = (MyData*)lpParam;
    int i = 1;
    while (TRUE)
    {
        SOCKET sclient = accept(s->sclient, (sockaddr*)&s->addr, &s->addrlen);
        char name[30];
        sprintf_s(name, "%d%s", i, "号网友加入了群聊");
        s->map->insert(make_pair(i, sclient));
        
        for (map<int, SOCKET>::iterator it = s->map->begin(); it != s->map->end(); it++)
        {
            SOCKET temp = it->second;
            send(temp, name, strlen(name) + 1, 0);
        }
        cout << endl << endl << name << "加入群聊" << endl << endl << endl;

        

        //4.send/recv
        MYDATA data;
        data.sclient = sclient;
        data.isclose = false;
        data.map = s->map;
        data.id = i;
        CreateThread(NULL, 0, recvthread, &data, 0, NULL);
        
        
        i++;
    }


}


int main()
{
    map<int, SOCKET> nclient;

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
    addr.sin_port = htons(10087);
    bind(s, (sockaddr*)&addr, length);


    listen(s, 5);

    //3.accept

    sockaddr_in clientaddr = {0};
    clientaddr.sin_family = AF_INET;
    int addrlen=sizeof(sockaddr_in);

    SOCKET sclient;

    MYDATA acpthread;
    acpthread.sclient = s;
    acpthread.addr = clientaddr;
    acpthread.addrlen = addrlen;
    acpthread.map = &nclient;
    CreateThread(NULL, 0, serverthread, &acpthread, 0, NULL);

    








    while (true)
    {
        char wbuf[256] = { 0 };

        cout << endl << "请输入要发送的信息" << endl;
        gets_s(wbuf);
        for (map<int, SOCKET>::iterator it = nclient.begin(); it != nclient.end(); it++)
        {
            SOCKET temp = it->second;
            int getdate=send(temp, wbuf, sizeof(wbuf), 0);

            if (nclient.empty() == true)
            {
                cout << "当前没有客户端连接" << endl;
            }
           else if (getdate == SOCKET_ERROR)
           {
                cout << endl << "发送失败!!!!!!!" << endl;
                Sleep(1000);
                cout << it->first <<"号客户端断开连接" << endl;

                nclient.erase(it);

                break;
           }
        }

        

    }















    //5.closesocket
    closesocket(sclient);



    WSACleanup();

    return 0;
}