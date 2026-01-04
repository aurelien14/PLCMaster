/* Backend registry/factory declarations. */

#ifndef BACKEND_REGISTRY_H
#define BACKEND_REGISTRY_H

#include <stddef.h>

#include "backends/backend_iface.h"
#include "system/config/config_static.h"

BackendDriver_t *backend_create(
	const BackendConfig_t *cfg,
	size_t backend_index,
	size_t max_devices,
	size_t iomap_size_hint);
void backend_destroy(BackendDriver_t *drv);

#endif /* BACKEND_REGISTRY_H */
