#include <iostream>
#include <string>
#include <iostream>
#include "../../MemorySearchDll/MemorySearcher.h"

#pragma comment(lib, "MemorySearchDll.lib")

using namespace std;

int main()
{
    string testString1 = "Hello, world! Test dll's";
    string testString2 = "Hi, world! New string ha-ha";
    cout << "Before: \n" << testString1 << endl << testString2 <<endl;
    try {
        ExchangeMemoryStrings("world", "Earth");
        ExchangeMemoryStrings("a", "_");
    }
    catch (...) {

    }
    cout << "\nAfter: \n" << testString1 << endl << testString2;
    //cin.get();
}