/* PLC runtime orchestration. */

#include <inttypes.h>
#include <stdio.h>

#include "core/platform/platform_thread.h"
#include "core/platform/platform_atomic.h"
#include "core/plc/plc_scheduler.h"
#include "core/runtime/runtime.h"
#include "backends/ethercat/ec_backend.h"
#include "app/app.h"
#include "app/plc/plc_runtime.h"

#ifdef DEV
#include "app/demo/demo_tag_io.h"
#endif /* DEV */

static void log_ethercat_debug(const BackendDriver_t *drv)
{
	EthercatDriver_t *impl;

	if (drv == NULL || drv->type != BACKEND_TYPE_ETHERCAT || drv->impl == NULL)
	{
		return;
	}

	impl = (EthercatDriver_t *)drv->impl;

	printf("[EC] last_wkc=%d in_op=%s fault_latched=%d rt_overruns=%" PRIu64 " jitter_max_ns=%" PRIi64 "\n",
		plat_atomic_load_i32(&impl->last_wkc),
		plat_atomic_load_bool(&impl->in_op) ? "true" : "false",
		plat_atomic_load_i32(&impl->fault_latched),
		plat_atomic_load_u64(&impl->rt_overruns),
		plat_atomic_load_i64(&impl->rt_jitter_max_ns));
}

static void log_backend_debug(const Runtime_t *rt)
{
	size_t i;

	if (rt == NULL || rt->backends == NULL)
	{
		return;
	}

	for (i = 0; i < rt->backend_count; ++i)
	{
		const BackendDriver_t *drv = &rt->backends[i];
		if (drv->type == BACKEND_TYPE_ETHERCAT)
		{
			log_ethercat_debug(drv);
		}
	}
}

int plc_runtime_run(Runtime_t *rt, PlcScheduler_t *sched)
{
	int rc = -1;

	if (rt == NULL || sched == NULL)
	{
		return -1;
	}

#ifdef DEV
	rc = demo_tag_io_run(rt);
	if (rc != 0)
	{
		return rc;
	}
#endif /* DEV */

	rc = plc_scheduler_start(sched);

	if (rc == 0)
	{
		uint32_t elapsed_ms = 0;

		while (elapsed_ms < 3000)
		{
			log_backend_debug(rt);
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
		app_log_bindings();
	}
	else
	{
		printf("FAIL rc=%d\n", rc);
	}

	return rc;
}
