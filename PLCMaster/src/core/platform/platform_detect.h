#ifndef PLATFORM_DETECT_H
#define PLATFORM_DETECT_H

#include <stdbool.h>
#include <stdint.h>

#if defined(_WIN32)
#define PLAT_WINDOWS 1
#else
#define PLAT_WINDOWS 0
#endif

#if defined(__linux__)
#define PLAT_LINUX 1
#else
#define PLAT_LINUX 0
#endif

#if defined(_WIN64)
#define PLAT_64BIT 1
#elif defined(__x86_64__) || defined(__aarch64__) || defined(__ppc64__) || defined(__LP64__)
#define PLAT_64BIT 1
#else
#define PLAT_64BIT 0
#endif

#define PLAT_INLINE static inline
#define PLAT_UNUSED(x) ((void)(x))

#endif /* PLATFORM_DETECT_H */
