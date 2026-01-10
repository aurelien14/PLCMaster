/* Application entrypoints. */

#include <string.h>
#include <stdio.h>

#include "app/app.h"
#include "app/plc/plc_app.h"
#include "app/plc/plc_tasks_demo.h"

static PlcApp_t g_app;
static PlcDemoBlinkCtx_t g_blink_ctx;
static PlcDemoControlCtx_t g_control_ctx;

const SystemConfig_t *app_get_config(void)
{
	return get_static_config();
}

int app_register_plc_tasks(PlcScheduler_t *sched, Runtime_t *rt)
{
	int rc = -1;
	PlcTask_t blink_task;
	PlcTask_t control_task;
	PlcTask_t iotest_task;



	if (sched == NULL || rt == NULL)
	{
		return -1;
	}

	memset(&g_app, 0, sizeof(g_app));
	memset(&g_blink_ctx, 0, sizeof(g_blink_ctx));
	memset(&g_control_ctx, 0, sizeof(g_control_ctx));

	/*rc = plc_app_bind(&g_app, &rt->tag_table);
	if (rc != 0)
	{
		return rc;
	}*/
	g_app.runtime = rt;

	g_control_ctx.runtime = rt;
	g_control_ctx.temp_sp_id = tag_table_find_id(&rt->tag_table, "proc.temp_sp");

	blink_task = (PlcTask_t){
		.name = "blink",
		.fn = plc_demo_task_blink,
		.ctx = &g_blink_ctx,
		.period_ms = 200,
		.phase_ms = 0,
		.last_run_ms = 0,
	};
	control_task = (PlcTask_t){
		.name = "control",
		.fn = plc_demo_task_control,
		.ctx = &g_control_ctx,
		.period_ms = 1000,
		.phase_ms = 0,
		.last_run_ms = 0,
	};
	control_task = (PlcTask_t){
		.name = "io",
		.fn = plc_dio_test,
		.ctx = &g_app,
		.period_ms = 1000,
		.phase_ms = 0,
		.last_run_ms = 0,
	};

	rc = plc_scheduler_add_task(sched, &blink_task);
	if (rc == 0)
	{
		rc = plc_scheduler_add_task(sched, &control_task);
	}

	return rc;
}

void app_log_bindings(void)
{
	printf("ID CPU_IO.X15_Out0 = %u\n", g_app.io.X15_Out0);
	printf("ID CPU_IO.X21_CPU_Pt1.Pt_Value = %u\n", g_app.io.X21_CPU_Pt1_Pt_Value);
}
