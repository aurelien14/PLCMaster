/* Device registry interface for registering and retrieving devices. TODO: finalize registry design. */

#ifndef DEVICE_REGISTRY_H
#define DEVICE_REGISTRY_H

int device_registry_register(const char *identifier);
void *device_registry_find(const char *identifier);

#endif /* DEVICE_REGISTRY_H */
