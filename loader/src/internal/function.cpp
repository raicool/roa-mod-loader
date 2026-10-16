#include "internal/function.h"

#include "internal/search.h"
#include "loader/log.h"

#include <vector>
#include <unordered_map>

// Function to call built-in GML functions
typedef RValue& (*PFUNC_YYLegacyCall)(
	_In_ CInstance* self,
	_In_ CInstance* other,
	_Out_ RValue& out,
	_In_ int argc,
	_In_ int func_id,
	_In_ RValue* args
	);

PFUNC_YYLegacyCall YYLegacyCall = nullptr;
std::unordered_map<std::string, FunctionInfo>* function_id_map = new std::unordered_map<std::string, FunctionInfo>();

RVariableRoutine* builtin_array = nullptr;
int32_t* builtin_count = nullptr;

bool internal_find_function_map(void* ptr)
{
	// Observing a near empty compiled GM project, each function definition is stored as:
	// 64 uint8_ts: function name (remaining uint8_ts are 0'd out)
	// 4 uint8_ts: address of function?
	// 1 uint8_t: argument count?
	// 3 uint8_ts: padding (0)
	// 1 uint8_t: unknown
	// 3 uint8_ts: padding (0)
	// 4 uint8_ts of 0xFF
	// For a total of 80 uint8_ts each
	// However, the size is not consistent in some games
	// Function list ends when a zero length name is hit.
	uint8_t* func_array = *(uint8_t**)ptr;

	// Automatically determine size of a function entry
	// Different between test project and Rivals of Aether, can't be hardcoded
	std::vector<uint8_t> entry_terminator{ 0xFF, 0xFF, 0xFF, 0xFF };
	void* ptr_entry_terminator = internal_scan_local(entry_terminator, func_array, 120);
	if (ptr_entry_terminator == NULL)
	{
		// Failed to find entry terminator
		return false;
	}
	int entrySize = (int)((uint8_t*)ptr_entry_terminator + 4 - func_array);

	uint32_t index = 0; // Current index of function being read
	while (true)
	{
		uint8_t* entry = func_array + entrySize * index;

		// Read the next name
		const char* name = (const char*)(entry);
		if (strlen(name) == 0)
		{
			// No name - End of function list
			break;
		}
		else
		{
			// Add the name to the function map
			function_id_map->insert(
				{
					std::string(name),
					FunctionInfo
					{
						index,
						*(uint32_t**)(entry + 0x40)
					}
				});
			index += 1;
		}
	}

	return true;
}

bool internal_function_init()
{
	// Returns a string if loading failed
	if (YYLegacyCall != NULL) return false;

	////// Find address of callLegacyFunction
	// Contains the ASM of the function with inconsistent bits replaced with '?'
	std::vector<uint8_t> binLegacyCallFirst
	{
		0x55, 0x8B, 0xEC, 0x6A, 0xFF, '?', '?', '?', '?', '?', 0x64, 0xA1, 0x00, 0x00, 0x00, 0x00,
		0x50, 0x83, 0xEC, 0x14, 0xA1, '?', '?', '?', '?', 0x33, 0xC5, 0x89, 0x45, 0xF0, 0x53, 0x56,
		0x57, 0x50, 0x8D, 0x45, 0xF4, 0x64, 0xA3, 0x00, 0x00, 0x00, 0x00, '?', 0x45, 0x18
	};

	// Search for the function
	void* legacy_call_addr = internal_scan(binLegacyCallFirst);
	if (legacy_call_addr == NULL)
	{
		return false;
	}
	YYLegacyCall = (PFUNC_YYLegacyCall)legacy_call_addr;

	std::vector<uint8_t> pattern_legacy_call_second
	{
		0x03, 0x05, '?', '?', '?', '?', 0x89, 0x45,
		0xE4, 0x8B, 0xC7, 0xC1, 0xE0, 0x04, 0xE8
	};

	// Search for it starting from the end of the first chunk
	void* legacy_call_chunk_addr = internal_scan_local(pattern_legacy_call_second, (uint8_t*)legacy_call_addr + binLegacyCallFirst.size(), 64);
	if (legacy_call_chunk_addr == nullptr)
	{
		// Failed to find second halfcallGMLFunction
		YYLegacyCall = NULL;
		return false;
	}

	// Pull the address from the ASM
	void* func_array_addr_base = internal_read_ptr((uint8_t*)legacy_call_chunk_addr + 2);

	// Initialize function array
	if (internal_find_function_map(func_array_addr_base) == false)
	{
		// Failed to load function list
		YYLegacyCall = NULL;
		return false;
	}

	// set up builtin vars array
	std::vector<uint8_t> jnz_target_pattern =
	{
		0xa1, '?', '?', '?', '?',     // eax, [memory]
		0x3d, 0xf4, 0x01, 0x00, 0x00, // cmp eax, 0x1f4
		0x75, 0x0e,                   // jnz short 0x????
	};

	void* ptr = internal_scan(jnz_target_pattern);

	if (!ptr)
	{
		return false;
	}

	// relative address operand of the closest jnz instruction
	uint8_t offset = *((int8_t*)ptr + 11);
	uint16_t* jump_target = (uint16_t*)ptr + offset;

	std::vector<uint8_t> lea_pattern =
	{
		0x8d, 0xb8, '?', '?', '?', '?', // lea edi, [memory]
		0x8b, '?'                       // mov eax, s8
	};

	std::vector<uint8_t> mov_pattern =
	{
		0xa1, '?', '?', '?', '?', // mov eax, [memory]
		0x83, 0xc4, 0x04          // add esp, 0x4
	};

	uint8_t* nearest_lea = ((uint8_t*)internal_scan_local(lea_pattern, jump_target, 0x40)) + 2;
	uint8_t* nearest_mov = ((uint8_t*)internal_scan_local(mov_pattern, jump_target, 0x40)) + 1;

	builtin_array = reinterpret_cast<RVariableRoutine*>(*(uint32_t*)nearest_lea);
	builtin_count = reinterpret_cast<int32_t*>(*(uint32_t*)nearest_mov);

	return true;
}

FunctionInfo internal_get_func_info(
	const char* name
)
{
	auto t = function_id_map->find(name);
	if (t == function_id_map->end()) return { 0, nullptr };
	return t->second;
}

RValue* internal_call_gml_function(
	int functionID,
	int argCount,
	RValue* args
)
{
	if (YYLegacyCall == NULL) return NULL;

	RValue* out = new RValue();

	YYLegacyCall(nullptr, nullptr, *out, argCount, functionID, args);

	return out;
}

void internal_call_builtin_ex(
	RValue& out,
	const char* function,
	CInstance* self,
	CInstance* other,
	std::vector<RValue*> args
)
{
	if (function_id_map->contains(function))
	{
		auto func = function_id_map->at(function);
		//out = internal_call_gml_function(func.id, static_cast<int>(args.size()), (RValue*)args.data());
		YYLegacyCall(self, other, out, static_cast<int>(args.size()), func.id, (RValue*)args.data());
	}
	else
	{
		loader_log_error("internal_call_builtin_ex(): invalid builtin function name provided (\"{}\")", function);
	}
}