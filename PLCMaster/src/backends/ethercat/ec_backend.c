/* EtherCAT backend implementation with double buffering and SOEM lifecycle. */

#include "ec_backend.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/platform/platform_thread.h"
#include "core/platform/platform_time.h"

#define EC_MAX_IFNAME_LEN 63

static BackendDriverOps_t g_ec_ops;

static EthercatDriver_t *get_impl(BackendDriver_t *driver)
{
    if (driver == NULL) {
        return NULL;
    }
    return (EthercatDriver_t *)driver->impl;
}

static EthercatDevice_t *find_device(EthercatDriver_t *impl, const DeviceConfig_t *cfg)
{
    size_t i;

    if (impl == NULL || cfg == NULL) {
        return NULL;
    }

    for (i = 0; i < impl->device_count; ++i) {
        if (impl->devices[i].cfg == cfg) {
            return &impl->devices[i];
        }
        if (impl->devices[i].cfg != NULL && impl->devices[i].cfg->name != NULL && cfg->name != NULL) {
            if (strcmp(impl->devices[i].cfg->name, cfg->name) == 0) {
                return &impl->devices[i];
            }
        }
    }

    return NULL;
}

static void free_device_buffers(EthercatDevice_t *dev)
{
    size_t i;

    if (dev == NULL) {
        return;
    }

    for (i = 0; i < 2; ++i) {
        free(dev->in_buffers[i]);
        free(dev->out_buffers[i]);
        dev->in_buffers[i] = NULL;
        dev->out_buffers[i] = NULL;
    }
    dev->in_size = 0U;
    dev->out_size = 0U;
}

static void free_driver_resources(EthercatDriver_t *impl)
{
    size_t i;

    if (impl == NULL) {
        return;
    }

    ecx_close(&impl->ctx);

    free(impl->iomap);
    impl->iomap = NULL;
    impl->iomap_size = 0U;

    if (impl->devices != NULL) {
        for (i = 0; i < impl->device_capacity; ++i) {
            free_device_buffers(&impl->devices[i]);
        }
        free(impl->devices);
        impl->devices = NULL;
        impl->device_capacity = 0U;
        impl->device_count = 0U;
    }
}

void print_available_adapters(void)
{
    ec_adaptert *adapter_list = ec_find_adapters();
    ec_adaptert *cur = adapter_list;

    printf("Available Ethernet adapters:\n");
    if (cur == NULL) {
        printf("  (none found)\n");
    }

    while (cur != NULL) {
        const char *name = (cur->name != NULL) ? cur->name : "(null)";
        const char *desc = (cur->desc != NULL) ? cur->desc : "";
        printf("  - %s : %s\n", name, desc);
        cur = cur->next;
    }

    ec_free_adapters(adapter_list);
}

static int ethercat_transition_safeop(EthercatDriver_t *impl)
{
    int state;

    impl->ctx.slavelist[0].state = EC_STATE_SAFE_OP;
    ecx_writestate(&impl->ctx, 0);
    state = ecx_statecheck(&impl->ctx, 0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE * 4);
    return (state == EC_STATE_SAFE_OP) ? 0 : -1;
}

static int ethercat_transition_op(EthercatDriver_t *impl)
{
    int state;

    impl->ctx.slavelist[0].state = EC_STATE_OPERATIONAL;
    ecx_writestate(&impl->ctx, 0);
    state = ecx_statecheck(&impl->ctx, 0, EC_STATE_OPERATIONAL, EC_TIMEOUTSTATE * 4);
    return (state == EC_STATE_OPERATIONAL) ? 0 : -1;
}

