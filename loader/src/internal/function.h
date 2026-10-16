#pragma once

#include "loader/yoyo.h"

#include <vector>

bool internal_function_init();

struct FunctionInfo
{
	uint32_t id;
	void* ptr;
};
FunctionInfo internal_get_func_info(
	const char* name
);

size_t internal_get_builtin_variable_index(
	const char* name
);


// Call a built-in GM function using its numerical ID
// Arguments are passed by array
RValue* internal_call_gml_function(
	int func_id,
	int arg_count,
	RValue* args
);

void internal_call_builtin_ex(
	RValue& out,
	const char* function,
	CInstance* self,
	CInstance* other,
	std::vector<RValue*> args
);