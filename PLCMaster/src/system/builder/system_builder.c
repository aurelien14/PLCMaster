/* System builder stub implementation. TODO: construct the control system. */

#include "system_builder.h"

#include "core/tag/tag_api.h"
#include "devices/registry/device_registry.h"
#include "backends/backend_iface.h"
#include "backends/registry/backend_registry.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void cleanup_backends(Runtime_t *rt)
{
	size_t idx;

	if (rt == NULL) {
		return;
	}

	for (idx = 0; idx < rt->backend_count && idx < MAX_BACKENDS; ++idx) {
		BackendDriver_t *drv = &rt->backend_array[idx];
		if (drv->impl != NULL) {
			backend_destroy(drv);
		}
		memset(drv, 0, sizeof(*drv));
	}

	rt->backend_count = 0U;
	rt->backends = NULL;
}

int system_build(Runtime_t *rt, const SystemConfig_t *config)
{
	size_t device_index;
	size_t backend_index;
	int backends_initialized = 0;

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

	if (config->backend_count > MAX_BACKENDS) {
		return -1;
	}

	memset(rt->backend_array, 0, sizeof(rt->backend_array));
	rt->backend_count = 0U;
	rt->backends = rt->backend_array;

	for (backend_index = 0; backend_index < config->backend_count; ++backend_index) {
		const BackendConfig_t *backend_cfg = &config->backends[backend_index];
		BackendDriver_t *drv;
		size_t iomap_size = 0U;

		if (backend_cfg->name == NULL || backend_cfg->ifname == NULL) {
			goto cleanup;
		}

		drv = &rt->backend_array[rt->backend_count];
		memset(drv, 0, sizeof(*drv));
		if (snprintf(drv->name, sizeof(drv->name), "%s", backend_cfg->name) < 0) {
			goto cleanup;
		}

		for (device_index = 0; device_index < config->device_count; ++device_index) {
			const DeviceConfig_t *device_cfg = &config->devices[device_index];
			const DeviceDesc_t *desc = device_registry_find(device_cfg->model);
			if (desc == NULL) {
				goto cleanup;
			}
			if (device_cfg->backend != NULL && strcmp(device_cfg->backend, backend_cfg->name) == 0) {
				iomap_size += desc->rx_bytes;
				iomap_size += desc->tx_bytes;
				iomap_size += 1; //TODO: see why soem append 1byte becaus ISize and OSize are same as pdo(L230)
			}
		}

		if (iomap_size == 0U) {
			iomap_size = 256U;
		}

		drv = backend_create(backend_cfg, backend_index, config->device_count, iomap_size);
		if (drv == NULL) {
			goto cleanup;
		}

		rt->backend_array[rt->backend_count] = *drv;
		drv = &rt->backend_array[rt->backend_count];
		rt->backend_count++;
		backends_initialized = 1;

		if (drv->ops == NULL || drv->ops->init == NULL || drv->ops->bind == NULL || drv->ops->finalize == NULL || drv->ops->start == NULL) {
			goto cleanup;
		}

		if (drv->ops->init(drv) != 0) {
			goto cleanup;
		}

		for (device_index = 0; device_index < config->device_count; ++device_index) {
			const DeviceConfig_t *device_cfg = &config->devices[device_index];
			const DeviceDesc_t *desc = device_registry_find(device_cfg->model);
			if (desc == NULL) {
				goto cleanup;
			}
			if (device_cfg->backend != NULL && strcmp(device_cfg->backend, backend_cfg->name) == 0) {
				if (drv->ops->bind(drv, device_cfg, desc) != 0) {
					goto cleanup;
				}
			}
		}

		if (drv->ops->finalize(drv) != 0) {
			goto cleanup;
		}

		if (drv->ops->start(drv) != 0) {
			goto cleanup;
		}
	}

	if (config->process_var_count > 0 && config->process_vars == NULL) {
		goto cleanup;
	}

	if (config->process_vars != NULL) {
		size_t process_index;
		size_t max_proc = sizeof(rt->proc_store.values) / sizeof(rt->proc_store.values[0]);

		if (config->process_var_count > max_proc) {
			goto cleanup;
		}

		for (process_index = 0; process_index < config->process_var_count; ++process_index) {
			const ProcessVarDesc_t *proc_desc = &config->process_vars[process_index];
			TagEntry_t entry;
			int rc;
			ProcValue_t *value_slot;

			memset(&entry, 0, sizeof(entry));
			rc = snprintf(entry.full_name, sizeof(entry.full_name), "proc.%s", proc_desc->name);
			if (rc < 0 || (size_t)rc >= sizeof(entry.full_name)) {
				goto cleanup;
			}

			rc = snprintf(entry.backend_name, sizeof(entry.backend_name), "proc");
			if (rc < 0 || (size_t)rc >= sizeof(entry.backend_name)) {
				goto cleanup;
			}

			entry.type = proc_desc->type;
			entry.dir = TAG_DIR_OUT;
			entry.offset_byte = (uint32_t)process_index;
			entry.bit_index = 0U;
			entry.kind = TAGK_PROC;
			entry.proc_index = (uint32_t)process_index;

			if (tag_table_add(&rt->tag_table, &entry) != 0) {
				goto cleanup;
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
				goto cleanup;
			}
		}

		rt->proc_store.count = config->process_var_count;
	}

	if (config->hmi_tag_count > 0 && config->hmi_tags == NULL) {
		goto cleanup;
	}

	if (config->hmi_tags != NULL) {
		size_t hmi_index;
		for (hmi_index = 0; hmi_index < config->hmi_tag_count; ++hmi_index) {
			const HmiTagDesc_t *hmi_desc = &config->hmi_tags[hmi_index];
			TagId_t target_id;
			const TagEntry_t *target_entry;
			TagEntry_t entry;
			int rc;

			if (hmi_desc->alias_of == NULL) {
				goto cleanup;
			}

			target_id = tag_table_find_id(&rt->tag_table, hmi_desc->alias_of);
			if (target_id == 0) {
				goto cleanup;
			}

			target_entry = tag_table_get(&rt->tag_table, target_id);
			if (target_entry == NULL) {
				goto cleanup;
			}

			memset(&entry, 0, sizeof(entry));
			rc = snprintf(entry.full_name, sizeof(entry.full_name), "hmi.%s", hmi_desc->name);
			if (rc < 0 || (size_t)rc >= sizeof(entry.full_name)) {
				goto cleanup;
			}

			rc = snprintf(entry.backend_name, sizeof(entry.backend_name), "hmi");
			if (rc < 0 || (size_t)rc >= sizeof(entry.backend_name)) {
				goto cleanup;
			}

			entry.type = target_entry->type;
			entry.dir = target_entry->dir;
			entry.offset_byte = 0U;
			entry.bit_index = 0U;
			entry.kind = TAGK_HMI_ALIAS;
			entry.proc_index = 0U;
			entry.alias_target = target_id;
			entry.hmi_access = (uint8_t)hmi_desc->access;

			if (tag_table_add(&rt->tag_table, &entry) != 0) {
				goto cleanup;
			}
		}
	}

	for (device_index = 0; device_index < config->device_count; ++device_index) {
		const DeviceConfig_t *device_cfg = &config->devices[device_index];
		const DeviceDesc_t *desc = device_registry_find(device_cfg->model);
		uint32_t tag_index;

		if (desc == NULL) {
			goto cleanup;
		}

		for (tag_index = 0; tag_index < desc->tag_count; ++tag_index) {
			const TagDesc_t *tag_desc = &desc->tags[tag_index];
			TagEntry_t entry;
			int rc;

			memset(&entry, 0, sizeof(entry));
			rc = snprintf(entry.full_name, sizeof(entry.full_name), "%s.%s", device_cfg->name, tag_desc->name);
			if (rc < 0 || (size_t)rc >= sizeof(entry.full_name)) {
				goto cleanup;
			}

			rc = snprintf(entry.backend_name, sizeof(entry.backend_name), "%s", device_cfg->backend);
			if (rc < 0 || (size_t)rc >= sizeof(entry.backend_name)) {
				goto cleanup;
			}

			entry.type = tag_desc->type;
			entry.dir = tag_desc->dir;
			entry.offset_byte = tag_desc->offset_byte;
			entry.bit_index = tag_desc->bit_index;
			entry.kind = TAGK_IO;
			entry.proc_index = 0U;

			if (tag_table_add(&rt->tag_table, &entry) != 0) {
				goto cleanup;
			}
		}
	}

	return 0;

cleanup:
	if (backends_initialized) {
		cleanup_backends(rt);
	}
	return -1;
}
