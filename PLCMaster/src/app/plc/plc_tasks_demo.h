/* Demo PLC tasks. */

#ifndef PLC_TASKS_DEMO_H
#define PLC_TASKS_DEMO_H

#include <stdint.h>

#include "core/runtime/runtime.h"
#include "core/tag/tag_api.h"

typedef struct
{
	uint32_t counter;
} PlcDemoBlinkCtx_t;

typedef struct
{
	Runtime_t*	runtime;
	TagId_t		temp_sp_id;
} PlcDemoControlCtx_t;

int plc_demo_task_blink(void* ctx);
int plc_demo_task_control(void* ctx);

#endif /* PLC_TASKS_DEMO_H */
