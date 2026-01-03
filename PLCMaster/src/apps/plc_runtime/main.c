/* PLC runtime entry point. TODO: wire runtime components together. */

#include <stdio.h>

#include "core/runtime/runtime.h"
#include "system/config/config_static.h"
#include "system/builder/system_builder.h"

int main(void)
{
	Runtime_t rt;
	runtime_init(&rt);

	const SystemConfig_t *cfg = get_static_config();
	int rc = system_build(&rt, cfg);

	if (rc == 0)
	{
		printf("OK\n");
		printf("Tag count: %u\n", rt.tag_table.count);
		printf("ID CPU_IO.X15_Out0 = %u\n", rt.io_view.X15_Out0);
		printf("ID CPU_IO.X21_CPU_Pt1.Pt_Value = %u\n", rt.io_view.X21_CPU_Pt1_Pt_Value);
	}
	else
	{
		printf("FAIL rc=%d\n", rc);
	}

	runtime_deinit(&rt);

	return rc == 0 ? 0 : 1;
}
