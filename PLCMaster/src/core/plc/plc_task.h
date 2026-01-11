/* PLC task definition. */

#ifndef PLC_TASK_H
#define PLC_TASK_H

#include <stdint.h>

typedef int (*plc_task_fn)(void* ctx);

typedef enum PlcTaskPolicy
{
	PLC_TASK_ALWAYS_RUN = 0,		/* tourne même en FAULT */
	PLC_TASK_SKIP_ON_FAULT = 1,		/* tourne en OK/WARN, pas en FAULT */
	PLC_TASK_ONLY_WHEN_OK = 2		/* tourne seulement en OK (réservé futur, mais le garder) */
} PlcTaskPolicy_t;

typedef struct
{
	const char*	name;
	plc_task_fn	fn;
	void*		ctx;
	uint32_t	period_ms;
	uint32_t	phase_ms;
	uint32_t	last_run_ms;
	PlcTaskPolicy_t	policy;
} PlcTask_t;

#endif /* PLC_TASK_H */
