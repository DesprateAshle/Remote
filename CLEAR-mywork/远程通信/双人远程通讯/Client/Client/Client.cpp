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
            cout << "-----------------------------���Է�����������-----------------------------" << endl;
            cout << endl << "��ȡ��������������һ������" << endl;
            cout << endl << rbuf << endl << endl;
            cout << "-----------------------------���Է�����������-----------------------------" << endl;
        }
        else if (getdate == 0)
        {
            cout << endl << endl << "�������ѹر�,���Ժ�����!!" << endl;

            cout << "�Ͽ����� ���ڳ�����������-----(����������ֵ���س���������)" << endl;
            break;
        }
        else
        {
            cout << endl << endl << "���ӶϿ�,��������!" << endl;
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

        cout << "----------------���ڵȴ���������Ӧ------------" << endl;

    
        while (true)
        {
            int isconncet = connect(s, (sockaddr*)&addr, length);
            if (isconncet == 0) break;
            Sleep(300);
        }
        cout <<endl<< "---------���ӳɹ�!!!!-------------" << endl;
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

            cout << endl << "������Ҫ���͵���Ϣ" << endl;
            gets_s(wbuf);
            int getdate = send(s, wbuf, strlen(wbuf) + 1, 0);
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



    WSACleanup();

    return 0;
}