#include "virtual_device_desc.h"
#include <stddef.h>
#include <stdint.h>

static const TagDesc_t FAKE_ECAT_DEV_TAGS[] = { NULL
    //{ "X30_2_In0", TAG_DIR_IN, TAG_T_BOOL, (uint32_t)offsetof(FAKE_ECAT_DEV_RX_PDO_t, FakeEcatDev_Byte0_bits), 0, NULL, NULL },
};

const DeviceDesc_t DEVICE_DESC_FAKE_ECAT_DEV = {
    .model = "FakEcatDev",
    .vendor_id = 0x1234567,
    .product_code = 0x123,
    //.rx_bytes = sizeof(FAKE_ECAT_DEV_RX_PDO_t),
    //.tx_bytes = sizeof(FAKE_ECAT_DEV_TX_PDO_t),
    //.tags = FAKE_ECAT_DEV_TAGS,
    .tag_count = (uint32_t)(sizeof(FAKE_ECAT_DEV_TAGS) / sizeof(FAKE_ECAT_DEV_TAGS[0])),
    .hooks = { NULL },
};

const char* virtual_device_get_model_identifier(void)
{
    return DEVICE_DESC_FAKE_ECAT_DEV.model;
}