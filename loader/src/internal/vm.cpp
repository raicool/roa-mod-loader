#include "internal/vm.h"

#include "internal/search.h"
#include "loader/log.h"

#include <vector>
#include <unordered_map>

// Function to call built-in GML functions
typedef RValue& (__cdecl *PFUNC_YYLegacyCall)(
	_In_ CInstance* self,
	_In_ CInstance* other,
	_Out_ RValue& out,
	_In_ int argc,
	_In_ int func_id,
	_In_ RValue* args
	);

typedef void (__cdecl *PFUNC_YYSetString)(
	_In_ RValue* val,
	_In_ const char* data
	);

PFUNC_YYLegacyCall YYLegacyCall = nullptr;
PFUNC_YYSetString YYSetString = nullptr;
SLLVMVars** g_LLVMVars = nullptr;

std::unordered_map<std::string, PFUNC_YYGMLScript> g_script_pointer;
std::unordered_map<std::string, PFUNC_F_CALL> g_functions;

bool internal_function_init()
{
	if (YYLegacyCall != nullptr) return false;

	std::vector<uint8_t> binLegacyCallFirst
	{
		0x55, 0x8B, 0xEC, 0x6A, 0xFF, '?', '?', '?', '?', '?', 0x64, 0xA1, 0x00, 0x00, 0x00, 0x00,
		0x50, 0x83, 0xEC, 0x14, 0xA1, '?', '?', '?', '?', 0x33, 0xC5, 0x89, 0x45, 0xF0, 0x53, 0x56,
		0x57, 0x50, 0x8D, 0x45, 0xF4, 0x64, 0xA3, 0x00, 0x00, 0x00, 0x00, '?', 0x45, 0x18
	};

	void* legacy_call_addr = internal_scan(binLegacyCallFirst);
	if (legacy_call_addr == nullptr)
	{
		return false;
	}
	YYLegacyCall = (PFUNC_YYLegacyCall)legacy_call_addr;

	std::vector<uint8_t> pattern_legacy_call_second
	{
		0x03, 0x05, '?', '?', '?', '?', 0x89, 0x45,
		0xE4, 0x8B, 0xC7, 0xC1, 0xE0, 0x04, 0xE8
	};

	void* legacy_call_chunk_addr = internal_scan_local(pattern_legacy_call_second, (uint8_t*)legacy_call_addr + binLegacyCallFirst.size(), 64);
	if (legacy_call_chunk_addr == nullptr)
	{
		YYLegacyCall = nullptr;
		return false;
	}

	void* func_array_addr_base = **reinterpret_cast<void***>(INCREMENT_POINTER(legacy_call_chunk_addr, 2));

	{
		logger_nestscope nest;

		for (size_t i = 0;; i++)
		{
			const FCallDef* entry = static_cast<FCallDef*>(func_array_addr_base) + i;

			if (entry->name_empty())
			{
				break;
			}

			g_functions.insert({ entry->name, (PFUNC_F_CALL)(entry->address) });
			loader_log_debug("internal_function_init(): {} = 0x{:08x}", entry->name, (int)(entry->address));
		}
	}

	loader_log_debug("internal_function_init(): YYLegacyCall = 0x{:08x}", (int)YYLegacyCall);
	loader_log_debug("internal_function_init(): found {} builtin functions", g_functions.size());

	return true;
}

