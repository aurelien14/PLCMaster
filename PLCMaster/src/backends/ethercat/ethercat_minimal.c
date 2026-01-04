/* Minimal EtherCAT helper that follows the strict SOEM init/OP sequence. */

#include "ethercat_minimal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core/platform/platform_thread.h"

static void print_available_adapters(void)
{
	ec_adaptert *adapter_list = ec_find_adapters();
	ec_adaptert *cur = adapter_list;

	printf("Available adapters:\n");
	if (cur == NULL)
	{
		printf("  (none found)\n");
	}

	while (cur != NULL)
	{
		const char *name = (cur->name != NULL) ? cur->name : "(null)";
		const char *desc = (cur->desc != NULL) ? cur->desc : "";
		printf("  - %s (%s)\n", name, desc);
		cur = cur->next;
	}

	ec_free_adapters(adapter_list);
}

static void reset_handle(EthercatHandle *h)
{
	if (h == NULL)
	{
		return;
	}

	free(h->iomap);
	h->iomap = NULL;
	h->iomap_size = 0U;
	h->dc_clock = false;
	h->slave_count = 0;
	h->expected_wkc = 0U;
	h->configured = false;
	h->opened = false;
	memset(&h->ctx, 0, sizeof(h->ctx));
}

int ethercat_open(EthercatHandle *h, const char *ifname, size_t iomap_size, bool dc_clock)
{
	if (h == NULL || ifname == NULL || iomap_size == 0U)
	{
		return -1;
	}

	reset_handle(h);

	h->iomap_size = iomap_size;
	h->dc_clock = dc_clock;

	printf("[EtherCAT] INIT on interface '%s'\n", ifname);
	if (ecx_init(&h->ctx, ifname) == 0)
	{
		print_available_adapters();
		reset_handle(h);
		return -1;
	}

	h->opened = true;
	return 0;
}

int ethercat_configure(EthercatHandle *h)
{
	int iomap_used;
	int rc = -1;

	if (h == NULL || !h->opened)
	{
		return -1;
	}

	printf("[EtherCAT] SCAN slaves...\n");
	h->slave_count = ecx_config_init(&h->ctx);
	if (h->slave_count <= 0)
	{
		return -1;
	}
	printf("[EtherCAT] Found %d slave(s)\n", h->slave_count);

	h->iomap = (uint8_t *)calloc(1, h->iomap_size);
	if (h->iomap == NULL)
	{
		goto cleanup;
	}

	printf("[EtherCAT] MAP process data (iomap=%zu bytes)\n", h->iomap_size);
	iomap_used = ecx_config_map_group(&h->ctx, h->iomap, 0);
	if (iomap_used <= 0 || (size_t)iomap_used > h->iomap_size)
	{
		goto cleanup;
	}
	printf("[EtherCAT] MAP size=%d bytes\n", iomap_used);

	h->expected_wkc = (uint16_t)((h->ctx.grouplist[0].outputsWKC * 2) + h->ctx.grouplist[0].inputsWKC);

	if (h->dc_clock)
	{
		printf("[EtherCAT] CONFIGURE DC clock\n");
		ecx_configdc(&h->ctx);
	}

	printf("[EtherCAT] Transition to SAFE-OP...\n");
	h->ctx.slavelist[0].state = EC_STATE_SAFE_OP;
	ecx_writestate(&h->ctx, 0);

	for (int attempt = 0; attempt < 10; ++attempt)
	{
		int state = ecx_statecheck(&h->ctx, 0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE);
		if (state == EC_STATE_SAFE_OP)
		{
			printf("[EtherCAT] SAFE-OP reached\n");
			h->configured = true;
			rc = 0;
			goto cleanup;
		}
		plat_thread_sleep_ms(10);
	}

cleanup:
	if (rc != 0)
	{
		free(h->iomap);
		h->iomap = NULL;
		h->expected_wkc = 0U;
		h->slave_count = 0;
	}

	return rc;
}

int ethercat_start_op(EthercatHandle *h)
{
	int state = 0;

	if (h == NULL || !h->configured || h->iomap == NULL)
	{
		return -1;
	}

	printf("[EtherCAT] Zeroing outputs before OP...\n");
	memset(h->iomap, 0, h->iomap_size);

	for (int i = 0; i < 3; ++i)
	{
		ecx_send_processdata(&h->ctx);
		(void)ecx_receive_processdata(&h->ctx, EC_TIMEOUTRET);
		plat_thread_sleep_ms(1);
	}

	printf("[EtherCAT] Request OPERATIONAL...\n");
	h->ctx.slavelist[0].state = EC_STATE_OPERATIONAL;
	ecx_writestate(&h->ctx, 0);

	for (int attempt = 0; attempt < 20; ++attempt)
	{
		int wkc;

		ecx_send_processdata(&h->ctx);
		wkc = ecx_receive_processdata(&h->ctx, EC_TIMEOUTRET);
		state = ecx_statecheck(&h->ctx, 0, EC_STATE_OPERATIONAL, EC_TIMEOUTSTATE);
		if (state == EC_STATE_OPERATIONAL)
		{
			printf("[EtherCAT] OPERATIONAL reached (WKC=%d expected=%u)\n", wkc, (unsigned)h->expected_wkc);
			if (h->expected_wkc != 0U && wkc < (int)h->expected_wkc)
			{
				printf("[EtherCAT] Warning: WKC below expected during start\n");
			}
			/* TODO: add realtime thread cycle + watchdog + mailbox/CoE handling */
			return 0;
		}
		plat_thread_sleep_ms(5);
	}

	printf("[EtherCAT] Failed to reach OPERATIONAL (last state=%d)\n", state);
	return -1;
}

void ethercat_close(EthercatHandle *h)
{
	if (h == NULL)
	{
		return;
	}

	if (h->opened)
	{
		printf("[EtherCAT] Closing master\n");
		h->ctx.slavelist[0].state = EC_STATE_INIT;
		ecx_writestate(&h->ctx, 0);
		ecx_close(&h->ctx);
	}

	reset_handle(h);
}
