/* PLC runtime entry point. TODO: wire runtime components together. */

#include <stdio.h>
#include <string.h>

#include "core/runtime/runtime.h"
#include "app/user_entry.h"
#include "core/plc/plc_scheduler.h"
#include "system/builder/system_builder.h"
#include "app/plc/plc_runtime.h"

typedef struct
{
	Runtime_t *rt;
	PlcScheduler_t *sched;
} PlcCycleCtx_t;

static void plc_cycle_end(void *user)
{
	PlcCycleCtx_t *ctx = (PlcCycleCtx_t *)user;
	Runtime_t *rt = NULL;

	if (ctx != NULL)
	{
		rt = ctx->rt;
	}

	if (rt == NULL)
	{
		return;
	}

	runtime_backends_sync_outputs(rt);
}

static void plc_cycle_begin(void *user)
{
	PlcCycleCtx_t *ctx = (PlcCycleCtx_t *)user;
	Runtime_t *rt = NULL;
	PlcScheduler_t *sched = NULL;

	if (ctx != NULL)
	{
		rt = ctx->rt;
		sched = ctx->sched;
	}

	if (rt == NULL || sched == NULL)
	{
		return;
	}

	runtime_backends_cycle_begin(rt);
	PlcHealthLevel_t level = runtime_get_health_level(rt);
	plc_scheduler_set_health(sched, level);

	{
		static PlcHealthLevel_t last_level = PLC_HEALTH_OK;
		if (level != last_level)
		{
			const char *level_str = "OK";
			if (level == PLC_HEALTH_WARN)
			{
				level_str = "WARN";
			}
			else if (level == PLC_HEALTH_FAULT)
			{
				level_str = "FAULT";
			}
			printf("[PLC] health=%s\n", level_str);
			last_level = level;
		}
	}
}

int main(void)
{
	Runtime_t rt;
	PlcApp_t app;
	PlcScheduler_t sched;
	PlcCycleCtx_t cycle_ctx;
	memset(&sched, 0, sizeof(sched));
	runtime_init(&rt);
	memset(&app, 0, sizeof(app));
	memset(&cycle_ctx, 0, sizeof(cycle_ctx));

	const SystemConfig_t *cfg = user_get_config();
	int rc = system_build(&rt, cfg);

	if (rc == 0)
	{
		rc = user_bind(&app, &rt);
	}

	if (rc == 0)
	{
		rc = plc_scheduler_init(&sched, cfg->plc_scheduler_base_cycle_ms);
	}

	if (rc == 0)
	{
		plc_scheduler_set_health(&sched, PLC_HEALTH_OK);
	}

	if (rc == 0)
	{
		cycle_ctx.rt = &rt;
		cycle_ctx.sched = &sched;
		rc = plc_scheduler_set_callbacks(&sched, plc_cycle_begin, plc_cycle_end, &cycle_ctx);
	}

	if (rc == 0)
	{
		rc = user_register_plc_tasks(&sched, &rt, &app);
	}

	if (rc == 0)
	{
		rc = plc_runtime_run(&rt, &sched);
	}

	runtime_deinit(&rt);

	return rc == 0 ? 0 : 1;
}
