/* PLC task registry mapping names to implementations. */

#include <stddef.h>
#include <string.h>

#include "app/plc/plc_task_registry.h"
#include "app/plc/tasks/plc_tasks.h"

static const PLC_TaskRegistryEntry_t kPlcTaskRegistry[] = {
	{ "noop", plc_task_noop },
	{ "heartbeat", plc_task_heartbeat },
};

PLC_TaskRunFn plc_task_registry_find(const char *name)
{
	size_t index;

	if (name == NULL)
	{
		return NULL;
	}

	for (index = 0; index < (sizeof(kPlcTaskRegistry) / sizeof(kPlcTaskRegistry[0])); ++index)
	{
		const PLC_TaskRegistryEntry_t *entry = &kPlcTaskRegistry[index];

		if (entry->name != NULL && strcmp(entry->name, name) == 0)
		{
			return entry->fn;
		}
	}

	return NULL;
}
