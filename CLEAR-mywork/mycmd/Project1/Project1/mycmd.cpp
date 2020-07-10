#include <iostream>
#include<Windows.h>
using namespace std;
int main()
{

    bool ret;

    HANDLE myhwritepipe;
    HANDLE cmdhreadpipe;

    HANDLE cmdhwritepipe;
    HANDLE myhreadpipe;

    SECURITY_ATTRIBUTES sa = {0};
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    

    ret=CreatePipe(&cmdhreadpipe, &myhwritepipe, &sa, 0);
    ret=CreatePipe(&myhreadpipe, &cmdhwritepipe, &sa, 0);


    STARTUPINFO st = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    //TCHAR a[] = TEXT("/c calc");

    st.cb = sizeof(STARTUPINFO);
    st.dwFlags = STARTF_USESTDHANDLES;
    st.hStdInput = cmdhreadpipe;
    st.hStdOutput = cmdhwritepipe;
    st.hStdError = cmdhwritepipe;

    ret=CreateProcess(TEXT("C:\\Windows\\System32\\cmd.exe"),
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
    
    while (TRUE)
    {
        while (TRUE)
        {
            Sleep(100);
            char rbuf[256] = { 0 };
            DWORD isnull=TRUE;
            ret = PeekNamedPipe(myhreadpipe, rbuf, 1, &isnull, 0, 0);
            if (!isnull)  break;
            
             DWORD realrlength;
             ReadFile(myhreadpipe, rbuf, 255, &realrlength, NULL);
             cout << rbuf;
             
             
             
        }
        DWORD realwlength;
        char wbuf[256] = {0};
        cin >> wbuf;
        DWORD wlength = strlen(wbuf);
        wbuf[wlength++] = '\n';
        WriteFile(myhwritepipe, wbuf, wlength, &realwlength, NULL);
    }
        
    
    return 0;


}