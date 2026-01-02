/* Device registry stub implementation. TODO: store and query registered devices. */

#include "device_registry.h"

#include <stddef.h>

int device_registry_register(const char *identifier)
{
    (void)identifier;
    return -1;
}

void *device_registry_find(const char *identifier)
{
    (void)identifier;
    return NULL;
}
