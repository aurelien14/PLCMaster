#include "platform_time.h"

#if PLAT_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN /* Exclude some conflicting definitions in windows header */
#endif
#include <windows.h>
#include <mmsystem.h>
#elif PLAT_LINUX
#include <errno.h>
#include <sched.h>
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

static uint64_t plat_time_counter_to_ns(uint64_t counter, uint64_t freq)
{
	if (freq == 0)
	{
		return 0;
	}

	uint64_t seconds = counter / freq;
	uint64_t remainder = counter % freq;

	return seconds * 1000000000ULL + (remainder * 1000000000ULL) / freq;
}

uint64_t plat_time_now_ns(void)
{
#if PLAT_WINDOWS
	return plat_time_counter_to_ns(plat_query_counter(), plat_query_freq());
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

uint64_t plat_time_monotonic_ns(void)
{
	return plat_time_now_ns();
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

void plat_time_sleep_until_ns(uint64_t deadline_ns, uint32_t spin_threshold_ns)
{
	const uint64_t sleep_threshold_ns = 1000000ULL;

	for (;;)
	{
		uint64_t now = plat_time_now_ns();
		if (now == 0 || now >= deadline_ns)
		{
			break;
		}

		uint64_t remaining = deadline_ns - now;

		if (remaining > sleep_threshold_ns)
		{
#if PLAT_WINDOWS
			DWORD sleep_ms = (DWORD)((remaining - sleep_threshold_ns) / 1000000ULL);
			if (sleep_ms == 0)
			{
				sleep_ms = 1;
			}
			Sleep(sleep_ms);
#elif PLAT_LINUX
			struct timespec ts;
			ts.tv_sec = (time_t)(remaining / 1000000000ULL);
			ts.tv_nsec = (long)(remaining % 1000000000ULL);
			if (ts.tv_nsec < 1000000L)
			{
				ts.tv_nsec = 1000000L;
			}
			while (nanosleep(&ts, &ts) == -1 && errno == EINTR)
			{
			}
#else
			break;
#endif
			continue;
		}

		if (spin_threshold_ns > 0 && remaining <= spin_threshold_ns)
		{
			while (1)
			{
				now = plat_time_now_ns();
				if (now == 0 || now >= deadline_ns)
				{
					break;
				}
			}
			break;
		}

#if PLAT_WINDOWS
		SwitchToThread();
#elif PLAT_LINUX
		sched_yield();
#else
		break;
#endif
	}
}

void plat_time_busy_wait_ns(uint64_t duration_ns)
{
	uint64_t start = plat_time_now_ns();
	if (start == 0 || duration_ns == 0)
	{
		return;
	}

	while (1)
	{
		uint64_t now = plat_time_now_ns();
		if (now == 0)
		{
			break;
		}
		if ((now - start) >= duration_ns)
		{
			break;
		}
	}
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