BackendDriver_t *ethercat_backend_create(const BackendConfig_t *cfg, size_t iomap_size, size_t max_devices, size_t backend_index)
{
    EthercatDriver_t *impl;

    if (cfg == NULL || cfg->ifname == NULL) {
        return NULL;
    }

    impl = (EthercatDriver_t *)calloc(1, sizeof(*impl));
    if (impl == NULL) {
        return NULL;
    }

    impl->devices = (EthercatDevice_t *)calloc(max_devices, sizeof(EthercatDevice_t));
    if (impl->devices == NULL) {
        free(impl);
        return NULL;
    }
    impl->device_capacity = max_devices;

    impl->base.type = BACKEND_TYPE_ETHERCAT;
    impl->base.ops = &g_ec_ops;
    impl->base.impl = impl;
    if (cfg->name != NULL) {
        (void)snprintf(impl->base.name, sizeof(impl->base.name), "%s", cfg->name);
    }
    (void)snprintf(impl->base.system_name, sizeof(impl->base.system_name), "ec%zu", backend_index);

    if (strlen(cfg->ifname) > EC_MAX_IFNAME_LEN) {
        free_driver_resources(impl);
        free(impl);
        return NULL;
    }

    (void)strncpy_s(impl->ifname, sizeof(impl->ifname) - 1U, cfg->ifname, sizeof(impl->ifname) - 1U);
    impl->ifname[sizeof(impl->ifname) - 1U] = '\0';
    impl->cycle_time_us = cfg->cycle_time_us;
    impl->dc_clock = cfg->dc_clock;
    impl->iomap_size = iomap_size;
    impl->perf_freq = 0U;
    plat_atomic_store_bool(&impl->in_op, false);

    if (ecx_init(&impl->ctx, impl->ifname) == 0) {
        print_available_adapters();
        free_driver_resources(impl);
        free(impl);
        return NULL;
    }

    return &impl->base;
}

void ethercat_backend_destroy(BackendDriver_t *driver)
{
    EthercatDriver_t *impl = get_impl(driver);
    if (impl == NULL) {
        return;
    }

    free_driver_resources(impl);
    memset(impl, 0, sizeof(*impl));
    free(impl);
}

static int ethercat_init(BackendDriver_t *driver)
{
    EthercatDriver_t *impl = get_impl(driver);
    int slave_count;

    if (impl == NULL) {
        return -1;
    }

    impl->perf_freq = plat_time_perf_freq();

    slave_count = ecx_config_init(&impl->ctx);
    if (slave_count <= 0) {
        return -1;
    }

    impl->iomap_size = impl->iomap_size != 0U ? impl->iomap_size : 4096U;
    impl->iomap = (uint8_t *)calloc(1, impl->iomap_size);
    if (impl->iomap == NULL) {
        return -1;
    }

    printf("SOEM: %d slaves found on %s\n", slave_count, impl->ifname);
    return 0;
}

static int ethercat_bind_device(BackendDriver_t *driver, const DeviceConfig_t *cfg, const DeviceDesc_t *desc)
{
    EthercatDriver_t *impl = get_impl(driver);
    EthercatDevice_t *dev;
    uint16_t pos;
    size_t i;

    if (impl == NULL || cfg == NULL || desc == NULL) {
        return -1;
    }

    if (impl->device_count >= impl->device_capacity) {
        return -1;
    }

    pos = cfg->ethercat.expected_position;
    if (pos == 0 || (impl->ctx.slavecount > 0 && pos > impl->ctx.slavecount)) {
        return -1;
    }

    for (i = 0; i < impl->device_count; ++i) {
        if (impl->devices[i].slave_index == pos) {
            return -1;
        }
    }

    dev = &impl->devices[impl->device_count++];
    memset(dev, 0, sizeof(*dev));
    dev->cfg = cfg;
    dev->desc = desc;
    dev->slave_index = pos;
    return 0;
}

