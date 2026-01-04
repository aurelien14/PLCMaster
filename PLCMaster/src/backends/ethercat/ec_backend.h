/* EtherCAT backend declarations. */

#ifndef EC_BACKEND_H
#define EC_BACKEND_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "backends/backend_iface.h"
#include "core/platform/platform_atomic.h"
#include <soem/soem.h>
#include "system/config/config_static.h"

typedef struct EthercatDevice
{
    const DeviceConfig_t *cfg;
    const DeviceDesc_t *desc;
    uint16_t slave_index;
    uint8_t *soem_inputs;
    uint8_t *soem_outputs;
    uint8_t *in_buffers[2];
    uint8_t *out_buffers[2];
    uint32_t in_size;
    uint32_t out_size;
} EthercatDevice_t;

typedef struct EthercatDriver
{
    BackendDriver_t base;
    ecx_contextt ctx;
    char ifname[64];
    bool dc_clock;
    uint32_t cycle_time_us;
    uint8_t *iomap;
    size_t iomap_size;
    uint16_t expected_wkc;
    EthercatDevice_t *devices;
    size_t device_count;
    size_t device_capacity;
    uint64_t perf_freq;
    plat_atomic_i32_t active_in_buffer_idx;
    plat_atomic_i32_t active_out_buffer_idx;
    plat_atomic_i32_t rt_out_buffer_idx;
    plat_atomic_bool_t in_op;
} EthercatDriver_t;

BackendDriver_t *ethercat_backend_create(const BackendConfig_t *cfg, size_t iomap_size, size_t max_devices, size_t backend_index);
void ethercat_backend_destroy(BackendDriver_t *driver);

int ethercat_cycle_once(BackendDriver_t *driver);
void print_available_adapters(void);

#endif /* EC_BACKEND_H */
