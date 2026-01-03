/* Static configuration declarations. TODO: provide configuration structures and accessors. */

#ifndef CONFIG_STATIC_H
#define CONFIG_STATIC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "devices/device_desc.h"

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
    uint16_t io_addr; /* Stable logical address for %I/%Q (independent of EtherCAT position) */
    struct
    {
        uint32_t vendor_id;    /* TODO: set vendor id */
        uint32_t product_code; /* TODO: set product code */
    } expected_identity;
} DeviceConfig_t;

typedef struct ProcessVarDesc
{
	const char *name;          /* ex: "temp_sp" */
	TagType_t type;            /* BOOL/U32/REAL... */
	union
	{
		int i;
		uint32_t u32;
		float f;
		uint8_t u8;
		uint16_t u16;
		bool b;
	} initial;
} ProcessVarDesc_t;

typedef enum { HMI_RO = 0, HMI_RW = 1 } HmiAccess_t;
typedef struct HmiTagDesc
{
    const char *name;     /* ex: "temp_setpoint" */
    const char *alias_of; /* ex: "proc.temp_sp" */
    HmiAccess_t access;   /* RO/RW */
} HmiTagDesc;

typedef struct SystemConfig
{
    const BackendConfig_t *backends;
    size_t backend_count;
    const DeviceConfig_t *devices;
    size_t device_count;
	const ProcessVarDesc_t *process_vars;
    size_t process_var_count;
    const HmiTagDesc *hmi_tags;
    size_t hmi_tag_count;
} SystemConfig_t;

const SystemConfig_t *get_static_config(void);

#endif /* CONFIG_STATIC_H */
