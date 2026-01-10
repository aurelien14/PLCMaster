/* PLC task registry implementation. */

#include "app/plc/plc_tasks.h"

static int plc_task_noop(void* ctx)
{
	(void)ctx;
	return 0;
}

static int plc_task_heartbeat(void* ctx)
{
	(void)ctx;
	return 0;
}

static const PLC_TaskDesc_t kPlcTasks[] = {
	{
		.name = "noop",
		.run = plc_task_noop,
		.period_ms = 1000U,
	},
	{
		.name = "heartbeat",
		.run = plc_task_heartbeat,
		.period_ms = 500U,
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
