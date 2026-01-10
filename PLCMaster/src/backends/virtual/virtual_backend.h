#ifndef VIRTUAL_DEVICE_H
#define VIRTUAL_DEVICE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "backends/backend_iface.h"
#include "core/platform/platform_thread.h"
#include "core/platform/platform_atomic.h"
#include <soem/soem.h>
#include "app/config/config_static.h"

typedef struct VirtualDevice
{
	const DeviceConfig_t* cfg;
	const DeviceDesc_t* desc;
	uint16_t slave_index;
	uint8_t* in_buffers;
	uint8_t* out_buffers;
	uint32_t in_size;
	uint32_t out_size;
} VirtualDevice_t;

typedef struct VirtualDriver
{
	BackendDriver_t base;
	char ifname[64];
	VirtualDevice_t* devices;
	PlatThreadExParams_t rt_params;
} VirtualDriver_t;

BackendDriver_t* virtual_backend_create(const BackendConfig_t* cfg, size_t backend_index);
int virtual_backend_destroy(BackendDriver_t* driver);

#endif /* VIRTUAL_DEVICE_H */