static int ethercat_finalize_mapping(BackendDriver_t *driver)
{
    EthercatDriver_t *impl = get_impl(driver);
    int iomap_used;
    size_t i;

    if (impl == NULL || impl->iomap == NULL) {
        return -1;
    }

    plat_thread_sleep_ms(1000);

    iomap_used = ecx_config_map_group(&impl->ctx, impl->iomap, 0);
    if (iomap_used <= 0 || (size_t)iomap_used > impl->iomap_size) {
        return -1;
    }

    impl->expected_wkc = (uint16_t)((impl->ctx.grouplist[0].outputsWKC * 2) + impl->ctx.grouplist[0].inputsWKC);

    for (i = 0; i < impl->device_count; ++i) {
        EthercatDevice_t *dev = &impl->devices[i];
        ec_slavet *sl;
        uint32_t expected_vendor;
        uint32_t expected_product;

        if (dev->slave_index == 0 || dev->slave_index > impl->ctx.slavecount) {
            return -1;
        }

        sl = &impl->ctx.slavelist[dev->slave_index];
        dev->soem_inputs = sl->inputs;
        dev->soem_outputs = sl->outputs;

        expected_vendor = dev->cfg->expected_identity.vendor_id != 0 ? dev->cfg->expected_identity.vendor_id : dev->desc->vendor_id;
        expected_product = dev->cfg->expected_identity.product_code != 0 ? dev->cfg->expected_identity.product_code : dev->desc->product_code;
        if (expected_vendor != 0 && sl->eep_man != expected_vendor) {
            return -1;
        }
        if (expected_product != 0 && sl->eep_id != expected_product) {
            return -1;
        }

        if (sl->Ibytes < dev->desc->tx_bytes || sl->Obytes < dev->desc->rx_bytes) {
            return -1;
        }

        dev->in_size = dev->desc->tx_bytes;
        dev->out_size = dev->desc->rx_bytes;
        dev->in_buffers[0] = (uint8_t *)calloc(1, dev->in_size);
        dev->in_buffers[1] = (uint8_t *)calloc(1, dev->in_size);
        dev->out_buffers[0] = (uint8_t *)calloc(1, dev->out_size);
        dev->out_buffers[1] = (uint8_t *)calloc(1, dev->out_size);
        if (dev->in_buffers[0] == NULL || dev->in_buffers[1] == NULL || dev->out_buffers[0] == NULL || dev->out_buffers[1] == NULL) {
            return -1;
        }
    }

    plat_atomic_store_i32(&impl->active_in_buffer_idx, 0);
    plat_atomic_store_i32(&impl->active_out_buffer_idx, 0);
    plat_atomic_store_i32(&impl->rt_out_buffer_idx, 0);

    if (impl->dc_clock) {
        ecx_configdc(&impl->ctx);
    }

    if (ethercat_transition_safeop(impl) != 0) {
        return -1;
    }

    return 0;
}

static int ethercat_start(BackendDriver_t *driver)
{
    EthercatDriver_t *impl = get_impl(driver);
    int i;
    int wkc = 0;

    if (impl == NULL || impl->iomap == NULL) {
        return -1;
    }

    memset(impl->iomap, 0, impl->iomap_size);

    for (i = 0; i < 3; ++i) {
        ecx_send_processdata(&impl->ctx);
        wkc = ecx_receive_processdata(&impl->ctx, (int)impl->cycle_time_us);
    }

    plat_thread_sleep_ms(2);

    if (ethercat_transition_op(impl) != 0) {
        return -1;
    }

    if (wkc < (int)impl->expected_wkc) {
        printf("Warning: WKC %d below expected %u during startup\n", wkc, (unsigned)impl->expected_wkc);
    }

    plat_atomic_store_bool(&impl->in_op, true);
    /* TODO Phase B: start EtherCAT loop thread + watchdog */
    return 0;
}

static int ethercat_stop(BackendDriver_t *driver)
{
    EthercatDriver_t *impl = get_impl(driver);

    if (impl == NULL) {
        return -1;
    }

    impl->ctx.slavelist[0].state = EC_STATE_SAFE_OP;
    ecx_writestate(&impl->ctx, 0);
    (void)ecx_statecheck(&impl->ctx, 0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE * 4);
    plat_atomic_store_bool(&impl->in_op, false);
    return 0;
}

static int ethercat_process(BackendDriver_t *driver)
{
    /* Phase A: no dedicated thread. Optional single cycle helper. */
    return ethercat_cycle_once(driver);
}

static void ethercat_sync_buffers(BackendDriver_t *driver)
{
    EthercatDriver_t *impl = get_impl(driver);
    int cur_out;
    int next_out;
    size_t i;

    if (impl == NULL) {
        return;
    }

    cur_out = plat_atomic_load_i32(&impl->active_out_buffer_idx);
    if (cur_out < 0 || cur_out > 1) {
        cur_out = 0;
        plat_atomic_store_i32(&impl->active_out_buffer_idx, 0);
    }

    next_out = 1 - cur_out;
    plat_atomic_store_i32(&impl->rt_out_buffer_idx, cur_out);

    for (i = 0; i < impl->device_count; ++i) {
        EthercatDevice_t *dev = &impl->devices[i];
        if (dev->out_buffers[cur_out] != NULL && dev->out_buffers[next_out] != NULL && dev->out_size > 0U) {
            memcpy(dev->out_buffers[next_out], dev->out_buffers[cur_out], dev->out_size);
        }
    }

    plat_atomic_store_i32(&impl->active_out_buffer_idx, next_out);
}

