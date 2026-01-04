/* EtherCAT backend implementation with double buffering and SOEM lifecycle. */

#include "ec_backend.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "core/platform/platform_thread.h"
#include "core/platform/platform_time.h"
#include "ec_soem_lifecycle.h"

#define EC_MAX_IFNAME_LEN 63
#define EC_DEFAULT_PERIOD_NS 1000000ULL
#define EC_DEFAULT_SPIN_THRESHOLD_NS 50000U

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

	ec_soem_close(&impl->ctx);

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

    (void)snprintf(impl->ifname, sizeof(impl->ifname), "%s", cfg->ifname);
	{
		uint64_t period_ns = cfg->period_ns != 0U ? cfg->period_ns : ((cfg->cycle_time_us != 0U ? (uint64_t)cfg->cycle_time_us * 1000ULL : EC_DEFAULT_PERIOD_NS));
		uint32_t spin_threshold_ns = cfg->spin_threshold_ns != 0U ? cfg->spin_threshold_ns : EC_DEFAULT_SPIN_THRESHOLD_NS;
		PlatThreadPriority_t rt_priority = cfg->rt_priority;
		int rt_affinity = cfg->rt_affinity_cpu;
		uint32_t timer_res_ms = cfg->rt_timer_resolution_ms != 0U ? cfg->rt_timer_resolution_ms : 1U;

		if (rt_priority < PLAT_THREAD_PRIORITY_LOW || rt_priority > PLAT_THREAD_PRIORITY_CRITICAL)
		{
			rt_priority = PLAT_THREAD_PRIORITY_HIGH;
		}

		if (rt_affinity < 0 && cfg->rt_reserved_cpu >= 0)
		{
			rt_affinity = cfg->rt_reserved_cpu;
		}
		if (rt_affinity < 0)
		{
			rt_affinity = -1;
		}

		impl->period_ns = period_ns;
		impl->cycle_time_us = cfg->cycle_time_us != 0U ? cfg->cycle_time_us : (uint32_t)(period_ns / 1000ULL);
		impl->rt_cycle_us = (uint32_t)(period_ns / 1000ULL);
		impl->spin_threshold_ns = spin_threshold_ns;
		impl->rt_params.cls = PLAT_THREAD_RT;
		impl->rt_params.priority = rt_priority;
		impl->rt_params.affinity_cpu = rt_affinity;
		impl->rt_params.timer_resolution_ms = timer_res_ms;
		impl->rt_params.stack_size = 0U;
	}
	impl->dc_clock = cfg->dc_clock;
	impl->iomap_size = iomap_size;
	impl->perf_freq = 0U;
	plat_atomic_store_bool(&impl->in_op, false);

	if (ec_soem_context_init(&impl->ctx, impl->ifname) != 0)
	{
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

	slave_count = ec_soem_scan_slaves(&impl->ctx);
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
	size_t i;

	if (impl == NULL || impl->iomap == NULL)
	{
		return -1;
	}

	plat_thread_sleep_ms(1000);

	if (ec_soem_configure_iomap_group0(&impl->ctx, impl->iomap, impl->iomap_size, &impl->expected_wkc) <= 0)
	{
		return -1;
	}

	for (i = 0; i < impl->device_count; ++i)
	{
		EthercatDevice_t *dev = &impl->devices[i];
		ec_slavet *sl;
		uint32_t expected_vendor;
		uint32_t expected_product;

		if (dev->slave_index == 0 || dev->slave_index > impl->ctx.slavecount)
		{
			goto error;
		}

		sl = &impl->ctx.slavelist[dev->slave_index];
		dev->soem_inputs = sl->inputs;
		dev->soem_outputs = sl->outputs;

		expected_vendor = dev->cfg->expected_identity.vendor_id != 0 ? dev->cfg->expected_identity.vendor_id : dev->desc->vendor_id;
		expected_product = dev->cfg->expected_identity.product_code != 0 ? dev->cfg->expected_identity.product_code : dev->desc->product_code;
		if (expected_vendor != 0 && sl->eep_man != expected_vendor)
		{
			goto error;
		}
		if (expected_product != 0 && sl->eep_id != expected_product)
		{
			goto error;
		}

		if (sl->Ibytes < dev->desc->tx_bytes || sl->Obytes < dev->desc->rx_bytes)
		{
			goto error;
		}

		dev->in_size = dev->desc->tx_bytes;
		dev->out_size = dev->desc->rx_bytes;
		dev->in_buffers[0] = (uint8_t *)calloc(1, dev->in_size);
		dev->in_buffers[1] = (uint8_t *)calloc(1, dev->in_size);
		dev->out_buffers[0] = (uint8_t *)calloc(1, dev->out_size);
		dev->out_buffers[1] = (uint8_t *)calloc(1, dev->out_size);
		if (dev->in_buffers[0] == NULL || dev->in_buffers[1] == NULL || dev->out_buffers[0] == NULL || dev->out_buffers[1] == NULL)
		{
			goto error;
		}
	}

	plat_atomic_store_i32(&impl->active_in_buffer_idx, 0);
	plat_atomic_store_i32(&impl->active_out_buffer_idx, 0);
	plat_atomic_store_i32(&impl->rt_out_buffer_idx, 0);
	plat_atomic_store_i32(&impl->last_wkc, 0);
	plat_atomic_store_i32(&impl->fault_latched, 0);
	plat_atomic_store_i32(&impl->ec_state, EC_STATE_NONE);
	plat_atomic_store_bool(&impl->rt_should_run, false);
	plat_atomic_store_bool(&impl->rt_is_running, false);
	plat_atomic_store_u64(&impl->rt_cycle_count, 0ULL);
	plat_atomic_store_u64(&impl->rt_overruns, 0ULL);
	plat_atomic_store_i64(&impl->rt_jitter_min_ns, INT64_MAX);
	plat_atomic_store_i64(&impl->rt_jitter_max_ns, INT64_MIN);
	impl->rt_cycle_us = (uint32_t)(impl->period_ns / 1000ULL);

	if (impl->dc_clock)
	{
		ecx_configdc(&impl->ctx);
	}

	if (ethercat_transition_safeop(impl) != 0)
	{
		goto error;
	}

	plat_atomic_store_i32(&impl->ec_state, EC_STATE_SAFE_OP);
	return 0;

error:
	for (i = 0; i < impl->device_count; ++i)
	{
		free_device_buffers(&impl->devices[i]);
	}
	return -1;
}

