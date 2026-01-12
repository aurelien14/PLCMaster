/* Backend interface definitions for communication layers. */

#ifndef BACKEND_IFACE_H
#define BACKEND_IFACE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "core/backend/backend_config.h"

struct DeviceConfig;
struct DeviceDesc;

#define MAX_BACKENDS 4

typedef struct BackendDriver BackendDriver_t;

typedef struct BackendStatus
{
	bool in_op;
	bool fault_latched;
	int32_t last_error;
} BackendStatus_t;

typedef struct BackendDriverOps
{
	int (*init)(BackendDriver_t *driver);
	int (*bind)(BackendDriver_t *driver, const struct DeviceConfig *cfg, const struct DeviceDesc *desc);
	int (*finalize)(BackendDriver_t *driver);
	int (*start)(BackendDriver_t *driver);
	int (*stop)(BackendDriver_t *driver);
	int (*process)(BackendDriver_t *driver);
	int (*get_status)(BackendDriver_t *driver, BackendStatus_t *out);
	void (*cycle_begin)(BackendDriver_t *driver);
	void (*cycle_end)(BackendDriver_t *driver);
	/* Input data is a stable snapshot for the current cycle. */
	const uint8_t *(*get_input_data)(BackendDriver_t *driver, uint16_t device_index, uint32_t *size_out);
	/* Output data is a staging buffer applied on the next cycle_end. */
	uint8_t *(*get_output_data)(BackendDriver_t *driver, uint16_t device_index, uint32_t *size_out);
} BackendDriverOps_t;

struct BackendDriver
{
	BackendType_t type;
	const BackendDriverOps_t *ops;
	char name[16];
	char system_name[16];
	void *impl;
};

#endif /* BACKEND_IFACE_H */
