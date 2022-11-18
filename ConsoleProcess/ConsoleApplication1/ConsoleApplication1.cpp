// ConsoleApplication1.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#include <string>
#include <iostream>
#include <Windows.h>

using namespace std;

using fptrExcangeString = void (*)(const char* patternString, const char* exchangeString);

DWORD WINAPI RemoteThreadProc(LPVOID param);

int main()
{
    string testString1 = "Hello, world! Test dll's";
    RemoteThreadProc(NULL);
    string testString2 = "Hi, world! New string ha-ha";
    while (true) {
        cout << "\n\nAfter: \n" << testString1 << endl << testString2;
        Sleep(1000);
    }
}


DWORD WINAPI RemoteThreadProc(LPVOID param){
    HMODULE loadedDll = LoadLibrary(L"MemorySearchDll.dll");
    fptrExcangeString loadedFunction = (fptrExcangeString)GetProcAddress(loadedDll, "ExchangeMemoryStrings");
    string str = "Hello";
    loadedFunction(str.c_str(), "PATTERT");
    return 0;
}
