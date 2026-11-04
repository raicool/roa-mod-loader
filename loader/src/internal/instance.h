#pragma once

#include "loader/yoyo.h"

#include <stdint.h>
#include <vector>

bool internal_instance_init();

void internal_find_findalloc_slot_from_name();
void internal_find_getyyvarref();
void internal_find_global_instance();
void internal_find_object_hash();

// returns hash for a variable within a given object
int32_t internal_call_findalloc_slot_from_name(
	YYObjectBase* object,
	const char* name
);

int32_t internal_call_getyyvarref(
	YYObjectBase* object,
	int32_t hash
);

int32_t internal_call_getyyvar(
	YYObjectBase* object,
	int32_t hash
);

CInstance** internal_get_global_instance(
);

std::vector<CInstance*> internal_get_object_instances(
	const char* object_name
);

size_t internal_get_object_instance_count(
	const char* object_name
);