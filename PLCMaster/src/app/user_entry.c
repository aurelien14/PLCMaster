/* Application entrypoints. */

#include <stdio.h>
#include <string.h>

#include "app/user_entry.h"
#include "app/io/io_bind.h"
#include "core/plc/plc_task.h"
#include "app/plc/plc_task_ctx.h"
#include "app/plc/plc_task_registry.h"

static PlcTaskCtx_t g_task_ctx;

const SystemConfig_t *user_get_config(void)
{
	return get_static_config();
}

int user_bind(PlcApp_t *app, Runtime_t *rt)
{
	if (app == NULL || rt == NULL)
	{
		return -1;
	}

	return io_view_bind(&app->io, &rt->tag_table);
}

int user_register_plc_tasks(PlcScheduler_t *sched, Runtime_t *rt, PlcApp_t *app)
{
	const PLC_TaskConfig_t *tasks;
	size_t task_count;
	size_t task_index;
	int rc = 0;

	if (sched == NULL || rt == NULL || app == NULL)
	{
		return -1;
	}

	tasks = app_config_get_plc_tasks(&task_count);
	if (tasks == NULL || task_count == 0)
	{
		return 0;
	}

	g_task_ctx.rt = rt;
	g_task_ctx.app = app;

	for (task_index = 0; task_index < task_count; ++task_index)
	{
		const PLC_TaskConfig_t *task_desc = &tasks[task_index];
		PlcTask_t task;
		void *task_ctx = &g_task_ctx;
		PLC_TaskRunFn fn;

		if (!task_desc->enabled)
		{
			continue;
		}

		fn = plc_task_registry_find(task_desc->name);
		if (task_desc->name == NULL || fn == NULL)
		{
			printf("[PLC] unknown task '%s'\n", task_desc->name != NULL ? task_desc->name : "(null)");
			return -1;
		}

		memset(&task, 0, sizeof(task));
		task.name = task_desc->name;
		task.fn = fn;
		task.ctx = task_ctx;
		task.period_ms = task_desc->period_ms;
		task.phase_ms = task_desc->phase_ms;
		task.policy = PLC_TASK_SKIP_ON_FAULT;
		if (task_desc->policy == PLC_TASK_ALWAYS_RUN || task_desc->policy == PLC_TASK_SKIP_ON_FAULT || task_desc->policy == PLC_TASK_ONLY_WHEN_OK)
		{
			task.policy = task_desc->policy;
		}

		rc = plc_scheduler_add_task(sched, &task);
		if (rc != 0)
		{
			return rc;
		}
	}

	return rc;
}
