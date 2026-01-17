/* L230 device descriptor implementation. */

#include "l230_desc.h"

#include <stddef.h>
#include <stdint.h>
#include <soem/soem.h>

#define L230_MODULE_NUMBER 1
#define L230_WATCHDOG_TIMER_MS 1000

static const TagDesc_t L230_TAGS[] = {
    { "X30_2_In0", TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L230_TX_PDO_t, L230_DI_Byte0), 0, NULL, NULL },
    { "X30_4_In1", TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L230_TX_PDO_t, L230_DI_Byte0), 1, NULL, NULL },
    { "X30_6_In2", TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L230_TX_PDO_t, L230_DI_Byte0), 2, NULL, NULL },
    { "X30_8_In3", TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L230_TX_PDO_t, L230_DI_Byte0), 3, NULL, NULL },
    { "X30_11_In4", TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L230_TX_PDO_t, L230_DI_Byte0), 4, NULL, NULL },
    { "X30_13_In5", TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L230_TX_PDO_t, L230_DI_Byte0), 5, NULL, NULL },
    { "X30_19_In6", TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L230_TX_PDO_t, L230_DI_Byte0), 6, NULL, NULL },
    { "X30_21_In7", TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L230_TX_PDO_t, L230_DI_Byte0), 7, NULL, NULL },
    { "X55_In0", TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L230_TX_PDO_t, L230_DI_Byte1), 0, NULL, NULL },
    { "X21_CPU_Pt1.Pt_Value", TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L230_TX_PDO_t, X21_CPU_Pt1.Pt_Value), 0, NULL, NULL },
    { "X21_CPU_Pt1.Pt_State", TAGDIR_IN, TAG_T_U32, (uint32_t)offsetof(L230_TX_PDO_t, X21_CPU_Pt1.Pt_State), 0, NULL, NULL },
    { "X22_CPU_Pt2.Pt_Value", TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L230_TX_PDO_t, X22_CPU_Pt2.Pt_Value), 0, NULL, NULL },
    { "X22_CPU_Pt2.Pt_State", TAGDIR_IN, TAG_T_U32, (uint32_t)offsetof(L230_TX_PDO_t, X22_CPU_Pt2.Pt_State), 0, NULL, NULL },
    { "X23_CPU_VC1.VC_Value", TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L230_TX_PDO_t, X23_CPU_VC1.VC_Value), 0, NULL, NULL },
    { "X23_CPU_VC1.VC_State", TAGDIR_IN, TAG_T_U32, (uint32_t)offsetof(L230_TX_PDO_t, X23_CPU_VC1.VC_State), 0, NULL, NULL },
    { "X15_Out0", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte0), 0, NULL, NULL },
    { "X12_Out1", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte0), 1, NULL, NULL },
    { "X13_Out2", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte0), 2, NULL, NULL },
    { "X3_Out3", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte0), 3, NULL, NULL },
    { "X4_Out4", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte0), 4, NULL, NULL },
    { "X14_Out5", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte0), 5, NULL, NULL },
    { "X1_Out6", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte0), 6, NULL, NULL },
    { "X11_Out0", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte1), 0, NULL, NULL },
    { "X10_Out1", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte1), 1, NULL, NULL },
    { "X5_Out2", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte1), 2, NULL, NULL },
    { "X6_Out3", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte1), 3, NULL, NULL },
    { "X7_Out4", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte1), 4, NULL, NULL },
    { "X8_Out5", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte1), 5, NULL, NULL },
    { "X9_Out6", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte1), 6, NULL, NULL },
    { "X8a_Out7", TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L230_RX_PDO_t, L230_DO_Byte1), 7, NULL, NULL },
    { "X21_CPU_Pt1_Ctrl", TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L230_RX_PDO_t, X21_CPU_Pt1_Ctrl), 0, NULL, NULL },
    { "X22_CPU_Pt2_Ctrl", TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L230_RX_PDO_t, X22_CPU_Pt2_Ctrl), 0, NULL, NULL },
    { "X23_CPU_VC1_Ctrl", TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L230_RX_PDO_t, X23_CPU_VC1_Ctrl), 0, NULL, NULL },
    { "AliasAddress", TAGDIR_IN, TAG_T_U16, (uint16_t)offsetof(L230_TX_PDO_t, slaveinfo.AliasAddress), 0, NULL, NULL },
};

static void config_hook_EL230(ecx_contextt* ctx, uint16_t slave)
{
    ec_slavet* pslave = &ctx->slavelist[slave];

    pslave->SM[2].StartAddr = 0x1100;
    pslave->SM[2].SMflags = 0x64;
    pslave->SM[3].StartAddr = 0x1300;
    pslave->SM[3].SMflags = 0x20;

    uint16_t new_timeout = 1000;
    //ecx_FPWRw(&ctx->port, pslave->configadr, 0x420,
        //new_timeout, EC_TIMEOUTRET);
}

const DeviceDesc_t DEVICE_DESC_L230 = {
    .model = "L230 DAIO Micro",
    .vendor_id = 0x47535953,
    .product_code = 0x3213335,
    .rx_bytes = sizeof(L230_RX_PDO_t),
    .tx_bytes = sizeof(L230_TX_PDO_t),
    .tags = L230_TAGS,
    .tag_count = (uint32_t)(sizeof(L230_TAGS) / sizeof(L230_TAGS[0])),
    .hooks = { .on_init = config_hook_EL230 },
};

const char *l230_get_model_identifier(void)
{
    return DEVICE_DESC_L230.model;
}