bool internal_find_llvm_data()
{
	logger_nestscope nest;

	const char* error_str = "internal_find_llvm_data(): could not find LLVMInit caller func";

	void* llvminit_caller = nullptr;
	void* sub_search = nullptr;

	if ((llvminit_caller = internal_scan(
		{
			0x55,
			0x8d, 0xac, 0x24, 0xfc, 0xfb, 0xff, 0xff,
			0x81, 0xec, 0x04, 0x04, 0x00, 0x00,
			0x6a, 0xff,
			0x68, '?', '?', '?', '?',
			0x64, 0xa1, 0x00, 0x00, 0x00, 0x00,
			0x50,
			0x83, 0xec, '?',
			0xa1, '?', '?', '?', '?',
			0x33, 0xc5,
			0x89, 0x85, 0x00, 0x04, 0x00, 0x00,
			0x53,
			0x56,
			0x57,
			0x50,
			0x8d, 0x45, 0xf4,
			0x64, 0xa3, 0x00, 0x00, 0x00, 0x00,
			0xa1, '?', '?', '?', '?',
		}
		)) == nullptr)
	{
		loader_log_fatal(error_str);
		return false;
	}

	 

	if ((sub_search = internal_scan_local(
		{
			0xa3, '?', '?', '?', '?',
			0xe8
		},
		llvminit_caller,
		500
		)) == nullptr)
	{
		loader_log_fatal(error_str);
		return false;
	}

	g_LLVMVars = *reinterpret_cast<SLLVMVars***>(INCREMENT_POINTER(sub_search, 1));

	{
		logger_nestscope nest;

		for (size_t i = 0;; i++)
		{
			if (g_LLVMVars[0]->pGMLFuncs[i].pName == nullptr)
				break;

			g_script_pointer.insert(
				std::pair<const char*, PFUNC_YYGMLScript>(g_LLVMVars[0]->pGMLFuncs[i].pName, (PFUNC_YYGMLScript)g_LLVMVars[0]->pGMLFuncs[i].nData)
			);

			loader_log_debug("internal_find_llvm_data(): {} = 0x{:08x}", g_LLVMVars[0]->pGMLFuncs[i].pName, (int)(g_LLVMVars[0]->pGMLFuncs[i].nData));
		}
	}

	loader_log_debug("internal_find_llvm_data(): g_LLVMVars = 0x{:08x}", (int)g_LLVMVars);
	loader_log_debug("internal_find_llvm_data(): found {} scripts", g_script_pointer.size());

	return true;
}

bool internal_find_yysetstring()
{
	void* location = nullptr;
	if ((location = internal_scan(
		{
			0x55,
			0x8b, 0xec,
			0x6a, 0xff,
			0x68, '?', '?', '?', '?',
			0x64, 0xa1, 0x00, 0x00, 0x00, 0x00,
			0x50,
			0x51,
			0x56,
			0xa1, '?', '?', '?', '?',
			0x33, 0xc5,
			0x50,
			0x8d, 0x45, '?',
			0x64, 0xa3, 0x00, 0x00, 0x00, 0x00,
			0x6a, '?',
			0xe8, '?', '?', '?', '?',
			0x8b, 0xf0,
			0x83, 0xc4, 0x04,
			0x89, 0x75, '?',
			0xc7, 0x45, '?', 0x00, 0x00, 0x00, 0x00,
			0x85, 0xf6,
			0x74, '?',
			0x8d, 0x4e, 0x08,
			0x51,
			0xff, 0x75, 0x0c,
			0xe8, '?', '?', '?', '?'
		})) != nullptr
	)
	{
		YYSetString = (PFUNC_YYSetString)location;
		return true;
	}

	loader_log_debug("internal_find_yysetstring(): YYSetString = 0x{:08x}", (int)YYSetString);

	return false;
}

RValue* internal_call_builtin_ex(
	RValue* out,
	const char* function,
	CInstance* self,
	CInstance* other,
	std::vector<RValue> args
)
{
	if (g_functions.contains(function))
	{
		g_functions.at(function)(
			out, 
			self, 
			other, 
			static_cast<int>(args.size()), 
			args.data()
		);
	}
	else
	{
		loader_log_error("internal_call_builtin_ex(): invalid builtin function name provided (\"{}\")", function);
	}

	return out;
}

RValue* internal_call_script(
	RValue* out,
	const char* function,
	CInstance* self,
	CInstance* other,
	std::vector<RValue> args
)
{
	if (g_script_pointer.contains(function))
	{
		g_script_pointer.at(function)(
			self, 
			other, 
			out, 
			static_cast<int>(args.size()), 
			args.data()
		);
	}
	else
	{
		loader_log_error("internal_call_script(): invalid script name provided (\"{}\")", function);
	}

	return out;
}

void internal_set_string(
	RValue* val,
	const char* data
)
{
	if (YYSetString)
	{
		YYSetString(val, data);
	}
	else
	{
		loader_log_error("internal_set_string(): YYSetString is not set!");
	}
}