/* Minimal EtherCAT control API (SOEM-based). */

#ifndef ETHERCAT_MINIMAL_H
#define ETHERCAT_MINIMAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <soem/soem.h>

typedef struct EthercatHandle
{
	ecx_contextt ctx;
	uint8_t *iomap;
	size_t iomap_size;
	bool dc_clock;
	int slave_count;
	uint16_t expected_wkc;
	bool configured;
	bool opened;
} EthercatHandle;

int ethercat_open(EthercatHandle *h, const char *ifname, size_t iomap_size, bool dc_clock);
int ethercat_configure(EthercatHandle *h);
int ethercat_start_op(EthercatHandle *h);
void ethercat_close(EthercatHandle *h);

#endif /* ETHERCAT_MINIMAL_H */
