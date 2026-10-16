#include "loader/yyc.h"

#include "internal/function.h"
#include "internal/instance.h"

LOADER_DLL RValue loader_get_instance_variable(
	RValue instance,
	const char* name
)
{
	RValue variable_slot_exists;
	RValue instance_variable;

	if (instance.type != GML_TYPE_OBJECT)
	{
		// incorrect RValue type
		return instance_variable;
	}

	RValue var_name = name;
	var_name.type = GML_TYPE_STRING;

	RValue instance_id;
	instance_id.value_real = instance.value_instance->id;
	instance_id.type = GML_TYPE_REAL;

	internal_call_builtin_ex(
		variable_slot_exists,
		"variable_instance_exists",
		nullptr,
		nullptr,
		{ &instance_id, &var_name }
	);

	if (variable_slot_exists.truthy() == false)
	{
		return instance_variable;
	}

	int32_t hash = internal_call_findalloc_slot_from_name(instance.value_object, name);

	instance_variable = instance.value_instance->InternalGetYYVarRef(hash);
	
	return instance_variable;
}

LOADER_DLL int32_t loader_get_instance_variable_count(
	RValue instance
)
{
	RValue instance_variable;

	if (instance.type != GML_TYPE_OBJECT)
	{
		// incorrect RValue type
		return -1;
	}

	RValue instance_id;
	instance_id.value_real = instance.value_instance->id;
	instance_id.type = GML_TYPE_REAL;

	RValue result;
	internal_call_builtin_ex(
		result,
		"variable_instance_names_count",
		nullptr,
		nullptr,
		{ &instance_id }
	);

	return result.get_s32();
}

LOADER_DLL RefDynamicArrayOfRValue* loader_get_instance_variable_names(
	RValue instance
)
{
	RValue instance_variable;

	if (instance.type != GML_TYPE_OBJECT)
	{
		// incorrect RValue type
		return nullptr;
	}

	RValue instance_id;
	instance_id.value_real = instance.value_instance->id;
	instance_id.type = GML_TYPE_REAL;

	RValue result;
	internal_call_builtin_ex(
		result,
		"variable_instance_get_names",
		nullptr,
		nullptr,
		{ &instance_id }
	);

	return reinterpret_cast<RefDynamicArrayOfRValue*>(result.value_array);
}

LOADER_DLL int loader_yyc_get_funcid(const char* name)
{
	return internal_get_func_info(name).id;
}

LOADER_DLL void* loader_get_yyc_func_ptr(const char* name)
{
	return internal_get_func_info(name).ptr;
}

LOADER_DLL RValue* loader_yyc_call_func(const char* name, size_t args_count, RValue* args)
{
	uint32_t id = internal_get_func_info(name).id;
	return loader_yyc_call_func(id, args_count, args);
}

LOADER_DLL RValue* loader_yyc_call_func(uint32_t id, size_t args_count, RValue* args)
{
	return internal_call_gml_function(id, args_count, args);
}