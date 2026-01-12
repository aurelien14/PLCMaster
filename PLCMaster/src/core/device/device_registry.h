/* Device registry interface for registering and retrieving devices. */

#ifndef DEVICE_REGISTRY_H
#define DEVICE_REGISTRY_H

#include <stddef.h>

#include "core/device/device_desc.h"

extern const DeviceDesc_t *const g_device_descs[];
extern const size_t g_device_descs_count;

const DeviceDesc_t *device_registry_find(const char *model);

#endif /* DEVICE_REGISTRY_H */
