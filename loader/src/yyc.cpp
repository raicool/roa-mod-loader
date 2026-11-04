#include "loader/yyc.h"

#include "loader/yoyo.h"

#include "internal/vm.h"
#include "internal/instance.h"

LOADER_DLL RValue* loader_get_instance_variable(
	RValue instance,
	const char* name
)
{
	RValue variable_slot_exists;
	RValue* instance_variable = nullptr;

	if (instance.type != GML_TYPE_OBJECT || name == nullptr)
	{
		// incorrect RValue type
		return nullptr;
	}

	internal_call_builtin_ex(
		&variable_slot_exists,
		"variable_instance_exists",
		nullptr,
		nullptr,
		{ instance.value_instance->id, name }
	);

	if (variable_slot_exists.truthy() == false)
	{
		return nullptr;
	}

	int32_t hash = internal_call_findalloc_slot_from_name(instance.value_object, name);

	instance_variable = &instance.value_instance->InternalGetYYVarRef(hash);
	
	return instance_variable;
}

LOADER_DLL int32_t loader_get_instance_variable_count(
	RValue instance
)
{
	if (instance.type != GML_TYPE_OBJECT)
	{
		// incorrect RValue type
		return -1;
	}

	RValue result;
	internal_call_builtin_ex(
		&result,
		"variable_instance_names_count",
		nullptr,
		nullptr,
		{ instance.value_instance->id }
	);

	return result.get_s32();
}

LOADER_DLL RefDynamicArrayOfRValue* loader_get_instance_variable_names(
	RValue instance
)
{
	if (instance.type != GML_TYPE_OBJECT)
	{
		// incorrect RValue type
		return nullptr;
	}

	RValue result;
	internal_call_builtin_ex(
		&result,
		"variable_instance_get_names",
		nullptr,
		nullptr,
		{ instance.value_instance->id }
	);

	return reinterpret_cast<RefDynamicArrayOfRValue*>(result.value_array);
}

LOADER_DLL RefDynamicArrayOfRValue* loader_get_global_variable_names()
{
	RValue result;
	internal_call_builtin_ex(
		&result,
		"variable_instance_get_names",
		nullptr,
		nullptr,
		{ -5 }
	);

	return reinterpret_cast<RefDynamicArrayOfRValue*>(result.value_array);
}

LOADER_DLL RValue* loader_get_global_variable(
	const char* name
)
{
	CInstance* global = *internal_get_global_instance();

	int32_t hash = internal_call_findalloc_slot_from_name(global, name);

	return &(global->InternalGetYYVarRef(hash));
}

LOADER_DLL void loader_set_string(RValue* value, const char* data)
{
	internal_set_string(value, data);
}

LOADER_DLL RValue* loader_call_builtin_ex(const char* name, CInstance* self, CInstance* other, std::vector<RValue> args)
{
	RValue out;
	return internal_call_builtin_ex(&out, name, self, other, args);
}

LOADER_DLL PFUNC_F_CALL loader_get_builtin(const char* name)
{
	return internal_get_builtin(name);
}

LOADER_DLL RValue* loader_call_script(const char* name, CInstance* self, CInstance* other, std::vector<RValue*> args)
{
	RValue out;
	return internal_call_script(&out, name, self, other, args);
}

LOADER_DLL PFUNC_YYGMLScript loader_get_script(const char* name)
{
	return internal_get_script(name);
}

LOADER_DLL CInstance** loader_get_global_instance()
{
	return internal_get_global_instance();
}

LOADER_DLL std::vector<CInstance*> loader_get_object_instances(const char* object_name)
{
	return internal_get_object_instances(object_name);
}

LOADER_DLL size_t loader_get_object_instance_count(const char* object_name)
{
	return internal_get_object_instance_count(object_name);
}