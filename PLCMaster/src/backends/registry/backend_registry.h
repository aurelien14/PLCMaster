/* Backend registry/factory declarations. */

#ifndef BACKEND_REGISTRY_H
#define BACKEND_REGISTRY_H

#include <stddef.h>

#include "backends/backend_iface.h"
#include "app/config/config_static.h"

typedef struct BackendFactoryEntry
{
	BackendType_t type;
	const char *type_name;
	BackendDriver_t *(*create)(const BackendConfig_t *cfg,
		size_t iomap_hint,
		size_t max_devices,
		size_t backend_index);
	void (*destroy)(BackendDriver_t *drv);
} BackendFactoryEntry_t;

extern const BackendFactoryEntry_t g_backend_factories[];
extern const size_t g_backend_factories_count;

BackendDriver_t *backend_create(
	const BackendConfig_t *cfg,
	size_t backend_index,
	size_t max_devices,
	size_t iomap_size_hint);
void backend_destroy(BackendDriver_t *drv);
int backend_registry_register_dynamic(const BackendFactoryEntry_t *e);

#endif /* BACKEND_REGISTRY_H */
