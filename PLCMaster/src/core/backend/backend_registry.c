/* Backend registry/factory implementation. */

#include "core/backend/backend_registry.h"

static const BackendFactoryEntry_t *backend_registry_find_factory(BackendType_t type)
{
	size_t i;

	for (i = 0U; i < g_backend_factories_count; ++i) {
		if (g_backend_factories[i].type == type) {
			return &g_backend_factories[i];
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
