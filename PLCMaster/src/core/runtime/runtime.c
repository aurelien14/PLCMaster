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
		if (drv->ops->cycle_commit != NULL)
		{
			drv->ops->cycle_commit(drv);
		}
		else if (drv->ops->sync != NULL)
		{
			drv->ops->sync(drv);
		}
	}
}

void runtime_backends_process_nonrt(Runtime_t* rt)
{
	/* Placeholder for future non-RT backend work. */
	(void)rt;
}