static void ethercat_copy_out_to_iomap(EthercatDriver_t *impl, int out_idx, bool zero_outputs)
{
	size_t i;

	for (i = 0; i < impl->device_count; ++i)
	{
		EthercatDevice_t *dev = &impl->devices[i];
		if (dev->soem_outputs != NULL && dev->out_size > 0U)
		{
			if (zero_outputs || dev->out_buffers[out_idx] == NULL)
			{
				memset(dev->soem_outputs, 0, dev->out_size);
			}
			else
			{
				memcpy(dev->soem_outputs, dev->out_buffers[out_idx], dev->out_size);
			}
		}
	}
}

static void ethercat_copy_iomap_to_in(EthercatDriver_t *impl, int next_in_idx)
{
	size_t i;

	for (i = 0; i < impl->device_count; ++i)
	{
		EthercatDevice_t *dev = &impl->devices[i];
		if (dev->soem_inputs != NULL && dev->in_buffers[next_in_idx] != NULL && dev->in_size > 0U)
		{
			memcpy(dev->in_buffers[next_in_idx], dev->soem_inputs, dev->in_size);
		}
	}
}

static void ethercat_zero_output_buffers(EthercatDriver_t *impl)
{
	size_t i;
	size_t b;

	for (i = 0; i < impl->device_count; ++i)
	{
		EthercatDevice_t *dev = &impl->devices[i];
		if (dev->out_size == 0U)
		{
			continue;
		}
		for (b = 0; b < 2; ++b)
		{
			if (dev->out_buffers[b] != NULL)
			{
				memset(dev->out_buffers[b], 0, dev->out_size);
			}
		}
		if (dev->soem_outputs != NULL)
		{
			memset(dev->soem_outputs, 0, dev->out_size);
		}
	}
}

