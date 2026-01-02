/* Static configuration declarations. TODO: provide configuration structures and accessors. */

#ifndef CONFIG_STATIC_H
#define CONFIG_STATIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct BackendConfig
{
    const char *name;
    const char *ifname;     /* TODO: set interface name */
    uint32_t cycle_time_us;
    bool dc_clock;
} BackendConfig_t;

typedef struct DeviceConfig
{
    const char *name;
    const char *model;
    const char *backend;
    struct
    {
        uint16_t expected_position;
    } ethercat;
    uint16_t io_addr; /* Logical address for %I/%Q */
    struct
    {
        uint32_t vendor_id;    /* TODO: set vendor id */
        uint32_t product_code; /* TODO: set product code */
    } expected_identity;
} DeviceConfig_t;

typedef struct ProcessVarDesc
{
    uint8_t placeholder; /* TODO: describe process variables */
} ProcessVarDesc;

typedef struct HmiTagDesc
{
    uint8_t placeholder; /* TODO: describe HMI tags */
} HmiTagDesc;

typedef struct SystemConfig
{
    const BackendConfig_t *backends;
    size_t backend_count;
    const DeviceConfig_t *devices;
    size_t device_count;
    const ProcessVarDesc *process_vars;
    size_t process_var_count;
    const HmiTagDesc *hmi_tags;
    size_t hmi_tag_count;
} SystemConfig_t;

const SystemConfig_t *get_static_config(void);

#endif /* CONFIG_STATIC_H */
