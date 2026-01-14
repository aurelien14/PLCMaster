/* HMI view bindings. */

#include <stdio.h>
#include <string.h>

#include "app/io/hmi_view.h"

int hmi_view_bind(HMIView_t *hmi, const TagTable_t *tags)
{
	typedef struct
	{
		TagId_t *dst;
		const char *name;
	} HmiBindDesc_t;

	const HmiBindDesc_t kHmiBinds[] = {
		{ &hmi->temp_setpoint, "hmi.temp_setpoint" },
		{ &hmi->run, "hmi.run" },
		{ &hmi->alarm_state, "hmi.alarm_state" },
		{ &hmi->alarm_code, "hmi.alarm_code" },
		{ &hmi->counter_test, "hmi.counter_test" },
	};
	size_t index;

	if (hmi == NULL || tags == NULL)
	{
		return -1;
	}

	memset(hmi, 0, sizeof(*hmi));

	for (index = 0; index < (sizeof(kHmiBinds) / sizeof(kHmiBinds[0])); ++index)
	{
		TagId_t id = tag_table_find_id(tags, kHmiBinds[index].name);
		if (id == 0)
		{
			printf("Missing HMI tag: %s\n", kHmiBinds[index].name);
			return -1;
		}

		*(kHmiBinds[index].dst) = id;
	}

	return 0;
}
