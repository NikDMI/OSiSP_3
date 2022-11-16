#include "MemoryScanner.h"
#include <exception>
#include <cstring>

namespace LAB {
	
	struct HardwareInformation {
		HardwareInformation();
		DWORD pageSize;
	};

	HardwareInformation::HardwareInformation() {
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		pageSize = systemInfo.dwPageSize;
	}

	const HardwareInformation hardwareInformation;


	//
	///////////////// MEMORY SCANNER//////////////////////////////////////////////////////////////////////////////////////
	//

	//Default constructor, scan memory of the current process
	MemoryScanner::MemoryScanner():MemoryScanner(GetCurrentProcess()) {}


	//Create scanner of the special process
	MemoryScanner::MemoryScanner(HANDLE hProcess) {
		HANDLE hCurrentProcess = GetCurrentProcess();
		if (!DuplicateHandle(hCurrentProcess, hProcess, hCurrentProcess, &m_hProcess, 0, FALSE, 0)) {
			throw std::exception("Can't create handle duplicate");
		}
	}


	MemoryScanner::~MemoryScanner() {
		CloseHandle(m_hProcess);
	}


	void ExchangePatternString(char* start, char* end, const std::string& patternString, const std::string& exchangeString);


	void MemoryScanner::ExchangeProcessStrings(const std::string& targetString, const std::string& currentString) {
		//Resume all active threads or create image of virtual memory??
		PageIterator pageIterator{this};
		PageInfo pageInfo;
		while (true) {
			pageInfo = ++pageIterator;
			if (pageInfo.length == 0) break;
			if (pageInfo.isReadWrite) {
				//Search for pattern string in memory
				ExchangePatternString((char*)pageInfo.startAddress, (char*)pageInfo.startAddress + pageInfo.length,
					targetString, currentString);
			}
		}
	}


	//Searchs for patterns in the memory of the process
	void ExchangePatternString(char* start, char* end, const std::string& patternString, const std::string& exchangeString) {
		int memorySize = end - start;
		const char* c_patternString = patternString.c_str();
		while (memorySize && memorySize >= patternString.size()) {
			char* startAddr = start;
			const char* c_patternPtr = c_patternString;
			char ch;
			while (ch = *c_patternPtr && startAddr <= end && *startAddr == ch) {
				++startAddr; ++c_patternPtr;
			}
			if (!*c_patternPtr && (startAddr - start) == patternString.size()) {	//If match pattern string
				//exchange strings
				if (exchangeString.size() <= patternString.size()) {
					strcpy_s(start, exchangeString.size(), exchangeString.c_str());
					startAddr = start + exchangeString.size() + 1;
				}
				else {
					memcpy(start, exchangeString.c_str(), patternString.size());
				}
			}
			if (startAddr == start) {
				++start; --memorySize;
			}
			else {
				memorySize -= (startAddr - start);
				start = startAddr;
			}
		}
	}


	///
	/////////////// PAGE ITERATOR ////////////////////////////////////////////////////////////////////////////////////////////////////
	///

	MemoryScanner::PageIterator::PageIterator(MemoryScanner* scannerPtr) : m_currentAddress{ 0 }, m_innerScanner{scannerPtr} {

	}


	bool IsReadWriteProtection(DWORD protection) {
		if (protection == PAGE_EXECUTE_READWRITE || protection == PAGE_READWRITE)
			return true;
		return false;
	}


	MemoryScanner::PageInfo MemoryScanner::PageIterator::operator++() {
		MEMORY_BASIC_INFORMATION pageInformation;
		SIZE_T regionSize;
		if (!(regionSize = VirtualQueryEx(m_innerScanner->m_hProcess, m_currentAddress, &pageInformation, sizeof(pageInformation))) ||
			regionSize == ERROR_INVALID_PARAMETER) {
			return { 0, 0 };
		}
		PageInfo pageInfo{m_currentAddress, pageInformation.RegionSize};
		if (IsReadWriteProtection(pageInformation.AllocationProtect) ||
			IsReadWriteProtection(pageInformation.Protect)) {
			pageInfo.isReadWrite = true;
		}
		//Pointer to the next virtual page
		m_currentAddress = (LPVOID)((char*)m_currentAddress + pageInformation.RegionSize);
	}

}