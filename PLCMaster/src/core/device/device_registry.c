/* Device registry implementation. */

#include "core/device/device_registry.h"

#include <stddef.h>
#include <string.h>

const DeviceDesc_t *device_registry_find(const char *model)
{
	size_t i;

	if (model == NULL) {
		return NULL;
	}

	for (i = 0U; i < g_device_descs_count; ++i) {
		const DeviceDesc_t *desc = g_device_descs[i];

		if (desc != NULL && desc->model != NULL && strcmp(desc->model, model) == 0) {
			return desc;
		}
	}

	return NULL;
}
