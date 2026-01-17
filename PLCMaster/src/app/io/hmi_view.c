/* HMI view bindings. */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "app/config/config_static.h"
#include "app/io/hmi_view.h"

static int bind_view_by_offset(void *view, size_t view_size, const TagTable_t *tags, const AppTagBind_t *binds,
			       size_t bind_count, const char *label)
{
	size_t index;

	if (view == NULL || tags == NULL || binds == NULL || bind_count == 0)
	{
		return -1;
	}

	memset(view, 0, view_size);

	for (index = 0; index < bind_count; ++index)
	{
		TagId_t id = tag_table_find_id(tags, binds[index].name);
		if (id == 0)
		{
			printf("Missing %s tag: %s\n", label, binds[index].name);
			return -1;
		}

		{
			TagId_t *dst = (TagId_t *)((uint8_t *)view + binds[index].offset);
			*dst = id;
		}
	}

	return 0;
}

int hmi_view_bind(HMIView_t *hmi, const TagTable_t *tags)
{
	size_t count = 0;
	const AppTagBind_t *binds = app_config_get_hmi_view_binds(&count);

	return bind_view_by_offset(hmi, sizeof(*hmi), tags, binds, count, "HMI");
}
