#include "init.h"

#include "internal/function.h"
#include "internal/instance.h"

bool initialized = false;
int32_t internal_init()
{
	if (initialized == true)
	{
		return InitializationStatusEnum::ALREADY_INITIALIZED;
	}

	if (internal_function_init() == false)
	{
		return InitializationStatusEnum::INIT_FUNCTION_FAIL;
	}

	if (internal_instance_init() == false)
	{
		return InitializationStatusEnum::INIT_INSTANCE_FAIL;
	}

	initialized = true;
	return InitializationStatusEnum::OKAY;
}