#include "platform_thread.h"

#include <stdlib.h>

#if PLAT_WINDOWS
struct thread_start
{
	plat_thread_fn fn;
	void *arg;
};

static DWORD WINAPI plat_thread_entry(LPVOID param)
{
	struct thread_start *ctx = (struct thread_start *)param;
	plat_thread_fn entry_fn = ctx->fn;
	void *entry_arg = ctx->arg;
	free(ctx);
	if (entry_fn)
	{
		entry_fn(entry_arg);
	}
	return 0;
}
#elif PLAT_LINUX
#include <errno.h>
#include <sched.h>
#include <time.h>
#endif

int plat_thread_create(plat_thread_t *t, plat_thread_fn fn, void *arg, const char *name)
{
#if PLAT_WINDOWS
	struct thread_start *ctx = (struct thread_start *)malloc(sizeof(*ctx));
	if (!ctx)
	{
		return -1;
	}

	ctx->fn = fn;
	ctx->arg = arg;

	t->handle = CreateThread(NULL, 0, plat_thread_entry, ctx, 0, &t->tid);
	if (t->handle == NULL)
	{
		free(ctx);
		return -1;
	}

	PLAT_UNUSED(name);
	return 0;
#elif PLAT_LINUX
	int rc = pthread_create(&t->thread, NULL, fn, arg);
	if (rc != 0)
	{
		return rc;
	}

	PLAT_UNUSED(name);
	return 0;
#else
	PLAT_UNUSED(t);
	PLAT_UNUSED(fn);
	PLAT_UNUSED(arg);
	PLAT_UNUSED(name);
	return -1;
#endif
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
