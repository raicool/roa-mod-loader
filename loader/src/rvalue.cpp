#include "loader/yoyo.h"

#include <loader/loader.h>
#include <internal/vm.h>
#include <internal/memory/memorymanager.h>

LOADER_DLL void RValue::set_real(double value)
{
	free_stringref();
	type = GML_TYPE_REAL;
	value_real = value;
}

LOADER_DLL void RValue::set_s32(int value)
{
	free_stringref();
	type = GML_TYPE_INT32;
	value_s32 = value;
}

LOADER_DLL void RValue::set_s64(long long value)
{
	free_stringref();
	type = GML_TYPE_INT64;
	value_s64 = value;
}

LOADER_DLL void RValue::set_bool(bool value)
{
	free_stringref();
	type = GML_TYPE_BOOL;
	value_real = value ? 1 : 0;
}

LOADER_DLL void RValue::set_stringref(const char* value)
{
	free_stringref();
	type = GML_TYPE_STRING;
	value_stringref = new YYStringRef(_strdup(value));
}

LOADER_DLL void RValue::free_and_undefine()
{
	free_stringref();
	type = GML_TYPE_UNDEFINED;
	value_s64 = 0;
}


LOADER_DLL int RValue::get_s32() const
{
	switch (type)
	{
	case GML_TYPE_INT32: return value_s32;
	case GML_TYPE_INT64: return (int)(value_s64 & 0xffffffff);
	case GML_TYPE_BOOL: return (int)(value_s32 > 0);
	case GML_TYPE_REAL: return (int)value_real;
	case GML_TYPE_NULL: return 0;
	}

	return 0;
}

LOADER_DLL long long RValue::get_s64() const
{
	switch (type)
	{
	case GML_TYPE_INT32: return (long long)value_s32;
	case GML_TYPE_INT64: return value_s64;
	case GML_TYPE_BOOL: return (long long)(value_s32 > 0);
	case GML_TYPE_REAL: return (long long)value_real;
	case GML_TYPE_NULL: return 0;
	}

	return 0;
}

LOADER_DLL unsigned long long RValue::get_u64() const
{
	switch (type)
	{
	case GML_TYPE_INT32: return (unsigned long long)value_s32;
	case GML_TYPE_INT64: return value_s64;
	case GML_TYPE_BOOL: return (unsigned long long)(value_s32 > 0);
	case GML_TYPE_REAL: return (unsigned long long)value_real;
	case GML_TYPE_NULL: return 0;
	}

	return 0;
}

LOADER_DLL void RValue::free_stringref()
{
	if (type == GML_TYPE_STRING && value_stringref)
	{
		internal_memory_manager_free(value_stringref);
		value_stringref = nullptr;
	}
	type = GML_TYPE_UNDEFINED;
}

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
		internal_set_string(this, other.value_stringref->data);
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
		if (value_stringref->ref_count <= 1)
		{
			//internal_memory_manager_free(const_cast<char*>(value_stringref->string));
			internal_memory_manager_free(value_stringref);
		}
		type = GML_TYPE_UNDEFINED;
	}
}