#ifndef MEMORY_SEARCHER_H
#define MEMORY_SEARCHER_H
#include <numeric>
#include <Windows.h>

	#ifdef DLL_LIB_PROJECT
	#define DLL_ATTRIBUTE extern "C" __declspec(dllexport) __cdecl
	#else
	#define DLL_ATTRIBUTE extern "C" __declspec(dllimport)
	#endif	//#ifdef DLL_LIB_PROJECT


DLL_ATTRIBUTE void ExchangeMemoryStrings(const char* patternString, const char* exchangeString);


#endif	//#ifndef MEMORY_SEARCHER_H
