#include<iostream>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

typedef struct MyData
{
    SOCKET sclient;
    bool isclose;
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
            cout << "-----------------------------���Կͻ��˵�����----------------------" << endl;
            cout << endl << "��ȡ���ͻ��˷�����һ������" << endl;
            cout << endl << rbuf << endl << endl;
            cout << "-----------------------------���Կͻ��˵�����----------------------" << endl;
        }
        else if (getdate == 0)
        {
            cout << endl << "�ͻ����ѹر�,���Ժ�����!!" << endl;

            cout << "�Ͽ����� ���ڳ�����������-----(����������ֵ���س���������)" << endl;
            break;
        }
        else
        {
            cout << endl << "���ӶϿ�,��������!" << endl;
            cout << "�Ͽ����� ���ڳ�����������-----(����������ֵ���س���������)" << endl;
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
    cout << "------------------------------------------------------" << endl << endl;
    cout << "-----------------���ڵȴ�����------------------" << endl;
    SOCKET sclient;

    while (true)
    {
        sclient = accept(s, (sockaddr*)&clientaddr, &addrlen);

        cout << endl << "���ӳɹ�,������ת!!!!" << endl << endl;
        Sleep(1000);
        system("cls");

        //4.send/recv
        MYDATA data;
        data.sclient = sclient;
        data.isclose = false;
        CreateThread(NULL, 0, recvthread,&data,0,NULL );
        while (true)
        {
            char wbuf[256] = { 0 };

            cout << endl << "������Ҫ���͵���Ϣ" << endl;
            gets_s(wbuf);
            int getdate = send(sclient, wbuf, strlen(wbuf) + 1, 0);
            if (getdate > 0)
            {
                cout << endl << "�ɹ�����������!!" << endl;
            }
            else if (getdate == SOCKET_ERROR)
            {
                cout << endl << "����ʧ��!!!!!!!" << endl;
                Sleep(1000);
                cout << "�ͻ��˶Ͽ����� ���ڵȴ���������-----" << endl;
                data.isclose = true;
                break;
            }

        }
    }

    //5.closesocket
    closesocket(sclient);



    WSACleanup();

    return 0;
}