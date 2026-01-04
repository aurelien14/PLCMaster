#include "platform_time.h"

#if PLAT_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN /* Exclude some conflicting definitions in windows header */
#endif
#include <windows.h>
#include <mmsystem.h>
#elif PLAT_LINUX
#include <time.h>
#endif

#if PLAT_WINDOWS
static uint64_t plat_query_freq(void)
{
	static uint64_t cached_freq = 0;
	if (cached_freq == 0)
	{
		LARGE_INTEGER freq;
		if (QueryPerformanceFrequency(&freq))
		{
			cached_freq = (uint64_t)freq.QuadPart;
		}
	}
	return cached_freq;
}

static uint64_t plat_query_counter(void)
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return (uint64_t)counter.QuadPart;
}
#elif PLAT_LINUX
static uint64_t plat_timespec_ns(const struct timespec *ts)
{
	return (uint64_t)ts->tv_sec * 1000000000ULL + (uint64_t)ts->tv_nsec;
}
#endif

uint64_t plat_time_monotonic_ns(void)
{
#if PLAT_WINDOWS
	uint64_t freq = plat_query_freq();
	uint64_t ticks = plat_query_counter();
	if (freq == 0)
	{
		return 0;
	}
	return (ticks * 1000000000ULL) / freq;
#elif PLAT_LINUX
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0)
	{
		return 0;
	}
	return plat_timespec_ns(&ts);
#else
	return 0;
#endif
}

uint64_t plat_time_monotonic_us(void)
{
	return plat_time_monotonic_ns() / 1000ULL;
}

uint64_t plat_time_monotonic_ms(void)
{
	return plat_time_monotonic_ns() / 1000000ULL;
}

uint64_t plat_time_perf_counter(void)
{
#if PLAT_WINDOWS
	return plat_query_counter();
#elif PLAT_LINUX
	return plat_time_monotonic_ns();
#else
	return 0;
#endif
}

uint64_t plat_time_perf_freq(void)
{
#if PLAT_WINDOWS
	return plat_query_freq();
#elif PLAT_LINUX
	return 1000000000ULL;
#else
	return 0;
#endif
}

#if PLAT_WINDOWS
static LONG g_timer_res_refcount = 0;

int plat_time_begin_timer_resolution_1ms(void)
{
	LONG prev = InterlockedIncrement(&g_timer_res_refcount);
	if (prev == 1)
	{
		if (timeBeginPeriod(1) != TIMERR_NOERROR)
		{
			InterlockedDecrement(&g_timer_res_refcount);
			return -1;
		}
	}

	return 0;
}

int plat_time_end_timer_resolution_1ms(void)
{
	LONG prev = InterlockedDecrement(&g_timer_res_refcount);
	if (prev == 0)
	{
		timeEndPeriod(1);
	}

	return 0;
}
#else
int plat_time_begin_timer_resolution_1ms(void)
{
	return 0;
}

int plat_time_end_timer_resolution_1ms(void)
{
	return 0;
}
#endif
