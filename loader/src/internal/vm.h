#pragma once

#include "loader/yoyo.h"

#include <vector>

// Function to call built-in GML functions
typedef RValue& (__cdecl* PFUNC_YYLegacyCall)(
	_In_ CInstance* self,
	_In_ CInstance* other,
	_Out_ RValue& out,
	_In_ int argc,
	_In_ int func_id,
	_In_ RValue* args
	);

typedef void(__cdecl* PFUNC_YYSetString)(
	_In_ RValue* val,
	_In_ const char* data
	);

typedef void(__cdecl* PFUNC_Run_EndGame)(void);

bool internal_function_init();
bool internal_find_llvm_data();
bool internal_find_yysetstring();
bool internal_find_run_endgame();

PFUNC_Run_EndGame internal_get_run_endgame();

RValue* internal_call_builtin_ex(
	RValue* out,
	const char* function,
	CInstance* self,
	CInstance* other,
	std::vector<RValue> args
);

PFUNC_F_CALL internal_get_builtin(
	const char* function
);

RValue* internal_call_script(
	RValue* out,
	const char* function,
	CInstance* self,
	CInstance* other,
	std::vector<RValue*> args
);

PFUNC_YYGMLScript internal_get_script(
	const char* function
);

void internal_set_string(
	RValue* val,
	const char* data
);