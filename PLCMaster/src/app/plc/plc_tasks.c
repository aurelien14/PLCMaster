/* PLC task registry implementation. */

#include <stdbool.h>

#include "app/plc/plc_task_ctx.h"
#include "app/plc/plc_tasks.h"
#include "core/tag/tag_api.h"

static int plc_task_noop(void* ctx)
{
	(void)ctx;
	return 0;
}

static int plc_task_heartbeat(void* ctx)
{
	PlcTaskCtx_t *c = (PlcTaskCtx_t*)ctx;
	static bool state = false;
	if (c == NULL || c->rt == NULL || c->app == NULL)
	{
		return -1;
	}
	state = !state;
	return tag_write_bool(c->rt, c->app->io.X15_Out0, state);
}

static const PLC_TaskDesc_t kPlcTasks[] = {
	{
		.name = "noop",
		.run = plc_task_noop,
		.period_ms = 1000U,
		.policy = PLC_TASK_SKIP_ON_FAULT,
	},
	{
		.name = "heartbeat",
		.run = plc_task_heartbeat,
		.period_ms = 500U,
		.policy = PLC_TASK_ALWAYS_RUN,
	},
};

const PLC_TaskDesc_t* app_plc_get_tasks(size_t* out_count)
{
	if (out_count != NULL)
	{
		*out_count = sizeof(kPlcTasks) / sizeof(kPlcTasks[0]);
	}

	return kPlcTasks;
}
