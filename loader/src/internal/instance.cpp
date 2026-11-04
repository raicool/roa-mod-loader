#include "internal/instance.h"

#include "internal.h"
#include "internal/search.h"

#include "loader/yoyo.h"
#include "loader/log.h"

using SIGNATURE_CV_FINDALLOC_SLOT_FROM_NAME = int(*)(YYObjectBase* object, const char* name);
using SIGNATURE_INTERNAL_GETYYVAR = int(*)(YYObjectBase* object, int hash);

SIGNATURE_CV_FINDALLOC_SLOT_FROM_NAME Code_Variable_FindAlloc_Slot_From_Name = nullptr;
SIGNATURE_INTERNAL_GETYYVAR InternalGetYYVarRef = nullptr;

CInstance** g_pGlobal = nullptr;
ObjectsHash** g_ObjectHash = nullptr;

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

	internal_find_global_instance();
	if (g_pGlobal == nullptr)
	{
		return false;
	}

	internal_find_object_hash();
	if (g_ObjectHash == nullptr)
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

void internal_find_global_instance()
{
	void* location = nullptr;
	if ((location = internal_scan(
		{ 
			0x83, 0xec, '?',
			0x53,
			0x56,
			0x57,
			0x6a, 0x00,
			0xff, 0x74, 0x24, '?',
			0xe8, '?', '?', '?', '?',
			0x8b, 0x74, 0x24, '?',
			0x8b, 0xf8,
			0x8b, 0x1d, '?', '?', '?', '?',
			0x0f, 0x57, 0xc0,
			0x57,
			0xc7, 0x46, '?', '?', '?', '?', '?',
			0xf2, 0x0f, 0x11, 0x06
		})) != nullptr
		)
	{
		g_pGlobal = *reinterpret_cast<CInstance***>(INCREMENT_POINTER(location, 25));
	}
}

void internal_find_object_hash()
{
	void* location = nullptr;
	if ((location = internal_scan(
		{
			0xa1, '?', '?', '?', '?',
			0x8b, 0x54, 0x24, '?',
			0x8b, 0x48, '?',
			0x8b, 0x00,
			0x23, 0xca,
			0x8b, 0x04, '?',
			0x85, 0xc0,
			0x74, 0x0c,
			0x39, 0x50, '?',
			0x74, '?'
		})) != nullptr
		)
	{
		uint32_t real_ptr = *(uint32_t*)INCREMENT_POINTER(location, 1);

		g_ObjectHash = (ObjectsHash**)real_ptr;
	}
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

CInstance** internal_get_global_instance()
{
	return g_pGlobal;
}

std::vector<CInstance*> internal_get_object_instances(
	const char* object_name
)
{
	std::vector<CInstance*> instances;
	CObjectGM* object = nullptr;

	(*g_ObjectHash)->for_each([&object, &object_name](CObjectGM* this_object)
		{
			if (strcmp(this_object->m_Name, object_name) == 0)
			{
				object = this_object;
			}
		}
	);

	if (object == nullptr)
	{
		loader_log_warn("internal_get_object_instances(): could not find object \"{}\"", object_name);
		return instances;
	}

	const LinkedList<CInstancePadded>* inst_list = &object->m_Instances;

	if (inst_list->m_First == nullptr)
	{
		return instances;
	}

	CInstance* instance = inst_list->m_First->instance;

	for (uint32_t idx = 0; idx < inst_list->m_Count; idx++)
	{
		instances.emplace_back(instance);

		instance = instance->next;
	}

	return instances;
}

size_t internal_get_object_instance_count(
	const char* object_name
)
{
	std::vector<CInstance*> instances;
	CObjectGM* object = nullptr;

	(*g_ObjectHash)->for_each([&object, &object_name](CObjectGM* this_object)
		{
			if (strcmp(this_object->m_Name, object_name) == 0)
			{
				object = this_object;
			}
		}
	);

	if (object)
	{
		const LinkedList<CInstancePadded>* inst_list = &object->m_Instances;

		return inst_list->m_Count;
	}
	else
	{
		loader_log_warn("internal_get_object_instance_count(): could not find object \"{}\"", object_name);
		return 0;
	}
}