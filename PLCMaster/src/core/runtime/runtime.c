/* Runtime lifecycle stub implementation. TODO: wire in runtime components. */

#include <stddef.h>

#include "runtime.h"

int runtime_init(Runtime_t* rt)
{
    if (rt == NULL)
    {
        return -1;
    }

    rt->tag_table = NULL;
    rt->io_view = NULL;
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

    rt->tag_table = NULL;
    rt->io_view = NULL;
    rt->status_view = NULL;
    rt->backends = NULL;
}
