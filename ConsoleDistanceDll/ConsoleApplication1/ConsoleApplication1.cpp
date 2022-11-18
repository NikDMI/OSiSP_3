#include <iostream>
#include <Windows.h>
#include <string>

typedef DWORD (WINAPI *RemoteThreadProc)(LPVOID param);

using namespace std;

int main()
{
    static PROCESS_INFORMATION processInformation;
    static STARTUPINFO startupInfo{sizeof(STARTUPINFO)};
    std::wstring cmdLine = L"D:\\БГУИР 3 КУРС\\ОСиСП\\Labs\\LB3\\ConsoleProcess\\ConsoleApplication1\\Debug\\ConsoleApplication1.exe";
    if (!CreateProcess(NULL, (LPWSTR)cmdLine.c_str(),
        NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &processInformation)) {
        throw std::exception("Can't create process");
    }
    DWORD threadID;
    HANDLE remoteThread = CreateRemoteThread(processInformation.hProcess, NULL, 0, (RemoteThreadProc)0x009f4760, NULL, 0, &threadID);
    DWORD exitCode;
    WaitForSingleObject(processInformation.hProcess, INFINITE);
}
