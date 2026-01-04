/* PLC scheduler running periodic PLC tasks in a dedicated thread. */

#ifndef PLC_SCHEDULER_H
#define PLC_SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>

#include "core/platform/platform_atomic.h"
#include "core/platform/platform_thread.h"
#include "plc/plc_task.h"

typedef struct
{
	uint32_t		base_cycle_ms;
	uint32_t		task_count;
	PlcTask_t		tasks[32];
	plat_atomic_bool_t	running;
	plat_thread_t		thread;
} PlcScheduler_t;

int plc_scheduler_init(PlcScheduler_t* s, uint32_t base_cycle_ms);
int plc_scheduler_add_task(PlcScheduler_t* s, const PlcTask_t* task);
int plc_scheduler_start(PlcScheduler_t* s);
int plc_scheduler_stop(PlcScheduler_t* s);
bool plc_scheduler_is_running(const PlcScheduler_t* s);

#endif /* PLC_SCHEDULER_H */
