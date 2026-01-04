/* SOEM lifecycle helpers shared by EtherCAT backend. */

#include "ec_soem_lifecycle.h"

#include <stdio.h>
#include <string.h>

#include "core/platform/platform_thread.h"

void ec_soem_print_available_adapters(void)
{
	ec_adaptert *adapter_list = ec_find_adapters();
	ec_adaptert *cur = adapter_list;

	printf("Available Ethernet adapters:\n");
	if (cur == NULL)
	{
		printf("  (none found)\n");
	}

	while (cur != NULL)
	{
		const char *name = (cur->name != NULL) ? cur->name : "(null)";
		const char *desc = (cur->desc != NULL) ? cur->desc : "";
		printf("  - %s : %s\n", name, desc);
		cur = cur->next;
	}

	ec_free_adapters(adapter_list);
}

int ec_soem_context_init(ecx_contextt *ctx, const char *ifname)
{
	if (ctx == NULL || ifname == NULL)
	{
		return -1;
	}

	if (ecx_init(ctx, ifname) == 0)
	{
		ec_soem_print_available_adapters();
		return -1;
	}

	return 0;
}

int ec_soem_scan_slaves(ecx_contextt *ctx)
{
	if (ctx == NULL)
	{
		return -1;
	}

	return ecx_config_init(ctx);
}

int ec_soem_configure_iomap_group0(ecx_contextt *ctx, uint8_t *iomap, size_t iomap_size, uint16_t *expected_wkc_out)
{
	int iomap_used;

	if (ctx == NULL || iomap == NULL || iomap_size == 0U)
	{
		return -1;
	}

	iomap_used = ecx_config_map_group(ctx, iomap, 0);
	if (iomap_used <= 0 || (size_t)iomap_used > iomap_size)
	{
		return -1;
	}

	if (expected_wkc_out != NULL)
	{
		*expected_wkc_out = (uint16_t)((ctx->grouplist[0].outputsWKC * 2) + ctx->grouplist[0].inputsWKC);
	}

	return iomap_used;
}

int ec_soem_request_safe_op(ecx_contextt *ctx)
{
	int attempt;

	if (ctx == NULL)
	{
		return -1;
	}

	ctx->slavelist[0].state = EC_STATE_SAFE_OP;
	ecx_writestate(ctx, 0);

	for (attempt = 0; attempt < 10; ++attempt)
	{
		int state = ecx_statecheck(ctx, 0, EC_STATE_SAFE_OP, EC_TIMEOUTSTATE);
		if (state == EC_STATE_SAFE_OP)
		{
			return 0;
		}
		plat_thread_sleep_ms(10);
	}

	return -1;
}

int ec_soem_request_op(ecx_contextt *ctx, uint16_t expected_wkc)
{
	int attempt;

	if (ctx == NULL)
	{
		return -1;
	}

	ctx->slavelist[0].state = EC_STATE_OPERATIONAL;
	ecx_writestate(ctx, 0);

	for (attempt = 0; attempt < 20; ++attempt)
	{
		int wkc;
		int state;

		ecx_send_processdata(ctx);
		wkc = ecx_receive_processdata(ctx, EC_TIMEOUTRET);
		state = ecx_statecheck(ctx, 0, EC_STATE_OPERATIONAL, EC_TIMEOUTSTATE);
		if (state == EC_STATE_OPERATIONAL)
		{
			if (expected_wkc != 0U && wkc < (int)expected_wkc)
			{
				printf("[EC] Warning: WKC below expected during OP start (wkc=%d expected=%u)\n", wkc, (unsigned)expected_wkc);
			}
			return 0;
		}
		plat_thread_sleep_ms(5);
	}

	return -1;
}

void ec_soem_close(ecx_contextt *ctx)
{
	if (ctx == NULL)
	{
		return;
	}

	if (ctx->slavelist != NULL)
	{
		ctx->slavelist[0].state = EC_STATE_INIT;
		ecx_writestate(ctx, 0);
	}
	ecx_close(ctx);
	memset(ctx, 0, sizeof(*ctx));
}
