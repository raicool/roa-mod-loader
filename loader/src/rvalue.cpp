#include "loader/yoyo.h"

#include <loader/loader.h>
#include <internal/vm.h>
#include <internal/memory/memorymanager.h>

LOADER_DLL RValue::RValue() 
{ 
	value_s64 = 0;
}

LOADER_DLL RValue::RValue(RValue* other)
{
	RValue();

	this->value_ptr = static_cast<void*>(other);
}


LOADER_DLL RValue::RValue(const RValue& other)
{
	RValue();

	this->type = other.type;
	this->flags = other.flags;

	if (other.type == GML_TYPE_STRING && other.value_stringref)
	{
		internal_set_string(this, other.value_stringref->string);
		return;
	}

	this->value_s64 = other.value_s64;
}

LOADER_DLL RValue::RValue(double value)
{
	this->set_real(value);
}

LOADER_DLL RValue::RValue(float value)
{
	this->set_real((double)value);
}

LOADER_DLL RValue::RValue(int value)
{
	this->set_s32(value);
}

LOADER_DLL RValue::RValue(long long value)
{
	this->set_s64(value);
}

LOADER_DLL RValue::RValue(bool value)
{
	this->set_real(value ? 1 : 0);
}

LOADER_DLL RValue::RValue(const char* value)
{
	RValue();

	internal_set_string(this, value);
}

LOADER_DLL RValue::RValue(YYObjectBase* value)
{
	type = GML_TYPE_OBJECT;
	value_object = value;
}

LOADER_DLL RValue::RValue(CInstance* value)
{
	type = GML_TYPE_OBJECT;
	value_instance = value;
}

LOADER_DLL RValue::RValue(std::string value)
{
	RValue();

	internal_set_string(this, value.data());
}

LOADER_DLL RValue::~RValue()
{
	if (type == GML_TYPE_STRING)
	{
		if (value_stringref->refCountGML <= 1)
		{
			//internal_memory_manager_free(const_cast<char*>(value_stringref->string));
			internal_memory_manager_free(value_stringref);
		}
		type = GML_TYPE_UNDEFINED;
	}
}