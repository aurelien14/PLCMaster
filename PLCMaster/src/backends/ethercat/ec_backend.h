/* EtherCAT backend declarations. */

#ifndef EC_BACKEND_H
#define EC_BACKEND_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "core/platform/platform_atomic.h"
#include "ethercat_main.h"

typedef struct EcBackend
{
\tecx_contextt ctx;
\tchar ifname[64];
\tbool dc_clock;
\tuint32_t cycle_time_us;
\tuint8_t *iomap;
\tsize_t iomap_size;
\tuint8_t *in_buf[2];
\tsize_t in_size;
\tuint8_t *out_buf[2];
\tsize_t out_size;
\tplat_atomic_i32_t active_in_idx;
\tplat_atomic_i32_t active_out_idx;
\tplat_atomic_i32_t build_out_idx;
\tplat_atomic_bool_t out_dirty;
} EcBackend_t;

int ec_backend_init(EcBackend_t *ec, const char *ifname, size_t iomap_size, bool dc_clock, uint32_t cycle_time_us);
void ec_backend_deinit(EcBackend_t *ec);

const uint8_t *ec_backend_get_in_ptr(const EcBackend_t *ec);
uint8_t *ec_backend_get_out_build_ptr(EcBackend_t *ec);

void ec_backend_out_commit(EcBackend_t *ec);

void ec_backend_on_cycle_rx(EcBackend_t *ec, const uint8_t *src_iomap, size_t nbytes);
void ec_backend_on_cycle_tx(EcBackend_t *ec, uint8_t *dst_iomap, size_t nbytes);

#endif /* EC_BACKEND_H */
