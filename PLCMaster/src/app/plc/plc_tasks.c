/* PLC task registry implementation. */

#include <stdbool.h>
#include <stdio.h>
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
	tag_write_bool(c->rt, c->app->io.X15_Out0, state);
	tag_write_bool(c->rt, c->app->io.X14_Out5, !state);

	TagId_t counter = tag_table_find_id(&c->rt->tag_table, "proc.counter_test");
	if (counter == 0) {
		printf("error to find tag\n");
	}
	uint32_t counter_val;
	if (tag_read_u32(c->rt, counter, &counter_val) < 0) {
		printf("error to read tag\n");
	}
	counter_val++;
	tag_write_u32(c->rt, counter, counter_val);
	if (counter_val % 10 == 0) {
		printf("[TASK] counter = %d\n", counter_val);
	}
	return 0;
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
