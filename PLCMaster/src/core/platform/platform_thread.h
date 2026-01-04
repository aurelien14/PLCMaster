#ifndef PLATFORM_THREAD_H
#define PLATFORM_THREAD_H

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

int plat_thread_create(plat_thread_t *t, plat_thread_fn fn, void *arg, const char *name);
int plat_thread_join(plat_thread_t *t);
void plat_thread_sleep_ms(uint32_t ms);
void plat_thread_yield(void);

#endif /* PLATFORM_THREAD_H */
