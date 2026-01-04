/* Minimal EtherCAT test harness (configure iface then SAFE-OP/OP). */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "backends/ethercat/ethercat_minimal.h"

static const char *k_ifname = "eth0"; /* TODO: adjust to your NIC name */
static const size_t k_iomap_size = 4096U;
static const bool k_use_dc = false;

int main(void)
{
	EthercatHandle h;
	int rc;

	memset(&h, 0, sizeof(h));

	rc = ethercat_open(&h, k_ifname, k_iomap_size, k_use_dc);
	if (rc != 0)
	{
		printf("ethercat_open failed for %s (see adapter list above)\n", k_ifname);
		return 1;
	}

	rc = ethercat_configure(&h);
	if (rc != 0)
	{
		printf("ethercat_configure failed\n");
		ethercat_close(&h);
		return 1;
	}

	rc = ethercat_start_op(&h);
	if (rc != 0)
	{
		printf("ethercat_start_op failed\n");
		ethercat_close(&h);
		return 1;
	}

	printf("EtherCAT SAFE-OP and OP transitions completed\n");
	ethercat_close(&h);
	return 0;
}
