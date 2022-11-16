/*
* This file represent realization of dll functions
*/
#include "MemorySearcher.h"
#include "MemoryScanner/MemoryScanner.h"

using namespace LAB;

void ExchangeMemoryStrings(const char* patternString, const char* exchangeString) {
	MemoryScanner memScanner;
	memScanner.ExchangeProcessStrings(patternString, exchangeString);
}