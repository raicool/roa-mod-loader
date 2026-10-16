#pragma once

#include "loader/yoyo.h"

#include <stdint.h>

bool internal_instance_init();

void internal_find_findalloc_slot_from_name();
void internal_find_getyyvarref();

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