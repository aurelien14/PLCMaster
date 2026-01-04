/* PLC runtime entry point. TODO: wire runtime components together. */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/platform/platform_thread.h"
#include "core/runtime/runtime.h"
#include "core/tag/tag_api.h"
#include "plc/plc_app.h"
#include "plc/plc_scheduler.h"
#include "plc/plc_tasks_demo.h"
#include "system/config/config_static.h"
#include "system/builder/system_builder.h"

int main(void)
{
	Runtime_t rt;
	PlcApp_t app;
	PlcScheduler_t sched;
	PlcDemoBlinkCtx_t blink_ctx;
	PlcDemoControlCtx_t control_ctx;
	memset(&sched, 0, sizeof(sched));
	memset(&app, 0, sizeof(app));
	memset(&blink_ctx, 0, sizeof(blink_ctx));
	memset(&control_ctx, 0, sizeof(control_ctx));
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
		TagId_t hmi_temp_sp_id = tag_table_find_id(&rt.tag_table, "hmi.temp_setpoint");
		TagId_t hmi_alarm_code_id = tag_table_find_id(&rt.tag_table, "hmi.alarm_code");
		float temp_read = 0.0f;
		bool run_read = false;
		uint16_t alarm_read = 0U;
		TagId_t io_in_id = tag_table_find_id(&rt.tag_table, "CPU_IO.X30_2_In0");
		TagId_t io_out_id = tag_table_find_id(&rt.tag_table, "CPU_IO.X15_Out0");

		if (temp_sp_id == 0 || run_cmd_id == 0 || hmi_temp_sp_id == 0 || hmi_alarm_code_id == 0)
		{
			rc = -1;
		}
		if (rc == 0 && (io_in_id == 0 || io_out_id == 0))
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
		if (rc == 0)
		{
			rc = tag_read_real(&rt, hmi_temp_sp_id, &temp_read);
		}
		if (rc == 0)
		{
			printf("hmi.temp_setpoint initial = %.2f\n", temp_read);
			rc = tag_write_real(&rt, hmi_temp_sp_id, 30.0f);
		}
		if (rc == 0)
		{
			rc = tag_read_real(&rt, temp_sp_id, &temp_read);
		}
		if (rc == 0)
		{
			printf("proc.temp_sp after hmi write = %.2f\n", temp_read);
			rc = tag_write_u16(&rt, hmi_alarm_code_id, 1234U);
			if (rc == 0)
			{
				rc = -1;
			}
			else
			{
				rc = 0;
			}
		}
		if (rc == 0)
		{
			rc = tag_read_u16(&rt, hmi_alarm_code_id, &alarm_read);
		}
		if (rc == 0)
		{
			printf("hmi.alarm_code after failed write = %u\n", alarm_read);
		}
	}

	if (rc == 0)
	{
		rc = plc_scheduler_init(&sched, 10);
	}

	if (rc == 0)
	{
		PlcTask_t blink_task = {
			.name = "blink",
			.fn = plc_demo_task_blink,
			.ctx = &blink_ctx,
			.period_ms = 200,
			.phase_ms = 0,
			.last_run_ms = 0,
		};
		PlcTask_t control_task = {
			.name = "control",
			.fn = plc_demo_task_control,
			.ctx = &control_ctx,
			.period_ms = 1000,
			.phase_ms = 0,
			.last_run_ms = 0,
		};

		control_ctx.runtime = &rt;
		control_ctx.temp_sp_id = tag_table_find_id(&rt.tag_table, "proc.temp_sp");

		rc = plc_scheduler_add_task(&sched, &blink_task);
		if (rc == 0)
		{
			rc = plc_scheduler_add_task(&sched, &control_task);
		}
	}

	if (rc == 0)
	{
		rc = plc_scheduler_start(&sched);
	}

	if (rc == 0)
	{
		plat_thread_sleep_ms(3000);
		if (plc_scheduler_stop(&sched) != 0)
		{
			rc = -1;
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
