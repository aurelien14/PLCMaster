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
	memset(&rt->hmi_store, 0, sizeof(rt->hmi_store));
	rt->status_view = NULL;
	memset(rt->backend_array, 0, sizeof(rt->backend_array));
	rt->backends = rt->backend_array;
	rt->backend_count = 0U;

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
	memset(&rt->hmi_store, 0, sizeof(rt->hmi_store));
	rt->status_view = NULL;
	memset(rt->backend_array, 0, sizeof(rt->backend_array));
	rt->backends = rt->backend_array;
	rt->backend_count = 0U;
}

int runtime_backends_start(Runtime_t* rt)
{
	size_t i;
	size_t started = 0U;

	if (rt == NULL || rt->backends == NULL)
	{
		return -1;
	}

	for (i = 0; i < rt->backend_count; ++i)
	{
		BackendDriver_t *drv = rt->backends[i];
		if (drv == NULL)
		{
			continue;
		}
		if (drv->ops != NULL && drv->ops->start != NULL)
		{
			if (drv->ops->start(drv) != 0)
			{
				size_t stop_index;
				for (stop_index = started; stop_index > 0U; --stop_index)
				{
					BackendDriver_t *stop_drv = rt->backends[stop_index - 1U];
					if (stop_drv != NULL && stop_drv->ops != NULL && stop_drv->ops->stop != NULL)
					{
						(void)stop_drv->ops->stop(stop_drv);
					}
				}
				return -1;
			}
			started++;
		}
	}

	return 0;
}

int runtime_backends_stop(Runtime_t* rt)
{
	size_t i;
	int rc = 0;

	if (rt == NULL || rt->backends == NULL)
	{
		return -1;
	}

	for (i = 0; i < rt->backend_count; ++i)
	{
		BackendDriver_t *drv = rt->backends[i];
		if (drv == NULL)
		{
			continue;
		}
		if (drv->ops != NULL && drv->ops->stop != NULL)
		{
			if (drv->ops->stop(drv) != 0)
			{
				rc = -1;
			}
		}
	}

	return rc;
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
		BackendDriver_t *drv = rt->backends[i];
		if (drv == NULL)
		{
			continue;
		}
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
		BackendDriver_t *drv = rt->backends[i];
		if (drv == NULL || drv->ops == NULL)
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

	if (rt == NULL)
	{
		return PLC_HEALTH_FAULT;
	}

	if (rt->backends == NULL)
	{
		return PLC_HEALTH_OK;
	}

	for (i = 0; i < rt->backend_count; ++i)
	{
		BackendDriver_t *drv = rt->backends[i];
		BackendStatus_t status = {
			.in_op = true,
			.fault_latched = false,
			.last_error = 0,
		};

		if (drv == NULL)
		{
			continue;
		}

		if (drv->ops != NULL && drv->ops->get_status != NULL)
		{
			(void)drv->ops->get_status(drv, &status);
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
