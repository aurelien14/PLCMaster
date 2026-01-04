#ifndef PLATFORM_TIME_H
#define PLATFORM_TIME_H

#include <stdint.h>

#include "platform_detect.h"

uint64_t plat_time_monotonic_ns(void);
uint64_t plat_time_monotonic_us(void);
uint64_t plat_time_monotonic_ms(void);
uint64_t plat_time_perf_counter(void);
uint64_t plat_time_perf_freq(void);
uint64_t plat_time_now_ns(void);
void plat_time_sleep_until_ns(uint64_t deadline_ns, uint32_t spin_threshold_ns);
void plat_time_busy_wait_ns(uint64_t duration_ns);
int plat_time_begin_timer_resolution_1ms(void);
int plat_time_end_timer_resolution_1ms(void);

#endif /* PLATFORM_TIME_H */
