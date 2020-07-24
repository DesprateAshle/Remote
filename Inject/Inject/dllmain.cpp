// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
extern "C" {
    void __stdcall test()
    {
        MessageBox(NULL, L"666",L"666",0);
    }
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        test();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

