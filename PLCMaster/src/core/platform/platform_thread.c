#include "platform_thread.h"

#include <stdlib.h>

#include "platform_time.h"

#if PLAT_WINDOWS
#include <process.h>
#include <mmsystem.h>

struct thread_start
{
	plat_thread_fn fn;
	void *arg;
	PlatThreadExParams_t params;
};

static int plat_thread_apply_priority_windows(HANDLE handle, PlatThreadPriority_t priority)
{
	int native_prio = THREAD_PRIORITY_NORMAL;

	if (priority == PLAT_THREAD_PRIORITY_LOW)
	{
		native_prio = THREAD_PRIORITY_BELOW_NORMAL;
	}
	else if (priority == PLAT_THREAD_PRIORITY_HIGH)
	{
		native_prio = THREAD_PRIORITY_HIGHEST;
	}
	else if (priority == PLAT_THREAD_PRIORITY_CRITICAL)
	{
		native_prio = THREAD_PRIORITY_TIME_CRITICAL;
	}

	return SetThreadPriority(handle, native_prio) ? 0 : -1;
}

static unsigned __stdcall plat_thread_entry(void *param)
{
	struct thread_start *ctx = (struct thread_start *)param;
	plat_thread_fn entry_fn = ctx->fn;
	void *entry_arg = ctx->arg;
	PlatThreadExParams_t params = ctx->params;
	int timer_res_acquired = 0;
	UINT timer_period = (UINT)params.timer_resolution_ms;

	if (timer_period > 0)
	{
		if ((timer_period == 1 && plat_time_begin_timer_resolution_1ms() == 0) ||
		    (timer_period != 1 && timeBeginPeriod(timer_period) == TIMERR_NOERROR))
		{
			timer_res_acquired = 1;
		}
	}

	if (params.affinity_cpu >= 0)
	{
		DWORD_PTR mask = ((DWORD_PTR)1) << params.affinity_cpu;
		SetThreadAffinityMask(GetCurrentThread(), mask);
	}

	plat_thread_apply_priority_windows(GetCurrentThread(), params.priority);

	free(ctx);
	if (entry_fn)
	{
		entry_fn(entry_arg);
	}

	if (timer_res_acquired)
	{
		if (timer_period == 1)
		{
			plat_time_end_timer_resolution_1ms();
		}
		else
		{
			timeEndPeriod(timer_period);
		}
	}

	return 0;
}
#elif PLAT_LINUX
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static int plat_thread_set_affinity(pthread_t thread, int cpu_index)
{
	if (cpu_index < 0)
	{
		return 0;
	}

	cpu_set_t cpu_set;
	CPU_ZERO(&cpu_set);
	CPU_SET((unsigned int)cpu_index, &cpu_set);

	return pthread_setaffinity_np(thread, sizeof(cpu_set), &cpu_set);
}

static int plat_thread_setup_sched(const PlatThreadExParams_t *params, pthread_attr_t *attr)
{
	if (params->cls != PLAT_THREAD_RT)
	{
		return 0;
	}

	int max_prio = sched_get_priority_max(SCHED_FIFO);
	int min_prio = sched_get_priority_min(SCHED_FIFO);
	if (max_prio == -1 || min_prio == -1)
	{
		fprintf(stderr, "plat_thread_create_ex: unable to query SCHED_FIFO range\n");
		return -1;
	}

	int desired_prio = max_prio;
	if (params->priority == PLAT_THREAD_PRIORITY_LOW)
	{
		desired_prio = min_prio;
	}
	else if (params->priority == PLAT_THREAD_PRIORITY_NORMAL)
	{
		desired_prio = min_prio + (max_prio - min_prio) / 2;
	}
	else if (params->priority == PLAT_THREAD_PRIORITY_HIGH && max_prio > min_prio)
	{
		desired_prio = max_prio - 1;
	}

	struct sched_param sp;
	memset(&sp, 0, sizeof(sp));
	sp.sched_priority = desired_prio;

	if (pthread_attr_setschedpolicy(attr, SCHED_FIFO) != 0)
	{
		fprintf(stderr, "plat_thread_create: pthread_attr_setschedpolicy failed\n");
		return -1;
	}
	if (pthread_attr_setschedparam(attr, &sp) != 0)
	{
		fprintf(stderr, "plat_thread_create: pthread_attr_setschedparam failed\n");
		return -1;
	}
	if (pthread_attr_setinheritsched(attr, PTHREAD_EXPLICIT_SCHED) != 0)
	{
		fprintf(stderr, "plat_thread_create: pthread_attr_setinheritsched failed\n");
		return -1;
	}

	return 0;
}
#endif

