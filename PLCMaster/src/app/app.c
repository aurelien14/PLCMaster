/* Application entrypoints. */

#include <string.h>

#include "app/app.h"
#include "core/plc/plc_task.h"

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
	const SystemConfig_t *cfg;
	size_t task_index;
	int rc = 0;

	if (sched == NULL || rt == NULL || app == NULL)
	{
		return -1;
	}

	cfg = app_get_config();
	if (cfg == NULL)
	{
		return -1;
	}

	if (cfg->task_count == 0 || cfg->tasks == NULL)
	{
		return 0;
	}

	for (task_index = 0; task_index < cfg->task_count; ++task_index)
	{
		const PlcTaskConfig_t *task_cfg = &cfg->tasks[task_index];
		PlcTask_t task;
		void *task_ctx = NULL;

		if (task_cfg->name == NULL || task_cfg->fn == NULL)
		{
			return -1;
		}

		if (task_cfg->create_ctx != NULL)
		{
			task_ctx = task_cfg->create_ctx(rt, app);
		}
		else
		{
			task_ctx = task_cfg->ctx;
		}

		if (task_ctx == NULL)
		{
			task_ctx = rt;
		}

		memset(&task, 0, sizeof(task));
		task.name = task_cfg->name;
		task.fn = task_cfg->fn;
		task.ctx = task_ctx;
		task.period_ms = task_cfg->period_ms;
		task.phase_ms = 0;

		rc = plc_scheduler_add_task(sched, &task);
		if (rc != 0)
		{
			return rc;
		}
	}

	return rc;
}
