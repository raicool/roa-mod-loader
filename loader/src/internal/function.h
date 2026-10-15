#pragma once

#include "loader/yoyo.h"

enum InitializationStatusEnum
{
	OKAY,
	ALREADY_INITIALIZED,
	INVALID_CALLLEGACYFUNCTION_ADDR,
	INVALID_CALLLEGACYFUNCTION,
	ERROR_GENERIC,
};

struct func_info
{
	uint32_t id;
	void* ptr;
};

static RVariableRoutine* builtin_array = nullptr;
static int32_t* builtin_count = nullptr;

uint8_t internal_init();

func_info internal_get_func_info(
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