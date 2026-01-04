/* PLC task definition. */

#ifndef PLC_TASK_H
#define PLC_TASK_H

#include <stdint.h>

typedef int (*plc_task_fn)(void* ctx);

typedef struct
{
	const char*	name;
	plc_task_fn	fn;
	void*		ctx;
	uint32_t	period_ms;
	uint32_t	phase_ms;
	uint32_t	last_run_ms;
} PlcTask_t;

#endif /* PLC_TASK_H */
