/* PLC runtime entry point. TODO: wire runtime components together. */

#include <stdbool.h>
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
		TagId_t temp_sp_id = tag_table_find_id(&rt.tag_table, "proc.temp_sp");
		TagId_t run_cmd_id = tag_table_find_id(&rt.tag_table, "proc.run_cmd");
		float temp_read = 0.0f;
		bool run_read = false;

		if (temp_sp_id == 0 || run_cmd_id == 0)
		{
			rc = -1;
		}
		if (rc == 0)
		{
			rc = tag_read_real(&rt, temp_sp_id, &temp_read);
		}
		if (rc == 0)
		{
			printf("proc.temp_sp initial = %.2f\n", temp_read);
			rc = tag_write_real(&rt, temp_sp_id, 25.5f);
		}
		if (rc == 0)
		{
			rc = tag_read_real(&rt, temp_sp_id, &temp_read);
		}
		if (rc == 0)
		{
			printf("proc.temp_sp after write = %.2f\n", temp_read);
			rc = tag_read_bool(&rt, run_cmd_id, &run_read);
		}
		if (rc == 0)
		{
			printf("proc.run_cmd initial = %s\n", run_read ? "true" : "false");
			rc = tag_write_bool(&rt, run_cmd_id, true);
		}
		if (rc == 0)
		{
			rc = tag_read_bool(&rt, run_cmd_id, &run_read);
		}
		if (rc == 0)
		{
			printf("proc.run_cmd after write = %s\n", run_read ? "true" : "false");
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
