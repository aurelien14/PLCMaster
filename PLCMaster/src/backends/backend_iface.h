/* Backend interface definitions for communication layers. */

#ifndef BACKEND_IFACE_H
#define BACKEND_IFACE_H

#include <stddef.h>
#include <stdint.h>

struct DeviceConfig;
struct DeviceDesc;

typedef enum BackendType
{
	BACKEND_TYPE_NONE = 0,
	BACKEND_TYPE_ETHERCAT = 1,
	BACKEND_TYPE_MODBUS = 2,
} BackendType_t;

#define MAX_BACKENDS 4

typedef struct BackendDriver BackendDriver_t;

typedef struct BackendDriverOps
{
	int (*init)(BackendDriver_t *driver);
	int (*bind)(BackendDriver_t *driver, const struct DeviceConfig *cfg, const struct DeviceDesc *desc);
	int (*finalize)(BackendDriver_t *driver);
	int (*start)(BackendDriver_t *driver);
	int (*stop)(BackendDriver_t *driver);
	int (*process)(BackendDriver_t *driver);
	void (*sync)(BackendDriver_t *driver);
	void (*cycle_begin)(BackendDriver_t *driver);
	void (*cycle_commit)(BackendDriver_t *driver);
	/* Input data is a stable snapshot for the current cycle. */
	const uint8_t *(*get_input_data)(BackendDriver_t *driver, uint16_t device_index, uint32_t *size_out);
	/* Output data is a staging buffer applied on the next sync. */
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
