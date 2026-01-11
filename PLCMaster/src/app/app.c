/* Application entrypoints. */

#include <string.h>

#include "app/app.h"
#include "core/plc/plc_task.h"
#include "app/plc/plc_task_ctx.h"
#include "app/plc/plc_tasks.h"

static PlcTaskCtx_t g_task_ctx;

const SystemConfig_t *app_get_config(void)
{
	return get_static_config();
}

int app_bind(PlcApp_t *app, Runtime_t *rt)
{
	if (app == NULL || rt == NULL)
	{
		return -1;
	}

	return plc_app_bind(app, &rt->tag_table);
}

int app_register_tasks(PlcScheduler_t *sched, Runtime_t *rt, PlcApp_t *app)
{
	const PLC_TaskDesc_t *tasks;
	size_t task_count;
	size_t task_index;
	int rc = 0;

	if (sched == NULL || rt == NULL || app == NULL)
	{
		return -1;
	}

	tasks = app_plc_get_tasks(&task_count);
	if (tasks == NULL || task_count == 0)
	{
		return 0;
	}

	g_task_ctx.rt = rt;
	g_task_ctx.app = app;

	for (task_index = 0; task_index < task_count; ++task_index)
	{
		const PLC_TaskDesc_t *task_desc = &tasks[task_index];
		PlcTask_t task;
		void *task_ctx = &g_task_ctx;

		if (task_desc->name == NULL || task_desc->run == NULL)
		{
			return -1;
		}

		memset(&task, 0, sizeof(task));
		task.name = task_desc->name;
		task.fn = task_desc->run;
		task.ctx = task_ctx;
		task.period_ms = task_desc->period_ms;
		task.phase_ms = 0;

		rc = plc_scheduler_add_task(sched, &task);
		if (rc != 0)
		{
			return rc;
		}
	}

	return rc;
}
