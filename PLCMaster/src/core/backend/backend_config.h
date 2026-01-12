/* Backend configuration definitions. */

#ifndef BACKEND_CONFIG_H
#define BACKEND_CONFIG_H

#include <stdbool.h>
#include <stdint.h>

#include "core/platform/platform_thread.h"

typedef enum BackendType
{
	BACKEND_TYPE_NONE = 0,
	BACKEND_TYPE_ETHERCAT = 1,
	BACKEND_TYPE_MODBUS = 2,
} BackendType_t;

typedef struct BackendConfig
{
	BackendType_t type;
	const char *name;
	const char *ifname;	/* TODO: set interface name */
	uint32_t cycle_time_us;
	bool dc_clock;
	uint32_t period_ns;
	uint32_t spin_threshold_ns;
	PlatThreadPriority_t rt_priority;
	int rt_affinity_cpu;	/* -1 = none */
	int rt_reserved_cpu;	/* shared default for EtherCAT RT affinity */
	uint32_t rt_timer_resolution_ms;
	uint32_t wkc_warn_threshold;
	uint32_t wkc_fail_threshold;
	uint32_t warmup_cycles;
	uint32_t state_check_period_ms;
} BackendConfig_t;

#endif /* BACKEND_CONFIG_H */
