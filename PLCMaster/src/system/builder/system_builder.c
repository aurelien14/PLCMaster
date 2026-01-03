/* System builder stub implementation. TODO: construct the control system. */

#include "system_builder.h"

#include "devices/registry/device_registry.h"

#include <string.h>

int system_build(Runtime_t *rt, const SystemConfig_t *config)
{
    size_t device_index;
    size_t backend_index;

    if (rt == NULL || config == NULL) {
        return -1;
    }

    if (config->backends == NULL || config->devices == NULL) {
        return -1;
    }

    if (config->backend_count == 0 || config->device_count == 0) {
        return -1;
    }

    (void)rt;

    for (device_index = 0; device_index < config->device_count; ++device_index) {
        const DeviceConfig_t *device_cfg = &config->devices[device_index];
        int backend_found = 0;

        if (device_cfg->backend == NULL || device_cfg->model == NULL) {
            return -1;
        }

        for (backend_index = 0; backend_index < config->backend_count; ++backend_index) {
            const BackendConfig_t *backend_cfg = &config->backends[backend_index];
            if (backend_cfg->name != NULL && strcmp(backend_cfg->name, device_cfg->backend) == 0) {
                backend_found = 1;
                break;
            }
        }

        if (!backend_found) {
            return -1;
        }

        if (device_registry_find(device_cfg->model) == NULL) {
            return -1;
        }
    }

    /* TODO: build runtime with validated configuration (no EtherCAT/threads here). */
    return 0;
}
