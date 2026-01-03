/* System builder stub implementation. TODO: construct the control system. */

#include "system_builder.h"

#include "core/tag/tag_api.h"
#include "devices/registry/device_registry.h"

#include <stdint.h>
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

	if (config->process_var_count > 0 && config->process_vars == NULL) {
		return -1;
	}

	if (config->process_vars != NULL) {
		size_t process_index;
		size_t max_proc = sizeof(rt->proc_store.values) / sizeof(rt->proc_store.values[0]);

		if (config->process_var_count > max_proc) {
			return -1;
		}

		for (process_index = 0; process_index < config->process_var_count; ++process_index) {
			const ProcessVarDesc_t *proc_desc = &config->process_vars[process_index];
			TagEntry_t entry;
			int rc;
			ProcValue_t *value_slot;

			memset(&entry, 0, sizeof(entry));
			rc = snprintf(entry.full_name, sizeof(entry.full_name), "proc.%s", proc_desc->name);
			if (rc < 0 || (size_t)rc >= sizeof(entry.full_name)) {
				return -1;
			}

			rc = snprintf(entry.backend_name, sizeof(entry.backend_name), "proc");
			if (rc < 0 || (size_t)rc >= sizeof(entry.backend_name)) {
				return -1;
			}

			entry.type = proc_desc->type;
			entry.dir = TAG_DIR_OUT;
			entry.offset_byte = (uint32_t)process_index;
			entry.bit_index = 0U;
			entry.kind = TAGK_PROC;
			entry.proc_index = (uint32_t)process_index;

			if (tag_table_add(&rt->tag_table, &entry) != 0) {
				return -1;
			}

			value_slot = &rt->proc_store.values[process_index];
			memset(value_slot, 0, sizeof(*value_slot));
			value_slot->type = proc_desc->type;
			switch (proc_desc->type) {
			case TAG_T_BOOL:
				value_slot->v.b = proc_desc->initial.b;
				break;
			case TAG_T_U8:
				value_slot->v.u8 = proc_desc->initial.u8;
				break;
			case TAG_T_U16:
				value_slot->v.u16 = proc_desc->initial.u16;
				break;
			case TAG_T_U32:
				value_slot->v.u32 = proc_desc->initial.u32;
				break;
			case TAG_T_REAL:
				value_slot->v.f = proc_desc->initial.f;
				break;
			default:
				return -1;
			}
		}

		rt->proc_store.count = config->process_var_count;
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
			entry.kind = TAGK_IO;
			entry.proc_index = 0U;

			if (tag_table_add(&rt->tag_table, &entry) != 0) {
				return -1;
			}
		}
	}

	return 0;
}
