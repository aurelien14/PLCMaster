/* Static configuration stub implementation. TODO: load and manage static configuration. */

#include "app/config/config_static.h"

static const BackendConfig_t kBackendConfigs[] = {
	{
		.type = BACKEND_TYPE_ETHERCAT,
		.name = "ec0",
		.ifname = "\\Device\\NPF_{FB092E67-7CA1-4E8F-966D-AC090D396487}",
		.cycle_time_us = 5000,
		.dc_clock = false,
		.period_ns = 0,
		.spin_threshold_ns = 50000,
		.rt_priority = PLAT_THREAD_PRIORITY_CRITICAL,
		.rt_affinity_cpu = 1,
		.rt_reserved_cpu = 1,
		.rt_timer_resolution_ms = 1,
		.wkc_warn_threshold = 2,
		.wkc_fail_threshold = 10,
		.warmup_cycles = 20,
		.state_check_period_ms = 200,
	},
};

static const DeviceConfig_t kDeviceConfigs[] = {
	{
		.name = "CPU_IO",
		.model = "L230",
		.backend = "ec0",
		.ethercat =
			{
				.expected_position = 1,
			},
		.io_addr = 1,
		.expected_identity =
			{
				.vendor_id = 0x47535953,
				.product_code = 0x3213335,
			},
	},
};

static const ProcessVarDesc_t kProcessVarDescs[] = {
	{
		.name = "temp_sp",
		.type = TAG_T_REAL,
		.initial = {.f = 20.0f},
	},
	{
		.name = "run_cmd",
		.type = TAG_T_BOOL,
		.initial = {.b = false},
	},
	{
		.name = "alarm_code",
		.type = TAG_T_U16,
		.initial = {.u16 = 0},
	},
	{
		.name = "counter_test",
		.type = TAG_T_U32,
		.initial = {.u32 = 0},
	},
};

static const HmiTagDesc_t kHmiTagDescs[] = {
	{
		.name = "temp_setpoint",
		.alias_of = "proc.temp_sp",
		.access = HMI_RW,
	},
	{
		.name = "run",
		.alias_of = "proc.run_cmd",
		.access = HMI_RW,
	},
	{
		.name = "alarm_code",
		.alias_of = "proc.alarm_code",
		.access = HMI_RO,
	},
	{
		.name = "counter_test",
		.alias_of = "proc.counter_test",
		.access = HMI_RO,
	},
};

static const PLC_TaskConfig_t kPlcTaskConfigs[] = {
	{
		.name = "noop",
		.period_ms = 1000U,
		.phase_ms = 0U,
		.policy = PLC_TASK_SKIP_ON_FAULT,
		.enabled = true,
	},
	{
		.name = "heartbeat",
		.period_ms = 500U,
		.phase_ms = 0U,
		.policy = PLC_TASK_ALWAYS_RUN,
		.enabled = true,
	},
};

static const SystemConfig_t kStaticConfig = {
	.backends = kBackendConfigs,
	.backend_count = sizeof(kBackendConfigs) / sizeof(kBackendConfigs[0]),
	.devices = kDeviceConfigs,
	.device_count = sizeof(kDeviceConfigs) / sizeof(kDeviceConfigs[0]),
	.process_vars = kProcessVarDescs,
	.process_var_count = sizeof(kProcessVarDescs) / sizeof(kProcessVarDescs[0]),
	.hmi_tags = kHmiTagDescs,
	.hmi_tag_count = sizeof(kHmiTagDescs) / sizeof(kHmiTagDescs[0]),
	.plc_scheduler_base_cycle_ms = 10,
};

const SystemConfig_t *get_static_config(void)
{
	return &kStaticConfig;
}

const PLC_TaskConfig_t *app_config_get_plc_tasks(size_t *out_count)
{
	if (out_count != NULL)
	{
		*out_count = sizeof(kPlcTaskConfigs) / sizeof(kPlcTaskConfigs[0]);
	}

	return kPlcTaskConfigs;
}
