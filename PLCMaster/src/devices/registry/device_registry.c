/* Device registry implementation. */

#include "device_registry.h"
#include "../l230/l230_desc.h"

#include <stddef.h>
#include <string.h>

#define DEVICE_REGISTRY_CAPACITY 16

static const DeviceDesc_t *registered_devices[DEVICE_REGISTRY_CAPACITY];
static size_t registered_device_count = 0U;

static void device_registry_init(void)
{
    static int initialized = 0;
    if (initialized) {
        return;
    }

    initialized = 1;
    (void)device_registry_register(&DEVICE_DESC_L230);
}

int device_registry_register(const DeviceDesc_t *descriptor)
{
    size_t i;

    if (descriptor == NULL || descriptor->model == NULL) {
        return -1;
    }

    device_registry_init();

    for (i = 0; i < registered_device_count; ++i) {
        if (strcmp(registered_devices[i]->model, descriptor->model) == 0) {
            return 0;
        }
    }

    if (registered_device_count >= DEVICE_REGISTRY_CAPACITY) {
        return -1;
    }

    registered_devices[registered_device_count++] = descriptor;
    return 0;
}

const DeviceDesc_t *device_registry_find(const char *model)
{
    size_t i;

    if (model == NULL) {
        return NULL;
    }

    device_registry_init();

    for (i = 0; i < registered_device_count; ++i) {
        if (strcmp(registered_devices[i]->model, model) == 0) {
            return registered_devices[i];
        }
    }

    return NULL;
}
