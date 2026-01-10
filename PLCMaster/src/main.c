/* PLC runtime entry point. TODO: wire runtime components together. */

#include <string.h>

#include "core/runtime/runtime.h"
#include "app/app.h"
#include "core/plc/plc_scheduler.h"
#include "system/builder/system_builder.h"
#include "app/plc/plc_runtime.h"

static void plc_cycle_end(void *user)
{
	Runtime_t *rt = (Runtime_t *)user;

	if (rt == NULL)
	{
		return;
	}

	runtime_backends_sync_outputs(rt);
}

static void plc_cycle_begin(void *user)
{
	Runtime_t *rt = (Runtime_t *)user;

	if (rt == NULL)
	{
		return;
	}

	runtime_backends_cycle_begin(rt);
}

int main(void)
{
	Runtime_t rt;
	PlcApp_t app;
	PlcScheduler_t sched;
	memset(&sched, 0, sizeof(sched));
	runtime_init(&rt);
	memset(&app, 0, sizeof(app));

	const SystemConfig_t *cfg = app_get_config();
	int rc = system_build(&rt, cfg);

	if (rc == 0)
	{
		rc = app_bind(&app, &rt);
	}

	if (rc == 0)
	{
		rc = plc_scheduler_init(&sched, 10);
	}

	if (rc == 0)
	{
		rc = plc_scheduler_set_callbacks(&sched, plc_cycle_begin, plc_cycle_end, &rt);
	}

	if (rc == 0)
	{
		rc = app_register_tasks(&sched, &rt, &app);
	}

	if (rc == 0)
	{
		rc = plc_runtime_run(&rt, &sched);
	}

	runtime_deinit(&rt);

	return rc == 0 ? 0 : 1;
}
