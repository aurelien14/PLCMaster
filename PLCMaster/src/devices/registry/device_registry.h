/* Device registry interface for registering and retrieving devices. */

#ifndef DEVICE_REGISTRY_H
#define DEVICE_REGISTRY_H

#include "devices/device_desc.h"

int device_registry_register_dynamic(const DeviceDesc_t *descriptor);
int device_registry_register(const DeviceDesc_t *descriptor);
const DeviceDesc_t *device_registry_find(const char *model);

#endif /* DEVICE_REGISTRY_H */
