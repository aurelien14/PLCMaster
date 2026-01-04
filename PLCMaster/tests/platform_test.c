#include <inttypes.h>
#include <stdio.h>

#include "core/platform/platform_atomic.h"
#include "core/platform/platform_thread.h"
#include "core/platform/platform_time.h"

typedef struct
{
	int stop;
} rt_thread_ctx_t;

static void run_sleep_jitter_test(void)
{
	const uint32_t iterations = 1000;
	const uint64_t period_ns = 1000000ULL;
	const uint32_t spin_threshold_ns = 50000;

	int64_t min_err = INT64_MAX;
	int64_t max_err = INT64_MIN;
	int64_t sum_err = 0;

	uint64_t deadline = plat_time_now_ns();
	if (deadline == 0)
	{
		printf("plat_time_now_ns unavailable\n");
		return;
	}
	deadline += period_ns;

	for (uint32_t i = 0; i < iterations; ++i)
	{
		plat_time_sleep_until_ns(deadline, spin_threshold_ns);

		uint64_t now = plat_time_now_ns();
		if (now == 0)
		{
			printf("plat_time_now_ns failed\n");
			return;
		}

		int64_t err = (int64_t)(now - deadline);
		if (err < min_err)
		{
			min_err = err;
		}
		if (err > max_err)
		{
			max_err = err;
		}
		sum_err += err;

		deadline += period_ns;
	}

	printf("sleep_until jitter (ns): min=%" PRId64 " max=%" PRId64 " avg=%" PRId64 " (iterations=%u)\n",
	       min_err, max_err, sum_err / (int64_t)iterations, iterations);
}

static void *rt_thread_fn(void *arg)
{
	rt_thread_ctx_t *ctx = (rt_thread_ctx_t *)arg;
	uint64_t start = plat_time_monotonic_ms();
	printf("rt thread started\n");
	while (!ctx->stop)
	{
		uint64_t now = plat_time_monotonic_ms();
		if ((now - start) >= 1000)
		{
			printf("rt thread alive ms=%" PRIu64 "\n", now - start);
			start = now;
		}
		plat_thread_yield();
	}
	return NULL;
}

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

	PlatThreadRtParams_t rt_params;
	rt_params.priority_level = 1;
	rt_params.affinity_cpu = 0;
	rt_params.timer_res_ms = 1;
	rt_params.stack_size = 0;

	rt_thread_ctx_t ctx;
	ctx.stop = 0;
	plat_thread_t t;
	if (plat_thread_create(&t, PLAT_THREAD_RT, &rt_params, rt_thread_fn, &ctx) == 0)
	{
		plat_thread_sleep_ms(1200);
		ctx.stop = 1;
		plat_thread_join(&t);
	}
	else
	{
		printf("Failed to start RT thread\n");
	}

	run_sleep_jitter_test();

	return 0;
}
