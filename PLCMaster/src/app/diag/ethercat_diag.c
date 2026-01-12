/* EtherCAT diagnostics. */

#include <inttypes.h>
#include <stdio.h>

#include "app/diag/ethercat_diag.h"
#include "backends/ethercat/ec_backend.h"
#include "core/platform/platform_atomic.h"

static void ethercat_log_driver(const BackendDriver_t *drv)
{
	EthercatDriver_t *impl;

	if (drv == NULL || drv->type != BACKEND_TYPE_ETHERCAT || drv->impl == NULL)
	{
		return;
	}

	impl = (EthercatDriver_t *)drv->impl;

	printf("[EC] last_wkc=%d in_op=%s fault_latched=%d rt_overruns=%" PRIu64 " jitter_ns=%" PRIu64 " jitter_max_ns=%" PRIu64 "\n",
		plat_atomic_load_i32(&impl->last_wkc),
		plat_atomic_load_bool(&impl->in_op) ? "true" : "false",
		plat_atomic_load_i32(&impl->fault_latched),
		plat_atomic_load_u64(&impl->rt_overruns),
		plat_atomic_load_u64(&impl->rt_jitter_current_ns),
		plat_atomic_load_u64(&impl->rt_jitter_max_ns));
}

void ethercat_diag_print(const Runtime_t *rt)
{
	size_t i;

	if (rt == NULL || rt->backends == NULL)
	{
		return;
	}

	for (i = 0; i < rt->backend_count; ++i)
	{
		const BackendDriver_t *drv = rt->backends[i];
		if (drv == NULL)
		{
			continue;
		}
		if (drv->type == BACKEND_TYPE_ETHERCAT)
		{
			ethercat_log_driver(drv);
		}
	}
}
