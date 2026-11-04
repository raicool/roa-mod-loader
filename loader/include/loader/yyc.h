#pragma once

#include "loader.h"

#include <vector>

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
	RValue* value,
	const char* data
);

LOADER_DLL RValue* loader_call_builtin_ex(
	IN const char* name,
	IN CInstance* self,
	IN CInstance* other,
	IN std::vector<RValue> args
);

LOADER_DLL PFUNC_F_CALL loader_get_builtin(
	IN const char* name
);

LOADER_DLL RValue* loader_call_script(
	IN const char* name,
	IN CInstance* self,
	IN CInstance* other,
	IN std::vector<RValue*> args
);

LOADER_DLL PFUNC_YYGMLScript loader_get_script(
	IN const char* name
);

LOADER_DLL CInstance** loader_get_global_instance(
);

LOADER_DLL std::vector<CInstance*> loader_get_object_instances(
	IN const char* object_name
);

LOADER_DLL size_t loader_get_object_instance_count(
	IN const char* object_name
);