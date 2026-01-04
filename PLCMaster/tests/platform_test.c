#include <inttypes.h>
#include <stdio.h>

#include "core/platform/platform_atomic.h"
#include "core/platform/platform_thread.h"
#include "core/platform/platform_time.h"

int main(void)
{
	printf("PLAT_WINDOWS=%d PLAT_LINUX=%d PLAT_64BIT=%d\n", PLAT_WINDOWS, PLAT_LINUX, PLAT_64BIT);

	uint64_t freq = plat_time_perf_freq();
	uint64_t start_ticks = plat_time_perf_counter();
	uint64_t start_ms = plat_time_monotonic_ms();

	plat_thread_sleep_ms(10);

	uint64_t end_ticks = plat_time_perf_counter();
	uint64_t end_ms = plat_time_monotonic_ms();
	uint64_t delta_ms = end_ms - start_ms;

	printf("perf_freq=%" PRIu64 " perf_counter_start=%" PRIu64 " perf_counter_end=%" PRIu64 "\n", freq, start_ticks, end_ticks);
	printf("sleep_delta_ms=%" PRIu64 "\n", delta_ms);

	plat_atomic_i32_t x = PLAT_ATOMIC_I32_INIT(0);
	plat_atomic_store_i32(&x, 5);
	int32_t v = plat_atomic_load_i32(&x);
	int32_t old = plat_atomic_exchange_i32(&x, 9);
	int32_t add = plat_atomic_fetch_add_i32(&x, 3);

	printf("atomic_i32 load=%" PRId32 " exchange_old=%" PRId32 " fetch_add_prev=%" PRId32 " final=%" PRId32 "\n", v, old, add, plat_atomic_load_i32(&x));

	return 0;
}
