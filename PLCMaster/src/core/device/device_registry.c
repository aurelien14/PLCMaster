/* Device registry implementation. */

#include "core/device/device_registry.h"

#include <stddef.h>
#include <string.h>

#define DEVICE_REGISTRY_DYNAMIC_CAPACITY 64

static const DeviceDesc_t *g_device_descs_dyn[DEVICE_REGISTRY_DYNAMIC_CAPACITY];
static size_t g_device_descs_dyn_count = 0U;

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

	for (i = 0U; i < g_device_descs_dyn_count; ++i) {
		const DeviceDesc_t *desc = g_device_descs_dyn[i];

		if (desc != NULL && desc->model != NULL && strcmp(desc->model, model) == 0) {
			return desc;
		}
	}

	return NULL;
}

int device_registry_register_dynamic(const DeviceDesc_t *descriptor)
{
	size_t i;

	if (descriptor == NULL || descriptor->model == NULL) {
		return -1;
	}

	for (i = 0U; i < g_device_descs_count; ++i) {
		const DeviceDesc_t* desc = g_device_descs[i];

		if (desc != NULL && desc->model != NULL && strcmp(desc->model, descriptor->model) == 0) {
			return 0;
		}
	}

	for (i = 0U; i < g_device_descs_dyn_count; ++i) {
		const DeviceDesc_t* desc = g_device_descs_dyn[i];

		if (desc != NULL && desc->model != NULL && strcmp(desc->model, descriptor->model) == 0) {
			return 0;
		}
	}


	if (g_device_descs_dyn_count >= DEVICE_REGISTRY_DYNAMIC_CAPACITY) {
		return -1;
	}

	g_device_descs_dyn[g_device_descs_dyn_count++] = descriptor;
	return 0;
}

int device_registry_register(const DeviceDesc_t *descriptor)
{
	return device_registry_register_dynamic(descriptor);
}