static const uint8_t *ethercat_get_input_data(BackendDriver_t *driver, const DeviceConfig_t *cfg, size_t *size_out)
{
    EthercatDriver_t *impl = get_impl(driver);
    EthercatDevice_t *dev;
    int idx;

    if (size_out != NULL) {
        *size_out = 0U;
    }

    if (impl == NULL) {
        return NULL;
    }

    dev = find_device(impl, cfg);
    if (dev == NULL) {
        return NULL;
    }

    idx = plat_atomic_load_i32(&impl->active_in_buffer_idx);
    if (idx < 0 || idx > 1) {
        idx = 0;
        plat_atomic_store_i32(&impl->active_in_buffer_idx, 0);
    }

    if (size_out != NULL) {
        *size_out = dev->in_size;
    }
    return dev->in_buffers[idx];
}

static uint8_t *ethercat_get_output_data(BackendDriver_t *driver, const DeviceConfig_t *cfg, size_t *size_out)
{
    EthercatDriver_t *impl = get_impl(driver);
    EthercatDevice_t *dev;
    int idx;

    if (size_out != NULL) {
        *size_out = 0U;
    }

    if (impl == NULL) {
        return NULL;
    }

    dev = find_device(impl, cfg);
    if (dev == NULL) {
        return NULL;
    }

    idx = plat_atomic_load_i32(&impl->active_out_buffer_idx);
    if (idx < 0 || idx > 1) {
        idx = 0;
        plat_atomic_store_i32(&impl->active_out_buffer_idx, 0);
    }

    if (size_out != NULL) {
        *size_out = dev->out_size;
    }
    return dev->out_buffers[idx];
}

int ethercat_cycle_once(BackendDriver_t *driver)
{
    EthercatDriver_t *impl = get_impl(driver);
    int wkc = -1;
    int out_idx;
    int cur_in;
    int next_in;
    size_t i;

    if (impl == NULL) {
        return -1;
    }

    out_idx = plat_atomic_load_i32(&impl->rt_out_buffer_idx);
    if (out_idx < 0 || out_idx > 1) {
        out_idx = 0;
        plat_atomic_store_i32(&impl->rt_out_buffer_idx, 0);
    }

    for (i = 0; i < impl->device_count; ++i) {
        EthercatDevice_t *dev = &impl->devices[i];
        if (dev->soem_outputs != NULL && dev->out_buffers[out_idx] != NULL && dev->out_size > 0U) {
            memcpy(dev->soem_outputs, dev->out_buffers[out_idx], dev->out_size);
        }
    }

    ecx_send_processdata(&impl->ctx);
    wkc = ecx_receive_processdata(&impl->ctx, (int)impl->cycle_time_us);

    cur_in = plat_atomic_load_i32(&impl->active_in_buffer_idx);
    if (cur_in < 0 || cur_in > 1) {
        cur_in = 0;
        plat_atomic_store_i32(&impl->active_in_buffer_idx, 0);
    }
    next_in = 1 - cur_in;

    for (i = 0; i < impl->device_count; ++i) {
        EthercatDevice_t *dev = &impl->devices[i];
        if (dev->soem_inputs != NULL && dev->in_buffers[next_in] != NULL && dev->in_size > 0U) {
            memcpy(dev->in_buffers[next_in], dev->soem_inputs, dev->in_size);
        }
    }

    plat_atomic_store_i32(&impl->active_in_buffer_idx, next_in);
    return wkc;
}

static BackendDriverOps_t g_ec_ops = {
    .init = ethercat_init,
    .bind = ethercat_bind_device,
    .finalize = ethercat_finalize_mapping,
    .start = ethercat_start,
    .stop = ethercat_stop,
    .process = ethercat_process,
    .sync = ethercat_sync_buffers,
    .get_input_data = ethercat_get_input_data,
    .get_output_data = ethercat_get_output_data,
};
