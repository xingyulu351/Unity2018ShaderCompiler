//
// auto_quitter, run with process id and terminates it if the parent process quits
//

#include "stdafx.h"
#include <tlhelp32.h>

static DWORD pidParent;
static DWORD pidChild;

bool CheckProcesses()
{
    // Based on: http://www.codeproject.com/KB/threads/pausep.aspx
    HANDLE         hProcessSnap = NULL;
    PROCESSENTRY32 pe32;

    bool pidParentRunning = false;
    bool pidChildRunning = false;

    ZeroMemory(&pe32, sizeof(pe32));

    //  Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE)
        return false;

    //  Fill in the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    //  Walk the snapshot of the processes.
    if (Process32First(hProcessSnap, &pe32))
    {
        do
        {
            if (pe32.th32ProcessID == pidParent)
                pidParentRunning = true;
            else if (pe32.th32ProcessID == pidChild)
                pidChildRunning = true;
        }
        while (Process32Next(hProcessSnap, &pe32));
    }

    // Do not forget to clean up the snapshot object.
    CloseHandle(hProcessSnap);

    if (!pidChildRunning)
        return false; // child process exited, nothing more to do here

    if (!pidParentRunning) // parent exited/crashed, kill child
    {
        HANDLE procHandle = OpenProcess(PROCESS_TERMINATE, false, pidChild);

        if (procHandle != NULL) // if it's null theres nothing we can do
            TerminateProcess(procHandle, 0);

        CloseHandle(procHandle);
        return false;
    }

    return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc != 3)
    {
        printf("Usage:\n auto_quitter pidParent pidChild\n\n");
        return 0;
    }

    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_HIDE);

    pidParent = atoi(argv[1]);
    pidChild = atoi(argv[2]);

    while (CheckProcesses())
        Sleep(5000);

    return 0;
}
