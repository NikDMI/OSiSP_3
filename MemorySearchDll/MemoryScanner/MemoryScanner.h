#pragma once
#include <Windows.h>
#include <string>

namespace LAB {

	class MemoryScanner {
	public:
		//Default constructor, scan memory of the current process
		MemoryScanner();
		//Create scanner of the special process
		MemoryScanner(HANDLE hProcess);
		~MemoryScanner();

		//Change strings in the process memory
		void ExchangeProcessStrings(const std::string& targetString, const std::string& currentString);
	private:

		struct PageInfo {
			void* startAddress;
			int length;
			bool isReadWrite = false;
		};

		class PageIterator {
		public:
			PageIterator(MemoryScanner* scannerPtr);
			PageInfo operator++();	//PageInfo.lenght == 0 -> means end of available pages
		private:
			LPVOID m_currentAddress;
			MemoryScanner* m_innerScanner;
		};


		HANDLE m_hProcess;
	};

}
