#pragma once

#include "loader/yoyo.h"

#include <vector>

bool internal_function_init();
bool internal_find_llvm_data();
bool internal_find_yysetstring();

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