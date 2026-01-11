/* Runtime lifecycle stub implementation. TODO: wire in runtime components. */

#include "runtime.h"

#include <string.h>

int runtime_init(Runtime_t* rt)
{
	if (rt == NULL)
	{
		return -1;
	}

	if (tag_table_init(&rt->tag_table) != 0)
	{
		return -1;
	}

	memset(&rt->proc_store, 0, sizeof(rt->proc_store));
	rt->status_view = NULL;
	rt->backends = NULL;
	rt->backend_count = 0U;
	memset(rt->backend_array, 0, sizeof(rt->backend_array));

	return 0;
}

void runtime_deinit(Runtime_t* rt)
{
	if (rt == NULL)
	{
		return;
	}

	tag_table_deinit(&rt->tag_table);
	memset(&rt->proc_store, 0, sizeof(rt->proc_store));
	rt->status_view = NULL;
	rt->backends = NULL;
	rt->backend_count = 0U;
	memset(rt->backend_array, 0, sizeof(rt->backend_array));
}

void runtime_backends_cycle_begin(Runtime_t* rt)
{
	size_t i;

	if (rt == NULL || rt->backends == NULL)
	{
		return;
	}

	for (i = 0; i < rt->backend_count; ++i)
	{
		BackendDriver_t *drv = &rt->backends[i];
		if (drv->ops != NULL && drv->ops->cycle_begin != NULL)
		{
			drv->ops->cycle_begin(drv);
		}
	}
}

void runtime_backends_sync_outputs(Runtime_t* rt)
{
	size_t i;

	if (rt == NULL || rt->backends == NULL)
	{
		return;
	}

	for (i = 0; i < rt->backend_count; ++i)
	{
		BackendDriver_t *drv = &rt->backends[i];
		if (drv->ops == NULL)
		{
			continue;
		}
		if (drv->ops->cycle_end != NULL)
		{
			drv->ops->cycle_end(drv);
		}
	}
}

void runtime_backends_process_nonrt(Runtime_t* rt)
{
	/* Placeholder for future non-RT backend work. */
	(void)rt;
}

PlcHealthLevel_t runtime_get_health_level(const Runtime_t* rt)
{
	size_t i;
	PlcHealthLevel_t level = PLC_HEALTH_OK;

	if (rt == NULL || rt->backends == NULL)
	{
		return PLC_HEALTH_OK;
	}

	for (i = 0; i < rt->backend_count; ++i)
	{
		const BackendDriver_t *drv = &rt->backends[i];
		BackendStatus_t status;

		if (backend_get_status(drv, &status) != 0)
		{
			continue;
		}

		if (status.fault_latched)
		{
			return PLC_HEALTH_FAULT;
		}

		if (!status.in_op)
		{
			level = PLC_HEALTH_WARN;
		}
	}

	return level;
}
