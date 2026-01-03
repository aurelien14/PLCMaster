/* Runtime lifecycle stub implementation. TODO: wire in runtime components. */

#include "runtime.h"

#include <stdlib.h>

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

	rt->proc_storage = NULL;
	rt->proc_storage_size = 0U;
	rt->status_view = NULL;
	rt->backends = NULL;

	return 0;
}

void runtime_deinit(Runtime_t* rt)
{
	if (rt == NULL)
	{
		return;
	}

	tag_table_deinit(&rt->tag_table);
	free(rt->proc_storage);
	rt->proc_storage = NULL;
	rt->proc_storage_size = 0U;
	rt->status_view = NULL;
	rt->backends = NULL;
}
