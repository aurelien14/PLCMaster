/* Device registry static descriptor list. */

#include "devices/registry/device_registry.h"

#include "devices/l230/l230_desc.h"

#define DEVICE_ENTRY(desc) &(desc),

const DeviceDesc_t * const g_device_descs[] = {
#include "devices/registry/device_list.def"
};

const size_t g_device_descs_count =
	sizeof(g_device_descs) / sizeof(g_device_descs[0]);

#undef DEVICE_ENTRY
