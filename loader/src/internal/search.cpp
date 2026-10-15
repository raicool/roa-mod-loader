#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <windows.h>
#include <psapi.h>

// Credit for the code this is based on goes to JLBorges on the cplusplus.com forum.
// Taken from this thread: http://www.cplusplus.com/forum/general/202725/
const void* internal_scan_memory(void* address_low, std::size_t nbytes, const std::vector<uint8_t>& bytes_to_find) 
{
	// all readable pages: adjust this as required
	const DWORD pmask = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY |
		PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

	::MEMORY_BASIC_INFORMATION mbi{};

	BYTE* address = static_cast<BYTE*>(address_low);
	BYTE* address_high = address + nbytes;

	while (address < address_high && ::VirtualQuery(address, std::addressof(mbi), sizeof(mbi))) 
	{
		if ((mbi.State == MEM_COMMIT) && (mbi.Protect & pmask) && !(mbi.Protect & PAGE_GUARD)) 
		{
			const BYTE* begin = static_cast<const BYTE*>(mbi.BaseAddress);
			const BYTE* end = begin + mbi.RegionSize;

			const BYTE* found = std::search(begin, end, bytes_to_find.begin(), bytes_to_find.end(), [](char fromText, char from_pattern) 
			{
				return from_pattern == '?' || from_pattern == fromText;
			});
			if (found != end) 
			{
				return found;
			}
		}

		address += mbi.RegionSize;
		mbi = {};
	}
	return nullptr;
}

void* internal_scan(const std::vector<uint8_t>& bytes_to_find) 
{
	auto base = ::GetModuleHandle(NULL);
	MODULEINFO minfo{};
	::GetModuleInformation(GetCurrentProcess(), base, std::addressof(minfo), sizeof(minfo));
	return (BYTE*)internal_scan_memory(base, minfo.SizeOfImage, bytes_to_find);
}

void* internal_scan_local(const std::vector<uint8_t>& bytes_to_find, void* begin, int count)
{
	// Scan count bytes from begin for the data in bytes_to_find
	uint8_t* end = (uint8_t*)begin + count;

	const uint8_t* found = std::search((uint8_t*)begin, end, bytes_to_find.begin(), bytes_to_find.end(), [](char fromText, char from_pattern)
	{
		return from_pattern == '?' || from_pattern == fromText;
	});

	if (found != end) 
	{
		return (uint8_t*)found;
	}
	return nullptr;
}

void* internal_read_ptr(void* start)
{
	// Returns a pointer read from the first 4 bytes of memory starting at start
	uint8_t* address = 0;
	for (int i = 0; i < 4; i++) 
	{
		address += (uint64_t)(*((uint8_t*)start + i)) * (uint64_t)pow(16, i * 2);
	}
	return address;
}

void* internal_absolute_address(void* source, void* relative_addr, uint8_t instr_size)
{
	uint32_t addr = ((uint32_t)source + (uint32_t)relative_addr) + instr_size;

	return (void*)addr;
}
