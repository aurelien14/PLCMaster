/* PLC runtime entry point. TODO: wire runtime components together. */

#include <stdio.h>
#include <string.h>

#include "core/runtime/runtime.h"
#include "plc/plc_app.h"
#include "system/config/config_static.h"
#include "system/builder/system_builder.h"

int main(void)
{
	Runtime_t rt;
	PlcApp_t app;
	memset(&app, 0, sizeof(app));
	runtime_init(&rt);

	const SystemConfig_t *cfg = get_static_config();
	int rc = system_build(&rt, cfg);

	if (rc == 0)
	{
		rc = plc_app_bind(&app, &rt.tag_table);
	}

	if (rc == 0)
	{
		printf("OK\n");
		printf("Tag count: %u\n", rt.tag_table.count);
		printf("ID CPU_IO.X15_Out0 = %u\n", app.io.X15_Out0);
		printf("ID CPU_IO.X21_CPU_Pt1.Pt_Value = %u\n", app.io.X21_CPU_Pt1_Pt_Value);
	}
	else
	{
		printf("FAIL rc=%d\n", rc);
	}

	runtime_deinit(&rt);

	return rc == 0 ? 0 : 1;
}
