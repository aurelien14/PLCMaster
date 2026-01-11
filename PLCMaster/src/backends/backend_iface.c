/* Backend interface helpers. */

#include "backends/backend_iface.h"

#include "backends/ethercat/ec_backend.h"
#include "core/platform/platform_atomic.h"

int backend_get_status(const BackendDriver_t *drv, BackendStatus_t *out)
{
	if (out == NULL)
	{
		return -1;
	}

	out->in_op = true;
	out->fault_latched = false;
	out->last_error = 0;

	if (drv == NULL)
	{
		return -1;
	}

	switch (drv->type)
	{
	case BACKEND_TYPE_ETHERCAT:
		if (drv->impl != NULL)
		{
			const EthercatDriver_t *impl = (const EthercatDriver_t *)drv->impl;
			out->in_op = plat_atomic_load_bool(&impl->in_op);
			out->fault_latched = (plat_atomic_load_i32(&impl->fault_latched) != 0);
			out->last_error = plat_atomic_load_i32(&impl->ec_state);
		}
		return 0;
	default:
		return 0;
	}
}
