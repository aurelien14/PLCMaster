/* PLC task registry mapping names to implementations. */

#ifndef PLC_TASK_REGISTRY_H
#define PLC_TASK_REGISTRY_H

typedef int (*PLC_TaskRunFn)(void *ctx);

typedef struct PLC_TaskRegistryEntry
{
	const char *name;
	PLC_TaskRunFn fn;
} PLC_TaskRegistryEntry_t;

PLC_TaskRunFn plc_task_registry_find(const char *name);

#endif /* PLC_TASK_REGISTRY_H */
