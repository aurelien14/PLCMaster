/* Static configuration stub implementation. TODO: load and manage static configuration. */

#include "config_static.h"

static const BackendConfig_t kBackendConfigs[] = {
    {
        .name = "ec0",
        .ifname = "TODO_IFNAME",
        .cycle_time_us = 1000,
        .dc_clock = true,
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
                .vendor_id = 0,    /* TODO: set vendor id */
                .product_code = 0, /* TODO: set product code */
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
};

static const SystemConfig_t kStaticConfig = {
    .backends = kBackendConfigs,
    .backend_count = sizeof(kBackendConfigs) / sizeof(kBackendConfigs[0]),
    .devices = kDeviceConfigs,
    .device_count = sizeof(kDeviceConfigs) / sizeof(kDeviceConfigs[0]),
    .process_vars = kProcessVarDescs,
    .process_var_count = sizeof(kProcessVarDescs) / sizeof(kProcessVarDescs[0]),
    .hmi_tags = NULL,
    .hmi_tag_count = 0,
};

const SystemConfig_t *get_static_config(void)
{
    return &kStaticConfig;
}
