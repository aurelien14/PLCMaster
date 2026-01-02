/* PLC runtime entry point. TODO: wire runtime components together. */

#include <stdio.h>

#include "runtime.h"
#include "config_static.h"
#include "system_builder.h"

int main(void)
{
    Runtime_t rt;
    runtime_init(&rt);

    const SystemConfig_t *cfg = get_static_config();
    int rc = system_build(&rt, cfg);

    if (rc == 0)
    {
        printf("OK\n");
    }
    else
    {
        printf("FAIL rc=%d\n", rc);
    }

    runtime_deinit(&rt);

    return rc == 0 ? 0 : 1;
}
