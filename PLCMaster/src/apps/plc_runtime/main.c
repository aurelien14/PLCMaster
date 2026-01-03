/* PLC runtime entry point. TODO: wire runtime components together. */

#include <stdio.h>
#include <string.h>

#include "core/runtime/runtime.h"
#include "core/tag/tag_api.h"
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
		TagId_t proc_id = tag_table_find_id(&rt.tag_table, "proc.test_u32");
		uint32_t read_value = 0U;
		uint32_t written_value = 1234U;

		if (proc_id == 0)
		{
			rc = -1;
		}
		else
		{
			rc = tag_write_u32(proc_id, written_value);
			if (rc == 0)
			{
				rc = tag_read_u32(proc_id, &read_value);
			}
			if (rc == 0 && read_value != written_value)
			{
				rc = -1;
			}
			if (rc == 0)
			{
				printf("proc.test_u32 = %u\n", read_value);
			}
		}
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
