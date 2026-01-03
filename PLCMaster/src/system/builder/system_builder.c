/* System builder stub implementation. TODO: construct the control system. */

#include "system_builder.h"

#include "devices/registry/device_registry.h"

#include <stdio.h>
#include <string.h>

static int populate_io_view(Runtime_t *rt)
{
	size_t i;
	static const struct
	{
		const char *name;
		TagId_t *field;
	} kMappings[] = {
		{ "CPU_IO.X15_Out0", &rt->io_view.X15_Out0 },
		{ "CPU_IO.X12_Out1", &rt->io_view.X12_Out1 },
		{ "CPU_IO.X13_Out2", &rt->io_view.X13_Out2 },
		{ "CPU_IO.X3_Out3", &rt->io_view.X3_Out3 },
		{ "CPU_IO.X4_Out4", &rt->io_view.X4_Out4 },
		{ "CPU_IO.X14_Out5", &rt->io_view.X14_Out5 },
		{ "CPU_IO.X1_Out6", &rt->io_view.X1_Out6 },
		{ "CPU_IO.X11_Out0", &rt->io_view.X11_Out0 },
		{ "CPU_IO.X10_Out1", &rt->io_view.X10_Out1 },
		{ "CPU_IO.X5_Out2", &rt->io_view.X5_Out2 },
		{ "CPU_IO.X6_Out3", &rt->io_view.X6_Out3 },
		{ "CPU_IO.X7_Out4", &rt->io_view.X7_Out4 },
		{ "CPU_IO.X8_Out5", &rt->io_view.X8_Out5 },
		{ "CPU_IO.X9_Out6", &rt->io_view.X9_Out6 },
		{ "CPU_IO.X8a_Out7", &rt->io_view.X8a_Out7 },
		{ "CPU_IO.X30_2_In0", &rt->io_view.X30_2_In0 },
		{ "CPU_IO.X30_4_In1", &rt->io_view.X30_4_In1 },
		{ "CPU_IO.X30_6_In2", &rt->io_view.X30_6_In2 },
		{ "CPU_IO.X30_8_In3", &rt->io_view.X30_8_In3 },
		{ "CPU_IO.X30_11_In4", &rt->io_view.X30_11_In4 },
		{ "CPU_IO.X30_13_In5", &rt->io_view.X30_13_In5 },
		{ "CPU_IO.X30_19_In6", &rt->io_view.X30_19_In6 },
		{ "CPU_IO.X30_21_In7", &rt->io_view.X30_21_In7 },
		{ "CPU_IO.X55_In0", &rt->io_view.X55_In0 },
		{ "CPU_IO.X21_CPU_Pt1.Pt_Value", &rt->io_view.X21_CPU_Pt1_Pt_Value },
		{ "CPU_IO.X21_CPU_Pt1.Pt_State", &rt->io_view.X21_CPU_Pt1_Pt_State },
		{ "CPU_IO.X22_CPU_Pt2.Pt_Value", &rt->io_view.X22_CPU_Pt2_Pt_Value },
		{ "CPU_IO.X22_CPU_Pt2.Pt_State", &rt->io_view.X22_CPU_Pt2_Pt_State },
		{ "CPU_IO.X23_CPU_VC1.VC_Value", &rt->io_view.X23_CPU_VC1_VC_Value },
		{ "CPU_IO.X23_CPU_VC1.VC_State", &rt->io_view.X23_CPU_VC1_VC_State },
		{ "CPU_IO.X21_CPU_Pt1_Ctrl", &rt->io_view.X21_CPU_Pt1_Ctrl },
		{ "CPU_IO.X21_CPU_Pt2_Ctrl", &rt->io_view.X21_CPU_Pt2_Ctrl },
		{ "CPU_IO.X23_CPU_VC1_Ctrl", &rt->io_view.X23_CPU_VC1_Ctrl },
	};

	for (i = 0; i < sizeof(kMappings) / sizeof(kMappings[0]); ++i)
	{
		TagId_t id = tag_table_find_id(&rt->tag_table, kMappings[i].name);
		if (id == 0)
		{
			return -1;
		}
		*(kMappings[i].field) = id;
	}

	return 0;
}

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

	if (populate_io_view(rt) != 0)
	{
		return -1;
	}

	return 0;
}
