/* Static configuration stub implementation. TODO: load and manage static configuration. */

#include "app/config/config_static.h"
#include "app/io/hmi_view.h"
#include "app/io/io_view.h"
#include "app/io/proc_view.h"

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
		.name = "DAIO",
		.model = "L230 DAIO Micro",
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
		.retain = false,
	},
	{
		.name = "chamber_run_cmd",
		.type = TAG_T_BOOL,
		.initial = {.b = false},
		.retain = false,
	},
	{
		.name = "alarm_state",
		.type = TAG_T_BOOL,
		.initial = {.b = 0},
		.retain = false,
	},
	{
		.name = "alarm_code",
		.type = TAG_T_U16,
		.initial = {.u16 = 0},
		.retain = false,
	},
	{
		.name = "counter_test",
		.type = TAG_T_U32,
		.initial = {.u32 = 0},
		.retain = false,
	},
};

static const HmiTagDesc_t kHmiTagDescs[] = {
	{
		.name = "temp_setpoint",
		.access = HMI_RW,
		.kind = HMI_TAG_ALIAS,
		.alias = {.alias_of = "proc.temp_sp"},
	},
	{
		.name = "run",
		.access = HMI_RW,
		.kind = HMI_TAG_ALIAS,
		.alias = {.alias_of = "proc.chamber_run_cmd"},
	},
	{
		.name = "alarm_state",
		.access = HMI_RO,
		.kind = HMI_TAG_ALIAS,
		.alias = {.alias_of = "proc.alarm_state"},
	},
	{
		.name = "alarm_code",
		.access = HMI_RO,
		.kind = HMI_TAG_ALIAS,
		.alias = {.alias_of = "proc.alarm_code"},
	},
	{
		.name = "counter_test",
		.access = HMI_RO,
		.kind = HMI_TAG_ALIAS,
		.alias = {.alias_of = "proc.counter_test"},
	},
	{
		.name = "FactoryResetCmd",
		.access = HMI_RW,
		.kind = HMI_TAG_VAR,
		.var = {
			.type = TAG_T_BOOL,
			.initial = {.b = false},
			.retain = false,
		},
	},
};

static const AppTagBind_t kIoViewBinds[] = {
	{ offsetof(IOView_t, X15_Out0), "DAIO.X15_Out0" },
	{ offsetof(IOView_t, X12_Out1), "DAIO.X12_Out1" },
	{ offsetof(IOView_t, X13_Out2), "DAIO.X13_Out2" },
	{ offsetof(IOView_t, X3_Out3), "DAIO.X3_Out3" },
	{ offsetof(IOView_t, X4_Out4), "DAIO.X4_Out4" },
	{ offsetof(IOView_t, X14_Out5), "DAIO.X14_Out5" },
	{ offsetof(IOView_t, X1_Out6), "DAIO.X1_Out6" },
	{ offsetof(IOView_t, X11_Out0), "DAIO.X11_Out0" },
	{ offsetof(IOView_t, X10_Out1), "DAIO.X10_Out1" },
	{ offsetof(IOView_t, X5_Out2), "DAIO.X5_Out2" },
	{ offsetof(IOView_t, X6_Out3), "DAIO.X6_Out3" },
	{ offsetof(IOView_t, X7_Out4), "DAIO.X7_Out4" },
	{ offsetof(IOView_t, X8_Out5), "DAIO.X8_Out5" },
	{ offsetof(IOView_t, X9_Out6), "DAIO.X9_Out6" },
	{ offsetof(IOView_t, X8a_Out7), "DAIO.X8a_Out7" },
	{ offsetof(IOView_t, X30_2_In0), "DAIO.X30_2_In0" },
	{ offsetof(IOView_t, X30_4_In1), "DAIO.X30_4_In1" },
	{ offsetof(IOView_t, X30_6_In2), "DAIO.X30_6_In2" },
	{ offsetof(IOView_t, X30_8_In3), "DAIO.X30_8_In3" },
	{ offsetof(IOView_t, X30_11_In4), "DAIO.X30_11_In4" },
	{ offsetof(IOView_t, X30_13_In5), "DAIO.X30_13_In5" },
	{ offsetof(IOView_t, X30_19_In6), "DAIO.X30_19_In6" },
	{ offsetof(IOView_t, X30_21_In7), "DAIO.X30_21_In7" },
	{ offsetof(IOView_t, X55_In0), "DAIO.X55_In0" },
	{ offsetof(IOView_t, X21_CPU_Pt1_Pt_Value), "DAIO.X21_CPU_Pt1.Pt_Value" },
	{ offsetof(IOView_t, X21_CPU_Pt1_Pt_State), "DAIO.X21_CPU_Pt1.Pt_State" },
	{ offsetof(IOView_t, X22_CPU_Pt2_Pt_Value), "DAIO.X22_CPU_Pt2.Pt_Value" },
	{ offsetof(IOView_t, X22_CPU_Pt2_Pt_State), "DAIO.X22_CPU_Pt2.Pt_State" },
	{ offsetof(IOView_t, X23_CPU_VC1_VC_Value), "DAIO.X23_CPU_VC1.VC_Value" },
	{ offsetof(IOView_t, X23_CPU_VC1_VC_State), "DAIO.X23_CPU_VC1.VC_State" },
	{ offsetof(IOView_t, X21_CPU_Pt1_Ctrl), "DAIO.X21_CPU_Pt1_Ctrl" },
	{ offsetof(IOView_t, X21_CPU_Pt2_Ctrl), "DAIO.X22_CPU_Pt2_Ctrl" },
	{ offsetof(IOView_t, X23_CPU_VC1_Ctrl), "DAIO.X23_CPU_VC1_Ctrl" },
};

static const AppTagBind_t kHmiViewBinds[] = {
	{ offsetof(HMIView_t, temp_setpoint), "hmi.temp_setpoint" },
	{ offsetof(HMIView_t, run), "hmi.run" },
	{ offsetof(HMIView_t, alarm_state), "hmi.alarm_state" },
	{ offsetof(HMIView_t, alarm_code), "hmi.alarm_code" },
	{ offsetof(HMIView_t, counter_test), "hmi.counter_test" },
};

static const AppTagBind_t kProcViewBinds[] = {
	{ offsetof(ProcView_t, counter_test), "proc.counter_test" },
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

const AppTagBind_t *app_config_get_io_view_binds(size_t *out_count)
{
	if (out_count != NULL)
	{
		*out_count = sizeof(kIoViewBinds) / sizeof(kIoViewBinds[0]);
	}

	return kIoViewBinds;
}

const AppTagBind_t *app_config_get_hmi_view_binds(size_t *out_count)
{
	if (out_count != NULL)
	{
		*out_count = sizeof(kHmiViewBinds) / sizeof(kHmiViewBinds[0]);
	}

	return kHmiViewBinds;
}

const AppTagBind_t *app_config_get_proc_view_binds(size_t *out_count)
{
	if (out_count != NULL)
	{
		*out_count = sizeof(kProcViewBinds) / sizeof(kProcViewBinds[0]);
	}

	return kProcViewBinds;
}
