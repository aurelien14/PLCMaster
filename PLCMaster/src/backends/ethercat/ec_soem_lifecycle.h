/* SOEM lifecycle helpers shared by EtherCAT backend. */

#ifndef EC_SOEM_LIFECYCLE_H
#define EC_SOEM_LIFECYCLE_H

#include <stddef.h>
#include <stdint.h>

#include <soem/soem.h>

void ec_soem_print_available_adapters(void);
int ec_soem_context_init(ecx_contextt *ctx, const char *ifname);
int ec_soem_scan_slaves(ecx_contextt *ctx);
int ec_soem_configure_iomap_group0(ecx_contextt *ctx, uint8_t *iomap, size_t iomap_size, uint16_t *expected_wkc_out);
int ec_soem_request_safe_op(ecx_contextt *ctx);
int ec_soem_request_op(ecx_contextt *ctx, uint16_t expected_wkc);
void ec_soem_close(ecx_contextt *ctx);

#endif /* EC_SOEM_LIFECYCLE_H */
