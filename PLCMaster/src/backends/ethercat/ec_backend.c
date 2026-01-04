/* EtherCAT backend implementation with double buffering. */

#include "ec_backend.h"

#include <stdlib.h>
#include <string.h>

static void free_buffers(EcBackend_t *ec)
{
    int i;

    if (ec->iomap != NULL) {
        free(ec->iomap);
        ec->iomap = NULL;
        ec->iomap_size = 0U;
    }

    for (i = 0; i < 2; ++i) {
        if (ec->in_buf[i] != NULL) {
            free(ec->in_buf[i]);
            ec->in_buf[i] = NULL;
        }
    }
    ec->in_size = 0U;

    for (i = 0; i < 2; ++i) {
        if (ec->out_buf[i] != NULL) {
            free(ec->out_buf[i]);
            ec->out_buf[i] = NULL;
        }
    }
    ec->out_size = 0U;
}

int ec_backend_init(EcBackend_t *ec, const char *ifname, size_t iomap_size, bool dc_clock, uint32_t cycle_time_us)
{
    if (ec == NULL || ifname == NULL || iomap_size == 0U) {
        return -1;
    }

    memset(ec, 0, sizeof(*ec));

    if (strlen(ifname) >= sizeof(ec->ifname)) {
        return -1;
    }

    (void)strncpy_s(ec->ifname, sizeof(ec->ifname) - 1U, ifname, sizeof(ec->ifname) - 1U);
    ec->ifname[sizeof(ec->ifname) - 1U] = '\0';
    ec->dc_clock = dc_clock;
    ec->cycle_time_us = cycle_time_us;
    ec->iomap_size = iomap_size;
    ec->in_size = iomap_size;
    ec->out_size = iomap_size;

    /* allocate buffers */
    ec->iomap = (uint8_t *)malloc(ec->iomap_size);
    ec->in_buf[0] = (uint8_t *)malloc(ec->in_size);
    ec->in_buf[1] = (uint8_t *)malloc(ec->in_size);
    ec->out_buf[0] = (uint8_t *)malloc(ec->out_size);
    ec->out_buf[1] = (uint8_t *)malloc(ec->out_size);

    if (ec->iomap == NULL || ec->in_buf[0] == NULL || ec->in_buf[1] == NULL || ec->out_buf[0] == NULL || ec->out_buf[1] == NULL) {
        free_buffers(ec);
        return -1;
    }

    memset(ec->iomap, 0, ec->iomap_size);
    memset(ec->in_buf[0], 0, ec->in_size);
    memset(ec->in_buf[1], 0, ec->in_size);
    memset(ec->out_buf[0], 0, ec->out_size);
    memset(ec->out_buf[1], 0, ec->out_size);

    /* atomics */
    plat_atomic_store_i32(&ec->active_in_idx, 0);
    plat_atomic_store_i32(&ec->active_out_idx, 0);
    plat_atomic_store_i32(&ec->build_out_idx, 1);
    plat_atomic_store_bool(&ec->out_dirty, false);

    /* SOEM context init */
    if (ecx_init(&ec->ctx, ec->ifname) == 0) {
        free_buffers(ec);
        memset(ec, 0, sizeof(*ec));
        return -1;
    }

    return 0;
}

void ec_backend_deinit(EcBackend_t *ec)
{
    if (ec == NULL) {
        return;
    }
    
    ecx_close(&ec->ctx);

    free_buffers(ec);
    memset(ec, 0, sizeof(*ec));
}

const uint8_t *ec_backend_get_in_ptr(const EcBackend_t *ec)
{
    int idx;

    if (ec == NULL) {
        return NULL;
    }

    idx = plat_atomic_load_i32(&ec->active_in_idx);
    if (idx < 0 || idx > 1) {
        return NULL;
    }

    return ec->in_buf[idx];
}

uint8_t *ec_backend_get_out_build_ptr(EcBackend_t *ec)
{
    int idx;

    if (ec == NULL) {
        return NULL;
    }

    idx = plat_atomic_load_i32(&ec->build_out_idx);
    if (idx < 0 || idx > 1) {
        return NULL;
    }

    return ec->out_buf[idx];
}

void ec_backend_out_commit(EcBackend_t *ec)
{
    int build_idx;
    int active_out;
    int new_active;

    if (ec == NULL) {
        return;
    }

    build_idx = plat_atomic_load_i32(&ec->build_out_idx);
    active_out = plat_atomic_load_i32(&ec->active_out_idx);

    if (build_idx < 0 || build_idx > 1 || active_out < 0 || active_out > 1) {
        return;
    }

    new_active = build_idx;
    plat_atomic_store_i32(&ec->build_out_idx, active_out);
    plat_atomic_store_i32(&ec->active_out_idx, new_active);
    plat_atomic_store_bool(&ec->out_dirty, true);
}

void ec_backend_on_cycle_rx(EcBackend_t *ec, const uint8_t *src_iomap, size_t nbytes)
{
    int current_in;
    int next_in;
    size_t copy_sz;

    if (ec == NULL || src_iomap == NULL) {
        return;
    }

    current_in = plat_atomic_load_i32(&ec->active_in_idx);
    if (current_in < 0 || current_in > 1) {
        current_in = 0;
        plat_atomic_store_i32(&ec->active_in_idx, 0);
    }
    next_in = 1 - current_in;
    copy_sz = nbytes < ec->in_size ? nbytes : ec->in_size;
    memcpy(ec->in_buf[next_in], src_iomap, copy_sz);
    plat_atomic_store_i32(&ec->active_in_idx, next_in);
}

void ec_backend_on_cycle_tx(EcBackend_t *ec, uint8_t *dst_iomap, size_t nbytes)
{
    int active_out;
    size_t copy_sz;

    if (ec == NULL || dst_iomap == NULL) {
        return;
    }

    active_out = plat_atomic_load_i32(&ec->active_out_idx);
    if (active_out < 0 || active_out > 1) {
        active_out = 0;
        plat_atomic_store_i32(&ec->active_out_idx, 0);
    }

    copy_sz = nbytes < ec->out_size ? nbytes : ec->out_size;
    memcpy(dst_iomap, ec->out_buf[active_out], copy_sz);
}
