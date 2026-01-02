/* L230 device descriptor implementation. */

#include "l230_desc.h"

#include <stddef.h>
#include <stdint.h>

#define L230_MODULE_NUMBER 1

#define OFFSET_OF(type, member) ((uint32_t)((uintptr_t)&(((type *)0)->member)))

static const TagDesc_t L230_TAGS[] = {
    { "X30_2_In0", IN, BOOL, OFFSET_OF(L230_TX_PDO_t, L230_DI_Byte0), 0, "%I1.0.0", NULL },
    { "X30_4_In1", IN, BOOL, OFFSET_OF(L230_TX_PDO_t, L230_DI_Byte0), 1, "%I1.0.1", NULL },
    { "X30_6_In2", IN, BOOL, OFFSET_OF(L230_TX_PDO_t, L230_DI_Byte0), 2, "%I1.0.2", NULL },
    { "X30_8_In3", IN, BOOL, OFFSET_OF(L230_TX_PDO_t, L230_DI_Byte0), 3, "%I1.0.3", NULL },
    { "X30_11_In4", IN, BOOL, OFFSET_OF(L230_TX_PDO_t, L230_DI_Byte0), 4, "%I1.0.4", NULL },
    { "X30_13_In5", IN, BOOL, OFFSET_OF(L230_TX_PDO_t, L230_DI_Byte0), 5, "%I1.0.5", NULL },
    { "X30_19_In6", IN, BOOL, OFFSET_OF(L230_TX_PDO_t, L230_DI_Byte0), 6, "%I1.0.6", NULL },
    { "X30_21_In7", IN, BOOL, OFFSET_OF(L230_TX_PDO_t, L230_DI_Byte0), 7, "%I1.0.7", NULL },
    { "X55_In0", IN, BOOL, OFFSET_OF(L230_TX_PDO_t, L230_DI_Byte1), 0, "%I1.1.0", NULL },
    { "X21_CPU_Pt1.Pt_Value", IN, REAL, OFFSET_OF(L230_TX_PDO_t, X21_CPU_Pt1.Pt_Value), 0, "%ID1.2", NULL },
    { "X21_CPU_Pt1.Pt_State", IN, U32, OFFSET_OF(L230_TX_PDO_t, X21_CPU_Pt1.Pt_State), 0, "%ID1.6", NULL },
    { "X22_CPU_Pt2.Pt_Value", IN, REAL, OFFSET_OF(L230_TX_PDO_t, X22_CPU_Pt2.Pt_Value), 0, "%ID1.10", NULL },
    { "X22_CPU_Pt2.Pt_State", IN, U32, OFFSET_OF(L230_TX_PDO_t, X22_CPU_Pt2.Pt_State), 0, "%ID1.14", NULL },
    { "X23_CPU_VC1.VC_Value", IN, REAL, OFFSET_OF(L230_TX_PDO_t, X23_CPU_VC1.VC_Value), 0, "%ID1.18", NULL },
    { "X23_CPU_VC1.VC_State", IN, U32, OFFSET_OF(L230_TX_PDO_t, X23_CPU_VC1.VC_State), 0, "%ID1.22", NULL },
};

const DeviceDesc_t DEVICE_DESC_L230 = {
    .model = "L230",
    .vendor_id = 0,
    .product_code = 0,
    .rx_bytes = sizeof(L230_RX_PDO_t),
    .tx_bytes = sizeof(L230_TX_PDO_t),
    .tags = L230_TAGS,
    .tag_count = (uint32_t)(sizeof(L230_TAGS) / sizeof(L230_TAGS[0])),
    .hooks = { 0 },
};

const char *l230_get_model_identifier(void)
{
    return DEVICE_DESC_L230.model;
}