static void *ethercat_rt_thread(void *arg)
{
	EthercatDriver_t *impl = (EthercatDriver_t *)arg;
	int expected_wkc = (int)impl->expected_wkc;
	uint64_t deadline = plat_time_monotonic_ns() + impl->period_ns;

	plat_atomic_store_bool(&impl->rt_is_running, true);
	printf("[EC] RT thread start period=%u us expected WKC=%u affinity=%d spin=%u ns\n",
		impl->rt_cycle_us,
		(unsigned)impl->expected_wkc,
		impl->rt_params.affinity_cpu,
		(unsigned)impl->spin_threshold_ns);

	while (plat_atomic_load_bool(&impl->rt_should_run))
	{
		int out_idx;
		int cur_in;
		int next_in;
		int wkc;
		uint64_t now_ns;
		int64_t jitter;
		bool zero_outputs;

		out_idx = plat_atomic_load_i32(&impl->rt_out_buffer_idx);
		if (out_idx < 0 || out_idx > 1)
		{
			out_idx = 0;
			plat_atomic_store_i32(&impl->rt_out_buffer_idx, 0);
		}

		ecx_send_processdata(&impl->ctx);
		wkc = ecx_receive_processdata(&impl->ctx, EC_TIMEOUTRET);
		plat_atomic_store_i32(&impl->last_wkc, wkc);

		cur_in = plat_atomic_load_i32(&impl->active_in_buffer_idx);
		if (cur_in < 0 || cur_in > 1)
		{
			cur_in = 0;
			plat_atomic_store_i32(&impl->active_in_buffer_idx, 0);
		}
		next_in = 1 - cur_in;

		ethercat_copy_iomap_to_in(impl, next_in);
		plat_atomic_store_i32(&impl->active_in_buffer_idx, next_in);

		zero_outputs = plat_atomic_load_i32(&impl->fault_latched) != 0;
		if ((expected_wkc > 0 && wkc < expected_wkc) || plat_atomic_load_i32(&impl->ec_state) != EC_STATE_OPERATIONAL)
		{
			if (plat_atomic_exchange_i32(&impl->fault_latched, 1) == 0)
			{
				printf("[EC] Fault latched: WKC below expected (%d < %d) or state left OP\n", wkc, expected_wkc);
			}
			plat_atomic_store_bool(&impl->in_op, false);
			zero_outputs = true;
			ethercat_zero_output_buffers(impl);
		}

		ethercat_copy_out_to_iomap(impl, out_idx, zero_outputs);

		now_ns = plat_time_monotonic_ns();
		jitter = (int64_t)(now_ns - deadline);
		if (jitter < plat_atomic_load_i64(&impl->rt_jitter_min_ns))
		{
			plat_atomic_store_i64(&impl->rt_jitter_min_ns, jitter);
		}
		if (jitter > plat_atomic_load_i64(&impl->rt_jitter_max_ns))
		{
			plat_atomic_store_i64(&impl->rt_jitter_max_ns, jitter);
		}
		if (jitter > 0)
		{
			(void)plat_atomic_fetch_add_u64(&impl->rt_overruns, 1ULL);
		}

		if (now_ns > deadline + (impl->period_ns * 5ULL))
		{
			deadline = now_ns + impl->period_ns;
		}

		plat_time_sleep_until_ns(deadline, impl->spin_threshold_ns);
		deadline += impl->period_ns;
		(void)plat_atomic_fetch_add_u64(&impl->rt_cycle_count, 1ULL);
	}

	ethercat_zero_output_buffers(impl);
	ethercat_copy_out_to_iomap(impl, 0, true);
	ecx_send_processdata(&impl->ctx);
	(void)ecx_receive_processdata(&impl->ctx, EC_TIMEOUTRET);
	plat_atomic_store_bool(&impl->rt_is_running, false);
	return NULL;
}

