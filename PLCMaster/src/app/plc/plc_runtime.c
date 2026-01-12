/* PLC runtime orchestration. */

#include <stdio.h>

#include "core/platform/platform_thread.h"
#include "core/plc/plc_scheduler.h"
#include "core/runtime/runtime.h"
#include "app/diag/ethercat_diag.h"
#include "app/plc/plc_runtime.h"

int plc_runtime_run(Runtime_t *rt, PlcScheduler_t *sched)
{
	int rc = -1;
	int stop_rc = 0;

	if (rt == NULL || sched == NULL)
	{
		return -1;
	}

	rc = runtime_backends_start(rt);
	if (rc != 0)
	{
		return -1;
	}

	rc = plc_scheduler_start(sched);

	if (rc == 0)
	{
		uint32_t elapsed_ms = 0;

		while (elapsed_ms < 3000000)
		{
			ethercat_diag_print(rt);
			plat_thread_sleep_ms(1000);
			elapsed_ms += 1000;
		}
		if (plc_scheduler_stop(sched) != 0)
		{
			rc = -1;
		}
	}

	if (rc == 0)
	{
		printf("OK\n");
		printf("Tag count: %u\n", rt->tag_table.count);
	}
	else
	{
		printf("FAIL rc=%d\n", rc);
	}

	stop_rc = runtime_backends_stop(rt);
	if (rc == 0 && stop_rc != 0)
	{
		rc = -1;
	}

	return rc;
}
