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