static int ethercat_start(BackendDriver_t *driver)
{
	EthercatDriver_t *impl = get_impl(driver);
	int i;
	int wkc = 0;

	if (impl == NULL || impl->iomap == NULL)
	{
		return -1;
	}

	memset(impl->iomap, 0, impl->iomap_size);
	ethercat_zero_output_buffers(impl);

	for (i = 0; i < 3; ++i)
	{
		ecx_send_processdata(&impl->ctx);
		wkc = ecx_receive_processdata(&impl->ctx, EC_TIMEOUTRET);
	}
	plat_atomic_store_i32(&impl->last_wkc, wkc);

	plat_thread_sleep_ms(2);

	if (ec_soem_request_op(&impl->ctx, impl->expected_wkc) != 0)
	{
		return -1;
	}

	plat_atomic_store_i32(&impl->ec_state, EC_STATE_OPERATIONAL);
	if (wkc < (int)impl->expected_wkc)
	{
		printf("Warning: WKC %d below expected %u during startup\n", wkc, (unsigned)impl->expected_wkc);
	}

	plat_atomic_store_bool(&impl->in_op, true);
	plat_atomic_store_i32(&impl->fault_latched, 0);
	plat_atomic_store_bool(&impl->rt_should_run, true);
	plat_atomic_store_bool(&impl->rt_is_running, false);
	plat_atomic_store_u64(&impl->rt_cycle_count, 0ULL);
	plat_atomic_store_u64(&impl->rt_overruns, 0ULL);
	plat_atomic_store_i64(&impl->rt_jitter_min_ns, INT64_MAX);
	plat_atomic_store_i64(&impl->rt_jitter_max_ns, INT64_MIN);
	if (plat_thread_create_ex(&impl->rt_thread, ethercat_rt_thread, impl, &impl->rt_params) != 0)
	{
		plat_atomic_store_bool(&impl->in_op, false);
		plat_atomic_store_bool(&impl->rt_should_run, false);
		return -1;
	}
	return 0;
}

static int ethercat_stop(BackendDriver_t *driver)
{
	EthercatDriver_t *impl = get_impl(driver);

	if (impl == NULL)
	{
		return -1;
	}

	plat_atomic_store_bool(&impl->rt_should_run, false);
	if (plat_atomic_load_bool(&impl->rt_is_running))
	{
		plat_thread_join(&impl->rt_thread);
	}

	ethercat_zero_output_buffers(impl);
	ethercat_copy_out_to_iomap(impl, 0, true);
	ecx_send_processdata(&impl->ctx);
	(void)ecx_receive_processdata(&impl->ctx, EC_TIMEOUTRET);

	if (ec_soem_request_safe_op(&impl->ctx) != 0)
	{
		plat_atomic_store_bool(&impl->in_op, false);
		plat_atomic_store_i32(&impl->ec_state, EC_STATE_NONE);
		return -1;
	}
	plat_atomic_store_bool(&impl->in_op, false);
	plat_atomic_store_i32(&impl->ec_state, EC_STATE_SAFE_OP);
	return 0;
}

static int ethercat_process(BackendDriver_t *driver)
{
	/* Phase B: realtime thread handles process data. TODO: mailbox/CoE service thread. */
	PLAT_UNUSED(driver);
	return 0;
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
	/* Legacy helper retained for compatibility; RT thread now handles IO. */
	PLAT_UNUSED(driver);
	return 0;
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
