#ifndef PLATFORM_TIME_H
#define PLATFORM_TIME_H

#include <stdint.h>

#include "platform_detect.h"

uint64_t plat_time_monotonic_ns(void);
uint64_t plat_time_monotonic_us(void);
uint64_t plat_time_monotonic_ms(void);
uint64_t plat_time_perf_counter(void);
uint64_t plat_time_perf_freq(void);

#endif /* PLATFORM_TIME_H */
