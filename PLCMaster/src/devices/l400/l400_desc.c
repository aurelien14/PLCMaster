/* L400 device descriptor implementation. */

#include "l400_desc.h"

#include <stddef.h>
#include <stdint.h>
#include <soem/soem.h>

#define L400_MODULE_NUMBER 1
#define L400_WATCHDOG_TIMER_MS 1000

static const TagDesc_t L400_TAGS[] = {
	/* =======================
	 * INPUTS (TX PDO -> PLC IN)
	 * ======================= */

	 /* TxPDO 0x1A00 - L400_DI_Byte0 */
	 { "X31_In0",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte0), 0, NULL, NULL },
	 { "X32_In1",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte0), 1, NULL, NULL },
	 { "X33_In2",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte0), 2, NULL, NULL },
	 { "X34_In3",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte0), 3, NULL, NULL },
	 { "X75_1_In4",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte0), 4, NULL, NULL },
	 { "X75_3_In5",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte0), 5, NULL, NULL },
	 { "X75_5_In6",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte0), 6, NULL, NULL },
	 { "X75_7_In7",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte0), 7, NULL, NULL },

	 /* TxPDO 0x1A01 - L400_DI_Byte1 */
	 { "X39_In0",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte1), 0, NULL, NULL },
	 { "X42_In1",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte1), 1, NULL, NULL },
	 { "X41_In2",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte1), 2, NULL, NULL },
	 { "X44_In3",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte1), 3, NULL, NULL },
	 { "X38_In4",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte1), 4, NULL, NULL },
	 { "X37_In5",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte1), 5, NULL, NULL },
	 { "X36_In6",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte1), 6, NULL, NULL },
	 { "X35_In7",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte1), 7, NULL, NULL },

	 /* TxPDO 0x1A02 - L400_DI_Byte2 */
	 { "X55_In0",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, L400_DI_Byte2), 0, NULL, NULL },

	 /* TxPDO 0x1A03 - X1_In0_In7 */
	 { "X1_In0",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, X1_In0_In7), 0, NULL, NULL },
	 { "X1_In1",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, X1_In0_In7), 1, NULL, NULL },
	 { "X1_In2",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, X1_In0_In7), 2, NULL, NULL },
	 { "X1_In3",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, X1_In0_In7), 3, NULL, NULL },
	 { "X1_In4",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, X1_In0_In7), 4, NULL, NULL },
	 { "X1_In5",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, X1_In0_In7), 5, NULL, NULL },
	 { "X1_In6",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, X1_In0_In7), 6, NULL, NULL },
	 { "X1_In7",	TAGDIR_IN, TAG_T_BOOL, (uint32_t)offsetof(L400_TX_PDO_t, X1_In0_In7), 7, NULL, NULL },

	 /* Pt / VC inputs */
	 { "X21_CPU_Pt1.Pt_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X21_CPU_Pt1.Pt_Value), 0, NULL, NULL },
	 { "X21_CPU_Pt1.Pt_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X21_CPU_Pt1.Pt_State), 0, NULL, NULL },

	 { "X22_CPU_Pt2.Pt_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X22_CPU_Pt2.Pt_Value), 0, NULL, NULL },
	 { "X22_CPU_Pt2.Pt_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X22_CPU_Pt2.Pt_State), 0, NULL, NULL },

	 { "X23_CPU_VC1.VC_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X23_CPU_VC1.VC_Value), 0, NULL, NULL },
	 { "X23_CPU_VC1.VC_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X23_CPU_VC1.VC_State), 0, NULL, NULL },

	 { "X31_DAIO_Pt1.Pt_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X31_DAIO_Pt1.Pt_Value), 0, NULL, NULL },
	 { "X31_DAIO_Pt1.Pt_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X31_DAIO_Pt1.Pt_State), 0, NULL, NULL },

	 { "X32_DAIO_Pt2.Pt_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X32_DAIO_Pt2.Pt_Value), 0, NULL, NULL },
	 { "X32_DAIO_Pt2.Pt_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X32_DAIO_Pt2.Pt_State), 0, NULL, NULL },

	 { "X33_DAIO_Pt3.Pt_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X33_DAIO_Pt3.Pt_Value), 0, NULL, NULL },
	 { "X33_DAIO_Pt3.Pt_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X33_DAIO_Pt3.Pt_State), 0, NULL, NULL },

	 { "X34_DAIO_Pt4.Pt_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X34_DAIO_Pt4.Pt_Value), 0, NULL, NULL },
	 { "X34_DAIO_Pt4.Pt_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X34_DAIO_Pt4.Pt_State), 0, NULL, NULL },

	 { "X35_DAIO_Pt5.Pt_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X35_DAIO_Pt5.Pt_Value), 0, NULL, NULL },
	 { "X35_DAIO_Pt5.Pt_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X35_DAIO_Pt5.Pt_State), 0, NULL, NULL },

	 { "X36_DAIO_Pt6.Pt_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X36_DAIO_Pt6.Pt_Value), 0, NULL, NULL },
	 { "X36_DAIO_Pt6.Pt_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X36_DAIO_Pt6.Pt_State), 0, NULL, NULL },

	 { "X37_DAIO_Pt7.Pt_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X37_DAIO_Pt7.Pt_Value), 0, NULL, NULL },
	 { "X37_DAIO_Pt7.Pt_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X37_DAIO_Pt7.Pt_State), 0, NULL, NULL },

	 { "X38_DAIO_Pt8.Pt_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X38_DAIO_Pt8.Pt_Value), 0, NULL, NULL },
	 { "X38_DAIO_Pt8.Pt_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X38_DAIO_Pt8.Pt_State), 0, NULL, NULL },

	 { "X41_1_DAIO_VC1.VC_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X41_1_DAIO_VC1.VC_Value), 0, NULL, NULL },
	 { "X41_1_DAIO_VC1.VC_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X41_1_DAIO_VC1.VC_State), 0, NULL, NULL },

	 { "X41_3_DAIO_VC2.VC_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X41_3_DAIO_VC2.VC_Value), 0, NULL, NULL },
	 { "X41_3_DAIO_VC2.VC_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X41_3_DAIO_VC2.VC_State), 0, NULL, NULL },

	 { "X40_1_DAIO_VC3.VC_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X40_1_DAIO_VC3.VC_Value), 0, NULL, NULL },
	 { "X40_1_DAIO_VC3.VC_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X40_1_DAIO_VC3.VC_State), 0, NULL, NULL },

	 { "X40_3_DAIO_VC4.VC_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X40_3_DAIO_VC4.VC_Value), 0, NULL, NULL },
	 { "X40_3_DAIO_VC4.VC_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X40_3_DAIO_VC4.VC_State), 0, NULL, NULL },

	 { "X40_5_DAIO_VC5.VC_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X40_5_DAIO_VC5.VC_Value), 0, NULL, NULL },
	 { "X40_5_DAIO_VC5.VC_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X40_5_DAIO_VC5.VC_State), 0, NULL, NULL },

	 { "X40_7_DAIO_VC6.VC_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X40_7_DAIO_VC6.VC_Value), 0, NULL, NULL },
	 { "X40_7_DAIO_VC6.VC_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X40_7_DAIO_VC6.VC_State), 0, NULL, NULL },

	 { "X42_1_DAIO_AC1.VC_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X42_1_DAIO_AC1.VC_Value), 0, NULL, NULL },
	 { "X42_1_DAIO_AC1.VC_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X42_1_DAIO_AC1.VC_State), 0, NULL, NULL },

	 { "X42_3_DAIO_AC2.VC_Value",	TAGDIR_IN, TAG_T_REAL, (uint32_t)offsetof(L400_TX_PDO_t, X42_3_DAIO_AC2.VC_Value), 0, NULL, NULL },
	 { "X42_3_DAIO_AC2.VC_State",	TAGDIR_IN, TAG_T_U32,  (uint32_t)offsetof(L400_TX_PDO_t, X42_3_DAIO_AC2.VC_State), 0, NULL, NULL },

	 /* Other info */
	 { "L400_X1_Drehfeld",			TAGDIR_IN, TAG_T_U32, (uint32_t)offsetof(L400_TX_PDO_t, L400_X1_Drehfeld), 0, NULL, NULL },

	 { "slaveinfo.ProductCode",		TAGDIR_IN, TAG_T_U32, (uint32_t)offsetof(L400_TX_PDO_t, slaveinfo.ProductCode), 0, NULL, NULL },
	 { "slaveinfo.SerialNumber",	TAGDIR_IN, TAG_T_U32, (uint32_t)offsetof(L400_TX_PDO_t, slaveinfo.SerialNumber), 0, NULL, NULL },
	 { "slaveinfo.HW_Version",		TAGDIR_IN, TAG_T_U32, (uint32_t)offsetof(L400_TX_PDO_t, slaveinfo.HW_Version), 0, NULL, NULL },
	 { "slaveinfo.FW_Version",		TAGDIR_IN, TAG_T_U32, (uint32_t)offsetof(L400_TX_PDO_t, slaveinfo.FW_Version), 0, NULL, NULL },
	 { "slaveinfo.AliasAddress",	TAGDIR_IN, TAG_T_U16, (uint32_t)offsetof(L400_TX_PDO_t, slaveinfo.AliasAddress), 0, NULL, NULL },

	 { "L400Info.L400_SerialNumber",TAGDIR_IN, TAG_T_U32, (uint32_t)offsetof(L400_TX_PDO_t, L400Info.L400_SerialNumber), 0, NULL, NULL },
	 { "L400Info.L400_HW_Version",	TAGDIR_IN, TAG_T_U32, (uint32_t)offsetof(L400_TX_PDO_t, L400Info.L400_HW_Version), 0, NULL, NULL },
	 { "L400Info.L400_FW_Version",	TAGDIR_IN, TAG_T_U32, (uint32_t)offsetof(L400_TX_PDO_t, L400Info.L400_FW_Version), 0, NULL, NULL },
	 { "L400Info.L400_State",		TAGDIR_IN, TAG_T_U32, (uint32_t)offsetof(L400_TX_PDO_t, L400Info.L400_State), 0, NULL, NULL },

	 /* =======================
	  * OUTPUTS (RX PDO -> PLC OUT)
	  * ======================= */

	  /* RxPDO 0x1600 - L400_DO_Byte0 (bits) */
	  { "X14_Out0",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte0), 0, NULL, NULL },
	  { "X15_Out1",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte0), 1, NULL, NULL },
	  { "X16A_Out2",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte0), 2, NULL, NULL },
	  { "X17_Out3",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte0), 3, NULL, NULL },
	  { "X18_Out4",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte0), 4, NULL, NULL },
	  { "X19_Out5",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte0), 5, NULL, NULL },
	  { "X20_Out6",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte0), 6, NULL, NULL },
	  { "X21_Out7",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte0), 7, NULL, NULL },

	  /* RxPDO 0x1601 - L400_DO_Byte1 (bits) */
	  { "X49_Out0",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte1), 0, NULL, NULL },
	  { "X48_Out1",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte1), 1, NULL, NULL },
	  { "X8_Out2",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte1), 2, NULL, NULL },
	  { "X9_Out3",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte1), 3, NULL, NULL },
	  { "X10_Out4",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte1), 4, NULL, NULL },
	  { "X11_Out5",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte1), 5, NULL, NULL },
	  { "X12_Out6",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte1), 6, NULL, NULL },
	  { "X13_Out7",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte1), 7, NULL, NULL },

	  /* RxPDO 0x1602 - L400_DO_Byte2 (bits) */
	  { "X22_Out0",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte2), 0, NULL, NULL },
	  { "X24_Out1",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte2), 1, NULL, NULL },
	  { "X23_Out2",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte2), 2, NULL, NULL },
	  { "X46_Out3",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte2), 3, NULL, NULL },
	  { "X53_Out4",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte2), 4, NULL, NULL },
	  { "X54_Out5",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte2), 5, NULL, NULL },
	  { "X16B_Out6",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte2), 6, NULL, NULL },

	  /* RxPDO 0x1603 - L400_DO_Byte3 (bits) */
	  { "X76_1_Out0",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte3), 0, NULL, NULL },
	  { "X76_3_Out1",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte3), 1, NULL, NULL },
	  { "X76_5_Out2",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte3), 2, NULL, NULL },
	  { "X76_7_Out3",	TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, L400_DO_Byte3), 3, NULL, NULL },

	  /* RxPDO 0x1604 - X3_Out0_Out7 */
	  { "X3_Out0",		TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, X3_Out0_Out7), 0, NULL, NULL },
	  { "X3_Out1",		TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, X3_Out0_Out7), 1, NULL, NULL },
	  { "X3_Out2",		TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, X3_Out0_Out7), 2, NULL, NULL },
	  { "X3_Out3",		TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, X3_Out0_Out7), 3, NULL, NULL },
	  { "X3_Out4",		TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, X3_Out0_Out7), 4, NULL, NULL },
	  { "X3_Out5",		TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, X3_Out0_Out7), 5, NULL, NULL },
	  { "X3_Out6",		TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, X3_Out0_Out7), 6, NULL, NULL },
	  { "X3_Out7",		TAGDIR_OUT, TAG_T_BOOL, (uint32_t)offsetof(L400_RX_PDO_t, X3_Out0_Out7), 7, NULL, NULL },

	  /* Control words / setpoints (U32) */
	  { "X21_CPU_Pt1_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X21_CPU_Pt1_Ctrl), 0, NULL, NULL },
	  { "X22_CPU_Pt2_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X22_CPU_Pt2_Ctrl), 0, NULL, NULL },
	  { "X23_CPU_VC1_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X23_CPU_VC1_Ctrl), 0, NULL, NULL },

	  { "X31_DAIO_Pt1_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X31_DAIO_Pt1_Ctrl), 0, NULL, NULL },
	  { "X32_DAIO_Pt2_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X32_DAIO_Pt2_Ctrl), 0, NULL, NULL },
	  { "X33_DAIO_Pt3_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X33_DAIO_Pt3_Ctrl), 0, NULL, NULL },
	  { "X34_DAIO_Pt4_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X34_DAIO_Pt4_Ctrl), 0, NULL, NULL },
	  { "X35_DAIO_Pt5_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X35_DAIO_Pt5_Ctrl), 0, NULL, NULL },
	  { "X36_DAIO_Pt6_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X36_DAIO_Pt6_Ctrl), 0, NULL, NULL },
	  { "X37_DAIO_Pt7_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X37_DAIO_Pt7_Ctrl), 0, NULL, NULL },
	  { "X38_DAIO_Pt8_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X38_DAIO_Pt8_Ctrl), 0, NULL, NULL },

	  { "X41_1_DAIO_VC1_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X41_1_DAIO_VC1_Ctrl), 0, NULL, NULL },
	  { "X41_3_DAIO_VC2_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X41_3_DAIO_VC2_Ctrl), 0, NULL, NULL },
	  { "X40_1_DAIO_VC3_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X40_1_DAIO_VC3_Ctrl), 0, NULL, NULL },
	  { "X40_3_DAIO_VC4_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X40_3_DAIO_VC4_Ctrl), 0, NULL, NULL },
	  { "X40_5_DAIO_VC5_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X40_5_DAIO_VC5_Ctrl), 0, NULL, NULL },
	  { "X40_7_DAIO_VC6_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X40_7_DAIO_VC6_Ctrl), 0, NULL, NULL },
	  { "X42_1_DAIO_AC1_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X42_1_DAIO_AC1_Ctrl), 0, NULL, NULL },
	  { "X42_3_DAIO_AC2_Ctrl",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X42_3_DAIO_AC2_Ctrl), 0, NULL, NULL },

	  /* Analog outputs values (U32) */
	  { "X5_1_DAIO_Aout1_Value",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X5_1_DAIO_Aout1_Value), 0, NULL, NULL },
	  { "X5_3_DAIO_Aout2_Value",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X5_3_DAIO_Aout2_Value), 0, NULL, NULL },
	  { "X5_5_DAIO_Aout3_Value",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X5_5_DAIO_Aout3_Value), 0, NULL, NULL },
	  { "X5_7_DAIO_Aout4_Value",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X5_7_DAIO_Aout4_Value), 0, NULL, NULL },
	  { "X6_1_DAIO_Aout5_Value",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X6_1_DAIO_Aout5_Value), 0, NULL, NULL },
	  { "X6_3_DAIO_Aout6_Value",	TAGDIR_OUT, TAG_T_U32, (uint32_t)offsetof(L400_RX_PDO_t, X6_3_DAIO_Aout6_Value), 0, NULL, NULL },
};


static void config_hook_EL400(ecx_contextt* ctx, uint16_t slave)
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

const DeviceDesc_t DEVICE_DESC_L400 = {
    .model = "L400 DAIO MAXI",
    .vendor_id = 0x47535953,
    .product_code = 0x3213329,
    .rx_bytes = sizeof(L400_RX_PDO_t),
    .tx_bytes = sizeof(L400_TX_PDO_t),
    .tags = L400_TAGS,
    .tag_count = (uint32_t)(sizeof(L400_TAGS) / sizeof(L400_TAGS[0])),
    .hooks = {.on_init = config_hook_EL400 },
};

const char* l400_get_model_identifier(void)
{
    return DEVICE_DESC_L400.model;
}
