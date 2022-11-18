#include <iostream>
#include <string>
#include <Windows.h>

//#include "../../MemorySearchDll/MemorySearcher.h"

using fptrExcangeString = void (*)(const char* patternString, const char* exchangeString);

using namespace std;


int main()
{
    HMODULE loadedDll = LoadLibrary(L"../../MemorySearchDll/Debug/MemorySearchDll.dll");
    fptrExcangeString loadedFunction = (fptrExcangeString)GetProcAddress(loadedDll, "ExchangeMemoryStrings");
    string testString1 = "Hello, world! Test dll's";
    string testString2 = "Hi, world! New string ha-ha";
    cout << "Before: \n" << testString1 << endl << testString2 << endl;
    try {
        loadedFunction("world", "Earth");
        cout << "\nAfter: \n" << testString1 << endl << testString2;
    }
    catch (...) {

    }
    cout << "\nAfter: \n" << testString1 << endl << testString2;
}
