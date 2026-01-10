/* PLC scheduler running periodic PLC tasks in a dedicated thread. */

#ifndef PLC_SCHEDULER_H
#define PLC_SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>

#include "core/platform/platform_atomic.h"
#include "core/platform/platform_thread.h"
#include "core/plc/plc_task.h"

typedef struct
{
	uint32_t		base_cycle_ms;
	uint32_t		task_count;
	PlcTask_t		tasks[32];
	plat_atomic_u32_t	pending_write;
	plat_atomic_u32_t	pending_read;
	plat_atomic_bool_t	pending_ready[32];
	PlcTask_t		pending_tasks[32];
	plat_atomic_bool_t	running;
	void			(*backend_cycle_begin)(void *user);
	void			(*backend_cycle_commit)(void *user);
	void			*backend_user;
	void			(*on_cycle_begin)(void *user);
	void			(*on_cycle_end)(void *user);
	void			*user;
	plat_thread_t		thread;
} PlcScheduler_t;

int plc_scheduler_init(PlcScheduler_t* s, uint32_t base_cycle_ms);
int plc_scheduler_add_task(PlcScheduler_t* s, const PlcTask_t* task);
int plc_scheduler_add_task_runtime(PlcScheduler_t* s, const PlcTask_t* task);
int plc_scheduler_set_backend_hooks(PlcScheduler_t* s, void (*cycle_begin)(void *user), void (*cycle_commit)(void *user), void *user);
int plc_scheduler_set_callbacks(PlcScheduler_t* s, void (*on_cycle_begin)(void *user), void (*on_cycle_end)(void *user), void *user);
int plc_scheduler_start(PlcScheduler_t* s);
int plc_scheduler_stop(PlcScheduler_t* s);
bool plc_scheduler_is_running(const PlcScheduler_t* s);

#endif /* PLC_SCHEDULER_H */
