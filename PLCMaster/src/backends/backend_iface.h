/* Backend interface definitions for communication layers. */

#ifndef BACKEND_IFACE_H
#define BACKEND_IFACE_H

#include <stddef.h>

typedef enum BackendType
{
	BACKEND_TYPE_NONE = 0,
	BACKEND_TYPE_ETHERCAT = 1,
	BACKEND_TYPE_MODBUS = 2,
} BackendType_t;

#define MAX_BACKENDS 4

typedef struct BackendDriver
{
	char name[16];
	BackendType_t type;
	void *impl;
} BackendDriver_t;

#endif /* BACKEND_IFACE_H */
