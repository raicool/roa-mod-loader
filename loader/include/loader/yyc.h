#pragma once

#include "loader.h"

LOADER_DLL RValue* loader_get_instance_variable(
	IN RValue instance,
	IN const char* name
);

LOADER_DLL int32_t loader_get_instance_variable_count(
	IN RValue instance
);

LOADER_DLL RefDynamicArrayOfRValue* loader_get_instance_variable_names(
	IN RValue instance
);

LOADER_DLL RefDynamicArrayOfRValue* loader_get_global_variable_names(
);

LOADER_DLL RValue* loader_get_global_variable(
	IN const char* name
);


LOADER_DLL void loader_set_string(
	RValue& value,
	const char* data
);

LOADER_DLL RValue* loader_call_builtin_ex(
	IN const char* name,
	IN CInstance* self,
	IN CInstance* other,
	IN std::vector<RValue> args
);

LOADER_DLL RValue* loader_call_script(
	IN const char* name,
	IN CInstance* self,
	IN CInstance* other,
	IN std::vector<RValue> args
);

LOADER_DLL CInstance** loader_get_global_instance(
);