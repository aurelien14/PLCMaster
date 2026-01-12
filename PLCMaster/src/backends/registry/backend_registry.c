/* Backend registry/factory implementation. */

#include "backends/registry/backend_registry.h"

#define BACKEND_REGISTRY_DYNAMIC_CAPACITY 16

static BackendFactoryEntry_t g_backend_factories_dyn[BACKEND_REGISTRY_DYNAMIC_CAPACITY];
static size_t g_backend_factories_dyn_count = 0U;

static const BackendFactoryEntry_t *backend_registry_find_factory(BackendType_t type)
{
	size_t i;

	for (i = 0U; i < g_backend_factories_count; ++i) {
		if (g_backend_factories[i].type == type) {
			return &g_backend_factories[i];
		}
	}

	for (i = 0U; i < g_backend_factories_dyn_count; ++i) {
		if (g_backend_factories_dyn[i].type == type) {
			return &g_backend_factories_dyn[i];
		}
	}

	return NULL;
}

BackendDriver_t *backend_create(
	const BackendConfig_t *cfg,
	size_t backend_index,
	size_t max_devices,
	size_t iomap_size_hint)
{
	const BackendFactoryEntry_t *factory;
	BackendType_t backend_type;

	if (cfg == NULL || cfg->type == BACKEND_TYPE_NONE) {
		return NULL;
	}

	backend_type = cfg->type;
	factory = backend_registry_find_factory(backend_type);
	if (factory == NULL || factory->create == NULL) {
		return NULL;
	}

	return factory->create(cfg, iomap_size_hint, max_devices, backend_index);
}

void backend_destroy(BackendDriver_t *drv)
{
	const BackendFactoryEntry_t *factory;
	BackendType_t backend_type;

	if (drv == NULL) {
		return;
	}

	if (drv->ops != NULL && drv->ops->stop != NULL) {
		(void)drv->ops->stop(drv);
	}

	backend_type = drv->type;
	factory = backend_registry_find_factory(backend_type);
	if (factory != NULL && factory->destroy != NULL) {
		factory->destroy(drv);
	}
}

int backend_registry_register_dynamic(const BackendFactoryEntry_t *e)
{
	if (e == NULL || e->type == BACKEND_TYPE_NONE || e->create == NULL) {
		return -1;
	}

	if (backend_registry_find_factory(e->type) != NULL) {
		return 0;
	}

	if (g_backend_factories_dyn_count >= BACKEND_REGISTRY_DYNAMIC_CAPACITY) {
		return -1;
	}

	g_backend_factories_dyn[g_backend_factories_dyn_count++] = *e;
	return 0;
}
