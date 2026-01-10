#ifndef FAKE_ECAT_DEV_DESC
#define FAKE_ECAT_DEV_DESC

#include "devices/device_desc.h"
#include <stdint.h>

static_assert(sizeof(float) == 4, "float must be 32-bit");


extern const DeviceDesc_t DEVICE_DESC_FAKE_ECAT_DEV;

const char* virtual_device_get_model_identifier(void);
#endif // !FAKE_ECAT_DEV_DESC
