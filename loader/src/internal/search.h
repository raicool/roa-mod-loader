#pragma once

#include <string>
#include <vector>
#include <windows.h>

void* internal_scan(const std::vector<uint8_t>& bytes_to_find);
void* internal_scan_local(const std::vector<uint8_t>& bytes_to_find, void* begin, int count);

void* internal_read_ptr(void* start);
void* internal_absolute_address(void* source, void* relative_addr, uint8_t instr_size);