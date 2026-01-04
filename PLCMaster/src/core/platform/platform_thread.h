#ifndef PLATFORM_THREAD_H
#define PLATFORM_THREAD_H

#include <stddef.h>
#include <stdint.h>

#include "platform_detect.h"

#if PLAT_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN /* Exclude some conflicting definitions in windows header */
#endif
#include <windows.h>
#elif PLAT_LINUX
#include <pthread.h>
#endif

typedef void *(*plat_thread_fn)(void *arg);

typedef enum
{
	PLAT_THREAD_NORMAL = 0,
	PLAT_THREAD_RT = 1
} PlatThreadClass_t;

typedef enum
{
	PLAT_THREAD_PRIORITY_LOW = -1,
	PLAT_THREAD_PRIORITY_NORMAL = 0,
	PLAT_THREAD_PRIORITY_HIGH = 1,
	PLAT_THREAD_PRIORITY_CRITICAL = 2
} PlatThreadPriority_t;

typedef struct
{
	int priority_level; /* 0=default, 1=high, 2=time_critical */
	int affinity_cpu;   /* -1 = none, otherwise CPU index */
	int timer_res_ms;   /* 0=none, 1=request 1ms (Windows only) */
	size_t stack_size;  /* 0=default */
} PlatThreadRtParams_t;

typedef struct
{
	PlatThreadClass_t cls;
	PlatThreadPriority_t priority;
	int affinity_cpu;          /* -1 = none, otherwise CPU index */
	uint32_t timer_resolution_ms;
	size_t stack_size;         /* 0=default */
} PlatThreadExParams_t;

typedef struct plat_thread
{
#if PLAT_WINDOWS
	HANDLE handle;
	DWORD tid;
#elif PLAT_LINUX
	pthread_t thread;
#else
#error "Unsupported platform for threads"
#endif
} plat_thread_t;

int plat_thread_create_ex(plat_thread_t *t, plat_thread_fn fn, void *arg, const PlatThreadExParams_t *params);
int plat_thread_create(plat_thread_t *t, PlatThreadClass_t cls, const PlatThreadRtParams_t *rt, plat_thread_fn fn, void *arg);
int plat_thread_join(plat_thread_t *t);
void plat_thread_sleep_ms(uint32_t ms);
void plat_thread_yield(void);

#endif /* PLATFORM_THREAD_H */
