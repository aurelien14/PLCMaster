/* System builder stub implementation. TODO: construct the control system. */

#include "system_builder.h"

#include "devices/registry/device_registry.h"

#include <stdio.h>
#include <string.h>

int system_build(Runtime_t *rt, const SystemConfig_t *config)
{
	size_t device_index;
	size_t backend_index;

	if (rt == NULL || config == NULL) {
		return -1;
	}

	if (config->backends == NULL || config->devices == NULL) {
		return -1;
	}

	if (config->backend_count == 0 || config->device_count == 0) {
		return -1;
	}

	for (device_index = 0; device_index < config->device_count; ++device_index) {
		const DeviceConfig_t *device_cfg = &config->devices[device_index];
		int backend_found = 0;

		if (device_cfg->backend == NULL || device_cfg->model == NULL) {
			return -1;
		}

		for (backend_index = 0; backend_index < config->backend_count; ++backend_index) {
			const BackendConfig_t *backend_cfg = &config->backends[backend_index];
			if (backend_cfg->name != NULL && strcmp(backend_cfg->name, device_cfg->backend) == 0) {
				backend_found = 1;
				break;
			}
		}

		if (!backend_found) {
			return -1;
		}

		if (device_registry_find(device_cfg->model) == NULL) {
			return -1;
		}
	}

	for (device_index = 0; device_index < config->device_count; ++device_index) {
		const DeviceConfig_t *device_cfg = &config->devices[device_index];
		const DeviceDesc_t *desc = device_registry_find(device_cfg->model);
		uint32_t tag_index;

		if (desc == NULL) {
			return -1;
		}

		for (tag_index = 0; tag_index < desc->tag_count; ++tag_index) {
			const TagDesc_t *tag_desc = &desc->tags[tag_index];
			TagEntry_t entry;
			int rc;

			memset(&entry, 0, sizeof(entry));
			rc = snprintf(entry.full_name, sizeof(entry.full_name), "%s.%s", device_cfg->name, tag_desc->name);
			if (rc < 0 || (size_t)rc >= sizeof(entry.full_name)) {
				return -1;
			}

			rc = snprintf(entry.backend_name, sizeof(entry.backend_name), "%s", device_cfg->backend);
			if (rc < 0 || (size_t)rc >= sizeof(entry.backend_name)) {
				return -1;
			}

			entry.type = tag_desc->type;
			entry.dir = tag_desc->dir;
			entry.offset_byte = tag_desc->offset_byte;
			entry.bit_index = tag_desc->bit_index;

			if (tag_table_add(&rt->tag_table, &entry) != 0) {
				return -1;
			}
		}
	}

	return 0;
}
