/* Device registry static descriptor list. */

#include "core/device/device_registry.h"

#include "devices/l230/l230_desc.h"
#include "devices/l400/l400_desc.h"

#define DEVICE_ENTRY(desc) &(desc),

const DeviceDesc_t * const g_device_descs[] = {
#include "system/builder/device_list.def"
};

const size_t g_device_descs_count =
	sizeof(g_device_descs) / sizeof(g_device_descs[0]);

#undef DEVICE_ENTRY
