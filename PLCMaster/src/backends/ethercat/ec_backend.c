/* EtherCAT backend implementation with double buffering. */

#include "ec_backend.h"

#include <stdlib.h>
#include <string.h>

static void free_buffers(EcBackend_t *ec)
{
\tint i;

\tif (ec->iomap != NULL) {
\t\tfree(ec->iomap);
\t\tec->iomap = NULL;
\t\tec->iomap_size = 0U;
\t}

\tfor (i = 0; i < 2; ++i) {
\t\tif (ec->in_buf[i] != NULL) {
\t\t\tfree(ec->in_buf[i]);
\t\t\tec->in_buf[i] = NULL;
\t\t}
\t}
\tec->in_size = 0U;

\tfor (i = 0; i < 2; ++i) {
\t\tif (ec->out_buf[i] != NULL) {
\t\t\tfree(ec->out_buf[i]);
\t\t\tec->out_buf[i] = NULL;
\t\t}
\t}
\tec->out_size = 0U;
}

int ec_backend_init(EcBackend_t *ec, const char *ifname, size_t iomap_size, bool dc_clock, uint32_t cycle_time_us)
{
\tif (ec == NULL || ifname == NULL || iomap_size == 0U) {
\t\treturn -1;
\t}

\tmemset(ec, 0, sizeof(*ec));

\tif (strlen(ifname) >= sizeof(ec->ifname)) {
\t\treturn -1;
\t}

\t(void)strncpy(ec->ifname, ifname, sizeof(ec->ifname) - 1U);
\tec->ifname[sizeof(ec->ifname) - 1U] = '\\0';
\tec->dc_clock = dc_clock;
\tec->cycle_time_us = cycle_time_us;
\tec->iomap_size = iomap_size;
\tec->in_size = iomap_size;
\tec->out_size = iomap_size;

\t/* allocate buffers */
\tec->iomap = (uint8_t *)malloc(ec->iomap_size);
\tec->in_buf[0] = (uint8_t *)malloc(ec->in_size);
\tec->in_buf[1] = (uint8_t *)malloc(ec->in_size);
\tec->out_buf[0] = (uint8_t *)malloc(ec->out_size);
\tec->out_buf[1] = (uint8_t *)malloc(ec->out_size);

\tif (ec->iomap == NULL || ec->in_buf[0] == NULL || ec->in_buf[1] == NULL || ec->out_buf[0] == NULL || ec->out_buf[1] == NULL) {
\t\tfree_buffers(ec);
\t\treturn -1;
\t}

\tmemset(ec->iomap, 0, ec->iomap_size);
\tmemset(ec->in_buf[0], 0, ec->in_size);
\tmemset(ec->in_buf[1], 0, ec->in_size);
\tmemset(ec->out_buf[0], 0, ec->out_size);
\tmemset(ec->out_buf[1], 0, ec->out_size);

\t/* atomics */
\tplat_atomic_store_i32(&ec->active_in_idx, 0);
\tplat_atomic_store_i32(&ec->active_out_idx, 0);
\tplat_atomic_store_i32(&ec->build_out_idx, 1);
\tplat_atomic_store_bool(&ec->out_dirty, false);

\t/* SOEM context init */
\tif (ecx_init(&ec->ctx, ec->ifname) == 0) {
\t\tfree_buffers(ec);
\t\tmemset(ec, 0, sizeof(*ec));
\t\treturn -1;
\t}

\treturn 0;
}

void ec_backend_deinit(EcBackend_t *ec)
{
\tif (ec == NULL) {
\t\treturn;
\t}

\tif (ec->ctx.port != NULL) {
\t\tecx_close(&ec->ctx);
\t}

\tfree_buffers(ec);
\tmemset(ec, 0, sizeof(*ec));
}

const uint8_t *ec_backend_get_in_ptr(const EcBackend_t *ec)
{
\tint idx;

\tif (ec == NULL) {
\t\treturn NULL;
\t}

\tidx = plat_atomic_load_i32(&ec->active_in_idx);
\tif (idx < 0 || idx > 1) {
\t\treturn NULL;
\t}

\treturn ec->in_buf[idx];
}

uint8_t *ec_backend_get_out_build_ptr(EcBackend_t *ec)
{
\tint idx;

\tif (ec == NULL) {
\t\treturn NULL;
\t}

\tidx = plat_atomic_load_i32(&ec->build_out_idx);
\tif (idx < 0 || idx > 1) {
\t\treturn NULL;
\t}

\treturn ec->out_buf[idx];
}

void ec_backend_out_commit(EcBackend_t *ec)
{
\tint build_idx;
\tint active_out;
\tint new_active;

\tif (ec == NULL) {
\t\treturn;
\t}

\tbuild_idx = plat_atomic_load_i32(&ec->build_out_idx);
\tactive_out = plat_atomic_load_i32(&ec->active_out_idx);

\tif (build_idx < 0 || build_idx > 1 || active_out < 0 || active_out > 1) {
\t\treturn;
\t}

\tnew_active = build_idx;
\tplat_atomic_store_i32(&ec->build_out_idx, active_out);
\tplat_atomic_store_i32(&ec->active_out_idx, new_active);
\tplat_atomic_store_bool(&ec->out_dirty, true);
}

void ec_backend_on_cycle_rx(EcBackend_t *ec, const uint8_t *src_iomap, size_t nbytes)
{
\tint current_in;
\tint next_in;
\tsize_t copy_sz;

\tif (ec == NULL || src_iomap == NULL) {
\t\treturn;
\t}

\tcurrent_in = plat_atomic_load_i32(&ec->active_in_idx);
\tif (current_in < 0 || current_in > 1) {
\t\tcurrent_in = 0;
\t\tplat_atomic_store_i32(&ec->active_in_idx, 0);
\t}
\tnext_in = 1 - current_in;
\tcopy_sz = nbytes < ec->in_size ? nbytes : ec->in_size;
\tmemcpy(ec->in_buf[next_in], src_iomap, copy_sz);
\tplat_atomic_store_i32(&ec->active_in_idx, next_in);
}

void ec_backend_on_cycle_tx(EcBackend_t *ec, uint8_t *dst_iomap, size_t nbytes)
{
\tint active_out;
\tsize_t copy_sz;

\tif (ec == NULL || dst_iomap == NULL) {
\t\treturn;
\t}

\tactive_out = plat_atomic_load_i32(&ec->active_out_idx);
\tif (active_out < 0 || active_out > 1) {
\t\tactive_out = 0;
\t\tplat_atomic_store_i32(&ec->active_out_idx, 0);
\t}

\tcopy_sz = nbytes < ec->out_size ? nbytes : ec->out_size;
\tmemcpy(dst_iomap, ec->out_buf[active_out], copy_sz);
}