static void plat_thread_ex_defaults(PlatThreadExParams_t *params)
{
	params->cls = PLAT_THREAD_NORMAL;
	params->priority = PLAT_THREAD_PRIORITY_NORMAL;
	params->affinity_cpu = -1;
	params->timer_resolution_ms = 0;
	params->stack_size = 0;
}

int plat_thread_create_ex(plat_thread_t *t, plat_thread_fn fn, void *arg, const PlatThreadExParams_t *params)
{
#if PLAT_WINDOWS
	PlatThreadExParams_t local_params;
	if (params)
	{
		local_params = *params;
	}
	else
	{
		plat_thread_ex_defaults(&local_params);
	}

	struct thread_start *ctx = (struct thread_start *)malloc(sizeof(*ctx));
	if (!ctx)
	{
		return -1;
	}

	ctx->fn = fn;
	ctx->arg = arg;
	ctx->params = local_params;

	unsigned thread_id = 0;
	unsigned init_flags = 0;

	t->handle = (HANDLE)_beginthreadex(NULL, (unsigned)local_params.stack_size, plat_thread_entry, ctx, init_flags, &thread_id);
	if (t->handle == NULL)
	{
		free(ctx);
		return -1;
	}

	t->tid = (DWORD)thread_id;

	return 0;
#elif PLAT_LINUX
	PlatThreadExParams_t local_params;
	if (params)
	{
		local_params = *params;
	}
	else
	{
		plat_thread_ex_defaults(&local_params);
	}

	pthread_attr_t attr;
	int have_attr = 0;
	if (pthread_attr_init(&attr) == 0)
	{
		have_attr = 1;
		if (local_params.stack_size > 0)
		{
			pthread_attr_setstacksize(&attr, local_params.stack_size);
		}

		if (plat_thread_setup_sched(&local_params, &attr) != 0)
		{
			/* continue with default scheduling */
		}
	}

	int rc = pthread_create(&t->thread, have_attr ? &attr : NULL, fn, arg);

	if (have_attr)
	{
		pthread_attr_destroy(&attr);
	}

	if (rc != 0)
	{
		return rc;
	}

	if (local_params.affinity_cpu >= 0)
	{
		int aff_rc = plat_thread_set_affinity(t->thread, local_params.affinity_cpu);
		if (aff_rc != 0)
		{
			fprintf(stderr, "plat_thread_create_ex: pthread_setaffinity_np failed (%d)\n", aff_rc);
		}
	}

	return 0;
#else
	PLAT_UNUSED(t);
	PLAT_UNUSED(params);
	PLAT_UNUSED(fn);
	PLAT_UNUSED(arg);
	return -1;
#endif
}

int plat_thread_create(plat_thread_t *t, PlatThreadClass_t cls, const PlatThreadRtParams_t *rt, plat_thread_fn fn, void *arg)
{
	PlatThreadExParams_t params;
	plat_thread_ex_defaults(&params);
	params.cls = cls;

	if (rt)
	{
		params.affinity_cpu = rt->affinity_cpu;
		params.timer_resolution_ms = (uint32_t)((rt->timer_res_ms > 0) ? rt->timer_res_ms : 0);
		params.stack_size = rt->stack_size;

		if (rt->priority_level == 1)
		{
			params.priority = PLAT_THREAD_PRIORITY_HIGH;
		}
		else if (rt->priority_level >= 2)
		{
			params.priority = PLAT_THREAD_PRIORITY_CRITICAL;
		}
	}

	return plat_thread_create_ex(t, fn, arg, &params);
}

int plat_thread_join(plat_thread_t *t)
{
#if PLAT_WINDOWS
	DWORD rc = WaitForSingleObject(t->handle, INFINITE);
	CloseHandle(t->handle);
	return rc == WAIT_OBJECT_0 ? 0 : -1;
#elif PLAT_LINUX
	return pthread_join(t->thread, NULL);
#else
	PLAT_UNUSED(t);
	return -1;
#endif
}

void plat_thread_sleep_ms(uint32_t ms)
{
#if PLAT_WINDOWS
	Sleep(ms);
#elif PLAT_LINUX
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (long)(ms % 1000) * 1000000L;
	while (nanosleep(&ts, &ts) == -1 && errno == EINTR)
	{
	}
#else
	PLAT_UNUSED(ms);
#endif
}

void plat_thread_yield(void)
{
#if PLAT_WINDOWS
	SwitchToThread();
#elif PLAT_LINUX
	sched_yield();
#endif
}
