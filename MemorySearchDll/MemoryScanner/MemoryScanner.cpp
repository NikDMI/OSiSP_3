#include "MemoryScanner.h"
#include <exception>
#include <cstring>
#include <aclapi.h>
#include <tlhelp32.h>

#pragma comment (lib, "Advapi32.lib")

namespace LAB {
	
	struct HardwareInformation {
		HardwareInformation();
		DWORD pageSize;
		LPVOID minAppAddr;
	};


	HardwareInformation::HardwareInformation() {
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		pageSize = systemInfo.dwPageSize;
		minAppAddr = systemInfo.lpMinimumApplicationAddress;
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
		if (SetSecurityInfo(m_hProcess, SE_KERNEL_OBJECT, PROCESS_QUERY_INFORMATION, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) {
			throw std::exception("Can't get process handle");
		}
	}


	MemoryScanner::~MemoryScanner() {
		CloseHandle(m_hProcess);
	}


	void ExchangePatternString(char* start, char* end, const std::string& patternString, const std::string& exchangeString);


	void MemoryScanner::ExchangeProcessStrings(const std::string& targetString, const std::string& currentString) {
		//Resume all active threads or create image of virtual memory??
		SuspendBackgroundThreads();
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
		ResumeBackgroundThreads();
	}


	//Searchs for patterns in the memory of the process
	void ExchangePatternString(char* start, char* end, const std::string& patternString, const std::string& exchangeString) {
		int memorySize = end - start;
		const char* c_patternString = patternString.c_str();
		while (memorySize && memorySize >= patternString.size()) {
			char* startAddr = start;
			const char* c_patternPtr = c_patternString;
			char ch;
			while ((ch = *c_patternPtr) && startAddr <= end && *startAddr == ch) {
				++startAddr; ++c_patternPtr;
			}
			if (!*c_patternPtr && (startAddr - start) == patternString.size() && (start != patternString.c_str())) {	//If match pattern string
				//exchange strings
				if (exchangeString.size() <= patternString.size()) {
					memcpy(start, exchangeString.c_str(), exchangeString.size());
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


	void MemoryScanner::SuspendBackgroundThreads() {
		m_suspendedThreads.clear();
		HANDLE processSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, GetProcessId(m_hProcess));
		if (processSnapshot == INVALID_HANDLE_VALUE) {
			throw std::exception("Can't create process snapshot");
		}
		DWORD currentThreadId = GetThreadId(GetCurrentThread());
		THREADENTRY32 threadInfo{sizeof(THREADENTRY32)};
		if (Thread32First(processSnapshot, &threadInfo)) {
			do {
				if (threadInfo.th32ThreadID != currentThreadId) {
					m_suspendedThreads.push_back(OpenThread(0, FALSE, threadInfo.th32ThreadID));
					SuspendThread(m_suspendedThreads.back());
				}
			} while (Thread32Next(processSnapshot, &threadInfo));
		}
		CloseHandle(processSnapshot);
	}


	void MemoryScanner::ResumeBackgroundThreads() {
		for (auto thread : m_suspendedThreads) {
			if (thread != NULL) {
				ResumeThread(thread);
				CloseHandle(thread);
			}
		}
	}


	///
	/////////////// PAGE ITERATOR ////////////////////////////////////////////////////////////////////////////////////////////////////
	///

	MemoryScanner::PageIterator::PageIterator(MemoryScanner* scannerPtr) : m_currentAddress{ hardwareInformation.minAppAddr },
		m_innerScanner{scannerPtr} {

	}


	bool IsReadWriteProtection(DWORD protection) {
		if (protection == PAGE_READWRITE)
			return true;
		return false;
	}


	MemoryScanner::PageInfo MemoryScanner::PageIterator::operator++() {
		MEMORY_BASIC_INFORMATION pageInformation;
		SIZE_T regionSize;
		while (!(regionSize = VirtualQueryEx(GetCurrentProcess(), m_currentAddress, &pageInformation, sizeof(pageInformation))) ||
			regionSize == ERROR_INVALID_PARAMETER) {
			return { 0, 0 };
		}
		PageInfo pageInfo{pageInformation.BaseAddress, pageInformation.RegionSize};
		if (pageInformation.State == MEM_COMMIT && pageInformation.Type == MEM_PRIVATE &&
			IsReadWriteProtection(pageInformation.Protect)) {
			pageInfo.isReadWrite = true;
		}
		//Pointer to the next virtual page
		m_currentAddress = (LPVOID)((char*)m_currentAddress + pageInformation.RegionSize);
		return pageInfo;
	}

}