#include "internal/instance.h"

#include "internal/search.h"

#include "loader/yoyo.h"

using SIGNATURE_CV_FINDALLOC_SLOT_FROM_NAME = int(*)(YYObjectBase* object, const char* name);
using SIGNATURE_INTERNAL_GETYYVAR = int(*)(YYObjectBase* object, int hash);

SIGNATURE_CV_FINDALLOC_SLOT_FROM_NAME Code_Variable_FindAlloc_Slot_From_Name = nullptr;
SIGNATURE_INTERNAL_GETYYVAR InternalGetYYVarRef = nullptr;

bool internal_instance_init()
{
	internal_find_findalloc_slot_from_name();
	if (Code_Variable_FindAlloc_Slot_From_Name == nullptr)
	{
		return false;
	}

	internal_find_getyyvarref();

	if (InternalGetYYVarRef == nullptr)
	{
		return false;
	}

	return true;
}

void internal_find_findalloc_slot_from_name()
{
	const std::vector<uint8_t> pattern =
	{ 
		0x8b, 0x44, 0x24, 0x04,
		0x56,
		0x3b, 0x05, '?', '?', '?', '?',
		0x75, '?',
		0x80, 0x3d, '?', '?', '?', '?', 0x00,
		0x75, '?',
		0x8b, 0x74, 0x24, 0x0c,
		0x56,
		0xe8, '?', '?', '?', '?',
		0x83, 0xc4, 0x04 
	};

	void* address = internal_scan(pattern);

	Code_Variable_FindAlloc_Slot_From_Name = (SIGNATURE_CV_FINDALLOC_SLOT_FROM_NAME)(address);
}

void internal_find_getyyvarref()
{
	const std::vector<uint8_t> pattern =
	{
		0x53,
		0x56,
		0x57,
		0x8b, 0xf9,
		0x83, 0x7f, '?', '?',
		0x75, 0x05,
		0xe8, '?', '?', '?', '?',
		0x8b, 0x77, 0x2c,
		0x8d, 0x44, 0x24, '?',
		0x8b, 0x5c, 0x24, 0x10,
		0x8b, 0xce,
		0x50,
		0x89, 0x5c, 0x24, 0x14,
		0xe8, '?', '?', '?', '?',
		0x83, 0xf8, 0xff,
		0x74, '?',
		0x8d, 0x0c, 0x40,
		0x8b, 0x46, '?',
		0x8d, 0x04, 0x88,
		0x85, 0xc0,
		0x74, '?',
		0x8b, 0x00
	};

	void* address = internal_scan(pattern);

	InternalGetYYVarRef = (SIGNATURE_INTERNAL_GETYYVAR)(address);
}

int32_t internal_call_findalloc_slot_from_name(
	YYObjectBase* object,
	const char* name
)
{
	return Code_Variable_FindAlloc_Slot_From_Name(object, name);
}

int32_t internal_call_getyyvarref(
	YYObjectBase* object,
	int32_t hash
)
{
	return InternalGetYYVarRef(object, hash);
}