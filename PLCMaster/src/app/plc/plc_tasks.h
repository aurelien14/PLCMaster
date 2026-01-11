/* PLC task registry for the application layer. */

#ifndef APP_PLC_TASKS_H
#define APP_PLC_TASKS_H

#include <stddef.h>
#include <stdint.h>

#include "core/plc/plc_task.h"

typedef int (*PLC_TaskFn_t)(void* ctx);

typedef struct
{
	const char* name;
	PLC_TaskFn_t run;
	uint32_t period_ms;
	PlcTaskPolicy_t policy;
} PLC_TaskDesc_t;

const PLC_TaskDesc_t* app_plc_get_tasks(size_t* out_count);

#endif /* APP_PLC_TASKS_H */
