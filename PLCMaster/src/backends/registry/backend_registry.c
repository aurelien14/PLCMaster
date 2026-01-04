/* Backend registry/factory implementation. */

#include "backends/registry/backend_registry.h"

#include <string.h>

#include "backends/ethercat/ec_backend.h"

BackendDriver_t *backend_create(
	const BackendConfig_t *cfg,
	size_t backend_index,
	size_t max_devices,
	size_t iomap_size_hint)
{
	BackendType_t backend_type;

	if (cfg == NULL) {
		return NULL;
	}

	backend_type = cfg->type;
	if (backend_type == BACKEND_TYPE_NONE && cfg->name != NULL) {
		if (strncmp(cfg->name, "ec", 2) == 0) {
			backend_type = BACKEND_TYPE_ETHERCAT;
		}
	}

	switch (backend_type) {
	case BACKEND_TYPE_ETHERCAT:
		return ethercat_backend_create(cfg, iomap_size_hint, max_devices, backend_index);
	default:
		return NULL;
	}
}

void backend_destroy(BackendDriver_t *drv)
{
	BackendType_t backend_type;

	if (drv == NULL) {
		return;
	}

	if (drv->ops != NULL && drv->ops->stop != NULL) {
		(void)drv->ops->stop(drv);
	}

	backend_type = drv->type;
	switch (backend_type) {
	case BACKEND_TYPE_ETHERCAT:
		ethercat_backend_destroy(drv);
		break;
	default:
		break;
	}
}
