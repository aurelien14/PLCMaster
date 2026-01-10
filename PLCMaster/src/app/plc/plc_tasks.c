/* PLC task registry implementation. */

#include "app/plc/plc_tasks.h"
#include <stdbool.h>
#include "core/runtime/runtime.h"
#include "core/tag/tag_api.h"

static int plc_task_noop(void* ctx)
{
	(void)ctx;
	return 0;
}

static int plc_task_heartbeat(void* ctx)
{
	Runtime_t * rt = (Runtime_t*)ctx;
	static bool state = false;
	static TagId_t x15_id = 0;
	if (rt == NULL)
		return -1;
	/* Résolution lazy (1 seule fois) */
	if (x15_id == 0)
	{
		x15_id = tag_table_find_id(&rt->tag_table, "CPU_IO.X12_Out0");
		if (x15_id == 0)
		{
			/* Le bind IOView n’est peut-être pas complet, ou le tag n’existe pas */
			return -2;
		}
	}
	state = !state;
	return tag_write_bool(rt, x15_id, state);
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
