#ifndef PLATFORM_TIME_H
#define PLATFORM_TIME_H

#include <stdint.h>

#include "platform_detect.h"

uint64_t plat_time_monotonic_ns(void);
uint64_t plat_time_monotonic_us(void);
uint64_t plat_time_monotonic_ms(void);
uint64_t plat_time_perf_counter(void);
uint64_t plat_time_perf_freq(void);
int plat_time_begin_timer_resolution_1ms(void);
int plat_time_end_timer_resolution_1ms(void);

#endif /* PLATFORM_TIME_H */
