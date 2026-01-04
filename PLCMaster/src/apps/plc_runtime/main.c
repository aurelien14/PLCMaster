/* PLC runtime entry point. TODO: wire runtime components together. */

#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/platform/platform_thread.h"
#include "core/platform/platform_atomic.h"
#include "core/runtime/runtime.h"
#include "core/tag/tag_api.h"
#include "app/plc/plc_app.h"
#include "core/plc/plc_scheduler.h"
#include "app/config/config_static.h"
#include "system/builder/system_builder.h"
#include "backends/ethercat/ec_backend.h"
#include "app/plc/plc_tasks_demo.h"
#ifdef DEV
#include "app/demo/demo_tag_io.h"
#endif /* DEV */

static void plc_cycle_end(void *user)
{
	Runtime_t *rt = (Runtime_t *)user;

	if (rt == NULL)
	{
		return;
	}

	runtime_backends_sync_outputs(rt);
}

static void log_ethercat_debug(const BackendDriver_t *drv)
{
	EthercatDriver_t *impl;

	if (drv == NULL || drv->type != BACKEND_TYPE_ETHERCAT || drv->impl == NULL)
	{
		return;
	}

	impl = (EthercatDriver_t *)drv->impl;

	printf("[EC] last_wkc=%d in_op=%s fault_latched=%d rt_overruns=%" PRIu64 " jitter_max_ns=%" PRIi64 "\n",
		plat_atomic_load_i32(&impl->last_wkc),
		plat_atomic_load_bool(&impl->in_op) ? "true" : "false",
		plat_atomic_load_i32(&impl->fault_latched),
		plat_atomic_load_u64(&impl->rt_overruns),
		plat_atomic_load_i64(&impl->rt_jitter_max_ns));
}

static void log_backend_debug(const Runtime_t *rt)
{
	size_t i;

	if (rt == NULL || rt->backends == NULL)
	{
		return;
	}

	for (i = 0; i < rt->backend_count; ++i)
	{
		const BackendDriver_t *drv = &rt->backends[i];
		if (drv->type == BACKEND_TYPE_ETHERCAT)
		{
			log_ethercat_debug(drv);
		}
	}
}

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
		rc = plc_scheduler_init(&sched, 10);
	}

	if (rc == 0)
	{
		rc = plc_scheduler_set_callbacks(&sched, NULL, plc_cycle_end, &rt);
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

#ifdef DEV
	if (rc == 0)
	{
		rc = demo_tag_io_run(&rt);
	}
#endif /* DEV */

	if (rc == 0)
	{
		rc = plc_scheduler_start(&sched);
	}

	if (rc == 0)
	{
		uint32_t elapsed_ms = 0;

		while (elapsed_ms < 3000)
		{
			log_backend_debug(&rt);
			plat_thread_sleep_ms(1000);
			elapsed_ms += 1000;
		}
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
