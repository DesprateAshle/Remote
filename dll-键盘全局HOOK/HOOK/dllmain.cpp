// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include<stdio.h>
#include<Windows.h>


HHOOK hook;
HMODULE g_hModule;
DWORD pid;
HWND g_hwnd;
LRESULT CALLBACK KeyboardProc(int code,       // hook code
    WPARAM wParam,  // virtual-key code
    LPARAM lParam   // keystroke-message information);
    )
{
    if (wParam <= 'Z' && wParam >= 'A')
    {
        char press[256] = { 0 };
        sprintf_s(press, "%c press", wParam);

        COPYDATASTRUCT cd;
        cd.dwData = 0;
        cd.cbData = strlen(press) + 1;
        cd.lpData = press;
        SendMessage(g_hwnd, WM_COPYDATA, (WPARAM)g_hwnd, (LPARAM)&cd);
    }

    return CallNextHookEx(hook, code, wParam, lParam);
}

int mykeyboardhook(HWND hwnd)
{
    g_hwnd = hwnd;
    hook = SetWindowsHookEx(WH_KEYBOARD,        // hook type
        (HOOKPROC)KeyboardProc,     // hook procedure
        g_hModule,    // handle to application instance
        0   // thread identifier);
        );
    return 1;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        //dll加载
        g_hModule = hModule;
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:

        //dll释放


        break;
    }
    return TRUE;
}

