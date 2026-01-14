/* Proc view bindings. */

#include <stdio.h>
#include <string.h>

#include "app/io/proc_view.h"

int proc_view_bind(ProcView_t *proc, const TagTable_t *tags)
{
	typedef struct
	{
		TagId_t *dst;
		const char *name;
	} ProcBindDesc_t;

	const ProcBindDesc_t kProcBinds[] = {
		{ &proc->counter_test, "proc.counter_test" },
	};
	size_t index;

	if (proc == NULL || tags == NULL)
	{
		return -1;
	}

	memset(proc, 0, sizeof(*proc));

	for (index = 0; index < (sizeof(kProcBinds) / sizeof(kProcBinds[0])); ++index)
	{
		TagId_t id = tag_table_find_id(tags, kProcBinds[index].name);
		if (id == 0)
		{
			printf("Missing PROC tag: %s\n", kProcBinds[index].name);
			return -1;
		}

		*(kProcBinds[index].dst) = id;
	}

	return 0;
}
